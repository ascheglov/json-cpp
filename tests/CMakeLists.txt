add_executable(tests
  catch_wrap.hpp
  abstract_class_example.cpp
  examples.cpp
  generator_tests.cpp
  parser_tests.cpp
  tests_main.cpp
  variant_example.cpp
)

add_custom_command(
  TARGET tests POST_BUILD
  COMMAND tests
  COMMENT Testing...
)
