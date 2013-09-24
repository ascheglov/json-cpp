// JSON for C++ : tests
// Belongs to the public domain

#include "catch_wrap.hpp"

#include <json-cpp/details/string_writer.hpp>
#include <json-cpp/generate.hpp>
#include <json-cpp/serialization_helpers.hpp>
#include <json-cpp/std_types.hpp>

namespace
{
    template<std::size_t N>
    static std::string writeStr(const char(&strLit)[N])
    {
        std::string str{strLit, strLit + N - 1};
        std::ostringstream rawStream;
        auto&& sink = [&rawStream](char c) { rawStream.put(c); };
        jsoncpp::details::writeString(str, sink);
        return rawStream.str();
    }
}

TEST_CASE("Writing String", "[gen]")
{
    REQUIRE(writeStr("") == R"("")");
    REQUIRE(writeStr("some str") == R"("some str")");
    REQUIRE(writeStr("/") == R"("/")");

    auto&& expected = R"("\\\"\b\f\n\r\t")"; // workaround for a VC++ bug
    REQUIRE(writeStr("\\\"\b\f\n\r\t") == expected);

    REQUIRE(writeStr("\0\x1\x1f") == "\"\\u0000\\u0001\\u001F\"");

    REQUIRE(writeStr("\xA2") == "\"\xA2\"");
}

namespace
{
    struct SingleField
    {
        int x{1};
    };

    template<class X>
    inline void serialize(jsoncpp::Stream<X>& stream, SingleField& o)
    {
        fields(o, stream, "x", o.x);
    }

    struct ManyFields
    {
        int x{1}, y{2};
    };

    template<class X>
    inline void serialize(jsoncpp::Stream<X>& stream, ManyFields& o)
    {
        fields(o, stream, "x", o.x, "y", o.y);
    }

    struct Foo
    {
        Foo() = default;
        Foo(const Foo&) = delete;
        void operator=(const Foo&) = delete;

        std::vector<int> arr;
        std::unique_ptr<Foo> p;
    };

    template<class X>
    inline void serialize(jsoncpp::Stream<X>& stream, Foo& o)
    {
        fields(o, stream, "arr", o.arr, "p", o.p);
    }
}

TEST_CASE("Writing Object with single field", "[gen]")
{
    SingleField obj;
    REQUIRE(jsoncpp::to_string(obj) == R"({"x": 1})");
}

TEST_CASE("Writing Object with many fields", "[gen]")
{
    ManyFields obj;
    REQUIRE(jsoncpp::to_string(obj) == R"({"x": 1, "y": 2})");
}

TEST_CASE("Writing standard types", "[gen]")
{
    Foo obj;
    obj.arr.push_back(1);
    obj.arr.push_back(2);
    obj.p.reset(new Foo);
    REQUIRE(jsoncpp::to_string(obj) == R"({"arr": [1, 2], "p": {"arr": [], "p": null}})");
}