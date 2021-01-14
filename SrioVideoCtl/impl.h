#pragma once

#include "SrioVideoCtl.h"
#include "display.h"

class CapBoardListenerImpl : public CapBoardListner {
public:
	int linkStatusReportProc(unsigned int destId, unsigned int channelID, int state) { return 0; }

	int VideoFormatDetectedProc(unsigned int destID, unsigned int channelID, ULONG nVideoWidth, ULONG nVideoHeight, ULONG frameStoreType,
		ULLONG duration, ULONG frameRate) {
		return 9;
	}

	int frameReachedProc(unsigned int destID, unsigned int channelID, char*pFrameBuffer, ULONG nFrameBufferLen) { return 0; }

	int stopCapFinishedProc(unsigned int destID, unsigned int channelID) { return 0; }

	int recvTargetRecognizationResultProc(unsigned int destId, unsigned int channelID, BYTE targetNum, TargetInfo *targetInfo) { return 0; }

	int freshTrackStatusProc(unsigned int destId, unsigned int channelID, unsigned int x, unsigned int y, unsigned int trackStatus) { return 0; }

	int callBackMsg(unsigned int destID, unsigned int channelID, unsigned int messageType, unsigned int messageLegnth, char *messageData) { return 0; }
};


class BoardCaptureImpl : public BoardCapture {
public:
	BoardCaptureImpl(int channelId, HWND winId);

	unsigned int getLocalDevice() { return 0; }

	unsigned int getDestDevice() { return 0; }

	int init();

	int uninit();

	int swtichVideoType(BYTE videoType) { return 0; }

	int setOverlapCharacters(ULONG index, bool bOn, ULONG x, ULONG y, char* characters, ULONG length);

	int setImageEnhancementSwitch(bool on) { return 0; }

	int start();

	int stop();

	int clone(HWND winId, BYTE cloneId);

	int unClone(BYTE cloneId);

	int setMagnification(BYTE magnification) { return 0; }

	int setFogProcSwitch(bool on) { return 0; }

	int setViedoStabilizationSwitch(bool on) { return 0; }

	int setImageStitchingSwitch(bool on, int isc, unsigned int ext) { return 0; }

	int setTargetDetectSwitch(bool on) { return 0; }

	int setTargetRecognizeSwitch(bool on) { return 0; }

	int setTargetTrackMode(BYTE mode, bool on, unsigned int x, unsigned int y, unsigned int targetID) { return 0; }

	int setImageFusionSwitch(bool on, float KJGFocalDistance, float HWFocalDistance) { return 0; }

	int setRecordSettings(BYTE videoType, ULLONG MBitRate, ULONG timeDuration, int WScale, int HScale, int codecType = 0, int fileType = 0) { return 0; }

	int startRecord(char* path) { return 0; }

	int stopRecord() { return 0; }

	int snapshot(char* path);

	int setCross(bool on, unsigned int centerX, unsigned int centerY) { return 0; }

	int selectEth(unsigned int srcIp, unsigned int srcPort, unsigned int destIp, unsigned int destPort) { return 0; }

	int extInterface(ExtStruct extPara);

	~BoardCaptureImpl();

private:

	display* disp;
};
