#include "stream.hpp"
#include "formatspecifier.hpp"

namespace hls
{

    template <typename SinkImpl>
    void unsigned_printer(unsigned long long int v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs, bool first)
    {
        switch (fs.get_integer_display_type())
        {
            case OCT_FORMAT:
                if (first)
                    sink.receive_data(char32_t('0'));
                if (v / 8)
                    unsigned_printer(v / 8, sink, fs, false);
                sink.receive_data(char32_t('0') + (v % 8));
                break;
            case DEC_FORMAT:
                if (v / 10)
                    unsigned_printer(v / 10, sink, fs, false);
                sink.receive_data(char32_t('0') + (v % 10));
                ;
                break;
            case HEX_FORMAT:
                if (first)
                {
                    sink.receive_data(char32_t('0'));
                    sink.receive_data(char32_t('x'));
                }
                if (v / 0x10)
                    unsigned_printer(v / 0x10, sink, fs, false);
                if (v % 0x10 < 0xA)
                    sink.receive_data(char32_t('0') + v % 0x10);
                else
                    sink.receive_data(char32_t('A') + (v % 0x10) - 0xA);
                break;
            default:
                break;
        }
    }

    template <typename SinkImpl>
    void unsigned_hex_printer(unsigned long long int v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs,
                              bool first)
    {
    }

    template <typename T>
    class Formatter
    {
      public:
        template <typename SinkImpl>
        static void value_to_sink(const T &v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs)
        {
            if constexpr (std::is_signed_v<T>)
            {
            }
            else if constexpr (!std::is_signed_v<T>)
            {
                unsigned_printer(v, sink, fs, true);
            }
            else
            {
                []<bool flag = false>() {
                    static_assert(flag, "Custom implementation needed for printing requested type.");
                }();
            }
        }
    };

    template <>
    class Formatter<const char *>
    {
      public:
        template <typename SinkImpl>
        static void value_to_sink(const char *str, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs)
        {
            while (str && *str)
            {
                sink.receive_data((char32_t)(*str));
                ++str;
            }
        }
    };

    template <>
    class Formatter<char *>
    {
        template <typename SinkImpl>
        static void value_to_sink(char *str, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs)
        {
            Formatter<const char *>::value_to_sink(str, sink, fs);
        }
    };

} // namespace hls