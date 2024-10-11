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
#include "formatter.hpp"
#include "constants.hpp"

#include <iostream>

namespace hls
{
    template <typename T, typename SinkImpl>
    Result<size_t> value_to_sink(const FormatSpecifier &fs, const T &arg, StreamSink<SinkImpl> &sink)
    {
        Formatter<std::remove_cvref_t<T>>::value_to_sink(arg, sink, fs);
        return value(size_t(0));
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
