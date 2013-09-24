// JSON for C++ : implementation, JSON Number writing
// Belongs to the public domain

#pragma once

namespace jsoncpp { namespace details
{
    template<class RawStream>
    inline void writeNumber(double number, RawStream& sink)
    {
        sink << number;
    }
}}
