#include <iostream>
#include <exception>
#include "srio.h"



SRIO::SRIO() {
	m_init = false;
	m_hDev = INVALID_HANDLE_VALUE;
	m_devNum = 0;
	m_hostId = 0xff;
	m_destId = 0xff;
	for (int i = 0; i < SRIOCOM_CHANNEL_MAX_CNT; i++) {
		m_srioCaptures[i] = NULL;
		m_srioCaptureInit[i] = false;
	}
	_init();
	m_srioCaptures[0] = new SRIOCapture(m_hDev, 0, SRIOCOM_CMD_BUF_SIZE);
	m_srioCaptureInit[0] = true;
	m_srioCaptureNums = 1;
}

SRIO::~SRIO() {
	for (int i = 0; i < m_srioCaptureNums; i++) {
		free(m_srioCaptures[i]);
	}

	if (m_init) {
		_uinit();
	}
	
}

SRIO& SRIO::getSrio() {
	static SRIO srio;
	return srio;
}

DWORD SRIO::getHostId() {
	DWORD curId;
	DWORD dwErr;
	dwErr = TSI721GetLocalHostId(m_hDev, &curId);
	if (ERROR_SUCCESS != dwErr) {
		printf_s("ERROR(SRIO) GetLocalHostId error, code: 0x%x\n", dwErr);
		return -1;
	}
	m_hostId = curId & 0xff;
	return m_hostId;
}

void SRIO::bindHostId(DWORD hostId) {
	DWORD dwErr;
	dwErr = TSI721SetLocalHostId(m_hDev, hostId);
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) Set Local Host ID Failed, code: 0x%x\n", dwErr);
	}
	m_hostId = hostId & 0xff;
}

void SRIO::connect(DWORD destId) {
	m_destId = destId;
}

void SRIO::listenDoorbell() {
	if (!m_init) {
		_init();
	}
	m_doorbellThreadRunning = true;
	m_doorbellThread = std::thread(&SRIO::_doorBellThread, this);
	if (!m_doorbellThread.joinable()) {
		printf_s("ERROR(doorbellThread) start doorbell thread error\n");
		exit(0);
	}

}

DWORD SRIO::writeCommand(PVOID buf, DWORD dataSize, bool isServer) {
	DWORD dwRetSize;
	SRIOData srioData(0, 0, buf, dataSize);
	DWORD offset = srioData._totalSize - srioData._dataSize;
	if (0xff == m_destId) {
		printf_s("ERROR(writeCommand) please connect to destId first\n");
		return -1;
	}
	if (!isServer) {
		dwRetSize = _srioWrite(m_destId, SRIOCOM_COMMAND_ADDRESS_1, SRIOCOM_COMMAND_DOORBELL_1, srioData._buf, srioData._totalSize);
	}
	else {
		dwRetSize = _srioWrite(m_destId, SRIOCOM_COMMAND_ADDRESS_2, SRIOCOM_COMMAND_DOORBELL_2, srioData._buf, srioData._totalSize);
	}

	if (-1 == dwRetSize) {
		return -1;
	}
	if (dwRetSize < offset) {
		printf_s("ERROR(srioWrite) write no data\n");
		return -1;
	}
	return dwRetSize - offset;
}

DWORD SRIO::acceptCommand(PVOID buf, DWORD bufSize) {
	PVOID ibBuf = NULL;
	DWORD ibdataSize, dwRetSize = 0;
	{
		SRIOData* ibData = (SRIOData*)m_srioCaptures[0]->data();
		ibBuf = &ibData->_buf;
		ibdataSize = ibData->_dataSize;
		if (bufSize < ibdataSize) {
			memcpy(buf, ibBuf, bufSize);
			dwRetSize = bufSize;
			printf_s("WARNING: The size of buffer you provided is not adequate for inbound data.");
			printf_s(" Buffer is truncated to suit yours\n");
		}
		else {
			memcpy(buf, ibBuf, ibdataSize);
			dwRetSize = ibdataSize;
		}
	}
	return dwRetSize;
}

DWORD SRIO::writeImage(BYTE type, BYTE encoding, PVOID buf, DWORD dataSize) {
	DWORD dwRetSize = 0;
	SRIOData srioData(type, encoding, buf, dataSize);
	DWORD offset = srioData._totalSize - srioData._dataSize;
	if (0xff == m_destId) {
		printf_s("ERROR(writeCommand) please connect to destId first\n");
		return -1;
	}
	switch(type)
	{
	case 1:
		dwRetSize = _srioWrite(m_destId, CARDCAPTURE_SDI1_ADDRESS_1, CARDCAPTURE_SDI1_DOORBELL_1, srioData._buf, srioData._totalSize);
		break;
	case 2:
		dwRetSize = _srioWrite(m_destId, CARDCAPTURE_SDI2_ADDRESS_1, CARDCAPTURE_SDI2_DOORBELL_1, srioData._buf, srioData._totalSize);
		break;
	case 3:
		dwRetSize = _srioWrite(m_destId, CARDCAPTURE_SDI5_ADDRESS_1, CARDCAPTURE_SDI5_DOORBELL_1, srioData._buf, srioData._totalSize);
		break;
	default:
		printf_s("ERROR(srioWrite) not supported type, write no data\n");
		dwRetSize = 0;
		break;
	}
	if (-1 == dwRetSize) {
		return -1;
	}
	if (dwRetSize < offset) {
		printf_s("ERROR(srioWrite) write no data\n");
		return -1;
	}
	return dwRetSize - offset;
}

DWORD SRIO::acceptImage(BYTE type, PVOID buf, DWORD bufSize) {
	PVOID ibBuf = NULL;
	DWORD ibdataSize, dwRetSize = 0;
	if (type < 0 || type > 3) {
		printf_s("ERROR(SRIOCOM) no supported accept type\n");
		return -1;
	}
	if (!m_srioCaptureInit[type]) {
		if (type == 1) {
			m_srioCaptures[type] = new SRIOCapture(m_hDev, type, SRIOCOM_VIS_IMG_BUF_SIZE);
		}
		else if (type == 2) {
			m_srioCaptures[type] = new SRIOCapture(m_hDev, type, SRIOCOM_INF_IMG_BUF_SIZE);
		}
		else if (type == 3) {
			m_srioCaptures[type] = new SRIOCapture(m_hDev, type, SRIOCOM_UAV_IMG_BUF_SIZE);
		}
		m_srioCaptureInit[type] = true;
		m_srioCaptureNums++;
	}

	//去掉头部4字节0xffeebbaa
	SRIOData* ibData = (SRIOData*)m_srioCaptures[type]->data();
	ibBuf = (PUCHAR)ibData + 4;
	ibdataSize = ibData->_totalSize - 4;
	if (bufSize < ibdataSize) {
		memcpy(buf, ibBuf, bufSize);
		dwRetSize = bufSize;
		printf_s("WARNING: The size of buffer you provided is not adequate for inbound data.");
		printf_s(" Buffer is truncated to suit yours\n");
	}
	else {
		memcpy(buf, ibBuf, ibdataSize);
		dwRetSize = ibdataSize;
	}
	return dwRetSize;
}

void SRIO::enableImageChannel(DWORD localId, DWORD channelId, bool enable) {
	DWORD dwDataSize;
	unsigned char data[16] = { 0 };
	data[0] = 0x10;
	data[1] = 0x4B;
	data[2] = 0x10;
	data[3] = 0x00;
	data[4] = 0x02;
	data[5] = channelId & 0xff;
	data[6] = enable ? (unsigned char)0x01 : (unsigned char)0x00;
	data[7] = enable ? (unsigned char)0x01 : (unsigned char)0x00;
	data[8] = 0x00;
	data[9] = localId & 0xff;
	data[10] = (channelId + 1) & 0xff;
	data[15] = getCheckSum(data, 16);

	dwDataSize = _srioWrite(0x1e, 0x000ff000, 0x0100, data, 16);
	if (-1 == dwDataSize) {
		printf_s("ERROR(enableChannel) enable image channel failed\n");
		return;
	}

}

void SRIO::close() {

	enableImageChannel(m_hostId, 0, false);
	enableImageChannel(m_hostId, 1, false);
	enableImageChannel(m_hostId, 4, false);

	_uinit();
	m_init = false;
}

void SRIO::_init() {
	DWORD dwErr;
	// device open
	if (!TSI721DeviceOpen(&m_hDev, m_devNum, NULL)) {
		printf_s("ERROR(SRIO) Unable to open device tsi721_%d\n", m_devNum);
		_uinit();
		exit(0);
	}

	// set HOST, MAST_EN & DISC 
	dwErr = TSI721RegisterWrite(m_hDev, RIO_PORT_GEN_CTRL_CSR, 0xe0000000);
	if (ERROR_SUCCESS != dwErr) {
		printf_s("ERROR(SRIO) Unable to set HOST, MAST_EN & DISC, 0x%08x\n", dwErr);
		_uinit();
		exit(0);
	}

	dwErr = TSI721PortWriteEnable(m_hDev, TRUE);
	if (dwErr) {
		printf_s("ERROR(SRIO) TSI721PortWriteEnable() returned = %d (0x%x)\n", dwErr, dwErr);
		_uinit();
		exit(0);
	}

	// free all ibWin
	for (int i = 0; i < SRIO_IBWIN_MAXNUM; ++i) {
		TSI721FreeR2pWin(m_hDev, i);
		m_r2pWins[i]._used = false;
	}

	// Initialize inbound SRIO-to-PCIe window (uses IB_WIN number 0)
	for (int i = 0; i <= 3; i++) {
		initR2pWin(m_hDev, i, SRIOR2pWinBaseAddr[i], SRIO_DMA_BUF_SIZE);
		m_r2pWins[i]._used = true;
		m_r2pWins[i]._winNum = i;
		m_r2pWins[i]._winBaseAddrLo = SRIOR2pWinBaseAddr[i];
		m_r2pWins[i]._winSize = SRIO_DMA_BUF_SIZE;
	}
	m_init = true;
}

void SRIO::_uinit() {
	m_init = false;

	SetEvent(m_dbEvent);
	if (m_doorbellThread.joinable()) {
		m_doorbellThread.join();
	}
	m_doorbellThreadRunning = false;



	for (int i = 0; i < SRIO_IBWIN_MAXNUM; ++i) {
		TSI721FreeR2pWin(m_hDev, i);
	}
	TSI721PortWriteEnable(m_hDev, FALSE);
	TSI721DeviceClose(m_hDev, NULL);
}

DWORD SRIO::_srioRead(DWORD r2pWinNum, DWORD offset, PVOID buf, DWORD bufSize){
	DWORD dwErr;
	DWORD dwBufSize;

	dwBufSize = bufSize;
	dwErr = TSI721IbwBufferGet(m_hDev, r2pWinNum, offset, buf, &dwBufSize); // ???dwBufSize?????
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) IbwBufferGet error, code: 0x%x\n", dwErr);
		return -1;
	}

	return 0;
}

DWORD SRIO::_srioWrite(DWORD destId, DWORD addr, DWORD doorbell, PVOID buf, DWORD bufSize) {
	DWORD dwErr;
	DWORD dwDataSize;
	DMA_REQ_CTRL dmaCtrl;
	dmaCtrl.bits.Iof = 0;
	dmaCtrl.bits.Crf = 0;
	dmaCtrl.bits.Prio = 0;
	dmaCtrl.bits.Rtype = ALL_NWRITE; // Last packet NWRITE_R, all other NWRITE or SWRITE
	dmaCtrl.bits.XAddr = 0; // bits 65:64 of SRIO address

	if (bufSize > SRIO_DMA_BUF_SIZE_HALF) {
		printf_s("ERROR(SRIO) size of output buffer(%d bytes) shoule not be larger than %d bytes\n", bufSize, SRIO_DMA_BUF_SIZE_HALF);
		return -1;
	}

	dwDataSize = bufSize;
	dwErr = TSI721SrioWrite(m_hDev, destId, 0, addr, buf, &dwDataSize, dmaCtrl);
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) TSI721SrioWrite Failed, err = 0x%x\n", dwErr);
		return -1;
	}
	if (-1 == _srioSendDoorbell(destId, doorbell)) {
		return -1;
	}
	if (dwDataSize > bufSize) {
		printf_s("ERROR(SRIO) TSI721SrioWrite did not write enough data\n");
		return -1;
	}

	return bufSize - dwDataSize;
}

void SRIO::_doorBellThread() {
	HANDLE hDev = m_hDev;
	DWORD ulRetSize;
	DWORD ulDbNum;
	OVERLAPPED ovl;
	HANDLE hWait[2];
	DWORD dwWaitRet;
	IB_DB_ENTRY ibDbBuf[16];
	DWORD dwError;

	memset(&ovl, 0, sizeof(ovl));

	m_dbEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hWait[0] = ovl.hEvent;
	hWait[1] = m_dbEvent;

	while (m_doorbellThreadRunning)
	{
		ulDbNum = 0;
		dwError = TSI721SrioIbDoorbellWait(hDev, ibDbBuf, sizeof(ibDbBuf), &ulRetSize, &ovl);
		if (ERROR_IO_PENDING == dwError) {
			BOOL bResult = FALSE;

			dwWaitRet = WaitForMultipleObjects(2, hWait, FALSE, INFINITE);
			if (WAIT_OBJECT_0 == dwWaitRet)
			{
				bResult = GetOverlappedResult(hDev, &ovl, &ulRetSize, FALSE);
				if (!bResult)
					break;
			}
			else {
				CancelIo(hDev);
				break;
			}
		}
		else if (ERROR_SUCCESS == dwError) {}
		else {
			break;
		}

		ulDbNum = ulRetSize / sizeof(IB_DB_ENTRY);
		if (ulDbNum)
		{
			_processDoorbell(ibDbBuf, ulDbNum);
		}
	}

	CloseHandle(ovl.hEvent);
	CloseHandle(m_dbEvent);
	m_dbEvent = NULL;
	m_doorbellThreadRunning = false;
}

void SRIO::_processDoorbell(PVOID pData, ULONG dbCount) {
	PIB_DB_ENTRY dbE = (PIB_DB_ENTRY)pData;
	DWORD i;

	for (i = 0; i < dbCount; i++) {
		if (dbE->db.DstId != m_hostId) {
			continue;
		}
		SRIODoorbell db(dbE->db.SrcId, dbE->db.DstId, dbE->db.Info);
		for (UINT j = 0; j < SRIOCOM_CHANNEL_MAX_CNT; j++) {
			if (m_srioCaptures[j]) {
				m_srioCaptures[j]->updateDoorbell(db);
			}
		}
		dbE++;
	}
	return;
}

DWORD SRIO::_srioSendDoorbell(DWORD destId, DWORD doorbell) {
	DWORD dwErr;
	dwErr = TSI721SrioDoorbellSend(m_hDev, destId, doorbell);
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) SrioDoorbellSend error code: 0x%x\n", dwErr);
		return -1;
	}
	return 0;
}

unsigned char getCheckSum(const unsigned char* data, int length)
{
	if (data != 0)
	{
		int sum = 0;
		for (int i = 0; i < length - 1; ++i)
			sum += data[i];

		return (unsigned char)(sum & 0xFF);
	}

	return 0;
}

DWORD getR2pWinInfo(DWORD doorbell) {
	DWORD dwR2pWinNum = 0xff;
	switch (doorbell) {
	case SRIOCOM_COMMAND_DOORBELL_1:
	case SRIOCOM_COMMAND_DOORBELL_2:
		dwR2pWinNum = 0;
		break;
	case CARDCAPTURE_SDI1_DOORBELL_1:
	case CARDCAPTURE_SDI1_DOORBELL_2:
		dwR2pWinNum = 1;
		break;
	case CARDCAPTURE_SDI2_DOORBELL_1:
	case CARDCAPTURE_SDI2_DOORBELL_2:
		dwR2pWinNum = 2;
		break;
	case CARDCAPTURE_SDI5_DOORBELL_1:
	case CARDCAPTURE_SDI5_DOORBELL_2:
		dwR2pWinNum = 3;
		break;
	default:
		printf_s("ERROR(getR2pWinInfo) Doorbell matched no r2pWin\n");
		return -1;
	}
	return dwR2pWinNum;
}

DWORD getChannel(DWORD doorbell) {
	DWORD dwChannel = 0xff;
	switch (doorbell) {
	case SRIOCOM_COMMAND_DOORBELL_1:
	case SRIOCOM_COMMAND_DOORBELL_2:
		dwChannel = 0;
		break;
	case CARDCAPTURE_SDI1_DOORBELL_1:
	case CARDCAPTURE_SDI1_DOORBELL_2:
		dwChannel = 1;
		break;
	case CARDCAPTURE_SDI2_DOORBELL_1:
	case CARDCAPTURE_SDI2_DOORBELL_2:
		dwChannel = 2;
		break;
	case CARDCAPTURE_SDI5_DOORBELL_1:
	case CARDCAPTURE_SDI5_DOORBELL_2:
		dwChannel = 3;
		break;
	default:
		printf_s("ERROR(getChannel) Doorbell matched no channel\n");
		return -1;
	}
	return dwChannel;
}

void initR2pWin(HANDLE device, DWORD r2pWinNum, DWORD addrLo, DWORD WinBufSize) {
	R2P_WINCFG r2pWinCfg;
	DWORD dwErr;
	memset(&r2pWinCfg, 0, sizeof(R2P_WINCFG));
	r2pWinCfg.BAddrHi = 0;   // Upper part of base address of R2P window
	r2pWinCfg.BAddrLo = addrLo;   // Lower part of base address of R2P window
	r2pWinCfg.BAddrEx = 0;   // Bits [65:64] of 66-bit base address
	r2pWinCfg.Size = WinBufSize;  // Size of window in bytes (32KB - 16GB)
	dwErr = TSI721CfgR2pWin(device, r2pWinNum, &r2pWinCfg);
	if (dwErr != ERROR_SUCCESS) {
		printf_s("ERROR(SRIO) Failed to initialize IB_WIN_%d, err = 0x%x\n", r2pWinNum, dwErr);
		TSI721FreeR2pWin(device, r2pWinNum);
		exit(0);
	}
}