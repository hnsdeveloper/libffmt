#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <type_traits>
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
            type_const_ptr m_begin_ptr = nullptr; // Pointer to the first character of the string
            type_const_ptr m_end_ptr = nullptr;   // Pointer to the last character of the string (that not null)
            type_const_ptr m_curr_ptr = nullptr;  // Pointer to the current character the

            // Constructs an iterator at the beginning
            UTFStringViewIterator(type_const_ptr str)
            {
                m_begin_ptr = str;
                m_end_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(m_begin_ptr) +
                                                             utfbytelen(m_begin_ptr) - sizeof(CharType));
                if constexpr (reverse)
                    m_curr_ptr = m_end_ptr;
                else
                    m_curr_ptr = m_begin_ptr;
            }

            // Constructs an iterator at the end
            UTFStringViewIterator(type_const_ptr str, bool)
            {
                m_begin_ptr = str;
                m_end_ptr = reinterpret_cast<type_const_ptr>(reinterpret_cast<const byte *>(m_begin_ptr) +
                                                             utfbytelen(m_begin_ptr) - sizeof(CharType));
                if constexpr (reverse)
                    m_curr_ptr = m_begin_ptr;
                else
                    m_curr_ptr = m_end_ptr;
            }

            void move_forward()
            {
                if (m_curr_ptr <= m_end_ptr)
                    get_next_codepoint(&m_curr_ptr);
            }

            void move_backward()
            {
                if (m_curr_ptr >= m_begin_ptr)
                    get_previous_codepoint(&m_curr_ptr);
            }

          public:
            UTFStringViewIterator(const UTFStringViewIterator &other)
            {
                *this = other;
            }

            ~UTFStringViewIterator() = default;

            char32_t operator*() const
            {
                if (m_curr_ptr >= m_begin_ptr && m_curr_ptr <= m_end_ptr)
                    return peek_next_codepoint(m_curr_ptr).get_value();
                return 0;
            }

            UTFStringViewIterator &operator--()
            {
                if constexpr (reverse == true)
                    move_forward();
                else
                    move_backward();
                return *this;
            }

            UTFStringViewIterator operator--(int)
            {
                auto other = *this;
                if constexpr (reverse == true)
                    move_forward();
                else
                    move_backward();
                return other;
            }

            UTFStringViewIterator &operator++()
            {
                // If the condition is true, that means we are at the end of the string
                if constexpr (reverse == false)
                    move_forward();
                else
                    move_backward();
                return *this;
            }

            UTFStringViewIterator operator++(int)
            {
                auto other = *this;
                // If the condition is true, that means we are at the end of the string
                if constexpr (reverse == false)
                    move_forward();
                else
                    move_backward();
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

            bool operator==(const UTFStringViewIterator &other) const
            {
                return m_begin_ptr == other.m_begin_ptr && m_curr_ptr == other.m_curr_ptr &&
                       m_end_ptr == other.m_end_ptr;
            }

            bool operator!=(const UTFStringViewIterator &other) const
            {
                return !(*this == other);
            }

            UTFStringView from_it() const
            {
                if (m_curr_ptr >= m_begin_ptr && m_curr_ptr <= m_end_ptr)
                    return UTFStringView(m_curr_ptr);

                return UTFStringView(reinterpret_cast<CharType *>(nullptr));
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

} // namespace hls

#endif