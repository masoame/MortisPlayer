#include <common.hpp>
namespace media
{
	using namespace common;

	auto OpenFFmpegStream(std::string_view url, bool isNeedToPrintInfo = false)
		-> Expected<ScopeAVFormatContextPtr>;

}