#ifndef _FORMAT_HPP_
#define _FORMAT_HPP_

#ifdef __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
#include <utility>
#endif

#include "result.hpp"
#include "utfstring.hpp"
#include "stream.hpp"
#include "findcreativename.hpp"
#include "utfstringview.hpp"

#include <iostream>

namespace hls
{

    constexpr char32_t OP_FORMAT_CH = '{';
    constexpr char32_t CL_FORMAT_CH = '}';

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
        size_t m_argid;
        bool m_has_argid = false;
        char32_t m_align = '<';
        bool m_has_fill = false;
        char32_t m_fill;

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
                m_align = align;
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
        }
    };

    template <typename CharType>
    Result<size_t> parse_specifier(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            auto codepoint = *it;
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename CharType>
    Result<size_t> parse_argid(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        uint64_t number = 0;
        auto it = str.begin();
        size_t steps = 0;
        if (isdigit(*it))
        {
            while (isdigit(*it))
            {
                number = number * 10 + (*it - '0');
                ++it;
                ++steps;
            }
            fs.set_argid(number);
            return value(steps);
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename CharType>
    Result<size_t> parse_fs(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        if (*str.begin() == OP_FORMAT_CH || *str.begin() == CL_FORMAT_CH)
        {
            for (auto it = str.begin() + 1; it != str.end(); ++it)
            {
                auto cp = *it;
                if (isspace(cp))
                {
                    continue;
                }
                else if (isdigit(*it))
                {
                    auto argid_result = parse_argid(it.from_it(), fs);
                    if (!argid_result.is_error())
                        return argid_result;
                    it += argid_result.get_value();
                }
                else if (cp == ':')
                {
                    auto fsparse_result = parse_specifier(it.from_it(), fs);
                    if (fsparse_result.is_error())
                        return fsparse_result;
                    it += fsparse_result.get_value();
                }
                else if (cp == *str.begin() || cp == CL_FORMAT_CH)
                {
                    if (*str.begin() == OP_FORMAT_CH && cp == CL_FORMAT_CH)
                        fs.set_format_type(FormatSpecifier::FormatType::SPECIFIER);
                    // We have a literal character
                    return value(it - str.begin());
                }
                else
                {
                    // We have an error
                    break;
                }
            }
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename T>
    Result<size_t> value_to_sink(const FormatSpecifier &fs, const T &arg)
    {
        std::cout << arg;
        return value(size_t(0));
    }

    template <typename SinkImpl, typename T, typename... Args>
    Result<size_t> format_arg(const FormatSpecifier &fs, size_t argn, StreamSink<SinkImpl> &sink, const T &arg,
                              const Args &...args)
    {
        if constexpr (sizeof...(Args) != 0)
            if (argn != 0)
                return format_arg(fs, argn - 1, sink, args...);

        return value_to_sink(fs, arg);
    }

    // TODO: Handle error values properly
    template <typename CharType, typename SinkImpl, typename... Args>
    Result<size_t> format_string_to_sink(const UTFStringView<CharType> &str, StreamSink<SinkImpl> &sink,
                                         const Args &...args)
    {
        size_t curr_arg = 0;

        sink.open_sink();
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            auto codepoint = *it;
            if (codepoint == OP_FORMAT_CH || codepoint == CL_FORMAT_CH)
            {
                FormatSpecifier fs;
                auto parse_result = parse_fs(it.from_it(), fs);
                if (parse_result.is_error())
                {
                    format_string_to_sink(UTFStringView(u8"INVALID FORMAT SPECIFIER"), sink);
                    return error<size_t>(Error::UNDEFINED_ERROR);
                }
                if (fs.get_format_type() == FormatSpecifier::FormatType::LITERAL)
                {
                    sink.receive_data(codepoint);
                }
                else if constexpr (sizeof...(Args) != 0)
                {
                    if (fs.get_argid() != FormatSpecifier::INVALID_ARGID)
                        format_arg(fs, fs.get_argid(), sink, args...);
                    else
                        format_arg(fs, curr_arg, sink, args...);
                }
                it += parse_result.get_value();
            }
            else
            {
                sink.receive_data(codepoint);
            }
        }
        sink.close_sink();
        // TODO: Change return value
        return value(size_t(0));
    }

} // namespace hls

#endif
