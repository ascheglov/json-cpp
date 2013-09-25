// JSON for C++ : main parser class
// Belongs to the public domain

#pragma once

#include <memory>
#include <istream>
#include <iterator>
#include <string>
#include <type_traits>

#include <json-cpp/ParserError.hpp>
#include <json-cpp/Stream.hpp>
#include <json-cpp/value_types.hpp>
#include <json-cpp/details/parser_utility.hpp>
#include <json-cpp/details/number_parser.hpp>
#include <json-cpp/details/string_parser.hpp>

namespace jsoncpp
{
    template<typename CharT, typename InputIterator>
    class Stream<details::ParserTraits<details::Traits2<CharT, InputIterator>>>
    {
    public:
        using this_type = Parser<details::Traits2<CharT, InputIterator>>;

        explicit Stream(InputIterator first, InputIterator last)
            : m_reader{first, last}
        {
            nextValue();
        }

        Type getType() const { return m_type; }
        bool getBoolean() const { return m_boolean; }
        double getNumber() const { return m_number; }
        const std::string& getFieldName() const { return m_fieldName; }

        void checkType(Type type) const
        {
            if (getType() != type)
                throw makeError(ParserError::UnexpectedType);
        }

        bool isListEnd(char terminator)
        {
            eatWhitespace();
            if (*m_reader != terminator)
                return false;

            ++m_reader;
            return true;
        }

        void eatListSeparator()
        {
            eatWhitespace();
            check(',');
            eatWhitespace();
        }

        void nextNameValuePair()
        {
            eatWhitespace();
            check('"');
            parseString(m_fieldName);
            eatWhitespace();
            check(':');
            nextValue();
        }

        void nextValue()
        {
            eatWhitespace();
            m_type = nextValueImpl();
        }

        template<typename DstCharT>
        void parseString(std::basic_string<DstCharT>& str)
        {
            auto err = parseStringImpl(m_reader, str);
            if (err != ParserError::NoError)
                throw m_reader.m_diag.makeError(err);
        }

        ParserError makeError(ParserError::Type type) const
        {
            return m_reader.m_diag.makeError(type);
        }

    private:
        Type nextValueImpl()
        {
            switch (*m_reader)
            {
            case '{': ++m_reader; return Type::Object;
            case '[': ++m_reader; return Type::Array;
            case 't': ++m_reader; checkLiteral("true"); m_boolean = true; return Type::Boolean;
            case 'f': ++m_reader; checkLiteral("false"); m_boolean = false; return Type::Boolean;
            case 'n': ++m_reader; checkLiteral("null"); return Type::Null;
            case '"': ++m_reader; return Type::String;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                m_number = parseRealNumber(m_reader);
                return Type::Number;

            case '-':
                ++m_reader;
                m_number = -parseRealNumber(m_reader);
                return Type::Number;
            }

            throw unexpectedCharacter();
        }

        ParserError unexpectedCharacter() const
        {
            return makeError(ParserError::UnexpectedCharacter);
        }

        void check(char expectedChar)
        {
            if (*m_reader != expectedChar)
                throw unexpectedCharacter();

            ++m_reader;
        }

        template<std::size_t N>
        void checkLiteral(const char(&literal)[N])
        {
            static_assert(N > 2, "");
            for (auto i = 1; i != N - 1; ++i, ++m_reader)
                if (*m_reader != literal[i])
                    throw unexpectedCharacter();
        }

        void eatWhitespace()
        {
            for (;; ++m_reader)
            {
                switch (*m_reader)
                {
                case '\n':
                    m_reader.m_diag.newLine();
                    break;

                case ' ': case '\t': case '\r':
                    break;

                default:
                    return;
                }
            }
        }

        details::Reader<InputIterator> m_reader;

        Type m_type;
        double m_number;
        bool m_boolean;
        std::string m_fieldName;
    };

    template<class X>
    inline void serialize(Parser<X>& parser, bool& value)
    {
        parser.checkType(Type::Boolean);
        value = parser.getBoolean();
    }

    template<class X, typename T>
    inline typename std::enable_if<std::is_arithmetic<T>::value>::type
        serialize(Parser<X>& parser, T& value)
    {
        parser.checkType(Type::Number);
        auto number = parser.getNumber();
        value = static_cast<T>(number);
        if (value != number)
            throw parser.makeError(ParserError::NumberIsOutOfRange);
    }

    template<class X, typename DstCharT>
    inline void serialize(Parser<X>& parser, std::basic_string<DstCharT>& value)
    {
        parser.checkType(Type::String);
        parser.parseString(value);
    }

    namespace details
    {
        template<class X, typename Callback>
        inline void parseList(Parser<X>& parser, Type type, char terminator, Callback&& callback)
        {
            parser.checkType(type);

            while (!parser.isListEnd(terminator))
            {
                callback();

                if (parser.isListEnd(terminator))
                    return;

                parser.eatListSeparator();
            }
        }
    }

    template<class X, typename Callback>
    inline void parseObject(Parser<X>& parser, Callback&& callback)
    {
        details::parseList(parser, Type::Object, '}', [&]
        {
            parser.nextNameValuePair();
            callback(parser.getFieldName());
        });
    }

    template<class X, typename Callback>
    void parseArray(Parser<X>& parser, Callback&& callback)
    {
        details::parseList(parser, Type::Array, ']', [&]
        {
            parser.nextValue();
            callback();
        });
    }

    template<typename CharT, class T, typename InputIterator>
    inline void parse(T& object, InputIterator first, InputIterator last)
    {
        Parser<details::Traits2<CharT, InputIterator>> stream{first, last};
        serialize(stream, object);
    }

    template<typename T, typename CharT>
    inline void parse(T& object, const CharT* str)
    {
        details::CStrIterator<CharT> first{str}, last;
        parse<CharT>(object, first, last);
    }

    template<typename T, typename CharT>
    inline void parse(T& object, std::basic_string<CharT>& str)
    {
        parse<CharT>(object, std::begin(str), std::end(str));
    }

    template<typename T, typename CharT>
    inline void parse(T& object, std::basic_istream<CharT>& stream)
    {
        std::istreambuf_iterator<CharT> first{stream}, last;
        parse<CharT>(object, first, last);
    }
}
