#include <fstream>
#include <sstream>
#include <filesystem>
#include <string_view>
#include <cstring>
#include <memory>

/* The program name.  */
constexpr inline static const char *PROGNAME = "switch-case-generator";

/* The file to write the output.  */
static std::string dest { };

/* Sends an error to stderr.  */
[[noreturn]] auto emit_error(const std::string_view error_message)
  -> void
{
  fprintf(stderr, "%s: %s\n", PROGNAME, error_message.data());
  std::exit(EXIT_FAILURE);
}

/* Manual for the program.  */
[[noreturn]] auto usage() noexcept
  -> void
{
  std::printf("\
Usage: %s OUTPUT HEXADECIMAL...\n\
\n", PROGNAME);

  std::fputs("\
Generates a file named OUTPUT with a switch-case containing\n\
a case for each HEXADECIMAL number. HEXADECIMAL can also be\n\
an interval of valid hexadecimal numbers, e.g. 0388-038A.\n", stdout);

  std::exit(EXIT_FAILURE);
}

/* Generates the switch-case source code.  */
auto do_generate(const std::string_view source, std::ofstream &output) noexcept
  -> void
{
  char *chunk = std::strtok(const_cast<char *>(source.data()), " ");
  std::ostringstream outstrstream { "switch (/* unspecified */)\n{\n", std::ios::app };

  do
  {
    if (std::strstr(chunk, "0x") or std::strstr(chunk, "0X"))
    {
      output.close();
      std::remove(dest.c_str());
      ::emit_error("invalid number: Hexadecimal number must not be preceded by `0x' or `0X'.");
    }

    /* If we find a range of hexadecimal values,  */

    if (const char *const hyphen { strstr(chunk, "-") })
    {
      const std::ptrdiff_t position { hyphen - chunk };

      if (0 == position)
      {
        output.close();
        std::remove(dest.c_str());
        ::emit_error("invalid number: Cannot parse a negative number.");
      }

      std::unique_ptr<char[]> lower(new char[position + 1]);
      std::unique_ptr<char[]> upper(new char[position + 1]);

      std::uint32_t lower_ui { }, upper_ui { };

      /* get the individual lower and upper bounds  */

      (void)std::strncpy(lower.get(), chunk, static_cast<std::size_t>(position));
      (void)std::strcpy(upper.get(), 1 + chunk + position);

      lower[position] = '\0';

      /* and then, expand the hexadecimal numbers in between.  */

      std::sscanf(lower.get(), "%x", &lower_ui);
      std::sscanf(upper.get(), "%x", &upper_ui);

      if (upper_ui < lower_ui)
      {
        output.close();
        std::remove(dest.c_str());
        ::emit_error("illegal range: Lower bound cannot be greater than upper bound.");        
      }

      while (lower_ui <= upper_ui)
      {
        /* A C-like equivalent would be `std::fprintf(..., "case 0x%X:\n", lower_ui);'.  */
        outstrstream << "case 0x" << std::uppercase << std::hex << lower_ui << ":\n";
        ++lower_ui;
      }
    }
    else
    {
      std::uint32_t hexadecimal { };
      std::sscanf(chunk, "%x", &hexadecimal);
      outstrstream << "case 0x" << std::uppercase << std::hex << hexadecimal << ":\n";
    }
  }
  while ((chunk = std::strtok(nullptr, " ")));

  outstrstream << "}\n";
  output << outstrstream.str();
  output.close();
}

auto main([[maybe_unused]] std::int32_t argc, [[maybe_unused]] char **argv)
  -> std::int32_t
{
  if (argc < 3)
  {
    ::usage();
  }

  std::string input;
  char **begin = argv + 1;
  dest = argv[1];

  while (++begin < argv + argc)
  {
    /* FIXME: Avoid using concatenation.  */

    input += *begin;
    input += " ";
  }

  std::ofstream out_stream { dest };
  do_generate(input, out_stream);
  out_stream.close();

  return EXIT_SUCCESS;
}