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
    };

    template <typename CharType>
    Result<size_t> parse_specifier(const UTFStringView<CharType> &str)
    {
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename CharType>
    Result<size_t> parse_argid(const UTFStringView<CharType> &str)
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
            return value(steps);
        }
        return error<size_t>(Error::INVALID_ARGUMENT);
    }

    template <typename CharType>
    Result<size_t> parse_fs(const UTFStringView<CharType> &str)
    {
        if (*str.begin() == OP_FORMAT_CH || *str.begin() == CL_FORMAT_CH)
        {
            for (auto it = str.begin(); it != str.end(); ++it)
            {
                auto cp = *it;
                if (isspace(cp))
                {
                    continue;
                }
                else if (isdigit(*it))
                {
                    auto argid_result = parse_argid(it.from_it());
                    if (!argid_result.is_error())
                        return argid_result;
                    it += argid_result.get_value();
                }
                else if (cp == ':')
                {
                    auto fsparse_result = parse_specifier(it.from_it());
                    if (fsparse_result.is_error())
                        return fsparse_result;
                    it += fsparse_result.get_value();
                }
                else if (cp == *str.begin() || cp == CL_FORMAT_CH)
                {
                    // We have a literal character
                    // TODO: how many codepoints we want to advance
                    return value(size_t(0));
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
    Result<size_t> value_to_sink(const FormatSpecifier &fs, const T &arg);

    template <typename SinkImpl, typename T, typename... Args>
    Result<size_t> format_arg(const FormatSpecifier &fs, size_t argn, StreamSink<SinkImpl> &sink, const T &arg,
                              const Args &...args)
    {
        if (argn != 0)
            return format_arg(fs, argn - 1, sink, args...);

        return value_to_sink(fs, arg);
    }

    // TODO: Handle error values properly
    template <typename CharType, typename SinkImpl, typename... Args>
    Result<size_t> format_string_to_sink(const UTFStringView<CharType> &str, StreamSink<SinkImpl> &sink,
                                         const Args &...args)
    {
        sink.open_sink();
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            auto codepoint = *it;
            if (codepoint != OP_FORMAT_CH || codepoint != CL_FORMAT_CH)
            {
                sink.receive_data(codepoint);
            }
            else
            {
                FormatSpecifier fs;
                auto parse_result = parse_fs(it.from_it());
                if (parse_result.is_error())
                {
                    // TODO: Handle error
                }
            }
        }
        sink.close_sink();
        // TODO: Change return value
        return value(size_t(0));
    }

} // namespace hls

#endif
