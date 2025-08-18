#pragma once
#include <Utils.hpp>

#ifdef __cplusplus
extern"C"
{
#endif
#pragma warning(push) 
#pragma warning(disable : 4244)
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<libswscale/swscale.h>
#include<libavfilter/avfilter.h>
#include<libavutil/avutil.h>
#include<libswresample/swresample.h>
#include<libavutil/channel_layout.h>
#include<libavutil/pixdesc.h>
#include<libavutil/hwcontext.h>
#include<libavutil/opt.h>
#include<libavutil/avassert.h>
#include<libavutil/imgutils.h>
#pragma warning(pop) 

#include<curl/curl.h>

#define SDL_MAIN_HANDLED
#include<SDL.h>
#undef main

#ifdef __cplusplus
}
#endif


namespace common
{
	using namespace std::chrono_literals;
	using Mortis::ScopeHandle;
	using Mortis::BT::StaticFunctorWrapper;
	using Mortis::Expected;
	using Mortis::UnExpected;
	//using Mortis::MultiEnum;
	//管理内存的智能指针
	using ScopeAVPacketPtr = ScopeHandle<AVPacket, StaticFunctorWrapper<av_packet_free>>;
	using ScopeAVCodecContextPtr = ScopeHandle<AVCodecContext, StaticFunctorWrapper<avcodec_free_context>>;
	using ScopeAVFormatContextPtr = ScopeHandle<AVFormatContext, StaticFunctorWrapper<avformat_free_context>>;
	using ScopeSwsContextPtr = ScopeHandle<SwsContext, StaticFunctorWrapper<sws_freeContext>>;
	using ScopeSwrContextPtr = ScopeHandle<SwrContext, StaticFunctorWrapper<swr_free>>;
	using ScopeAVFramePtr = ScopeHandle<AVFrame, StaticFunctorWrapper<av_frame_free>>;

	using ScopeSDLWindowPtr = ScopeHandle<SDL_Window, StaticFunctorWrapper<SDL_DestroyWindow>>;
	using ScopeSDLRendererPtr = ScopeHandle<SDL_Renderer, StaticFunctorWrapper<SDL_DestroyRenderer>>;
	using ScopeSDLTexturePtr = ScopeHandle<SDL_Texture, StaticFunctorWrapper<SDL_DestroyTexture>>;

	using ScopeCURLPtr = ScopeHandle<CURL, StaticFunctorWrapper<curl_easy_cleanup>>;
	using ScopeCURLSlistPtr = ScopeHandle<curl_slist, StaticFunctorWrapper<curl_slist_free_all>>;

}