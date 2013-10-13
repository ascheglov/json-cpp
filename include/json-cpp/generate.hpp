// JSON for C++ : main generator class
// Belongs to the public domain

#pragma once

#include <sstream>
#include <string>

#include <json-cpp/Stream.hpp>
#include <json-cpp/details/string_writer.hpp>

namespace jsoncpp
{
    template<typename CharT, typename Sink>
    class Stream<details::GeneratorTraits<details::Traits2<CharT, Sink>>>
    {
    public:
        using this_type = Generator<details::Traits2<CharT, Sink>>;

        explicit Stream(Sink& sink) : m_sink(&sink) {}

        void objectBegin()
        {
            (*m_sink) << "{";
        }

        void fieldName(const char* name)
        {
            (*m_sink) << '"' << name << "\": ";
            // TODO: use writeString (?)
        }

        template<typename StrCharT>
        void fieldName(const std::basic_string<StrCharT>& name)
        {
            (*m_sink) << '"' << name << "\": ";
            // TODO: use writeString (?)
        }

        void separator()
        {
            (*m_sink) << ", ";
        }

        void objectEnd()
        {
            (*m_sink) << '}';
        }

        void arrayBegin()
        {
            (*m_sink) << '[';
        }

        void arrayEnd()
        {
            (*m_sink) << ']';
        }

        friend void serialize(this_type& stream, std::nullptr_t)
        {
            (*stream.m_sink) << "null";
        }

        friend void serialize(this_type& stream, bool value)
        {
            (*stream.m_sink) << (value ? "true" : "false");
        }

        template<typename T>
        friend typename std::enable_if<std::is_arithmetic<T>::value>::type serialize(this_type& stream, T& value)
        {
            (*stream.m_sink) << value;
        }

        template<typename SrcCharT>
        friend void serialize(this_type& stream, const std::basic_string<SrcCharT>& value)
        {
            details::writeString(value, [&stream](char c){ stream.m_sink->put(c); });
        }

    private:
        Sink* m_sink;
    };

    template<class X, typename Pointer>
    inline void writePointer(Generator<X>& generator, Pointer& ptr)
    {
        if (ptr)
        {
            serialize(generator, *ptr);
        }
        else
        {
            serialize(generator, nullptr);
        }
    }

    template<class X, typename Range>
    inline void writeRange(Generator<X>& generator, Range& range)
    {
        generator.arrayBegin();

        auto iter = std::begin(range);
        const auto& last = std::end(range);
        if (iter != last)
        {
            for (;;)
            {
                serialize(generator, *iter);

                ++iter;
                if (iter == last)
                    break;

                generator.separator();
            }
        }

        generator.arrayEnd();
    }

    template<class T>
    inline std::string to_string(const T& object)
    {
        std::ostringstream rawStream;
        Generator<details::Traits2<char, std::ostream>> stream{rawStream};
        serialize(stream, const_cast<T&>(object));
        return rawStream.str();
    }
}
