// JSON for C++ : implementation, parser
// Belongs to the public domain

#pragma once

#include <string>

#include <json-cpp/ParserError.hpp>
#include <json-cpp/value_types.hpp>
#include <json-cpp/details/parser_utility.hpp>
#include <json-cpp/details/number_parser.hpp>
#include <json-cpp/details/string_parser.hpp>

namespace jsoncpp { namespace details
{
    template<typename InputIterator>
    class ParserImpl
    {
    public:
        ParserImpl(InputIterator first, InputIterator last) : m_reader(first, last) {}

        Type m_type;
        double m_number;
        bool m_boolean;

        void nextValue()
        {
            eatWhitespace();
            m_type = nextValueImpl();
        }

        void check(Type type)
        {
            if (m_type != type)
                throw makeError(ParserError::UnexpectedType);
        }

        bool nextObjectField(bool first, std::string& name)
        {
            eatWhitespace();
            if (*m_reader == '}')
            {
                ++m_reader;
                return false;
            }

            if (!first)
            {
                check(',');
                eatWhitespace();
            }

            check('"');
            parseString(name);

            eatWhitespace();
            check(':');

            nextValue();
            return true;
        }

        bool nextArrayItem(bool first)
        {
            eatWhitespace();
            if (*m_reader == ']')
            {
                ++m_reader;
                return false;
            }

            if (!first)
            {
                check(',');
                eatWhitespace();
            }

            m_type = nextValueImpl();
            return true;
        }

        template<typename CharT>
        void parseString(std::basic_string<CharT>& str)
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
            case 't': ++m_reader; expect("true"); m_boolean = true; return Type::Boolean;
            case 'f': ++m_reader; expect("false"); m_boolean = false; return Type::Boolean;
            case 'n': ++m_reader; expect("null"); return Type::Null;
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
            return m_reader.m_diag.makeError(ParserError::UnexpectedCharacter);
        }

        void check(char expectedChar)
        {
            if (*m_reader != expectedChar)
                throw unexpectedCharacter();

            ++m_reader;
        }

        template<std::size_t N>
        void expect(const char (&s)[N])
        {
            static_assert(N > 2, "");
            for (auto i = 1; i != N - 1; ++i, ++m_reader)
                if (*m_reader != s[i])
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

        Reader<InputIterator> m_reader;
    };
}}