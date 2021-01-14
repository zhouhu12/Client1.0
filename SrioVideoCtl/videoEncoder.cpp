#include"videoEncoder.h"

ffmpegEncoder::ffmpegEncoder()
{
	saveStartFrameCount = 0;
	encoderState = false;
}

ffmpegEncoder::~ffmpegEncoder()
{
}

int ffmpegEncoder::openEncoder(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int bitratePara, std::string _frameType, std::string _savePath, int number)
{

	//首先判断信号来源是RGB或者GRAY
	if (_frameType == std::string("GRAY"))
	{
		srcCols = srcColsPara;
		srcRows = srcRowsPara;
		dstCols = dstColsPara;
		dstRows = dstRowsPara;
		FPS = FPSPara;
		bitrate = bitratePara;
		srcColsSwsScale = srcColsPara;
		src_pix_fmt = AV_PIX_FMT_GRAY8;
		dst_pix_fmt = AV_PIX_FMT_YUV420P;
	}
	else if (_frameType == "BGR")
	{
		srcCols = srcColsPara;
		srcRows = srcRowsPara;
		dstCols = dstColsPara;
		dstRows = dstRowsPara;
		FPS = FPSPara;
		bitrate = bitratePara;
		srcColsSwsScale = srcColsPara * 3;
		src_pix_fmt = AV_PIX_FMT_BGR24;
		dst_pix_fmt = AV_PIX_FMT_YUV420P;
	}

	//保存MP4文件全名

//windows
	cout << "get time" << endl;

	SYSTEMTIME t;
	GetLocalTime(&t);
	//linux
	//    struct timeval tv;
	//    struct tm *t;

	//    gettimeofday(&tv, NULL);
	//    t = localtime(&tv.tv_sec);


	cout << t.wYear << endl;
	string suffix = number == 0 ? "" : "-" + to_string(number);
	string videoSaveNameStr = _savePath + suffix + ".mp4";
	//+change(_savePath,number)+
//         /* +to_string(t.wYear)
//          +"_"+to_string(t.wMonth)
//          +"_"+to_string(t.wDay)
//          +"_"+to_string(t.wHour)
//          +"_"+to_string(t.wMinute)
//          +"_"+to_string(t.wSecond)
//          +"_"+to_string(t.wMilliseconds)*/
//     /*     +".mp4";*/
	char videoSaveName[200];
	strcpy_s(videoSaveName, videoSaveNameStr.c_str());


	//codec = avcodec_find_encoder(AV_CODEC_ID_H264);//查找编码器
	codec = avcodec_find_encoder_by_name("h264_nvenc"); //gpu


	c = avcodec_alloc_context3(codec);//用编码器初始化编码数据结构对象

	if (!c)
	{

		printf("Could not allocate video codec context\n");

		return 1;

	}



	pkt = av_packet_alloc();//初始化AVPacket
	outPkt = av_packet_alloc();

	if (!pkt)

		return 2;

	//设置编码属性

	c->bit_rate = bitrate * 1024 * 1024;//码率

	c->width = dstCols;//视频宽

	c->height = dstRows;//视频高

	c->time_base.den = FPS;//视频每秒帧数

	c->time_base.num = 1;

	c->framerate.den = 1;//视频帧速

	c->framerate.num = FPS;

	c->gop_size = 10;////设置GOP大小,该值表示每10帧会插入一个I帧

	c->max_b_frames = 1;//设置B帧最大数,该值表示在两个非B帧之间，所允许插入的B帧的最大帧数

	c->pix_fmt = dst_pix_fmt;//设置像素格式

	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	c->thread_count = 1;//可以限制打开几个线程


						//std::cout << "codec->type==" << codec->name << std::endl;

	if (codec->id == AV_CODEC_ID_H264)//设置编码器为非延时格式
	{
		if (std::string(codec->name) == "h264_nvenc")
		{
			av_opt_set(c->priv_data, "preset", "fast", 0);
			int opt = av_opt_set(c->priv_data, "gpu", "0", 0);
			//std::cout << "opt = " << opt << std::endl;
		}
		else
		{
			av_opt_set(c->priv_data, "preset", "superfast", 0);
			av_opt_set(c->priv_data, "tune", "zerolatency", 0);
		}
	}
	std::cout << "c-width" << c->width;

	ret = avcodec_open2(c, codec, NULL);//打开编码器


	if (ret < 0)
	{
		printf("Could not open codec");
		return 3;
	}


	ofmt_ctx = NULL;
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, videoSaveName);//初始化一个用于输出的AVFormatContext结构体，视频帧率和宽高在此函数里面设置

	AVStream* st = avformat_new_stream(ofmt_ctx, NULL);
	st->id = 0;
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(ofmt_ctx, 0, videoSaveName, 1);

	//设置待编码帧对象
	frame = av_frame_alloc();
	if (!frame)

	{

		printf("Could not allocate video frame\n");

		return 4;

	}

	frame->format = c->pix_fmt;

	frame->width = dstCols;

	frame->height = dstRows;

	ret = av_frame_get_buffer(frame, 32);


	if (ret < 0)

	{

		printf("Could not allocate the video frame data\n");

		return 5;

	}

	//初始化格式转换器
	//如果数据来源与输出格式宽高均一致则无需此项
	sws_ctx = sws_getContext(srcColsPara, srcRowsPara, src_pix_fmt, dstCols, dstRows, dst_pix_fmt, SWS_POINT, NULL, NULL, NULL);


	ret = avio_open(&ofmt_ctx->pb, videoSaveName, AVIO_FLAG_WRITE);//打开文件
	if (ret < 0)
	{
		printf("avio_open  failed!");
		return 6;
	}

	AVDictionary* opt = NULL;
	av_dict_set_int(&opt, "video_track_timescale", FPSPara, 0);

	ret = avformat_write_header(ofmt_ctx, &opt);//写文件头
	if (ret < 0)
	{
		printf("avformat_write_header  failed!\n");
		return 7;
	}
	encoderState = true;
	return 0;
}



int ffmpegEncoder::EncoderPerFrame(uint8_t* data)
{
	fflush(stdout);
	ret = av_frame_make_writable(frame);//检测帧对象是否可读
	if (ret < 0)
		return 1;
	if (data != NULL)
		sws_scale(sws_ctx, &data, &srcColsSwsScale, 0, srcRows, frame->data, frame->linesize);//像素格式转换
	frame->pts = pts++;
	//std::cout << "pts=" << pts << std::endl;
	//std::cout << "1AVFRAE PTS=" << frame->pts << std::endl;


	//if (count == 200)
	//{
	//	av_write_trailer(ofmt_ctx);
	//	avio_close(ofmt_ctx->pb);
	//	avformat_free_context(ofmt_ctx);
	//	ofmt_ctx = nullptr;

	//	openAnotherFile(1920, 1080, 1920, 1080, FPS, 60, ".\\testvideo\\pano");

	//	//pkt->pts = pkt->pts - 300;
	//	//pkt->dts = pkt->dts - 300;
	//}

	ret = avcodec_send_frame(c, frame);//帧送入编码器
	if (ret < 0)
	{
		//printf("ret=%d\n",ret);
		//system("pause");
		printf("Error sending a frame for encoding\n");
		return 2;
	}


	while (ret >= 0)
	{
		ret = avcodec_receive_packet(c, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			return 0;
		}
		else if (ret < 0)
		{
			printf("Error during encoding\n");
			return 3;
		}
		av_interleaved_write_frame(ofmt_ctx, pkt);
	}
	return 0;
}

int ffmpegEncoder::closeEncoder()
{
	if (encoderState)
	{
		pts = 0;
		av_write_trailer(ofmt_ctx);//写文件尾
		av_frame_free(&frame);
		av_packet_free(&pkt);
		avio_close(ofmt_ctx->pb);
		avcodec_close(c);
		avcodec_free_context(&c);
		sws_freeContext(sws_ctx);
		encoderState = false;
	}
	else
	{

	}
	return 0;
}

int ffmpegEncoder::EncodeEndFrame()//最后输入空帧刷新
{
	int ret;
	ret = avcodec_send_frame(c, NULL);
	while (ret >= 0) {
		ret = avcodec_receive_packet(c, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			//printf("pts=%dsize=%d\n", pkt->pts, pkt->size);
			return 0;
		}
		else if (ret < 0)
		{
			fprintf(stderr, "Error during encoding\n");
			return 1;
		}
		else
		{
			//fprintf(stderr, "Error encoding\n");
			return 1;
		}
		av_interleaved_write_frame(ofmt_ctx, pkt);
	}
	return 0;
}


int ffmpegEncoder::openAnotherFile(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int bitratePara, std::string _savePath)
{
	//	char videoSaveName[200];
	//	SYSTEMTIME sys;
	//	GetLocalTime(&sys);
	//	sprintf_s(videoSaveName, 200, "%s\\%d%02d%02d%02d%02d%02d%03d.mp4", _savePath.c_str(), sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

	//	if (avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, videoSaveName) >= 0)
	//	{
	//		AVStream* ostream = avformat_new_stream(ofmt_ctx, NULL);
	//		ostream->id = 0;
	//		ostream->codecpar->codec_tag = 0;
	//		avcodec_parameters_from_context(ostream->codecpar, c);
	//		av_dump_format(ofmt_ctx, 0, videoSaveName, 1);//create output file

	//		if (avio_open(&ofmt_ctx->pb, videoSaveName, AVIO_FLAG_WRITE) != 0)
	//		{
	//			//LOGERROR("startRecord:save file open error");
	//			return false;
	//		}
	//		AVDictionary* opt = NULL;
	//		av_dict_set_int(&opt, "video_track_timescale", 30, 0);

	//		if (avformat_write_header(ofmt_ctx, &opt) < 0)
	//		{
	//			//LOGERROR("startRecord:save file write header error");
	//			return false;
	//		}
	//		saveStartFrameCount = frame->pts;
	//		//_config.saveFileFlag = true;
	//		//LOGINFO("startRecored:%s", filename.c_str());
	//		return true;
	//	}
	return false;
}

