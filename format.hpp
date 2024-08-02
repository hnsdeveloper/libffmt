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

namespace hls
{

    constexpr char32_t op_format_ch = '{';
    constexpr char32_t cl_format_ch = '}';

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
    Result<uint64_t> atou(const CharType *str)
    {
        if (str)
        {
            auto peekresult = peek_next_codepoint(str);
            if (!(peekresult.is_error() || !isdigit(peekresult.get_value())))
            {
                uint64_t number = 0;
                for (auto result = get_next_codepoint(&str); !result.is_error() && isdigit(result.get_value());
                     result = get_next_codepoint(&str))
                {
                    number = number * 10 + result.get_value();
                }

                return value(number);
            }
        }

        return error<uint64_t>(Error::INVALID_ARGUMENT);
    }

    template <typename CharType>
    Result<uint64_t> atoi(const CharType *str)
    {
        uint64_t number = 0;
    }

    template <typename CharType, typename SinkImpl, typename... Args>
    Result<CharType *> format_arg_to_sink(const CharType *format_specifier, StreamSink<SinkImpl> &, const Args &...args)
    {
    }

    // TODO: Handle error values properly
    template <typename CharType, typename SinkImpl, typename... Args>
    Result<size_t> format_string_to_sink(UTFStringView<CharType> str, StreamSink<SinkImpl> &sink, const Args &...args)
    {
        sink.open_sink();
        for(auto it = str.begin(); it != str.end(); ++it)
        {
            // TODO: IMPLEMENT
        }
        sink.close_sink();
        // TODO: Change return value
        return value(size_t(0));
    }
} // namespace hls

#endif
