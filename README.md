# JSON parser and generator for C++

*Version 0.1 alpha*

**json-cpp** is a C++11 JSON serialization library.

## Example

    #include <json-cpp.hpp>

    struct Foo {
        int num;
        std::string str;
    };

    template<typename X>
    inline void serialize(jsoncpp::Stream<X>& stream, Foo& o) {
        fields(o, stream, "num", o.num, "str", o.str);
    }

    TEST_CASE("Parsing JSON") {
        Foo foo;
        jsoncpp::parse(foo, R"(
        {
            "num": 42,
            "str": "banana - \uD83C\uDF4c"
        } )");
        REQUIRE(foo.num == 42);
        REQUIRE(foo.str == "banana - \xF0\x9F\x8D\x8C");
    }

    TEST_CASE("Generating JSON") {
        Foo foo{42, "banana - \xF0\x9F\x8D\x8C"};
        auto expected = R"({"num": 42, "str": "banana - )" "\xF0\x9F\x8D\x8C\"}";
        REQUIRE(jsoncpp::to_string(foo) == expected);
    }

You can find more examples in the `/tests/` directory.

## Installation

This is a header-only library.  
Just download the [single header version](/single_header/json-cpp.hpp) and use it in your projects.

Or, you can checkout whole repository and add the `/include/` directory to compiler's header search path.
You might also want to run tests from a `/tests/{compiler}/` directory.

## Supported compilers and platforms

The library was developed and tested on:

* Microsoft Visual C++ 2013 RC x86-32, x86-64 on Win32  
* G++ 4.8.0 x86-32 on Win32 (MinGW)  

## License

**json-cpp** is placed in the public domain.

## External links

* [JSON Home page](http://www.json.org/)
* [RFC 4627](http://www.ietf.org/rfc/rfc4627.txt) - The application/json Media Type for JavaScript Object Notation (JSON)
