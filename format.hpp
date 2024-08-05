#ifndef _FORMAT_HPP_
#define _FORMAT_HPP_

#ifdef __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
#include <utility>
#endif

#include "result.hpp"
#include "utf.hpp"
#include "stream.hpp"
#include "string.hpp"

namespace hls
{

    constexpr char32_t OP_FORMAT_CH = '{';
    constexpr char32_t CL_FORMAT_CH = '}';

    bool isspace(char32_t codepoint)
    {
        // TODO: extend to tab character and other spacing characters
        return codepoint == 0x20;
    }

    bool isdigit(char32_t codepoint)
    {
        return codepoint >= '0' && codepoint <= '9';
    }

    template <typename CharType>
    Result<uint64_t> atou(const UTFStringView<CharType> &str_view)
    {
        if (!str_view.is_valid_view())
            return error<uint64_t>(Error::INVALID_ARGUMENT);

        uint64_t nmb = 0;
        bool digit_started = false;
        for (auto c : str_view)
        {
            if (!digit_started)
            {
                if ((digit_started = isdigit(c)))
                    nmb = c - '0';
                else
                    return error<uint64_t>(Error::INVALID_ARGUMENT);
            }
            else
            {
                if (isdigit(c))
                    nmb = nmb * 10 + (c - '0');
                else
                    break;
            }
        }
        return value(nmb);
    }

    template <typename CharType>
    Result<int64_t> atoi(const UTFStringView<CharType> &str_view)
    {
        if (!str_view.is_valid_view())
            return error<int64_t>(Error::INVALID_ARGUMENT);

        int64_t nmb = 0;
        int64_t sign = 1;
        bool digit_started = false;
        for (auto c : str_view)
        {
            if (!digit_started)
            {
                if (c == '-')
                    sign = sign * -1;
                else if ((digit_started = isdigit(c)))
                    nmb = c - '0';
                else
                    return error<int64_t>(Error::INVALID_ARGUMENT);
            }
            else
            {
                if (isdigit(c))
                    nmb = nmb * 10 + (c - '0');
                else
                    break;
            }
        }
        return value(nmb);
    }

    template <typename CharType, typename SinkImpl, typename... Args>
    Result<CharType *> format_arg_to_sink(const CharType *format_specifier, StreamSink<SinkImpl> &, const Args &...args)
    {
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
            if (codepoint == OP_FORMAT_CH || codepoint == CL_FORMAT_CH)
            {
                sink.receive_data(codepoint);
            }
            else
            {
            }
        }
        sink.close_sink();
        // TODO: Change return value
        return value(size_t(0));
    }

} // namespace hls

#endif
