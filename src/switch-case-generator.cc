#include <fstream>
#include <sstream>
#include <filesystem>
#include <string_view>
#include <cstring>

auto do_generate(const std::string_view source, std::ofstream &output) noexcept
  -> void
{
  char *chunk = std::strtok(const_cast<char *>(source.data()), " ");
  std::ostringstream outstrstream { "switch (/* unspecified */)\n{\n", std::ios::app };

  do
  {
    /* If we find a range of hexadecimal values,  */

    if (const char *const hyphen { strstr(chunk, "-") })
    {
      const std::ptrdiff_t position { hyphen - chunk };
      char lower[5], upper[5];
      std::uint32_t lower_ui { }, upper_ui { };

      /* get the individual lower and upper bounds  */

      (void)std::strncpy(lower, chunk, static_cast<std::size_t>(position));
      (void)std::strcpy(upper, 1 + chunk + position);

      lower[position] = '\0';

      /* and then, expand the hexadecimal numbers in between.  */

      std::sscanf(lower, "%x", &lower_ui);
      std::sscanf(upper, "%x", &upper_ui);

      while (lower_ui <= upper_ui)
      {
        /* A C-like equivalent would be `std::fprintf(..., "case 0x%X:\n", lower_ui);'.  */
        outstrstream << "  case 0x" << std::uppercase << std::hex << lower_ui << ":\n";
        ++lower_ui;
      }
    }
    else
    {
      std::uint32_t hexadecimal { };
      std::sscanf(chunk, "%x", &hexadecimal);
      outstrstream << "  case 0x" << std::uppercase << std::hex << hexadecimal << ":\n";
    }
  }
  while ((chunk = std::strtok(nullptr, " ")));

  outstrstream << "}\n";
  output << outstrstream.str();
  output.close();
}

auto main([[maybe_unused]] std::int32_t argc, [[maybe_unused]] char **argv) -> std::int32_t
{
  const std::string output_directory { "out" };

  if (!std::filesystem::exists(output_directory))
  {
    std::filesystem::create_directory(output_directory);
  }

  std::filesystem::path output_path{output_directory};
  std::ofstream out_stream { output_path.c_str() };
  const std::string fallback_output_file_name { "default_output.cpp" };

  output_path /= fallback_output_file_name;
  std::string default_input { "31 0386 0388-038a 038c 038e-03a1 03a3-03ce 03d0-03d6 03da 03dc 03de 03e0 32 03e2-03f3 1f00-1f15 1f18-1f1d 1f20-1f45 1f48-1f4d 1f50-1f57 1f59 1f5b 33 1f5d 1f5f-1f7d 1f80-1fb4 1fb6-1fbc 1fc2-1fc4 1fc6-1fcc 1fd0-1fd3 34 1fd6-1fdb 1fe0-1fec 1ff2-1ff4 1ff6-1ffc" };
  do_generate(default_input, out_stream);
  
  out_stream.close();
  
  return EXIT_SUCCESS;
}