#include "stream.hpp"
#include "formatspecifier.hpp"

namespace hls
{
    template <typename T>
    class Formatter;

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

    template <typename SinkImpl>
    void unsigned_dec_printer(unsigned long long int v, StreamSink<SinkImpl> &sink)
    {
        if (v / 10)
            number_printer(v / 10, sink);
        sink.receive_data(char32_t('0') + (v % 10));
    }

    template <typename SinkImpl>
    void unsigned_hex_printer(unsigned long long int v, StreamSink<SinkImpl> &sink)
    {
        if (v / 0x10)
            number_printer(v / 0x10, sink);
        auto p = v % 0x10;
        if (p < 0xA)
            sink.receive_data(char32_t('0') + p);
        else
        {
            sink.receive_data(char32_t('A') + p - 0x9);
        }
    }

} // namespace hls