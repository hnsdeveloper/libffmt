#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <type_traits>
#include <iostream>
#include "utfstring.hpp"

namespace hls
{
    template <typename CharType>
    class UTFStringView
    {
        SET_USING_CLASS(CharType, type);
        type_const_ptr m_str = nullptr;
        size_t m_string_size = 0;

        template <bool reverse>
        class UTFStringViewIterator
        {
            SET_USING_CLASS(CharType, type);
            type_const_ptr m_begin_ptr = nullptr; // Pointer to the first character of the string
            type_const_ptr m_end_ptr = nullptr;   // Pointer to the last character of the string (that not null)
            type_const_ptr m_curr_ptr = nullptr;  // Pointer to the current character the

            // Constructs an iterator at the beginning
            UTFStringViewIterator(type_const_ptr str)
            {
                if constexpr (reverse)
                {
                    // Begin at the last character before the null character
                    m_begin_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(str) +
                                                                   utfbytelen(str) - sizeof(CharType));
                    m_end_ptr = str - 1;
                }
                else
                {
                    m_begin_ptr = str;
                    // We willfuly ignore the null character. For all encodings, it is just one code unit.
                    m_end_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(m_begin_ptr) +
                                                                 utfbytelen(m_begin_ptr) - sizeof(CharType));
                }
                m_curr_ptr = m_begin_ptr;
            }

            // Constructs an iterator at the end
            UTFStringViewIterator(type_const_ptr str, bool)
            {
                if constexpr (reverse)
                {
                    // Begin at the last character before the null character
                    m_begin_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(str) +
                                                                   utfbytelen(str) - sizeof(CharType));
                    m_end_ptr = str - 1;
                }
                else
                {
                    m_begin_ptr = str;
                    // We willfuly ignore the null character. For all encodings, it is just one code unit.
                    m_end_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(m_begin_ptr) +
                                                                 utfbytelen(m_begin_ptr) - sizeof(CharType));
                }
                m_curr_ptr = m_end_ptr;
            }

          public:
            UTFStringViewIterator(const UTFStringViewIterator &other)
            {
                *this = other;
            }

            ~UTFStringViewIterator() = default;

            char32_t operator*() const
            {
                if (m_curr_ptr != m_end_ptr)
                    return peek_next_codepoint(m_curr_ptr);
                return 0;
            }

            UTFStringViewIterator &operator--()
            {
                // TODO: implement
                return *this;
            }

            UTFStringViewIterator operator--(int)
            {
                // TODO: implement
                return *this;
            }

            UTFStringViewIterator &operator++()
            {
                if constexpr (reverse)
                {
                    if (m_curr_ptr > m_end_ptr)
                        get_previous_codepoint(&m_curr_ptr);
                }
                else
                {
                    if (m_curr_ptr < m_end_ptr)
                        get_next_codepoint(&m_curr_ptr);
                }

                return *this;
            }

            UTFStringViewIterator operator++(int)
            {
                auto other = *this;
                // If the condition is true, that means we are at the end of the string
                ++(*this);
                return other;
            }

            UTFStringViewIterator &operator=(const UTFStringViewIterator &other)
            {
                if (this != &other)
                {
                    m_begin_ptr = other.m_begin_ptr;
                    m_end_ptr = other.m_end_ptr;
                    m_curr_ptr = other.m_curr_ptr;
                }
                return *this;
            }

            UTFStringView from_it() const
            {
                // TODO: implement when it is a reverse iterator to get a string up to that point
                if (m_curr_ptr != m_end_ptr)
                    return UTFStringView(m_curr_ptr);

                return UTFStringView(static_cast<const CharType *>(nullptr));
            }

            UTFStringViewIterator operator+(size_t n)
            {
                auto other = *this;
                while (n > 0)
                {
                    --n;
                    ++other;
                }
                return other;
            }

            UTFStringViewIterator &operator+=(size_t n)
            {
                while (n > 0)
                {
                    ++(*this);
                    --n;
                }

                return *this;
            }

            // TODO: IMPLEMENT
            // UTFStringViewIterator operator-(size_t n)
            //{
            //    auto other = *this;
            //    while (n > 0)
            //    {
            //        --n;
            //        ++other;
            //    }
            //    return other;
            //}
            //
            // UTFStringViewIterator &operator-=(size_t n)
            //{
            //    while (n > 0)
            //    {
            //        ++(*this);
            //        --n;
            //    }
            //    return *this;
            //}

            // This is quite wrong, given that a codepoint might take multiple codeunits
            size_t operator-(const UTFStringViewIterator &other)
            {
                if (m_begin_ptr == other.m_begin_ptr)
                {
                    auto calc_diff = [](auto a, auto b) {
                        size_t i = 0;
                        while (a != b)
                        {
                            get_next_codepoint(&a);
                            ++i;
                        }
                        return i;
                    };
                    if (m_curr_ptr > other.m_curr_ptr)
                        return calc_diff(other.m_curr_ptr, m_curr_ptr);
                    return calc_diff(m_curr_ptr, other.m_curr_ptr);
                }
                return 0;
            }

            bool operator==(const UTFStringViewIterator &other) const
            {
                return m_begin_ptr == other.m_begin_ptr && m_curr_ptr == other.m_curr_ptr &&
                       m_end_ptr == other.m_end_ptr;
            }

            bool operator!=(const UTFStringViewIterator &other) const
            {
                return !(*this == other);
            }

            friend class UTFStringView;
        };

        SET_USING_CLASS(UTFStringViewIterator<false>, iterator);
        SET_USING_CLASS(UTFStringViewIterator<true>, reverse_iterator);

      public:
        UTFStringView(type_const_ptr str)
        {
            if (str && is_valid_utf_sequence(str))
            {
                m_str = str;
                m_string_size = utfstrlen(str);
            }
        }

        UTFStringView(iterator begin, iterator end)
        {
        }

        iterator begin() const
        {
            return UTFStringViewIterator<false>(m_str);
        }

        iterator end() const
        {
            return UTFStringViewIterator<false>(m_str, true);
        }

        reverse_iterator rbegin() const
        {
            return UTFStringViewIterator<true>(m_str);
        }

        reverse_iterator rend() const
        {
            return UTFStringViewIterator<true>(m_str, true);
        }

        size_t length() const
        {
            return m_string_size;
        }

        bool is_valid_view() const
        {
            return m_str != nullptr;
        }
    };

    template <typename CharType>
    UTFStringView(CharType) -> UTFStringView<std::remove_cvref_t<std::remove_pointer_t<CharType>>>;

    template <typename IteratorType>
    UTFStringView(IteratorType, IteratorType) -> UTFStringView<typename IteratorType::type>;

} // namespace hls

#endif