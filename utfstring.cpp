#include "utfstring.hpp"

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
#define UTF8_ACCEPT 0
#define UTF8_REJECT 12
static const char8_t utf8d[] = {
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    7,
    8,
    8,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    10,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    4,
    3,
    3,
    11,
    6,
    6,
    6,
    5,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
    0,
    12,
    24,
    36,
    60,
    96,
    84,
    12,
    12,
    12,
    48,
    72,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    0,
    12,
    12,
    12,
    12,
    12,
    0,
    12,
    0,
    12,
    12,
    12,
    24,
    12,
    12,
    12,
    12,
    12,
    24,
    12,
    24,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    24,
    12,
    12,
    12,
    12,
    12,
    24,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    24,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    36,
    12,
    36,
    12,
    12,
    12,
    36,
    12,
    12,
    12,
    12,
    12,
    36,
    12,
    36,
    12,
    12,
    12,
    36,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
};

char32_t inline decode(uint32_t *state, char32_t *codep, char32_t byte)
{
    char32_t type = utf8d[byte];
    *codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
    *state = utf8d[256 + *state + type];
    return *state;
}

namespace hls
{
    bool do_buffers_overlap(const void *buffer_a, size_t buffer_a_size, const void *buffer_b, size_t buffer_b_size)
    {
        auto is_between = [](const char *ptr, const char *begin, const char *end) {
            return begin <= ptr && ptr <= end;
        };

        const char *_abegin = (const char *)(buffer_a);
        const char *_aend = _abegin + buffer_a_size;
        const char *_bbegin = (const char *)(buffer_b);
        const char *_bend = (const char *)(_bbegin);

        if (is_between(_abegin, _bbegin, _bend - 1) || is_between(_aend, _bbegin, _bend - 1))
            return true;
        if (is_between(_bbegin, _abegin, _aend - 1) || is_between(_bend, _abegin, _aend - 1))
            return true;

        return false;
    }

    // Assumes UTF-8 encoding for char8_t
    template <>
    char32_t get_next_codepoint<char8_t>(const char8_t **str)
    {
        uint32_t st = UTF8_ACCEPT;
        char32_t codepoint = 0;

        while (str)
        {
            ++(*str);
            if (!decode(&st, &codepoint, *((*str) - 1)))
                break;
        }
        if (st == UTF8_ACCEPT)
            return codepoint;

        return INVALID_CODEPOINT;
    }

    // Assumes UTF-16 encoding for char16_t
    template <>
    char32_t get_next_codepoint<char16_t>(const char16_t **str)
    {
        if (str)
        {
            char16_t w1 = *((*str)++);
            if (w1 < 0xD800 || w1 > 0xDFFF)
                return static_cast<char32_t>(w1);

            if (w1 >= 0xD800 && w1 <= 0xDBFF)
            {
                char16_t w2 = *((*str)++);
                if (w2 >= 0xDC00 && w2 <= 0xDFFF)
                {
                    char32_t result = ((((char32_t)(w1) & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
                    return result;
                }
            }
        }
        return INVALID_CODEPOINT;
    }

    // Assumes UTF-32 encoding for char32_t
    template <>
    char32_t get_next_codepoint<char32_t>(const char32_t **str)
    {
        if (str && *str)
            return *((*str)++);

        return INVALID_CODEPOINT;
    }

    template <>
    hls::Result<size_t> encode_char<char8_t>(char32_t codepoint, char8_t *dest, size_t byte_size)
    {
        size_t needed_bytes = get_converted_codepoint_byte_size<char8_t>(codepoint);
        if (needed_bytes > byte_size || needed_bytes == 0)
            return error<size_t>(Error::UNDEFINED_ERROR);

        size_t byte = needed_bytes - 1;
        size_t i = 0;
        dest[0] = 0;
        switch (needed_bytes)
        {
            case 4:
                dest[byte--] = 0x80 | ((codepoint >> (i * 6)) & 0x3F);
                dest[0] = dest[0] | ((needed_bytes == 4) * (((codepoint >> 18) & 0x7) | 0xF0));
                ++i;
            case 3:
                dest[byte--] = 0x80 | ((codepoint >> (i * 6)) & 0x3F);
                dest[0] = dest[0] | ((needed_bytes == 3) * (((codepoint >> 12) & 0xF) | 0xE0));
                ++i;
            case 2:
                dest[byte--] = 0x80 | ((codepoint >> (i * 6)) & 0x3F);
                dest[0] = dest[0] | ((needed_bytes == 2) * (((codepoint >> 6) & 0x1F) | 0xC0));
                ++i;
            case 1:
                dest[0] = dest[0] | ((needed_bytes == 1) * (codepoint & 0x7F));
        }

        return value(needed_bytes);
    }

    template <>
    hls::Result<size_t> encode_char<char16_t>(char32_t codepoint, char16_t *dest, size_t byte_size)
    {
        size_t needed_bytes = get_converted_codepoint_byte_size<char16_t>(codepoint);
        if (needed_bytes > byte_size || needed_bytes == 0)
            return error<size_t>(Error::UNDEFINED_ERROR);

        switch (needed_bytes)
        {
            case 4:
                codepoint = codepoint - 0x10000;
                dest[0] = 0xD800 | ((codepoint >> 10) & 0x3FF);
                dest[1] = 0xDC00 | (codepoint & 0x3FF);
                break;
            case 2:
                dest[0] = static_cast<char16_t>(codepoint);
                break;
        }

        return value(needed_bytes);
    }

    template <>
    hls::Result<size_t> encode_char<char32_t>(char32_t codepoint, char32_t *dest, size_t byte_size)
    {
        if (byte_size < sizeof(char32_t))
            return error<size_t>(Error::UNDEFINED_ERROR);

        *dest = codepoint;
        return value(sizeof(char32_t));
    }

    template <>
    size_t get_converted_codepoint_byte_size<char8_t>(const char32_t codepoint)
    {
        if (codepoint <= 0x10FFFF)
        {
            if (codepoint <= 0x7F)
                return 1;
            if (codepoint >= 0x80 && codepoint <= 0x7FF)
                return 2;
            if (codepoint >= 0x800 && codepoint <= 0xFFF)
                return 3;
            if (codepoint >= 0x10000 && codepoint <= 0x10FFFF)
                return 4;
        }
        return 0;
    }

    template <>
    size_t get_converted_codepoint_byte_size<char16_t>(const char32_t codepoint)
    {
        if (codepoint <= 0x10FFFF)
            return codepoint < 0x10000 ? sizeof(char16_t) : sizeof(char16_t) * 2;
        return 0;
    }

    template <>
    size_t get_converted_codepoint_byte_size<char32_t>(const char32_t codepoint)
    {
        if (codepoint <= 0x10FFFF)
            return sizeof(char32_t);
        return 0;
    }
} // namespace hls