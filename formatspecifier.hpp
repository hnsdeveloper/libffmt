#ifndef _FORMATSPECIFIER_HPP_
#define _FORMATSPECIFIER_HPP_

#ifdef __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
#include <utility>
#endif

#include <iostream>

#include "utfstringview.hpp"
#include "constants.hpp"
#include "result.hpp"

namespace hls
{
    class FormatSpecifier
    {
      public:
        enum FormatType : uint8_t
        {
            LITERAL,  // For when what we get is a literal character instead of a format specifier
            SPECIFIER // For when we get a format specifier
        };

      private:
        FormatType m_format;

        bool m_has_argid = false;
        size_t m_argid;

        bool m_has_align = false;
        char32_t m_align = '<';

        bool m_has_fill = false;
        char32_t m_fill;

        bool m_has_sign = true;
        char32_t m_sign = NEG_SIGN;

        char32_t m_integer_type;

      public:
        static constexpr size_t INVALID_ARGID = size_t(0) - 1;
        // A format specifier is considered a literal until set otherwise
        FormatSpecifier() : m_format(FormatType::LITERAL), m_argid(INVALID_ARGID) {};

        void set_format_type(FormatType type)
        {
            m_format = type;
        }
        FormatType get_format_type() const
        {
            return m_format;
        }

        void set_argid(size_t id)
        {
            m_argid = id;
            m_has_argid = true;
            m_format = FormatType::SPECIFIER;
        }

        bool has_argid() const
        {
            return m_has_argid;
        }

        size_t get_argid() const
        {
            return m_argid;
        }

        void set_align(char align)
        {
            if (align == '<' || align == '>' || align == '^')
            {
                m_has_align = true;
                m_align = align;
            }
        }

        char32_t get_align() const
        {
            return m_align;
        }

        void set_fill(char32_t codepoint)
        {
            if (codepoint != OP_FORMAT_CH || codepoint != CL_FORMAT_CH)
            {
                m_fill = codepoint;
                m_has_fill = true;
            }
        }

        bool has_fill() const
        {
            return m_has_fill;
        }

        char32_t get_fill() const
        {
            return m_fill;
        }

        void set_sign(char32_t codepoint)
        {
            if (codepoint == '+' || codepoint == '-' || codepoint == ' ')
            {
                m_has_sign = true;
                m_sign = codepoint;
            }
        }

        bool has_sign() const
        {
            return m_has_sign;
        }

        char32_t get_sign() const
        {
            return m_sign;
        }

        void set_integer_display_type(char32_t codepoint)
        {
            m_integer_type = codepoint;
            switch (codepoint)
            {
                case OCT_FORMAT:
                    set_sign(' ');
                    break;
                case DEC_FORMAT:
                    set_sign('-');
                    break;
                case HEX_FORMAT:
                    set_sign(' ');
                    break;
            }
        }

        char32_t get_integer_display_type() const
        {
            return m_integer_type;
        }
    };

    template <typename CharType>
    Result<size_t> parse_specifier(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            auto codepoint = *it;
            switch (codepoint)
            {
                case OCT_FORMAT:
                case DEC_FORMAT:
                case HEX_FORMAT:
                    fs.set_integer_display_type(codepoint);
                    break;
                default:
                    return error<size_t>(Error::INVALID_ARGUMENT);
            }
            break;
        }
        return value(size_t(1));
    }

    template <typename CharType>
    Result<size_t> parse_fs(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        if (*str.begin() == OP_FORMAT_CH || *str.begin() == CL_FORMAT_CH)
        {
            for (auto it = str.begin() + 1; it != str.end(); ++it)
            {
                auto codepoint = *it;
                if (isdigit(*it))
                {
                    auto argid_result = parse_argid(it.from_it(), fs);
                    it += argid_result.get_value();
                }
                else if (codepoint == ':')
                {
                    // We want to parse from the next character, not from : itself
                    auto fsparse_result = parse_specifier((it + 1).from_it(), fs);
                    if (fsparse_result.is_error())
                        return fsparse_result;
                    it += fsparse_result.get_value();
                }
                else if (codepoint == *str.begin() || codepoint == CL_FORMAT_CH)
                {
                    if (*str.begin() == OP_FORMAT_CH && codepoint == CL_FORMAT_CH)
                        fs.set_format_type(FormatSpecifier::FormatType::SPECIFIER);
                    // We have a literal character
                    return value(it - str.begin());
                }
                else
                {
                    break;
                }
            }
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }
} // namespace hls

#endif