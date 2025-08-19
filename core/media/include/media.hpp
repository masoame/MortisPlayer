#include <common.hpp>
namespace media
{
	using namespace common;

	auto OpenFFmpegStream(std::string_view url, bool isNeedToPrintInfo = false)
		-> Expected<ScopeAVFormatContextPtr>;

	auto CreateDecodecCtx(const ScopeAVFormatContextPtr& stream, AVMediaType meidaType,const std::unique_ptr<int>& outIndex = {})
		-> Expected<ScopeAVCodecContextPtr>;

}