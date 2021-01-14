#pragma once
#include <Windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "tsi721ioctl.h"
#include "tsi721api.h"
#include "srioCommon.h"

#define SRIOCAPTRUE_BUF_SIZE (6*1024*1024)

class SRIOCapture{
public:
	SRIOCapture();
	SRIOCapture(HANDLE device, DWORD channel, DWORD bufSize = SRIOCAPTRUE_BUF_SIZE);
	~SRIOCapture();

	void updateDoorbell(SRIODoorbell doorbell);
	DWORD channel();
	PVOID data();


private:
	HANDLE					  m_hDev;
	DWORD					  m_channelId;
							  
	PVOID					  m_buffer;
	DWORD					  m_bufferSize;
	std::mutex				  m_bufferMutex;
	std::condition_variable   m_bufferCV;
							  
	SRIODBInfo				  m_dbInfo;
	std::mutex				  m_dbMutex;
	std::condition_variable   m_dbCV;
							  
	std::atomic<bool>		  m_dbThreadRunning;
	std::thread				  m_dbThread;
	

	void	_dbThread();
	DWORD	_srioRead(DWORD r2pWinNum, DWORD offset, PVOID buf, DWORD bufSize);
};
