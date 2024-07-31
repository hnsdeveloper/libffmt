#ifndef _UTF_HPP_
#define _UTF_HPP_

#ifdef __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
#endif

#include "result.hpp"

namespace hls
{
    enum class Encoding : char {
        ASCII,
        UTF8,
        UTF16, 
        UTF32
    };

    template<typename CharType>
    hls::Result<char32_t> get_next_codepoint(const CharType* *str);
    template<>
    hls::Result<char32_t> get_next_codepoint<char8_t>(const char8_t **str);
    template<>
    hls::Result<char32_t> get_next_codepoint<char16_t>(const char16_t **str);
    template<>
    hls::Result<char32_t> get_next_codepoint<char32_t>(const char32_t **str);

    template<typename DstType>
    size_t get_converted_codepoint_byte_size(const char32_t codepoint);
    template<>
    size_t get_converted_codepoint_byte_size<char8_t>(const char32_t codepoint);
    template<>
    size_t get_converted_codepoint_byte_size<char16_t>(const char32_t codepoint);
    template<>
    size_t get_converted_codepoint_byte_size<char32_t>(const char32_t codepoint);

    template<typename CharType>
    hls::Result<size_t> encode_char(char32_t codepoint, CharType* dest, size_t byte_size);
    template<>
    hls::Result<size_t> encode_char<char8_t>(char32_t codepoint, char8_t* dest, size_t byte_size);
    template<>
    hls::Result<size_t> encode_char<char16_t>(char32_t codepoint, char16_t* dest, size_t byte_size);
    template<>
    hls::Result<size_t> encode_char<char32_t>(char32_t codepoint, char32_t* dest, size_t byte_size);

    bool do_buffers_overlap(const void* buffer_a, size_t buffer_a_size, const void* buffer_b, size_t buffer_b_size);

    template<typename SrcCharType, typename DstCharType>
    size_t get_converted_string_byte_size(const SrcCharType* src)
    {
        size_t sz = 0;
        if(src)
        {
            while(true)
            {
                auto result = get_next_codepoint(&src);
                if(result.is_error())
                    return 0;
                
                sz += get_converted_codepoint_byte_size<DstCharType>(result.get_value());
                if(result.get_value() == 0)
                    break;
            }
        }
        return sz;
    }

    template<typename CharType>
    size_t utfcodelen(const CharType* str)
    {
        size_t i = 0;    
        if(str)
        {
            while(true)
            {
                auto result = get_next_codepoint(&str);
                if(result.is_error())
                    return 0;
                    
                // Null value
                if(result.get_value() == 0)
                    break;
                ++i;
            }
        }
        return i + (str ? 1 : 0); // Taking into account the null character.
    }

    template<typename CharType>
    size_t utfbytelen(const CharType* str)
    {
        const char* begin = reinterpret_cast<const char*>(str);
        if(str)
        {
            while(true)
            {
                auto result = get_next_codepoint(&str);
                if(result.is_error())
                    return 0;
                // Null value
                if(result.get_value() == 0)
                    break;
            }
        }
        return reinterpret_cast<const char*>(str) - begin;
    }

    // This is not entirely correct, given that ligatures might happen and thus need to be handled separately.
    // Taking it into account, this is an "intuitive" strlen, in the sense that it counts the codepoints and returns its value
    // minus 1, given the null character.
    template<typename CharType>
    size_t utfstrlen(const CharType* str)
    {
        size_t s = utfcodelen(str);
        return s - (s > 0) * 1;
    }

    template<typename CharType>
    bool is_valid_utf_sequence(const CharType* src)
    {
        if(!src)
            return false;
        while(true)
        {
            auto result = get_next_codepoint(&src);
            if(result.is_error())
                return false;
            if(result.get_value() == 0)
                break;
        }
        return true;
    }

    template<typename SrcCharType, typename DstCharType>
    hls::Result<DstCharType*> encode_utfstr(const SrcCharType* src, DstCharType* dest, size_t dest_byte_size)
    {
        if(!src || !dest || !dest_byte_size || !is_valid_utf_sequence(src))
            return error<DstCharType*>(Error::INVALID_ARGUMENT);

        size_t bytelen = utfbytelen(src);
        if(do_buffers_overlap(src, bytelen, dest, dest_byte_size))
            return error<DstCharType*>(Error::INVALID_ARGUMENT);        
        
        auto retval = dest;
        while(true)
        {
            char32_t cp = get_next_codepoint(&src).get_value();
            size_t sz = get_converted_codepoint_byte_size<DstCharType>(cp);
            if(sz >= dest_byte_size || cp == 0)
            {
                // No space for the null character if we keep encoding, thus we add it manually and finish
                *((unsigned char*)(dest)) = 0;
                return value(retval);
            }
            encode_char(cp, dest, dest_byte_size).get_value();
            dest = reinterpret_cast<DstCharType*>((reinterpret_cast<unsigned char*>(dest) + sz));
            dest_byte_size = dest_byte_size - sz;
        }
        return value(retval);
    }

}

#endif
