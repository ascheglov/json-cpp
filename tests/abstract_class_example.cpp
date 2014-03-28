// JSON for C++ : tests
// Belongs to the public domain

#include "catch_wrap.hpp"

#include <json-cpp.hpp>

#include <memory>
#include <string>
#include <vector>

namespace
{
    struct AbstractClass
    {
        virtual ~AbstractClass() = default;
        virtual std::string toString() = 0;
    };

    struct IntClass : AbstractClass
    {
        int m_value;
        virtual std::string toString() override { return std::to_string(m_value); }
        
        template<class X> void serialize(jsoncpp::Stream<X>& stream)
        {
            fields(*this, stream, "value", m_value);
        }
    };

    struct StrClass : AbstractClass
    {
        std::string m_value;
        virtual std::string toString() override { return m_value; }

        template<class X> void serialize(jsoncpp::Stream<X>& stream)
        {
            fields(*this, stream, "value", m_value);
        }
    };
    
    template<class T, class X>
    void serialize_helper(jsoncpp::Parser<X>& parser, std::unique_ptr<AbstractClass>& v)
    {
        std::unique_ptr<T> tmp(new T);
        serialize(parser, *tmp);
        v = std::move(tmp);
    }
    
    template<class X>
    void serialize(jsoncpp::Parser<X>& parser, std::unique_ptr<AbstractClass>& v)
    {
        parser.checkType(jsoncpp::Type::Array);
        if (parser.isListEnd(']')) throw std::runtime_error("expected object type");
        parser.nextValue();

        std::string type;
        serialize(parser, type);

        if (parser.isListEnd(']')) throw std::runtime_error("expected object data");
        parser.eatListSeparator();
        parser.nextValue();

        if (type == "int") serialize_helper<IntClass>(parser, v);
        else if (type == "str") serialize_helper<StrClass>(parser, v);
        else throw std::runtime_error("unknown type");

        if (!parser.isListEnd(']')) throw std::runtime_error("expected array end");
    }
}

TEST_CASE("Parsing an Abstract class")
{
    std::vector<std::unique_ptr<AbstractClass>> arr;
    jsoncpp::parse(arr, R"(
        [
            ["str", {"value": "text"}],
            ["int", {"value": 42}],
        ]
    )");

    REQUIRE(arr.size() == 2);
    CHECK(arr[0]->toString() == "text");
    CHECK(arr[1]->toString() == "42");
}
