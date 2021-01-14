#pragma once

#ifndef INT64_C 

#define INT64_C(c) (c ## LL) 

#define UINT64_C(c) (c ## ULL) 

#endif 



#ifdef __cplusplus 

extern "C"

{

#endif 

	/*Include ffmpeg header file*/
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 
#include <libavutil/imgutils.h>  
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>

#include <windows.h>

#ifdef __cplusplus 

}

#endif 

#include<iostream>
#include<string>
#include<time.h>
using namespace std;

class ffmpegEncoder
{
public:
	ffmpegEncoder();
	~ffmpegEncoder();

	int openEncoder(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara,int bitratePara,std::string _frameType, std::string _savePath,int number);
	/*
	成功返回0，错误返回错误号
	1：初始化AVCodecContext编码数据结构失败
	2：初始化AVPacket失败
	3：打开编码器失败
	4：设置编码帧失败
	5：分配编码帧空间失败。
	6：打开文件时便
	7：写文件头失败
	*/
	int EncoderPerFrame(uint8_t* data);
	//正确返回0，
	//1：Frame不可读
	//2：Frame不可编码
	//3：Frame编码错误
	//4：送入空帧刷新时错误
	int EncodeEndFrame();
	int closeEncoder();
	int openAnotherFile(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int bitratePara, std::string _savePath);


private:

	//编码器相关
	AVCodec* codec;//编码器
	AVCodecContext* c = NULL;//编码数据结构对象
	AVFrame* frame;//帧
	AVPacket* pkt;//数据包
	AVPacket* outPkt;//数据包
	AVFormatContext* ofmt_ctx; //输出数据结构
	int ret;//返回值
	struct SwsContext* sws_ctx;//格式转换器
	enum AVPixelFormat src_pix_fmt;//格式对象s
	enum AVPixelFormat dst_pix_fmt;//格式对象d

    bool encoderState;//编码器是否开启。
	int pts = 0;//时间戳对象
	int savepts = 0;

	int count = 0;

	int srcCols;
	int srcRows;
	int dstCols;
	int dstRows;
	int FPS;
	int srcColsSwsScale;//这个参数用于尺寸转换时的buffer size确认，如果是RGB，则为srcColsPara*3.
	int bitrate;

	double saveStartFrameCount;
};
