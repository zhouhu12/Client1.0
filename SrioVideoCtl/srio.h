#pragma once
#include <Windows.h>
#include <winioctl.h>
#include <SetupAPI.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <atomic>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>

#include "srioCaptrue.h"

#define SRIOCOM_VIS_IMG_BUF_SIZE			(4*1024*1024)
#define SRIOCOM_INF_IMG_BUF_SIZE			(3*1024*1024)
#define SRIOCOM_UAV_IMG_BUF_SIZE			(4*1024*1024)
#define SRIOCOM_CMD_BUF_SIZE				(1*1024*1024)
#define SRIOCOM_CHANNEL_MAX_CNT				8

class SRIO {
public:
	~SRIO();
	static SRIO&  getSrio();
	void		  bindHostId(DWORD hostId);
	void		  connect(DWORD destId);
	DWORD		  getHostId();
	void		  listenDoorbell();
	void		  enableImageChannel(DWORD localId, DWORD channelId, bool enable);
	void		  close();

	DWORD		  writeCommand(PVOID buf, DWORD bufSize, bool isServer);
	DWORD		  acceptCommand(PVOID buffer, DWORD bufSize);

	DWORD		  writeImage(BYTE type, BYTE encoding, PVOID buffer, DWORD bufSize);
	DWORD		  acceptImage(BYTE type, PVOID buffer, DWORD bufSize); 

private:
	SRIO();
	void	_init();
	void	_uinit();
	DWORD	_srioRead(DWORD r2pWinNum, DWORD offset, PVOID buf, DWORD bufSize);
	DWORD	_srioWrite(DWORD destId, DWORD addr, DWORD doorbell, PVOID buf, DWORD bufSize);
	void	_doorBellThread();
	DWORD	_srioSendDoorbell(DWORD destId, DWORD doorbell);
	void	_processDoorbell(PVOID pData, ULONG dbCount);

	bool	m_init;
	DWORD	m_devNum;
	HANDLE	m_hDev;
	DWORD	m_hostId;
	DWORD	m_destId;

	SRIOR2pWin					m_r2pWins[SRIO_IBWIN_MAXNUM];
	SRIOCapture*				m_srioCaptures[SRIOCOM_CHANNEL_MAX_CNT];
	bool						m_srioCaptureInit[SRIOCOM_CHANNEL_MAX_CNT];
	std::atomic<DWORD>			m_srioCaptureNums;
	

	std::thread					m_doorbellThread;
	std::atomic<bool>			m_doorbellThreadRunning;
	HANDLE						m_dbEvent;

};
