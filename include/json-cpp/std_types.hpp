// JSON for C++ : standard types
// Belongs to the public domain

#pragma once

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <json-cpp/parse.hpp>
#include <json-cpp/generate.hpp>

namespace jsoncpp
{
    template<class X, typename T>
    inline void serialize(Parser<X>& parser, std::shared_ptr<T>& obj)
    {
        if (parser.peekType() != jsoncpp::Type::Null)
        {
            obj = std::make_shared<T>();
            serialize(parser, *obj);
        }
        else
        {
            obj.reset();
        }
    }

    template<class X, typename T>
    inline void serialize(Generator<X>& generator, std::shared_ptr<T>& obj)
    {
        writePointer(generator, obj);
    }

    template<class X, typename T>
    inline void serialize(Parser<X>& parser, std::unique_ptr<T>& obj)
    {
        if (parser.peekType() != jsoncpp::Type::Null)
        {
            obj->reset(new T());
            serialize(parser, *obj);
        }
        else
        {
            obj.reset();
        }
    }

    template<class X, typename T>
    inline void serialize(Generator<X>& generator, std::unique_ptr<T>& obj)
    {
        writePointer(generator, obj);
    }

    namespace details
    {
        template<class X, typename C>
        inline void serializeContainer(Parser<X>& parser, C& c)
        {
            c.clear();

            parseArray(parser, [&]
            {
                c.emplace_back();
                serialize(parser, c.back());
            });
        }

        template<class X, typename C>
        inline void serializeContainer(Generator<X>& generator, C& c)
        {
            writeRange(generator, c);
        }

        template<class X, typename C>
        inline void serializeSet(Parser<X>& parser, C& c)
        {
            c.clear();

            parseArray(parser, [&]
            {
                typename C::value_type value;
                serialize(parser, value);
                c.insert(value);
            });
        }

        template<class X, typename C>
        inline void serializeSet(Generator<X>& generator, C& c)
        {
            writeRange(generator, c);
        }

        template<class X, typename C>
        inline void serializeStrMap(Parser<X>& parser, C& c)
        {
            c.clear();

            parseObject(parser, [&](const std::string& name)
            {
                serialize(parser, c[name]);
            });
        }

        template<class X, typename C>
        inline void serializeStrMap(Generator<X>& generator, C& c)
        {
            generator.objectBegin();
            
            auto iter = std::begin(c);
            const auto& last = std::end(c);
            if (iter != last)
            {
                for (;;)
                {
                    generator.fieldName(iter->first);
                    serialize(generator, iter->second);

                    ++iter;
                    if (iter == last)
                        break;

                    generator.separator();
                }
            }

            generator.objectEnd();
        }
    }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::vector<T>& arr)
    { details::serializeContainer(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::list<T>& arr)
    { details::serializeContainer(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::forward_list<T>& arr)
    { details::serializeContainer(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::deque<T>& arr)
    { details::serializeContainer(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::set<T>& arr)
    { details::serializeSet(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::unordered_set<T>& arr)
    { details::serializeSet(stream, arr); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::map<std::string, T>& t)
    { details::serializeStrMap(stream, t); }

    template<class X, typename T>
    inline void serialize(Stream<X>& stream, std::unordered_map<std::string, T>& t)
    { details::serializeStrMap(stream, t); }
}