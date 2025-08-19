﻿#pragma once
#include<media.hpp>

namespace FFmpegLayer
{
	using namespace common;

	//函数回调类型
	using insert_callback_type = std::function<void(AVFrame*&, char*& buf)>;

	using framedata_type = std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>;
	using auto_framedata_type = std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>;
	//错误枚举
	enum RESULT
	{
		SUCCESS, UNKONW_ERROR, ARGS_ERROR,
		ALLOC_ERROR, OPEN_ERROR, INIT_ERROR,
		UNNEED_SWR
	};

	//流类型
	enum ios :char
	{
		in = 0x1,
		out = 0x2,
		io = in | out,
		video = 0x4,
		audio = 0x8
	};

	//输入或输出帧格式
	typedef struct MediaType
	{
		int w, h;
		AVPixelFormat video_type;
		AVSampleFormat audio_type;
	}IOMediaType;

	extern const char sample_bit_size[13];
	extern const AVSampleFormat map_palnner_to_packad[13];

	//播放工具类
	class PlayTool
	{
	protected:
		PlayTool() = default;
		PlayTool(std::string_view srcUrl, std::string_view dstUrl, unsigned char type = ios::in | ios::video | ios::audio);
		PlayTool(const PlayTool&) = delete;
		PlayTool(const PlayTool&&) = delete;
	public:
		static PlayTool& Instance();

		~PlayTool();

		//打开流
        Expected<> open(std::string_view srcUrl, std::string_view dstUrl = {}, unsigned char type = in | video | audio);
		//初始化各种编解码器
		RESULT init_decode(AVMediaType type);
		//初始化编码器
		RESULT init_encode(AVMediaType type);

		//初始化音频重采样(planner到packed格式转化)
		RESULT init_swr();
		//转化函数(planner到packed格式转化)
		RESULT sample_planner_to_packed(AVFrame* frame, uint8_t** data, int* linesize);
		//帧格式转化
		RESULT init_sws(AVFrame* work, const AVPixelFormat dstFormat = AV_PIX_FMT_YUV420P, const int dstW = 0, const int dstH = 0);
		//转化为
		RESULT sws_scale_420P(AVFrame*& data);
		//音视频包读取线程
		RESULT start_read_thread() noexcept;
		//解码线程
		RESULT start_decode_thread() noexcept;
		//编码线程
		RESULT start_encode_thread() noexcept;
		//音视频拉流解码线程
		RESULT start_pull_steam_thread() noexcept;

		void insert_queue(AVMediaType index, ScopeAVFramePtr&& avf) noexcept;

		//定位到对应的时间
		void seek_time(int64_t usec)noexcept;

		void close()noexcept;
	public:

		::Mortis::bounded_queue<std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>> FrameQueue[3]{
			::Mortis::bounded_queue < std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>>(12ULL),
			::Mortis::bounded_queue < std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>>(50ULL),
			::Mortis::bounded_queue < std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>>(1ULL) };

		::Mortis::bounded_queue<ScopeAVPacketPtr> PacketQueue{ 10 };


		//输入输出格式指针
		ScopeAVFormatContextPtr _p_avfctx_input, _p_avfctx_output;

		//解码上下文指针
		ScopeAVCodecContextPtr _decode_ctx_arr[3];
		std::mutex decode_mutex;

		//编码上下文指针
		ScopeAVCodecContextPtr encode_ctx[3];

		//图像帧转化上下文
		ScopeSwsContextPtr sws_ctx;
		//音频解码上下文
		ScopeSwrContextPtr swr_ctx;

		//各个工作帧
		framedata_type avframe_work[6];

		//insert_callback[AVMediaType(帧格式)] == 处理函数指针
		std::function<void(AVFrame*&, char*& buf)> insert_callback[6];

		// AVStreamIndexToType[流的索引] == 流类型
		AVMediaType AVStreamIndexToType[6]{ AVMEDIA_TYPE_UNKNOWN ,AVMEDIA_TYPE_UNKNOWN ,AVMEDIA_TYPE_UNKNOWN ,AVMEDIA_TYPE_UNKNOWN ,AVMEDIA_TYPE_UNKNOWN,AVMEDIA_TYPE_UNKNOWN };

		// AVTypeToStreamIndex[流类型] == 流的索引
		int AVTypeToStreamIndex[6]{ -1,-1,-1,-1,-1,-1 };

		//各个流的时间基
		double secBaseTime[3]{ 0 };

		//读取Packet线程
		std::jthread ThrRead;

		//解码Packet线程
		std::jthread ThrDecode;

		// 播放线程
		std::jthread ThrPlay;

	};
}




