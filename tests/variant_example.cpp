// JSON for C++ : tests
// Belongs to the public domain

#include "catch_wrap.hpp"

#include <json-cpp.hpp>

#include <cassert>
#include <initializer_list>
#include <map>
#include <string>
#include <vector>

namespace
{
    using jsoncpp::Type;

    struct Variant
    {
        Variant() : m_type{Type::Undefined} {}
        Variant(std::nullptr_t) : m_type{Type::Null} {}
        Variant(bool b) : m_type{Type::Boolean}, m_bool{b} {}
        Variant(double n) : m_type{Type::Number}, m_number{n} {}
        Variant(const char* s) : m_type{Type::String}, m_string{s} {}
        Variant(std::vector<Variant> a) : m_type{Type::Array}, m_array(a) {}

        Type m_type;
        union
        {
            bool m_bool;
            double m_number;
        };
        std::string m_string;
        std::vector<Variant> m_array;
        std::map<std::string, Variant> m_object;

        bool get_bool() const { assert(m_type == Type::Boolean); return m_bool; }
        double get_num() const { assert(m_type == Type::Number); return m_number; }
        std::string get_str() const { assert(m_type == Type::String); return m_string; }
        Variant& operator[](std::size_t idx) { assert(m_type == Type::Array); return m_array[idx]; }
        Variant& operator[](const std::string& name) { assert(m_type == Type::Object); return m_object[name]; }
    };

    template<typename... Ts>
    Variant Array(Ts... ts)
    {
        return std::vector<Variant>{ts...};
    }

    template<class X>
    void serialize(jsoncpp::Parser<X>& parser, Variant& v)
    {
        v.m_type = parser.peekType();
        switch (v.m_type)
        {
        case Type::Null: case Type::Undefined: return;
        case Type::Boolean: serialize(parser, v.m_bool); return;
        case Type::Number: serialize(parser, v.m_number); return;
        case Type::String: serialize(parser, v.m_string); return;
        case Type::Array: serialize(parser, v.m_array); return;
        case Type::Object: serialize(parser, v.m_object); return;
        }
    }

    template<class X>
    void serialize(jsoncpp::Generator<X>& generator, Variant& v)
    {
        switch (v.m_type)
        {
        case Type::Undefined: assert(!"undefined value"); return;
        case Type::Null: serialize(generator, nullptr); return;
        case Type::Boolean: serialize(generator, v.m_bool); return;
        case Type::Number: serialize(generator, v.m_number); return;
        case Type::String: serialize(generator, v.m_string); return;
        case Type::Array: serialize(generator, v.m_array); return;
        case Type::Object: serialize(generator, v.m_object); return;
        }
    }
}

TEST_CASE("Parsing to a Variant type")
{
    Variant v;
    jsoncpp::parse(v, R"(
        {
            "b": true,
            "s": "text",
            "a": [null, {}, 42]
        }
    )");

    REQUIRE(v["b"].get_bool() == true);
    REQUIRE(v["s"].get_str() == "text");
    REQUIRE(v["a"][0].m_type == Type::Null);
    REQUIRE(v["a"][1].m_type == Type::Object);
    REQUIRE(v["a"][2].get_num() == 42);
}

TEST_CASE("Writing a Variant type")
{
    Variant v;
    v.m_type = Type::Object;
    v["b"] = true;
    v["s"] = "text";
    v["a"] = Array(nullptr, 1.2);
    // note that std::map sorts field names
    auto expected = R"({"a": [null, 1.2], "b": true, "s": "text"})";
    REQUIRE(jsoncpp::to_string(v) == expected);
}