#ifndef _STREAM_HPP_
#define _STREAM_HPP_

#ifdef __STDC_HOSTED__
#include <utility>
#else
namespace std = hls;
#endif

namespace hls
{
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

        template<typename DataType, typename ...CtxArgs>
        auto receive_data(DataType&& data, CtxArgs&&... ctx_args)
        {
            return static_cast<SinkImpl*>(this)->receive_data_impl(std::forward<DataType>(data), std::forward<CtxArgs>(ctx_args)...);
        }

        auto close_sink()
        {
            return static_cast<SinkImpl*>(this)->close_sink_impl();
        }
    };
}


#endif