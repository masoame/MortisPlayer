#include <media.hpp>

namespace Mortis::Player
{
	auto OpenFFmpegStream(std::string_view url, bool isNeedToPrintInfo)
		-> Expected<ScopeAVFormatContextPtr>
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

	auto CreateDecodecCtx(const ScopeAVFormatContextPtr& stream, AVMediaType meidaType,const std::unique_ptr<int>& pOutIndex)
		-> Expected<ScopeAVCodecContextPtr>
	{
		auto index = av_find_best_stream(stream, meidaType, -1, -1, NULL, 0);
		if (index == AVERROR_STREAM_NOT_FOUND) {
			return UnExpected("av_find_best_stream Stream Not_Found!!!");
		}
		ScopeAVCodecContextPtr decode_ctx = avcodec_alloc_context3(nullptr);
		if (decode_ctx == nullptr) {
			return UnExpected("avcodec_alloc_context3 error!!!");																
		}
		if (avcodec_parameters_to_context(decode_ctx, stream->streams[index]->codecpar) < 0) {
			return UnExpected("avcodec_parameters_to_context error!!!");
		}
		const AVCodec* codec = avcodec_find_decoder(decode_ctx->codec_id);
		if (avcodec_open2(decode_ctx, codec, NULL)) {
			return UnExpected("avcodec_open2 error!!!");
		}
		if (pOutIndex) {
			*pOutIndex = index;
		}
		return decode_ctx;
	}
}


