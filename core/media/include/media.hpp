#include <common.hpp>

namespace Mortis::Player
{
	auto OpenFFmpegStream(std::string_view url, bool isNeedToPrintInfo = false)
		-> Expected<ScopeAVFormatContextPtr>;

	auto CreateDecodecCtxByStream(const ScopeAVFormatContextPtr& stream, AVMediaType meidaType,const std::unique_ptr<int>& outStreamIndex = {})
		-> Expected<ScopeAVCodecContextPtr>;
}