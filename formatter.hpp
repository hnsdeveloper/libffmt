#include "stream.hpp"

namespace hls
{
    template <typename T>
    class Formatter;

    template <>
    class Formatter<char *>
    {
      public:
        template <typename SinkImpl>
        static void value_to_sink(const char *str, StreamSink<SinkImpl> &sink)
        {
            while (str && *str)
                sink.receive_data((char32_t)(*str));
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