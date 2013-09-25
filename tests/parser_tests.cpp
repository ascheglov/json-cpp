// JSON for C++ : tests
// Belongs to the public domain

#include "catch_wrap.hpp"

#include <json-cpp/details/number_parser.hpp>
#include <json-cpp/details/string_parser.hpp>

#include <json-cpp/details/parser_utility.hpp>

static double parseNum(std::string str)
{
    str += ",";
    auto it = str.begin();
    auto n = jsoncpp::details::parseRealNumber(it);
    REQUIRE(*it == ',');
    return n;
}

TEST_CASE("Parsing Number", "[parser]")
{
    REQUIRE(parseNum("0") == 0);
    REQUIRE(parseNum("1") == 1);
    REQUIRE(parseNum("1234567890") == 1234567890);
    REQUIRE(parseNum("0.5") == 0.5);
    REQUIRE(parseNum("0.25") == 0.25);
    REQUIRE(parseNum("1.25") == 1.25);
    REQUIRE(parseNum("10.25") == 10.25);
    REQUIRE(parseNum("10e2") == 10e2);
    REQUIRE(parseNum("10E2") == 10e2);
    REQUIRE(parseNum("10e+2") == 10e2);
    REQUIRE(parseNum("10e-1") == 10e-1);
    REQUIRE(parseNum("1e-10") == 1e-10);
    REQUIRE(parseNum("0.5e10") == 0.5e10);
    REQUIRE(parseNum("1.5e10") == 1.5e10);
}

template<typename CharT>
static std::basic_string<CharT> parseStr(std::string str)
{
    str += "\","; // string shall end with quote
    
    using StrReader = jsoncpp::details::Reader<std::string::const_iterator>;
    StrReader src{str.begin(), str.end()};

    std::basic_string<CharT> out;
    auto err = parseStringImpl(src, out);
    REQUIRE(err == jsoncpp::ParserError::NoError);
    REQUIRE(*src == ',');
    return out;
}

TEST_CASE("Parsing String to std::string", "[parser]")
{
    REQUIRE(parseStr<char>("") == "");
    REQUIRE(parseStr<char>("x") == "x");
    REQUIRE(parseStr<char>("\\\\") == "\\");
    REQUIRE(parseStr<char>("\\\"") == "\"");
    REQUIRE(parseStr<char>("\\/") == "/");
    REQUIRE(parseStr<char>("\\b\\f\\n\\r\\t") == "\b\f\n\r\t");
    REQUIRE(parseStr<char>("\\u007F") == "\x7f");
    REQUIRE(parseStr<char>("\\u0080") == "\xC2\x80");
    REQUIRE(parseStr<char>("\\u07FF") == "\xDF\xBF");
    REQUIRE(parseStr<char>("\\u0800") == "\xE0\xA0\x80");
    REQUIRE(parseStr<char>("\\uFFFF") == "\xEF\xBF\xBF");
    REQUIRE(parseStr<char>("\\uD800\\uDC00") == "\xF0\x90\x80\x80");
    REQUIRE(parseStr<char>("\\uD83C\\uDF4c") == "\xF0\x9F\x8D\x8C"); // u1F34C, BANANA
}

TEST_CASE("Parsing String to std::wstring", "[parser]")
{
    REQUIRE(parseStr<wchar_t>("\\u0189") == L"\u0189");
    REQUIRE(parseStr<wchar_t>("\\uaAfF") == L"\uaaff");
}

#include <json-cpp/parse.hpp>
#include <json-cpp/std_types.hpp>
#include <json-cpp/serialization_helpers.hpp>

#include <sstream>

namespace
{
    struct SingleField
    {
        int x;

        template<class X> void serialize(jsoncpp::Stream<X>& stream)
        {
            fields(*this, stream, "x", x);
        }
    };
}

TEST_CASE("Parsing Object", "[parser]")
{
    SingleField obj;
    auto&& text = R"( {"x": 42} )";

    SECTION("from const char*")
    {
        jsoncpp::parse(obj, text);
        REQUIRE(obj.x == 42);
    }

    SECTION("from istream")
    {
        std::stringstream stream{text};
        jsoncpp::parse(obj, stream);
        REQUIRE(obj.x == 42);
    }

    SECTION("from string")
    {
        std::string str{text};
        jsoncpp::parse(obj, str);
        REQUIRE(obj.x == 42);
    }

    SECTION("from iterators")
    {
        jsoncpp::parse<char>(obj, std::begin(text), std::end(text));
        REQUIRE(obj.x == 42);
    }
}

namespace
{
    struct BooleanField
    {
        bool b;

        template<class X> void serialize(jsoncpp::Stream<X>& stream)
        {
            fields(*this, stream, "b", b);
        }
    };
}

TEST_CASE("Parsing to a boolean", "[parser]")
{
    BooleanField obj;
    jsoncpp::parse(obj, R"( {"b": true} )");
    REQUIRE(obj.b == true);
    jsoncpp::parse(obj, R"( {"b": false} )");
    REQUIRE(obj.b == false);
}

namespace
{
    struct NumericFields
    {
        char _c;
        unsigned char uc;
        signed char sc;
        short _h;
        unsigned short uh;
        signed short sh;
        int _i;
        unsigned int ui;
        signed int si;
        long _l;
        unsigned long ul;
        signed long sl;

        float _f;
        double _d;

        // TODO: long long, long double

        template<class X> void serialize(jsoncpp::Stream<X>& stream)
        {
            fields(*this, stream
                , "_c", _c
                , "uc", uc
                , "sc", sc
                , "_h", _h
                , "uh", uh
                , "sh", sh
                , "_i", _i
                , "ui", ui
                , "si", si
                , "_l", _l
                , "ul", ul
                , "sl", sl
                , "_f", _f
                , "_d", _d
                );
        }
    };
}

TEST_CASE("Parsing to different arithmetic types", "[parser]")
{
    NumericFields obj;
    jsoncpp::parse(obj, R"({
        "_c": 10,
        "uc": 11,
        "sc": 12,
        "_h": 20,
        "uh": 21,
        "sh": 22,
        "_i": 30,
        "ui": 31,
        "si": 32,
        "_l": 40,
        "ul": 41,
        "sl": 42,
        "_f": 51,
        "_d": 52,
    })");
    REQUIRE(obj._c == 10);
    REQUIRE(obj.uc == 11);
    REQUIRE(obj.sc == 12);
    REQUIRE(obj._h == 20);
    REQUIRE(obj.uh == 21);
    REQUIRE(obj.sh == 22);
    REQUIRE(obj._i == 30);
    REQUIRE(obj.ui == 31);
    REQUIRE(obj.si == 32);
    REQUIRE(obj._l == 40);
    REQUIRE(obj.ul == 41);
    REQUIRE(obj.sl == 42);
    REQUIRE(obj._f == 51);
    REQUIRE(obj._d == 52);
}

namespace
{
    struct Point
    {
        int x, y;
    };

    template<class X>
    inline void serialize(jsoncpp::Stream<X>& stream, Point& o)
    {
        fields(o, stream, "x", o.x, "y", o.y);
    }
}

namespace parser_tests
{
    struct Foo
    {
        bool b;
        double d;
        std::string str;
        std::wstring wstr;
        Point pt;
        std::shared_ptr<Foo> ptr;
        std::shared_ptr<Foo> ptr2;
        std::vector<int> arr;
    };

    template<class X>
    inline void serialize(jsoncpp::Stream<X>& stream, Foo& obj)
    {
        fields(obj, stream,
            "b", obj.b,
            "d", obj.d,
            "str", obj.str,
            "wstr", obj.wstr,
            "pt", obj.pt,
            "ptr", obj.ptr,
            "ptr2", obj.ptr2,
            "arr", obj.arr);
    }
}

TEST_CASE("Parsing to a user-defined type")
{
    parser_tests::Foo foo;
    jsoncpp::parse(foo, R"(
        {
           "b": true,
           "d": 1.2,
           "str": "str",
           "wstr": "wstr",
           "pt": {"x": 1, "y": 2},
           "ptr": {"d": 42},
           "ptr2": null,
           "arr": [1, 2],
        }
    )");

    REQUIRE(foo.b == true);
    REQUIRE(foo.d == 1.2);
    REQUIRE(foo.str == "str");
    REQUIRE(foo.wstr == L"wstr");
    REQUIRE(foo.pt.x == 1);
    REQUIRE(foo.pt.y == 2);
    REQUIRE(foo.ptr->d == 42);
    REQUIRE(foo.ptr2 == nullptr);
    REQUIRE(foo.arr.size() == 2);
    REQUIRE(foo.arr[0] == 1);
    REQUIRE(foo.arr[1] == 2);
}
