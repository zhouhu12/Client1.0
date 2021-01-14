#pragma once
#include<string>
#include<vector>
#include<queue>
#include <ctime>

#include <sys/stat.h>
#include <sys/types.h>

#include<iostream>
#include<thread>
#include<mutex>

#include<opencv2/opencv.hpp>

#include <windows.h>
#include<direct.h>
#include<io.h>

#include"videoEncoder.h"

using namespace std;

class  videoEncoderManager
//class videoEncoderManager
{
public:
	videoEncoderManager();
	~videoEncoderManager();

	mutex *getEncoderMutex();

    /*
    成功返回0，错误返回错误号
    1：未启动编码器
    2：打开编码器失败
    3：编码该帧失败
    */
    int push(unsigned char *buf);
	int paraSet(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int segmentationTimePara, int bitratye,std::string _frameType, std::string _savePath);
	int startRecord();
	int stopRecord();

private:
	int deleteEarlyVideo();
	void getFiles(std::string path, std::string exd, std::vector<std::string>& files);
    bool createSavePath(std::string savePath);//创建保存视频所用的文件夹

	std::queue<cv::Mat> compressQueue;
	mutex encoderQueueLock;


	bool ifstart;
	int srcCols;
	int srcRows;
	int dstCols;
	int dstRows;
	int srcColsEven;
	int srcRowsEven;
	int dstColsEven;
	int dstRowsEven;
	int FPS;
	int segmentationTime;//分段时长/s
	int bitrate;
	std::string savePath;//保存MP4文件的目录名
	char videoSaveName[200];//保存MP4文件全名

	bool srcIsGRAY;//用于判断输入图像的类型。
	bool flagRunEncoder = false;
	int encoderCount;
    bool isEncoderBeenOpen = false;//编码器中存在至少一张被编码的图像

	std::thread th;

	ffmpegEncoder Encoder1;

	//录像次数
	int num=0;
};

