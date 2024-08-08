#ifndef _STDOUT_HPP_
#define _STDOUT_HPP_

#include "format.hpp"
#include "utfstringview.hpp"
#include <iostream>

namespace hls
{
    using byte = unsigned char;
    static constexpr size_t STREAM_SINK_PRINTER_BUFFER_SIZE = 128;

    extern "C" void printstr(const void *str, size_t byte_size);

    class PrinterSink : public StreamSink<PrinterSink>
    {
        static_assert(sizeof(STREAM_SINK_PRINTER_BUFFER_SIZE) > sizeof(char32_t) &&
                          ((STREAM_SINK_PRINTER_BUFFER_SIZE % 4) == 0),
                      "PrinterSink must have a buffer bigger than char32_t and be a multilple of sizeof(char32_t)");
        byte m_buffer[STREAM_SINK_PRINTER_BUFFER_SIZE];
        size_t m_available_size;
        Encoding m_encoding;

        void erase_buffer()
        {
            for (size_t i = 0; i < sizeof(m_buffer) / 4; ++i)
                *((uint32_t *)(m_buffer + 4)) = 0;

            m_available_size = sizeof(STREAM_SINK_PRINTER_BUFFER_SIZE);
        }

        bool open_sink_impl()
        {
            erase_buffer();
            return true;
        }

        bool close_sink_impl()
        {

            // If this is true, that means our buffer is not free, thus we should print the remaining characters.
            if (m_available_size != STREAM_SINK_PRINTER_BUFFER_SIZE)
            {
                // TODO: implement
            }
            return true;
        }

        bool receive_data_impl(char32_t data)
        {
            byte buffer[5] = {0, 0, 0, 0, 0};

            encode_char(data, (char8_t *)(buffer), 4);
            std::cout << buffer;
            return true;
        }

      public:
        PrinterSink(Encoding encoding) : m_encoding(encoding)
        {
        }

        friend class StreamSink<PrinterSink>;
    };

} // namespace hls

#endif