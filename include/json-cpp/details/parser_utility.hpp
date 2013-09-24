// JSON for C++ : implementation, parser utility classes
// Belongs to the public domain

#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

#include <json-cpp/ParserError.hpp>

namespace jsoncpp { namespace details
{
    template<typename CharT>
    struct CStrIterator
    {
        using this_type = CStrIterator<CharT>;

        CStrIterator()
        {
            static CharT null{0};
            m_ptr = &null;
        }

        CStrIterator(const CharT* ptr) : m_ptr{ptr} {}

        const CharT& operator*() { return *m_ptr; }
        const CharT* operator->() { return m_ptr; }

        this_type& operator++()
        {
            assert(!isEnd());
            ++m_ptr;
            return *this;
        }

        this_type operator++(int) { auto temp = *this; ++*this; return temp; }

        bool operator==(const this_type& rhs) const { return isEnd() == rhs.isEnd(); }
        bool operator!=(const this_type& rhs) const { return !this->operator==(rhs); }

    private:
        const CharT* m_ptr;

        bool isEnd() const { return *m_ptr == 0; }
    };

    class Diagnostics
    {
    public:
        void nextColumn() { ++m_column; }
        void newLine() { ++m_line; m_column = 0; }

        ParserError makeError(ParserError::Type type) const
        {
            return{type, m_line, m_column};
        }

    private:
        std::size_t m_column{0};
        std::size_t m_line{1};
    };

    template<typename InputIterator>
    struct Reader
    {
        using this_type = Reader<InputIterator>;

        Reader(InputIterator first, InputIterator last) : m_iter(first), m_end(last)
        {
            checkEnd();
        }

        char operator*() { return *m_iter; }
        this_type& operator++()
        {
            checkEnd();
            ++m_iter;
            m_diag.nextColumn();
            return *this;
        }

        void checkEnd()
        {
            if (m_iter == m_end)
                throw m_diag.makeError(ParserError::Eof);
        }

        char getNextChar()
        {
            auto prev = *m_iter;
            ++*this;
            return prev;
        }

        Diagnostics m_diag;
        InputIterator m_iter, m_end;
    };
}}
