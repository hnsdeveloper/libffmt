#ifndef _FINDNAME_HPP_
#define _FINDNAME_HPP_

#include "utfstring.hpp"
#include "utfstringview.hpp"

namespace hls
{
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
                else if (isspace(c))
                    continue;
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
                    nmb = sign * (c - '0');
                else if (isspace(c))
                    continue;
                else
                    return error<int64_t>(Error::INVALID_ARGUMENT);
            }
            else
            {
                if (isdigit(c))
                    nmb = nmb * 10 + sign * (c - '0');
                else
                    break;
            }
        }
        return value(nmb);
    }

} // namespace hls

#endif