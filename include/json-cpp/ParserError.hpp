// JSON for C++ : exception type
// Belongs to the public domain

#pragma once

#include <cassert>
#include <cstddef>
#include <exception>
#include <string>

#if defined _MSC_VER
#   define JSONCPP_INTERNAL_NOEXCEPT_ throw()
#else
#   define JSONCPP_INTERNAL_NOEXCEPT_ noexcept
#endif

namespace jsoncpp
{
    class ParserError : public std::exception
    {
    public:
        enum Type
        {
            NoError,
            Eof, UnexpectedCharacter,
            InvalidEscapeSequence, NoTrailSurrogate,
            UnexpectedType, UnknownField,
            NumberIsOutOfRange,
        };

        ParserError(Type type, std::size_t line, std::size_t column)
            : m_type{type}, m_line{line}, m_column{column}
        {
            assert(type != NoError);
        }

        virtual const char* what() const JSONCPP_INTERNAL_NOEXCEPT_ override
        {
            if (m_what.empty())
            {
                m_what = "JSON parser error at line ";
                m_what += std::to_string(m_line);
                m_what += ", column ";
                m_what += std::to_string(m_column);
                switch (m_type)
                {
                case Eof: m_what += ": unexpected end of file"; break;
                case UnexpectedCharacter: m_what += ": unexpected character"; break;
                case InvalidEscapeSequence: m_what += ": invalid escape sequence"; break;
                case NoTrailSurrogate: m_what += ": no UTF-16 trail surrogate"; break;
                case UnexpectedType: m_what += ": unexpected value type"; break;
                case UnknownField: m_what += ": unknown field name"; break;
                case NumberIsOutOfRange: m_what += ": number is out of range"; break;
                case NoError:
                default:
                    m_what += ": INTERNAL ERROR"; break;
                }
            }

            return m_what.c_str();
        }

        Type type() const { return m_type; }
        std::size_t line() const { return m_line; }
        std::size_t column() const { return m_column; }

    private:
        Type m_type;
        std::size_t m_line;
        std::size_t m_column;

        mutable std::string m_what;
    };
}

#undef JSONCPP_INTERNAL_NOEXCEPT_
