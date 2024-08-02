#include "utf.hpp"

namespace hls
{
    template <typename CharType>
    class UTFStringView
    {
        SET_USING_CLASS(byte, type);
        type_const_ptr m_str;

        class UTFStringViewIterator
        {
            type_const_ptr m_str;
            size_t m_string_size;
            size_t m_ptr_offset;

            UTFStringViewIterator(type_const_ptr str) : m_str(reinterpret_cast<type_const_ptr>(str)), m_ptr_offset(0)
            {
            }
            ~UTFStringViewIterator() = default;

          public:
            CharType operator*()
            {
                return peek_next_codepoint(reinterpret_cast<const CharType*>(m_str + m_ptr_offset)).get_value();
            }

            UTFStringViewIterator &operator--()
            {
            }

            UTFStringViewIterator operator--(int)
            {
            }

            UTFStringViewIterator &operator++()
            {
            }

            UTFStringViewIterator operator++(int)
            {
            }

            UTFStringViewIterator &operator=(const UTFStringViewIterator &other)
            {
            }
        };

        SET_USING_CLASS(UTFStringViewIterator, iterator);

      public:
        UTFStringView(type_const_ptr str) : m_str(str)
        {
        }

        iterator begin();
        iterator end();
    };
} // namespace hls