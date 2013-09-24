// JSON for C++ : value types enumeration
// Belongs to the public domain

#pragma once

namespace jsoncpp
{
    // Helper masks
    const auto TypeIsNotFundamental = 0x40;
    const auto TypeIsCollection = 0x80;

    enum class Type
    {
        Undefined = 0, // Helper type for debugging variant-like types
        Null = 0x01,
        Boolean = 0x02,
        Number = 0x04,
        String = 0x08 | TypeIsNotFundamental,
        Array = 0x10 | TypeIsNotFundamental | TypeIsCollection,
        Object = 0x20 | TypeIsNotFundamental | TypeIsCollection,
    };
}