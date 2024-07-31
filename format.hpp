#include <stdint.h>
#include <stddef.h>
#include "result.hpp"
#include "utf.hpp"


namespace hls
{

    static constexpr size_t STREAM_SINK_PRINTER_BUFFER_SIZE = 128;
    using byte = unsigned char;

    template<typename SinkImpl>
    class StreamSink
    {   

        protected:

        // Cannot be deleted through a pointer to the base type, thus we avoid a vtable
        // By not having to define a virtual destructor
        ~StreamSink() = default;

        public:

        auto open_sink()
        {
            return static_cast<SinkImpl*>(this)->open_sink_impl();
        }

        auto receive_data(char32_t data)
        {
            return static_cast<SinkImpl*>(this)->receive_data_impl(data);
        }

        auto close_sink()
        {
            return static_cast<SinkImpl*>(this)->close_sink_impl();
        }
    };
    
    template<typename CharType, typename SinkImpl, typename ...Args>
    Result<size_t> format_to_sink(const CharType* str, StreamSink<SinkImpl>& stream, const Args&&... args)
    {
        if(str)
        {
            stream.open_sink();
            while(true)
            {
                auto result = get_next_codepoint(&str);
                if(result.is_error())
                    break;
                auto codepoint = result.get_value();
                stream.receive_data(codepoint);
                if(codepoint == 0)
                    break;
            }
            stream.close_sink();
        }

        return value(size_t(0));
    }
}
