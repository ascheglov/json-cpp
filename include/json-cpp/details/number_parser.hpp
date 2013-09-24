// JSON for C++ : implementation, JSON Number parsing
// Belongs to the public domain

#pragma once

#include <cmath>

namespace jsoncpp { namespace details
{
    inline bool isDigit(char c) { return c >= '0' && c <= '9'; }

    template<typename Iterator>
    inline unsigned parseIntNumber(Iterator& iter)
    {
        auto intPart = 0U; // TBD: 0ULL ?

        do
        {
            intPart = intPart * 10 + (*iter - '0');

            ++iter;
        }
        while (isDigit(*iter));

        return intPart;
    }

    template<typename Iterator>
    inline double parseRealNumber(Iterator& iter)
    {
        double number = 0;

        if (*iter == '0')
        {
            ++iter;
        }
        else
        {
            number = parseIntNumber(iter);
        }

        // here `ch` is a peeked character, need to call eat()

        if (*iter == '.')
        {
            ++iter;

            auto mul = 0.1;
            while (isDigit(*iter))
            {
                number += (*iter - '0') * mul;
                mul /= 10;
                ++iter;
            }
        }

        // here `ch` is a peeked character, need to call eat()

        if (*iter == 'e' || *iter == 'E')
        {
            ++iter;

            auto negate = *iter == '-';
            if (negate || *iter == '+')
                ++iter;
            // FIXME: check `ch` for non-digit

            auto e = parseIntNumber(iter);

            if (negate)
                number /= std::pow(10, e);
            else
                number *= std::pow(10, e);
        }

        return number;
    }
}}