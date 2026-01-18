#pragma once
#include<media.hpp>

namespace Mortis::Player::FFmpeg
{
	constexpr unsigned char AVMediaTypeCOUNT = 6;

	using PacketQueue = bounded_queue<ScopeAVPacketPtr>;
	using FrameQueue = bounded_queue<std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>>;
	//函数回调类型
	using insert_callback_type = std::function<void(AVFrame*&, char*& buf)>;

	using WorkFrame = std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>;
	using auto_framedata_type = std::pair<ScopeAVFramePtr, std::unique_ptr<char[]>>;
	//错误枚举
	enum RESULT
	{
		SUCCESS, UNKONW_ERROR, ARGS_ERROR,
		ALLOC_ERROR, OPEN_ERROR, INIT_ERROR,
		UNNEED_SWR
	};

	//流类型
	enum PlayerMode
	{
		None = 0x0,
		in = 0x1,
		out = 0x2,
		io = in | out,
		video = 0x4,
		audio = 0x8
	};

	extern const char sample_bit_size[13];
	extern const AVSampleFormat map_palnner_to_packad[13];

	//播放工具类
	class PlayTool : public Mortis::Singleton<PlayTool>
	{
		class FrameStreamContext
		{
			ScopeAVCodecContextPtr _pCodecCtx;
			FrameQueue _frameQueue;
			double _dSecBaseTime;
			int _iStream;
		public:
			FrameStreamContext();
			void onceInit(ScopeAVCodecContextPtr&& pCodeCtx, std::size_t nQueueSize, double dSecBaseTime, int iStreamIdx);
			double getSecBaseTime() const noexcept { return _dSecBaseTime; }
			const ScopeAVCodecContextPtr& getCodecCtx() const noexcept { return _pCodecCtx; }
			FrameQueue& getFrameQueue() noexcept { return _frameQueue; }
			int getStreamIndex() const noexcept { return _iStream; }
		};
	private:
		ScopeAVFormatContextPtr pAVFmtStream;
		std::mutex decode_mutex;

		//图像帧转化上下文/音频解码上下文
		ScopeSwsContextPtr sws_ctx;
		ScopeSwrContextPtr swr_ctx;

	public:
		std::array<WorkFrame, AVMediaTypeCOUNT> avframe_work;
		std::jthread thrRead;
		std::jthread thrDecode;
		std::jthread ThrPlay;
		std::array<FrameStreamContext, AVMediaTypeCOUNT> _frameCtxArr;
		std::map<int, AVMediaType> _indexToMediaType;

		std::array<std::function<void(ScopeAVFramePtr&, char*&)>, 6> insert_callback;


		PlayTool();
		~PlayTool();

		PacketQueue packetQueue{ 10 };
		
		const ScopeAVFormatContextPtr& avFmtStream() const noexcept { return pAVFmtStream;}

		//打开流
        bool open(std::string_view srcUrl, std::string_view dstUrl = {},int mode = in | video | audio);
		//初始化各种编解码器
		bool initPlayStreamCtx(AVMediaType type,std::size_t queueSize = 1);
		//初始化编码器
		RESULT init_encode(AVMediaType type);

		//初始化音频重采样(planner到packed格式转化)
		RESULT init_swr();
		//转化函数(planner到packed格式转化)
		RESULT sample_planner_to_packed(AVFrame* frame, uint8_t** data, int* linesize);
		//帧格式转化
		RESULT init_sws(AVFrame* work, const AVPixelFormat dstFormat = AV_PIX_FMT_YUV420P, const int dstW = 0, const int dstH = 0);
		//转化为
		RESULT sws_scale_420P(ScopeAVFramePtr& data);
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
	
	};
}




