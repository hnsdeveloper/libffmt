#include "stream.hpp"
#include "formatspecifier.hpp"

namespace hls
{

    template <typename Integer, typename SinkImpl>
    void integer_printer(Integer v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs, bool first)
    {
        switch (fs.get_integer_display_type())
        {
            case OCT_FORMAT:
            {
                if (first)
                    sink.receive_data(char32_t('0'));
                std::make_unsigned_t<Integer> p = v;
                if (p / 8)
                    integer_printer(p / 8, sink, fs, false);
                sink.receive_data(char32_t('0') + (p % 8));
                break;
            }
            case DEC_FORMAT:
                if (first)
                {
                    if (fs.has_sign())
                    {
                        if (fs.get_sign() == POS_SIGN)
                        {
                            if (v > 0)
                                sink.receive_data(char32_t('+'));
                            else
                                sink.receive_data(char32_t('-'));
                        }
                        else if (fs.get_sign() == SPC_SIGN)
                        {
                            if (v > 0)
                                sink.receive_data(char32_t(' '));
                            else
                                sink.receive_data(char32_t('-'));
                        }
                        else if (fs.get_sign() == NEG_SIGN)
                        {
                            if (v < 0)
                                sink.receive_data(char32_t('-'));
                        }
                    }
                }
                if (v / 10)
                    integer_printer(v / 10, sink, fs, false);
                v = v < 0 ? -(v % 10) : (v % 10);
                sink.receive_data(char32_t('0') + v);
                break;
            case HEX_FORMAT:
            {
                if (first)
                {
                    sink.receive_data(char32_t('0'));
                    sink.receive_data(char32_t('x'));
                }
                // We don't care to the sign of the value, we just want the hex representation of it
                std::make_unsigned_t<Integer> p = v;
                if (p / 0x10)
                    integer_printer(p / 0x10, sink, fs, false);
                if (p % 0x10 < 0xA)
                    sink.receive_data(char32_t('0') + p % 0x10);
                else
                    sink.receive_data(char32_t('A') + (p % 0x10) - 0xA);
                break;
            }
            default:
                break;
        }
    }

    template <typename T>
    class Formatter
    {
      public:
        template <typename SinkImpl>
        static void value_to_sink(const T &v, StreamSink<SinkImpl> &sink, const FormatSpecifier &fs)
        {
            if constexpr (std::is_signed_v<T> || !std::is_signed_v<T>)
                integer_printer(v, sink, fs, true);
            else
                []<bool flag = false>() {
                    static_assert(flag, "Custom implementation needed for printing requested type.");
                }();
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