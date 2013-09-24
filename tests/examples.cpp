// JSON for C++ : tests
// Belongs to the public domain

#include "catch_wrap.hpp"

#include <json-cpp.hpp>

struct Foo {
    int num;
    std::string str;
};

template<class X>
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