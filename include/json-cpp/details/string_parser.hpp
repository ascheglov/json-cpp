// JSON for C++ : implementation, JSON Number parsing
// Belongs to the public domain

#pragma once

#include <string>

#include <json-cpp/ParserError.hpp>

namespace jsoncpp { namespace details
{
    inline char32_t utf16SurrogatePairToUtf32(char32_t lead, char32_t trail)
    {
        return 0x10000 | (lead - 0xD800) << 10 | (trail - 0xDC00);
    }

    inline void utf32ToUtf8(char32_t c, std::string& str)
    {
        auto add = [&str](char32_t c){ str.push_back(static_cast<char>(c)); };

        if (c < 0x80)
        {
            add(c);
        }
        else if (c < 0x800)
        {
            add(0xC0 | c >> 6);
            add(0x80 | c & 0x3f);
        }
        else if (c < 0x10000)
        {
            add(0xE0 | c >> 12);
            add(0x80 | (c >> 6) & 0x3f);
            add(0x80 | c & 0x3f);
        }
        else if (c < 0x200000)
        {
            add(0xF0 | c >> 18);
            add(0x80 | (c >> 12) & 0x3f);
            add(0x80 | (c >> 6) & 0x3f);
            add(0x80 | c & 0x3f);
        }
        else if (c < 0x4000000)
        {
            add(0xF8 | c >> 24);
            add(0x80 | (c >> 18) & 0x3f);
            add(0x80 | (c >> 12) & 0x3f);
            add(0x80 | (c >> 6) & 0x3f);
            add(0x80 | c & 0x3f);
        }
        else
        {
            add(0xFC | c >> 30);
            add(0x80 | (c >> 24) & 0x3f);
            add(0x80 | (c >> 18) & 0x3f);
            add(0x80 | (c >> 12) & 0x3f);
            add(0x80 | (c >> 6) & 0x3f);
            add(0x80 | c & 0x3f);
        }
    }

    enum class CharType { Raw, CodePoint, UTF16Pair };

    template<typename CharT, std::size_t CharSize>
    inline void addToStr(std::basic_string<CharT>& str, CharType type, char32_t c1, char32_t c2);

    template<>
    inline void addToStr<char, 1>(std::basic_string<char>& str, CharType type, char32_t c1, char32_t c2)
    {
        if (type == CharType::Raw)
        {
            str.push_back(static_cast<char>(c1));
        }
        else if (type == CharType::CodePoint)
        {
            utf32ToUtf8(c1, str);
        }
        else
        {
            auto c32 = utf16SurrogatePairToUtf32(c1, c2);
            utf32ToUtf8(c32, str);
        }
    }

    template<>
    inline void addToStr<wchar_t, 2>(std::basic_string<wchar_t>& str, CharType type, char32_t c1, char32_t c2)
    {
        str.push_back(static_cast<wchar_t>(c1));
        if (type == CharType::UTF16Pair)
            str.push_back(static_cast<wchar_t>(c2));
    }

    template<>
    inline void addToStr<wchar_t, 4>(std::basic_string<wchar_t>& str, CharType type, char32_t c1, char32_t c2)
    {
        auto c = (type == CharType::UTF16Pair) ? utf16SurrogatePairToUtf32(c1, c2) : c1;
        str.push_back(static_cast<wchar_t>(c));
    }

    template<typename Iterator>
    inline int parseHexDigit(Iterator& iter, ParserError::Type& err)
    {
        auto ch = *iter;
        ++iter;
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
        if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
        
        err = ParserError::InvalidEscapeSequence;
        return 0;
    }

    template<typename Iterator>
    inline char32_t parseUTF16CodeUnit(Iterator& iter, ParserError::Type& err)
    {
        auto n = parseHexDigit(iter, err) << 12;
        n |= parseHexDigit(iter, err) << 8;
        n |= parseHexDigit(iter, err) << 4;
        n |= parseHexDigit(iter, err);
        return static_cast<char32_t>(n);
    }

    template<typename Iterator, typename CharT>
    inline ParserError::Type parseStringImpl(Iterator& iter, std::basic_string<CharT>& str)
    {
        str.clear();
        auto add = [&str](CharType type, char32_t c1, char32_t c2)
        {
            addToStr<CharT, sizeof(CharT)>(str, type, c1, c2);
        };

        for (;;)
        {
            auto ch = static_cast<char32_t>(*iter);
            ++iter;
            if (ch == '"')
                return ParserError::NoError;

            if (ch == '\\')
            {
                ch = static_cast<char32_t>(*iter);
                ++iter;
                switch (ch)
                {
                case '\\': case '"': case '/':
                    break;

                case 'b': ch = '\b'; break;
                case 'f': ch = '\f'; break;
                case 'n': ch = '\n'; break;
                case 'r': ch = '\r'; break;
                case 't': ch = '\t'; break;

                case 'u':
                    {
                        ParserError::Type err{ParserError::NoError};
                        auto codeUnit = parseUTF16CodeUnit(iter, err);
                        if (err != ParserError::NoError)
                            return err;

                        if (codeUnit >= 0xD800 && codeUnit < 0xDC00)
                        {
                            if (*iter != '\\') return ParserError::NoTrailSurrogate;
                            ++iter;
                            if (*iter != 'u') return ParserError::NoTrailSurrogate;
                            ++iter;

                            auto trailSurrogate = parseUTF16CodeUnit(iter, err);
                            if (err != ParserError::NoError)
                                return err;


                            add(CharType::UTF16Pair, codeUnit, trailSurrogate);
                        }
                        else
                        {
                            add(CharType::CodePoint, codeUnit, 0);
                        }
                    }
                    continue;

                default:
                    return ParserError::InvalidEscapeSequence;
                }
            }

            add(CharType::Raw, ch, 0);
        }
    }
}}