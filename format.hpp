#ifndef _FORMAT_HPP_
#define _FORMAT_HPP_

#ifdef __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
#endif

#include "result.hpp"
#include "utf.hpp"
#include "stream.hpp"

namespace hls
{
   
    class FormatSpecifier
    {
        const char32_t m_initial_code_point;
        bool m_is_literal;
        

        public:
        template<typename CharType>
        FormatSpecifier(const CharType* str, char32_t initial_codepoint) : m_initial_code_point(initial_codepoint)
        {

        };

        bool is_literal() const
        {

        }
    };

    template<typename CharType>
    Result<FormatSpecifier> get_format(const CharType*, char32_t codepoint);


    // TODO: Handle error values properly
    template<typename CharType, typename SinkImpl, typename ...Args>
    Result<size_t> format_to_sink(const CharType* str, StreamSink<SinkImpl>& stream, const Args&&... args)
    {
        if(str)
        {
            stream.open_sink();
            while(true)
            {
                auto result = get_next_codepoint(&str);
                if(result.is_error())
                    break;
                auto codepoint = result.get_value();
                // If we find any character other than { or }, we are good, given that it is a text character
                // Otherwise we have to handle it first
                if(codepoint == '{' || codepoint == '}')
                {
                    auto fspecresult = get_format(str);
                    // We don't have a valid format specifier, thus we don't keep going
                    if(fspecresult.is_error())
                        return fspecresult.get_error();
                    auto& format_spec = fspecresult.get_value();
                    if(format_spec.is_literal())
                    {
                        stream.receive_data(codepoint);
                    }
                    else
                    {
                        // TODO: handle formatting
                    }
                    
                }
                else
                {
                    stream.receive_data(codepoint);
                }

                if(codepoint == 0)
                    break;
            }
            stream.close_sink();
        }

        // TODO: Change return value
        return value(size_t(0));
    }
}

#endif
