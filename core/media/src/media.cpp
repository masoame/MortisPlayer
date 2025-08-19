#include <media.hpp>

using namespace media;

Expected<ScopeAVFormatContextPtr> OpenFFmpegStream(std::string_view url,bool isNeedToPrintInfo)
{
	ScopeAVFormatContextPtr pAVFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pAVFormatCtx, url.data(), nullptr, nullptr) != 0) {
		return UnExpected("avformat_open_input error!!!");
	}
	if (avformat_find_stream_info(pAVFormatCtx, nullptr) < 0) {
		return UnExpected("avformat_find_stream_info error!!!");
	}
	if (isNeedToPrintInfo) {
		av_dump_format(pAVFormatCtx, 0, url.data(), false);
	}
	return pAVFormatCtx;
}
