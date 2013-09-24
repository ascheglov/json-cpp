// JSON for C++ : main parser/generator class
// Belongs to the public domain

#pragma once

namespace jsoncpp
{
    template<class Traits>
    class Stream;

    namespace details
    {
        template<typename CharT, class X>
        struct Traits2 {};

        template<class Traits>
        struct ParserTraits {};

        template<class Traits>
        struct GeneratorTraits {};
    }

    template<class X>
    using Parser = Stream<details::ParserTraits<X>>;

    template<class X>
    using Generator = Stream<details::GeneratorTraits<X>>;

    template<typename X, typename T>
    inline auto serialize(Stream<X>& stream, T& value) -> decltype(value.serialize(stream), void())
    {
        value.serialize(stream);
    }
}