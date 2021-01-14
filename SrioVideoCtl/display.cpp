#include "display.h"
#include <unordered_map>

#include "videoEncoderManager.h"
#include "srio.h"
#include "draw.h"

#include "spdlog/spdlog.h"

#define DEBUG(x, id) spdlog::get("srio")->debug("[{}] {} channelId={}", "display", x, id);
#define INFO(x, id) spdlog::get("srio")->info("[{}] {} channelId={}", "display", x, id);
#define SPDERROR(x, id) spdlog::get("srio")->error("[{}] {} channelId={}", "display", x, id);


display::display(int channelId, HWND winId)
	: channelId(channelId), winId(winId)
{

	buffer.resize(1920 * 1080 * 4);
	srioBuffer.resize(1920 * 1080 * 4);

	recordOn = false;
	displayOn = false;
	quitDisplay = true;
	//quitSnapshot = true;
	img = cv::Mat(540, 960, CV_8UC3, buffer.data() + 16);
}

display::~display()
{
	stop();
}

int display::start()
{
	displayOn = true;
	quitDisplay = false;
	displayThread = std::thread(&display::showPix, this, channelId);

	//quitSnapshot = false;
	//snapshotThread = std::thread(&display::snapshotLoop, this);

	return 0;
}

int display::stop()
{
	quitDisplay = true;
	if (recordOn)
	{
		recordOn = false;
		videoEncoderManager.stopRecord();
	}
	if (displayThread.joinable())
		displayThread.join();

	//quitSnapshot = true;
	//snapshotCv.notify_all();
	//if (snapshotThread.joinable()) {
	//	snapshotThread.join();
	//}

	return 0;
}

void display::showPix(int channelId)
{
	if (channelId != 0 && channelId != 1 && channelId != 4) {
		SPDERROR(fmt::format("showPix is called with invalid channelId {}", channelId), channelId);
		return;
	}
	int cap = channel2Srio.at(channelId);

	auto &srio = SRIO::getSrio();

	while (!quitDisplay) {
		static auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		static int frameCnt = 0;

		auto sendSize = srio.acceptImage(cap, srioBuffer.data(), MAXORISIZE);

		frameCnt++;
		if (frameCnt == 100) {
			auto stop = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			double fps = 100 * 1000.0 / (stop - start);
			DEBUG(fmt::format("FRAMES PER SECONDS: {}", fps), channelId);
		}

		{
			std::lock_guard<std::mutex> lck(imgMtx);
			//img = cv::Mat(540, 960, CV_8UC3, buffer.data() + 16);
			memcpy(buffer.data(), srioBuffer.data(), buffer.size());
			cv::cvtColor(img, img, cv::COLOR_BGRA2RGB);
		}

		if (recordOn)
		{
			videoEncoderManager.push(img.data);
		}
		if (displayOn) {
			DrawCVImage(img, winId);
			for (auto &p : clonedWin) {
				DrawCVImage(img, p.second);
			}
		}

	}
}

//void display::snapshotLoop()
//{
//	std::unique_lock<std::mutex> lck(snapshotMtx);
//	while (!quitSnapshot) {
//		snapshotCv.wait(lck);
//
//		cv::imwrite(filename, copy);
//	}
//}

int display::clone(HWND winId, BYTE channelId)
{
	INFO(fmt::format("clone channel {}", channelId), channelId);

	if (clonedWin.count(channelId)) {
		SPDERROR(fmt::format("cannot clone duplicated channel {}", channelId), channelId);
		return -1;
	}
	clonedWin[channelId] = winId;
}

int display::unClone(BYTE channelId)
{
	INFO(fmt::format("unClone channel {}", channelId), channelId);
	if (!clonedWin.count(channelId))
	{
		SPDERROR(fmt::format("cannot unclone unexisting channel {}", channelId), channelId);
		return -1;
	}
	clonedWin.erase(channelId);
}


int display::extInterface(ExtStruct extPara)
{
	auto tp = channel2Source.at(channelId);
	if (extPara.type == RECORD_START)
	{
		INFO("extInterface RECORD_START", channelId);
		if (extPara.para.recordSetting.sourceType != tp)
		{
			SPDERROR(fmt::format("extInterface sourceType mismatch! from para: {}, should be {}", extPara.para.recordSetting.sourceType, tp), channelId);
			return -1;
		}
		info.time = extPara.para.recordSetting.timeDuration * 60;
		info.path = extPara.para.recordSetting.path;
		info.FPS = extPara.para.recordSetting.encodePara.fRate;
		info.rate = extPara.para.recordSetting.encodePara.MBitRate / 1024 / 1024;
		info.targetWidth = extPara.para.recordSetting.encodePara.moiveScale.wScale;
		info.targetHeight = extPara.para.recordSetting.encodePara.moiveScale.hScale;
		videoEncoderManager.paraSet(960, 540, info.targetWidth, info.targetHeight, info.FPS, info.time, info.rate, info.picType, info.path);
		videoEncoderManager.startRecord();
		recordOn = true;
	}
	if (extPara.type == RECORD_STOP)
	{
		INFO("extInterface RECORD_STOP", channelId);

		if (extPara.para.recordSetting.sourceType != tp)
		{
			SPDERROR(fmt::format("extInterface sourceType mismatch! from para: {}, should be {}", extPara.para.recordSetting.sourceType, tp), channelId);

			return -1;
		}
		recordOn = false;
		videoEncoderManager.stopRecord();
	}
}

int display::snapshot(char * path)
{
	INFO(fmt::format("snapshot to {}", std::string(path)), channelId);
	static cv::Mat copy;
	{
		std::lock_guard<std::mutex> lck(imgMtx);
		img.copyTo(copy);
		//filename = path;
	}
	cv::imwrite(path, copy);
	//snapshotCv.notify_all();
	return 0;
}
