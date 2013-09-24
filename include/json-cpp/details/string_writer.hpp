// JSON for C++ : implementation, JSON String writing
// Belongs to the public domain

#pragma once

#include <string>

namespace jsoncpp { namespace details
{
    template<typename SrcCharT, typename Sink>
    inline void writeString(const std::basic_string<SrcCharT>& str, Sink&& sink)
    {
        sink('"');
        for (auto iter = std::begin(str), last = std::end(str); iter != last; ++iter)
        {
            switch (char32_t ch = static_cast<unsigned char>(*iter))
            {
            case '"': sink('\\'); sink('"'); break;
            case '\\': sink('\\'); sink('\\'); break;
            case '\b': sink('\\'); sink('b'); break;
            case '\f': sink('\\'); sink('f'); break;
            case '\n': sink('\\'); sink('n'); break;
            case '\r': sink('\\'); sink('r'); break;
            case '\t': sink('\\'); sink('t'); break;
            default:
                if (ch < '\x20')
                {
                    const auto table = "0123456789ABCDEF";
                    unsigned n = static_cast<unsigned char>(ch);
                    sink('\\');
                    sink('u');
                    sink('0');
                    sink('0');
                    sink(table[n >> 4]);
                    sink(table[n & 15]);
                }
                else
                {
                    sink(static_cast<char>(ch));
                }
            }
        }
        sink('"');
    }
}}
