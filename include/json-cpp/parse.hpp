// JSON for C++ : main parser class
// Belongs to the public domain

#pragma once

#include <memory>
#include <istream>
#include <iterator>
#include <string>
#include <type_traits>

#include <json-cpp/details/ParserImpl.hpp>
#include <json-cpp/Stream.hpp>
#include <json-cpp/value_types.hpp>

namespace jsoncpp
{
    template<typename CharT, typename InputIterator>
    class Stream<details::ParserTraits<details::Traits2<CharT, InputIterator>>>
    {
    public:
        using this_type = Parser<details::Traits2<CharT, InputIterator>>;

        explicit Stream(InputIterator first, InputIterator last) : m_impl(first, last)
        {
            m_impl.nextValue();
        }

        Type peekType()
        {
            return m_impl.m_type;
        }

        bool firstObjectField()
        {
            m_impl.check(Type::Object);
            return m_impl.nextObjectField(true, m_fieldName);
        }

        const std::string& getFieldName() const { return m_fieldName; }

        bool nextObjectField()
        {
            return m_impl.nextObjectField(false, m_fieldName);
        }

        bool firstArrayItem()
        {
            m_impl.check(Type::Array);
            return m_impl.nextArrayItem(true);
        }

        bool nextArrayItem()
        {
            return m_impl.nextArrayItem(false);
        }

        friend void serialize(this_type& stream, bool& value)
        {
            stream.m_impl.check(Type::Boolean);
            value = stream.m_impl.m_boolean;
        }

        template<typename DstCharT>
        friend void serialize(this_type& stream, std::basic_string<DstCharT>& value)
        {
            stream.m_impl.check(Type::String);
            stream.m_impl.parseString(value);
        }

        template<typename T>
        friend typename std::enable_if<std::is_arithmetic<T>::value>::type
            serialize(this_type& stream, T& value)
        {
            stream.m_impl.check(Type::Number);
            value = static_cast<T>(stream.m_impl.m_number);
            if (value != stream.m_impl.m_number)
                throw stream.m_impl.makeError(ParserError::NumberIsOutOfRange);
        }

        ParserError unknownField() const
        {
            return m_impl.makeError(ParserError::UnknownField);
        }

    public: // utility members
        Stream(const this_type&) = delete;
        Stream(this_type&& rhs) { swap(rhs); }
        void operator=(this_type rhs) { swap(rhs); }

        void swap(this_type& other)
        {
            m_impl.swap(other.m_impl);
            m_fieldName.swap(other.m_fieldName);
        }

    private:
        details::ParserImpl<InputIterator> m_impl;
        std::string m_fieldName;
    };

    template<class X, typename Sink>
    inline void parseObject(Parser<X>& parser, Sink&& sink)
    {
        if (parser.firstObjectField())
        {
            do
            {
                sink(parser.getFieldName());
            }
            while (parser.nextObjectField());
        }
    }

    template<class X, typename Sink>
    void parseArray(Parser<X>& parser, Sink&& sink)
    {
        if (parser.firstArrayItem())
        {
            do
            {
                sink();
            }
            while (parser.nextArrayItem());
        }
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
