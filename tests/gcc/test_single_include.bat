@echo off

rem Path to the CATCH library should be added to G++ header search path,
rem e.g. via the environment variable:
rem     set CPLUS_INCLUDE_PATH=path/to/catch

g++ -std=c++11 ^
    -Wall -Werror -Wno-parentheses ^
    -I..\..\single_include ^
    ..\tests_main.cpp ^
    ..\examples.cpp ^
    ..\variant_example.cpp ^
    && a.exe
