#include "impl.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>

#include "srio.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"




class LOG {
public:
	LOG() {
		auto logger = spdlog::rotating_logger_mt("srio", "log/SrioVideoCtlRotating.txt", 2 * 1024 * 1024, 3);
		spdlog::flush_on(spdlog::level::info);
		spdlog::get("srio")->set_level(spdlog::level::debug);
	}

	~LOG() {
		spdlog::get("srio")->flush();
	}
};

#define DEBUG(x) spdlog::get("srio")->debug("[{}] {}", "BoardCaptureImpl", x)
#define INFO(x) spdlog::get("srio")->info("[{}] {}", "BoardCaptureImpl", x)
#define SPDERROR(x) spdlog::get("srio")->error("[{}] {}", "BoardCaptureImpl", x)


//void DEBUG(const std::string &x) {
//	spdlog::get("srio")->debug("[{}] {}", "BoardCaptureImpl", x);
//}

BoardCaptureImpl::BoardCaptureImpl(int channelId, HWND winId) : disp(new display(channelId, winId)) {}

int BoardCaptureImpl::init() {
	return 0;
}

int BoardCaptureImpl::uninit() {
	return 0;
}

int BoardCaptureImpl::setOverlapCharacters(ULONG index, bool bOn, ULONG x, ULONG y, char * characters, ULONG length)
{
	return 0;
}

int BoardCaptureImpl::start() {
	INFO("start is called.");
	disp->start();
	return 0;
}

int BoardCaptureImpl::stop() {
	INFO("stop is called.");
	disp->stop();
	return 0;
}

int BoardCaptureImpl::clone(HWND winId, BYTE cloneId) {
	INFO(fmt::format("clone is called with winId={}, cloneId={}", (int)winId, cloneId));
	return disp->clone(winId, cloneId);
}

int BoardCaptureImpl::unClone(BYTE cloneId) {
	INFO(fmt::format("unClone is called with cloneId={}", cloneId));
	return disp->unClone(cloneId);
}

int BoardCaptureImpl::snapshot(char* path) {
	//static std::ofstream f("./SrioVideoCtlRun.log", std::fstream::out);
	//f << "path: " << path;
	//f.close();
	INFO(fmt::format("snapshot is called with path={}", std::string(path)));
	return disp->snapshot(path);
}

int BoardCaptureImpl::extInterface(ExtStruct extPara) {
	INFO("extInterface is called.");
	INFO(fmt::format("extPara.type={}, extPara.para.recordSetting:", extPara.type));
	auto recordSetting = extPara.para.recordSetting;
	INFO(fmt::format("\tsourceType={}, path={}, timeDuration={}, fileType={}, videoType={}",
		recordSetting.sourceType, std::string(recordSetting.path), recordSetting.timeDuration,
		recordSetting.fileType, recordSetting.videoType));
	auto encodePara = recordSetting.encodePara;
	INFO("extPara.para.recordSetting.encodePara: ");
	INFO(fmt::format("\tMBitRate={}, movieScale.wScale={}, movieScale.hScale={}, encodeType={}, fRate={}",
		encodePara.MBitRate, encodePara.moiveScale.wScale, encodePara.moiveScale.hScale, encodePara.encodeType, encodePara.fRate));
	return disp->extInterface(extPara);
}

BoardCaptureImpl::~BoardCaptureImpl() {
	INFO("Destructor is called");
	delete disp;
}

BoardCapture *createBoardCapture(int localId, unsigned int localPort, unsigned int destId, unsigned int channelID, unsigned int videoType, CapBoardListner *listner, PVOID winId) {
	//static std::ofstream f("./SrioVideoCtlRun.log", std::fstream::out);
	//f << "localId: " << localId << " localPort: " << localPort << " destId: " << destId << " channelId: " << channelID << " videoType: " << videoType << " listener: " << listner << " winId: " << winId;
	//f.close();

	static LOG log;
	INFO(fmt::format("createBoardCapture is called with localId={}, localPort={}, destId={}, channelId={}, videoType={}, listner={}, winId={}",
		localId, localPort, destId, channelID, videoType, (int)listner, (int)winId));

	auto p = new BoardCaptureImpl(channelID, (HWND)winId);

	static bool srioInited = false;
	if (!srioInited) {
		//DEBUG("SRIO initialized.");
		spdlog::get("srio")->debug("[{}] {}", "BoardCaptureImpl", "SRIO intialized.");
		auto &srio = SRIO::getSrio();
		srio.bindHostId(0x06);
		srio.listenDoorbell();
		srio.connect(0x0a);
		srioInited = true;
	}

	return p;
}

int destroyBoardCapture(BoardCapture *boardCap) {
	INFO("destroyBoardCapture is called.");
	delete boardCap;
	return 0;
}
