#include "stream.hpp"
#include "formatspecifier.hpp"

namespace hls
{

    template <typename SinkImpl>
    void unsigned_oct_printer(unsigned long long int v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs,
                              bool first)
    {
        if (v / 8)
            unsigned_oct_printer(v / 8, sink, fs, false);
        sink.receive_data(char32_t('0') + (v % 8));
    }

    template <typename SinkImpl>
    void unsigned_dec_printer(unsigned long long int v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs,
                              bool first)
    {
        if (v / 10)
            unsigned_dec_printer(v / 10, sink, fs, false);
        sink.receive_data(char32_t('0') + (v % 10));
    }

    template <typename SinkImpl>
    void unsigned_hex_printer(unsigned long long int v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs,
                              bool first)
    {
        if (v / 0x10)
            unsigned_hex_printer(v / 0x10, sink, fs, false);
        auto p = v % 0x10;
        if (p < 0xA)
            sink.receive_data(char32_t('0') + p);
        else
        {
            sink.receive_data(char32_t('A') + p - 0xA);
        }
    }

    template <typename T>
    class Formatter
    {
      public:
        template <typename SinkImpl>
        static void value_to_sink(const T &v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs)
        {
            switch (fs.get_integer_display_type())
            {
                case OCT_FORMAT:
                    unsigned_oct_printer(v, sink, fs, true);
                    break;
                case DEC_FORMAT:
                    unsigned_dec_printer(v, sink, fs, true);
                    break;
                case HEX_FORMAT:
                    unsigned_hex_printer(v, sink, fs, true);
                    break;
                default:
                    break;
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