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

    // Parses a string looking for a format specifier
    template <typename CharType>
    FormatSpecifier get_format_specifier(const UTFStringView<CharType> &str)
    {
        enum State
        {
            INITIAL,
            OPENING,
            ARGID,
            FORMAT_SPEC,
            CLOSING,
            END
        };

        State state = INITIAL;
        char32_t opchar = 0;
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            switch (state)
            {
                case INITIAL:
                {
                    if (isspace(*it))
                    {
                        continue;
                    }
                    else if (*it == OP_FORMAT_CH || *it == CL_FORMAT_CH)
                    {
                        state = OPENING;
                        opchar = *it;
                    }
                    else
                    {
                        // todo: return error
                    }
                }
                break;
                case OPENING:
                {
                    if (isspace(*it))
                    {
                        continue;
                    }
                    else if (*it == OP_FORMAT_CH || *it == CL_FORMAT_CH)
                    {
                        if (*it == opchar)
                        {
                            // We have a literal, return...
                        }
                        else if (opchar == OP_FORMAT_CH && *it == CL_FORMAT_CH)
                        {
                            // We have an empty format specifier, use defaults
                            // TODO: implement
                            state = END;
                        }
                        else
                        {
                            // We have an error
                        }
                    }
                    else if (isdigit(*it))
                    {
                        // We have an argid
                        state = ARGID;
                        --it;
                    }
                    else if (*it == ':')
                    {
                        // We have a format specifier
                        state = FORMAT_SPEC;
                    }
                    else
                    {
                        // We have an error
                    }
                    break;
                }
                case ARGID:
                {
                    // Implement format specifier
                    while (isdigit(*(it + 1)))
                        ++it;
                    state = OPENING;
                }
                break;
                case FORMAT_SPEC:
                {
                    state = CLOSING;
                }
                break;
                case CLOSING:
                {
                    if (isspace(*it))
                    {
                        continue;
                    }
                    else if (*it == CL_FORMAT_CH)
                    {
                        state = END;
                    }
                    else
                    {
                        // We have an error, we must report somehow
                    }
                }
                case END:
                {
                    it = str.end();
                    break;
                }
            }
        }
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
                auto fs = get_format_specifier((it++).from_it());
                while (it != str.end() || *it != OP_FORMAT_CH || *it != CL_FORMAT_CH)
                    ++it;
            }
        }
        sink.close_sink();
        // TODO: Change return value
        return value(size_t(0));
    }

} // namespace hls

#endif
