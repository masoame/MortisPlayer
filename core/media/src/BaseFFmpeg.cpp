#include"BaseFFmpeg.hpp"

namespace FFmpegLayer
{
    //sample_bit_size[AVSampleFormat(音频采样格式)] == 采样点的大小
    constexpr const char sample_bit_size[13]{ 1,2,4,4,8,1,2,4,4,8,8,8,-1 };
    //planner转化为对应的packed(AV_SAMPLE_FMT_NONE为错误格式)
    constexpr const AVSampleFormat map_palnner_to_packad[13]
    {
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_U8,
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_S32,
        AV_SAMPLE_FMT_FLT,
        AV_SAMPLE_FMT_DBL,
        AV_SAMPLE_FMT_NONE,
        AV_SAMPLE_FMT_S64,
        AV_SAMPLE_FMT_NONE
    };

	PlayTool::PlaySreamContext::PlaySreamContext(ScopeAVCodecContextPtr&& codecCtx, std::size_t frameSize)
        : _frame_queue(frameSize)
	{
        _codec_ctx = std::move(codecCtx);
	}

	PlayTool::PlaySreamContext::PlaySreamContext(std::size_t frameSize)
        : PlaySreamContext({},frameSize) { }

	PlayTool::PlaySreamContext::PlaySreamContext()
		: PlaySreamContext(0) { }

	PlayTool::PlayTool() :
        _play_stream_ctx{ }
	{ }

	PlayTool::~PlayTool() {
        close();
    }


    bool PlayTool::open(std::string_view srcUrl, std::string_view dstUrl, unsigned char type)
    {
        close();
        if (type & in)
        {
            auto ex_avfctx_input = media::OpenFFmpegStream(srcUrl, true);
            if (ex_avfctx_input.has_value()) {
                _p_avfctx_input = std::move(ex_avfctx_input.value());
            }
            else {
				throw std::runtime_error(ex_avfctx_input.error().data());
            }

            if (type & video) {
                initPlayStreamCtx(AVMEDIA_TYPE_VIDEO, 12);
            }
            if (type & audio) {
                initPlayStreamCtx(AVMEDIA_TYPE_AUDIO, 50);
            }
            return true;
        }
        if (type & out)
        {
            // 未完成重写
            _p_avfctx_output = avformat_alloc_context();
            if (avformat_alloc_output_context2(&_p_avfctx_output, nullptr, dstUrl.data(), nullptr) < 0) {

            };
            return true;
        }
        return false;
    }

	RESULT PlayTool::initPlayStreamCtx(AVMediaType mediaType,std::size_t queueSize)
    {
        auto pStreamIndex = std::make_unique<int>();

        auto& ref_ctx = _play_stream_ctx[mediaType];
        ref_ctx._codec_ctx = media::CreateDecodecCtx(_p_avfctx_input, mediaType, pStreamIndex).value_or(nullptr);
		ref_ctx._index = *pStreamIndex;
        ref_ctx._secBaseTime = av_q2d(_p_avfctx_input->streams[*pStreamIndex]->time_base);
		ref_ctx._frame_queue.setMaxSize(queueSize);

		_stream_index_to_type[*pStreamIndex] = mediaType;
        return SUCCESS;
    }


    RESULT PlayTool::init_swr()
    {
        ScopeAVCodecContextPtr& audio_ctx = _play_stream_ctx[AVMEDIA_TYPE_AUDIO]._codec_ctx;

        AVSampleFormat* format;
        avcodec_get_supported_config(audio_ctx, nullptr, AV_CODEC_CONFIG_SAMPLE_FORMAT, 0,(const void**)&format, nullptr);

        if (audio_ctx == nullptr)return ARGS_ERROR;
        if (*format == AV_SAMPLE_FMT_NONE || map_palnner_to_packad[*format] == AV_SAMPLE_FMT_NONE)return UNNEED_SWR;

        swr_ctx = swr_alloc();
        if (!swr_ctx)return ALLOC_ERROR;

        AVChannelLayout out_ch_layout;
        out_ch_layout.nb_channels = audio_ctx->ch_layout.nb_channels;
        out_ch_layout.order = audio_ctx->ch_layout.order;
        out_ch_layout.u.mask = ~((~0) << audio_ctx->ch_layout.nb_channels);
        out_ch_layout.opaque = nullptr;
        if (swr_alloc_set_opts2(&swr_ctx, &out_ch_layout, map_palnner_to_packad[*format], audio_ctx->sample_rate, &audio_ctx->ch_layout, *format, audio_ctx->sample_rate, 0, nullptr))return UNKONW_ERROR;
        if (swr_init(swr_ctx))return INIT_ERROR;
        return SUCCESS;
    }

    RESULT PlayTool::sample_planner_to_packed(AVFrame* frame, uint8_t** data, int* linesize)
    {
        *linesize = swr_convert(swr_ctx, data, *linesize, frame->data, frame->nb_samples);
        if (*linesize < 0)return ARGS_ERROR;
        *linesize *= frame->ch_layout.nb_channels * sample_bit_size[frame->format];
        return SUCCESS;
    }

    RESULT PlayTool::init_sws(AVFrame* work, const AVPixelFormat dstFormat, const int dstW, const int dstH)
    {

        if (dstW == 0 || dstH == 0)
            sws_ctx = sws_getContext(work->width, work->height, (AVPixelFormat)work->format, work->width, work->height, dstFormat, SWS_FAST_BILINEAR, nullptr, nullptr, 0);
        else
            sws_ctx = sws_getContext(work->width, work->height, (AVPixelFormat)work->format, dstW, dstH, dstFormat, SWS_FAST_BILINEAR, nullptr, nullptr, 0);

        return SUCCESS;
    }

    inline void PlayTool::insert_queue(AVMediaType meidaType, ScopeAVFramePtr&& avf) noexcept
    {
        char* userdata = nullptr;
        if (insert_callback[meidaType] != nullptr)
            insert_callback[meidaType](avf, userdata);

        _play_stream_ctx[meidaType]._frame_queue.emplace(std::move(avf), userdata);

        avf = av_frame_alloc();
    }

    RESULT PlayTool::sws_scale_420P(ScopeAVFramePtr& data)
    {

        AVFrame* dst = av_frame_alloc();

        if (sws_scale_frame(sws_ctx, dst, data) != 0) {
            return RESULT::UNKONW_ERROR;
        }

        dst->pts = data->pts;
        av_frame_unref(data);
        data.reset(dst);
        return SUCCESS;
    }

    void PlayTool::seek_time(int64_t sec) noexcept
    {
		auto& audio_frame_queue = _play_stream_ctx[AVMEDIA_TYPE_AUDIO]._frame_queue;
		auto& video_frame_queue = _play_stream_ctx[AVMEDIA_TYPE_VIDEO]._frame_queue;

        if(_p_avfctx_input == nullptr)
            return;
        std::lock_guard lock(decode_mutex);

        {
            std::scoped_lock lock_queue(PacketQueue._mtx, audio_frame_queue._mtx, video_frame_queue._mtx);

            PacketQueue.clear();
            audio_frame_queue.clear();
            video_frame_queue.clear();

            avcodec_flush_buffers(_play_stream_ctx[AVMEDIA_TYPE_VIDEO]._codec_ctx);
            avcodec_flush_buffers(_play_stream_ctx[AVMEDIA_TYPE_AUDIO]._codec_ctx);

            if (avformat_seek_file(_p_avfctx_input, -1, INT64_MIN, sec * AV_TIME_BASE, sec * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD) < 0) {
                spdlog::error("seek failed");
            }

            this->avframe_work[AVMEDIA_TYPE_VIDEO].first->pts = 0;

        }

        PacketQueue._cv_could_push.notify_one();
        audio_frame_queue._cv_could_push.notify_one();
        video_frame_queue._cv_could_push.notify_one();

    }

    void PlayTool::close() noexcept
    {
        if (ThrRead.joinable()) 
            ThrRead.request_stop();
        if (ThrDecode.joinable()) 
            ThrDecode.request_stop();
        if (ThrPlay.joinable()) 
            ThrPlay.request_stop();

        auto& play_stream_video_ctx = _play_stream_ctx[AVMEDIA_TYPE_VIDEO]._frame_queue;
        auto& play_stream_audio_ctx = _play_stream_ctx[AVMEDIA_TYPE_AUDIO]._frame_queue;

        PacketQueue._is_closed = true;
        play_stream_video_ctx._is_closed = true;
        play_stream_audio_ctx._is_closed = true;

        PacketQueue._cv_could_push.notify_all();
        PacketQueue._cv_could_pop.notify_all();
        play_stream_audio_ctx._cv_could_push.notify_all();
        play_stream_audio_ctx._cv_could_pop.notify_all();
        play_stream_video_ctx._cv_could_push.notify_all();
        play_stream_video_ctx._cv_could_pop.notify_all();

        if (ThrRead.joinable()) {
            ThrRead.join();
        }
        if (ThrDecode.joinable()) {
            ThrDecode.join();
        }
        if (ThrPlay.joinable()) {
            ThrPlay.join();
        }

        PacketQueue._is_closed = false;
        play_stream_audio_ctx._is_closed = false;
        play_stream_video_ctx._is_closed = false;
    }


    RESULT PlayTool::start_read_thread() noexcept
    {
        ThrRead = std::jthread([this](std::stop_token st)->void
            {
                std::stop_callback end_read_callback(st, [this]()->void {
                    spdlog::info("success exit read thread");
                });

                int err = AVERROR(EAGAIN);
                while (st.stop_requested() == false)
                {

                    ScopeAVPacketPtr avp(av_packet_alloc());
                    {
                        std::unique_lock lock(this->decode_mutex);
                        err = av_read_frame(_p_avfctx_input, avp);
                        if ((err < 0) || (err == AVERROR_EOF)) {
                            std::this_thread::sleep_for(1ms);
                            continue;
                        }
                    }
                    PacketQueue.push(std::move(avp));
                }
            });
        spdlog::info(std::format("create read thread id: {}", ThrRead.get_id()));
        return SUCCESS;
    }

    RESULT PlayTool::start_decode_thread() noexcept
    {
        ThrDecode = std::jthread([&](std::stop_token st)->void
            {
               std::stop_callback end_read_callback(st, [this]()->void {
                   spdlog::info("success exit decode thread");
               });

                int err = AVERROR(EAGAIN);
                ScopeAVFramePtr avf = av_frame_alloc();

                while (st.stop_requested() == false)
                {
                    auto _avp_optioal = PacketQueue.pop();

                    if (_avp_optioal.has_value() == false) { 
                        std::this_thread::sleep_for(1ms);
                        continue; 
                    }
                    auto& avp = _avp_optioal.value();

                    if (avp == nullptr) {
                        std::this_thread::sleep_for(1ms);
                        continue;
                    }

                    AVMediaType media_type = _stream_index_to_type[avp->stream_index];
                    if (media_type == AVMEDIA_TYPE_VIDEO || media_type == AVMEDIA_TYPE_AUDIO)
                    {
                        {
                            std::unique_lock lock(this->decode_mutex);
                            while ((err = avcodec_send_packet(_play_stream_ctx[media_type]._codec_ctx, avp)) == AVERROR(EAGAIN))
                                std::this_thread::sleep_for(1ms);
                        }


                        while (st.stop_requested() == false)
                        {
                            err = avcodec_receive_frame(_play_stream_ctx[media_type]._codec_ctx, avf);
                            if (err == 0)insert_queue(media_type,std::move(avf));
                            else if (err == AVERROR(EAGAIN)) break;
                            else if (err == AVERROR_EOF) continue;
                            else return;
                        }
                    }
                    else {}
                }
            });
       spdlog::info(std::format("create decode thread id: {}", ThrDecode.get_id()));
        return SUCCESS;
    }

    RESULT PlayTool::start_encode_thread() noexcept
    {

        return RESULT();
    }

    RESULT PlayTool::start_pull_steam_thread() noexcept
    {

        return RESULT();
    }



}





