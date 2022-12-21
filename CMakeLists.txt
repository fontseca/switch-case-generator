cmake_minimum_required(VERSION 3.24)

set(SRCDIR ${CMAKE_SOURCE_DIR}/src)

if (MSVC)
  set(
    CXXFLAGS
      /WX
      /W4
      /fpermissive
  )
else()
  set(
    CXXFLAGS
      -g
      -O3
      -Wall
      -Wextra
      -Werror
      -pedantic
      -fpermissive
      -fstrict-enums
      -fconcepts
      -fcoroutines
  )
endif()

project(
  switch-case-generator
  VERSION 1.0.0.0
  LANGUAGES C CXX
  DESCRIPTION "A C++ switch-case generator."
  HOMEPAGE_URL "https://github.com/fontseca/switch-case-generator"
)

add_executable(
  ${PROJECT_NAME}
      ${SRCDIR}/switch-case-generator.cc
)

target_compile_options(
  ${PROJECT_NAME}
    PUBLIC
      ${CXXFLAGS}
)

target_compile_features(
  ${PROJECT_NAME}
    PRIVATE
      cxx_std_23
      c_std_23
)
