#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <windows.h>
#include "prototyse.h"
#include "videoEncoderManager.h"
#include "opencv2/opencv.hpp"

struct RecordInfo {
	int videoWidth = 1920;
	int videoHeight = 1080;
	int targetWidth = 1920;
	int targetHeight = 1080;
	int FPS = 30;
	int time = 30;
	int rate = 8;
	std::string picType = "BGR";
	std::string path = "D:/pano";
};

class display
{
public:
	display(int channelId, HWND winId);

	display::~display();

	int start();

	int stop();

	int clone(HWND winId, BYTE channelId);

	int unClone(BYTE channelId);

	int extInterface(ExtStruct extPara);

	int snapshot(char *path);

private:

	videoEncoderManager videoEncoderManager;

	std::string name;

	int channelId;

	HWND winId;

	std::unordered_map<int, HWND> clonedWin;

	void display::showPix(int category);

	RecordInfo info;

	std::vector<unsigned char> buffer;
	std::vector<unsigned char> srioBuffer;

	cv::Mat img;
	cv::Mat copy;
	std::string filename;
	std::mutex imgMtx;

	const int MAXORISIZE = 1024 * 1024 * 8;

	std::atomic_bool recordOn;

	std::atomic_bool displayOn;

	std::thread displayThread;
	std::atomic_bool quitDisplay;

	const std::unordered_map<int, int> channel2Srio = {
		{ 0, 1 },{ 1, 2 },{ 4, 3 }
	};

	const std::unordered_map<int, SourcePara> channel2Source = {
		{0, VISIBLE}, {1, INFRARED}, {4, UAV}
	};

	//std::atomic_bool quitSnapshot;
	//std::thread snapshotThread;
	//void snapshotLoop();
	//std::condition_variable snapshotCv;
	//std::mutex snapshotMtx;

};

#endif // DISPLAY_H
