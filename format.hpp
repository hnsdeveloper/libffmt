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
#include "formatspecifier.hpp"
#include "constants.hpp"

#include <iostream>

namespace hls
{

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
    Result<size_t> parse_argid(const UTFStringView<CharType> &str, FormatSpecifier &fs)
    {
        uint64_t number = 0;
        auto it = str.begin();
        size_t digits = 0;
        if (isdigit(*it))
        {
            while (isdigit(*it))
            {
                number = number * 10 + (*it - '0');
                ++it;
                ++digits;
            }
            fs.set_argid(number);
            // We need to return on the last digit character so the outer loop finds whathever it needs to find
            return value(digits - 1);
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
                if (isdigit(*it))
                {
                    auto argid_result = parse_argid(it.from_it(), fs);
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
                    break;
                }
            }
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename T, typename SinkImpl>
    Result<size_t> value_to_sink(const FormatSpecifier &fs, const T &arg, StreamSink<SinkImpl> &sink)
    {
    }

    template <typename SinkImpl, typename T, typename... Args>
    Result<size_t> format_arg(const FormatSpecifier &fs, size_t argn, StreamSink<SinkImpl> &sink, const T &arg,
                              const Args &...args)
    {
        if constexpr (sizeof...(Args) != 0)
            if (argn != 0)
                return format_arg(fs, argn - 1, sink, args...);

        return value_to_sink(fs, arg, sink);
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
                    if (fs.has_argid() && fs.get_argid())
                        format_arg(fs, fs.get_argid() % sizeof...(args), sink, args...);
                    else
                        format_arg(fs, curr_arg++, sink, args...);
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
