// JSON for C++ : helpers for serialization
// Belongs to the public domain

#pragma once

#include <array>
#include <unordered_map>

#include <json-cpp/parse.hpp>
#include <json-cpp/generate.hpp>

namespace jsoncpp
{
    namespace details
    {
        template<class X, typename T>
        inline void writeField(Generator<X>& generator, const char* name, T& value)
        {
            generator.fieldName(name);
            serialize(generator, value);
        }

        template<class X, typename T, typename... F>
        inline void writeField(Generator<X>& generator, const char* name, T& value, F&&... fieldsDef)
        {
            writeField(generator, name, value);
            generator.separator();
            writeField(generator, fieldsDef...);
        }

        template<typename ParserT>
        class FieldsTable
        {
        public:
            template<typename... F>
            FieldsTable(F&&... fieldsDef)
            {
                m_map.reserve(sizeof...(fieldsDef) / 2);
                add(1, fieldsDef...);
            }

            struct FieldInfo
            {
                template<typename T>
                FieldInfo(T&, std::size_t idx)
                {
                    m_fieldIdx = idx;
                    m_parseFn = [](ParserT& parser, void* fieldPtr)
                    {
                        serialize(parser, static_cast<T&>(*reinterpret_cast<T*>(fieldPtr)));
                    };
                }

                std::size_t m_fieldIdx;
                void(*m_parseFn)(ParserT& parser, void* fieldPtr);
            };

            const FieldInfo* find(const std::string& name) const
            {
                auto it = m_map.find(name);
                return it == m_map.end() ? nullptr : &it->second;
            }

        private:
            template<typename T, typename... F>
            void add(std::size_t idx, const char* name, T& value, F&&... otherFields)
            {
                m_map.emplace(name, FieldInfo(value, idx));
                add(idx + 2, otherFields...);
            }

            void add(std::size_t /*idx*/) {}

            std::unordered_map<std::string, FieldInfo> m_map;
        };

        inline void* makePtrs(const char*) { return nullptr; }

        template<typename T>
        inline void* makePtrs(T& obj) { return &obj; }
    }

    template<class Cls, class X, typename... F>
    inline void fields(Cls&, Parser<X>& parser, F&&... fieldsDef)
    {
        std::array<void*, sizeof...(fieldsDef)> ptrs{details::makePtrs(fieldsDef)...};

        static const details::FieldsTable<Parser<X>> table{fieldsDef...};
        
        auto&& handler = [&](const std::string& fieldName)
        {
            auto fieldInfo = table.find(fieldName);
            if (fieldInfo == nullptr)
                throw parser.makeError(ParserError::UnknownField);

            auto fieldPtr = ptrs[fieldInfo->m_fieldIdx];
            fieldInfo->m_parseFn(parser, fieldPtr);
        };

        parseObject(parser, handler);
    }

    template<class Cls, class X, typename... F>
    inline void fields(Cls&, Generator<X>& generator, F&&... fieldsDef)
    {
        generator.objectBegin();
        details::writeField(generator, fieldsDef...);
        generator.objectEnd();
    }
}
