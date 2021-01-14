#pragma once

#include "tsi721ioctl.h"
#include "tsi721api.h"

#define RIO_DEV_ID_CAR              (0x000000)
#define RIO_SWITCH_PORT_INF_CAR     (0x000014)
#define RIO_COMPONENT_TAG_CSR       (0x00006C)

#define RIO_SWITCH_RTE_DESTID       (0x000070)
#define RIO_SWITCH_RTE_PORT         (0x000074)
#define RIO_SWITCH_PORT_CTL1(n)     (0x00015c + 0x20 * (n))

#define RIO_PORT_GEN_CTRL_CSR       (0x00013C)
#define RIO_PORT_N_ERR_STAT_CSR     (0x000158)

#define RIO_SWITCH_EM_PW_TGTID      (0x1028) // Port-Write Target Device ID CSR (in CPS1432)


#define SRIOCOM_COMMAND_ADDRESS_1					0x00000000
#define SRIOCOM_COMMAND_DOORBELL_1					0x2000

#define SRIOCOM_COMMAND_ADDRESS_2					0x00000000
#define SRIOCOM_COMMAND_DOORBELL_2					0x2001

#define CARDCAPTURE_CONTROL_ADDRESS					0x000FF000				// 控制指令地址
#define CARDCAPTURE_CONTROL_DOORBELL				0x0100					// 控制指令门铃

#define CARDCAPTURE_SERIALPORT1_ADDRESS				0x00100000				// 串口1地址
#define CARDCAPTURE_SERIALPORT1_DOORBELL			0x0101					// 串口1门铃
#define CARDCAPTURE_SERIALPORT2_ADDRESS				0x00101000				// 串口2地址
#define CARDCAPTURE_SERIALPORT2_DOORBELL            0x0102					// 串口2门铃

#define CARDCAPTURE_SDI1_ADDRESS_1					0x01000000				// SDI1地址1
#define CARDCAPTURE_SDI1_DOORBELL_1					0x1000					// SDI1门铃1
#define CARDCAPTURE_SDI1_ADDRESS_2					0x01800000				// SDI1地址2
#define CARDCAPTURE_SDI1_DOORBELL_2					0xEA60					// SDI1门铃2

#define CARDCAPTURE_SDI2_ADDRESS_1					0x02000000				// SDI2地址1
#define CARDCAPTURE_SDI2_DOORBELL_1					0x1001					// SDI2门铃1
#define CARDCAPTURE_SDI2_ADDRESS_2					0x02800000				// SDI2地址2
#define CARDCAPTURE_SDI2_DOORBELL_2					0xEA61					// SDI2门铃2

#define CARDCAPTURE_SDI3_ADDRESS_1					0x03000000				// SDI3地址1
#define CARDCAPTURE_SDI3_DOORBELL_1					0x1002					// SDI3门铃1
#define CARDCAPTURE_SDI3_ADDRESS_2					0x03800000				// SDI3地址2
#define CARDCAPTURE_SDI3_DOORBELL_2					0xEA62					// SDI3门铃2

#define CARDCAPTURE_SDI4_ADDRESS_1					0x04000000				// SDI4地址1
#define CARDCAPTURE_SDI4_DOORBELL_1					0x1003					// SDI4门铃1
#define CARDCAPTURE_SDI4_ADDRESS_2					0x04800000				// SDI4地址2
#define CARDCAPTURE_SDI4_DOORBELL_2					0xEA63					// SDI4门铃2

#define CARDCAPTURE_SDI5_ADDRESS_1					0x05000000				// SDI5地址1
#define CARDCAPTURE_SDI5_DOORBELL_1					0x1004					// SDI5门铃1
#define CARDCAPTURE_SDI5_ADDRESS_2					0x05800000				// SDI5地址2
#define CARDCAPTURE_SDI5_DOORBELL_2					0xEA64					// SDI5门铃2

#define CARDCAPTURE_SDI6_ADDRESS_1					0x06000000				// SDI6地址1
#define CARDCAPTURE_SDI6_DOORBELL_1					0x1005					// SDI6门铃1
#define CARDCAPTURE_SDI6_ADDRESS_2					0x06800000				// SDI6地址2
#define CARDCAPTURE_SDI6_DOORBELL_2					0xEA65					// SDI6门铃2

#define CARDCAPTURE_SDI7_ADDRESS_1					0x07000000				// SDI7地址1
#define CARDCAPTURE_SDI7_DOORBELL_1					0x1006					// SDI7门铃1
#define CARDCAPTURE_SDI7_ADDRESS_2					0x07800000				// SDI7地址2
#define CARDCAPTURE_SDI7_DOORBELL_2					0xEA66					// SDI7门铃2

#define CARDCAPTURE_SDI8_ADDRESS_1					0x08000000				// SDI8地址1
#define CARDCAPTURE_SDI8_DOORBELL_1					0x1007					// SDI8门铃1
#define CARDCAPTURE_SDI8_ADDRESS_2					0x08800000				// SDI8地址2
#define CARDCAPTURE_SDI8_DOORBELL_2					0xEA67					// SDI8门铃2

#define CARDCAPTURE_AV1_ADDRESS_1					0x09000000				// AV1地址1
#define CARDCAPTURE_AV1_DOORBELL_1					0x1008					// AV1门铃1
#define CARDCAPTURE_AV1_ADDRESS_2					0x09800000				// AV1地址2
#define CARDCAPTURE_AV1_DOORBELL_2					0xEA68					// AV1门铃2

#define CARDCAPTURE_AV2_ADDRESS_1					0x0A000000				// AV2地址1
#define CARDCAPTURE_AV2_DOORBELL_1					0x1009					// AV2门铃1
#define CARDCAPTURE_AV2_ADDRESS_2					0x0A800000				// AV2地址2
#define CARDCAPTURE_AV2_DOORBELL_2					0xEA69					// AV2门铃2

#define CARDCAPTURE_AV3_ADDRESS_1					0x0B000000				// AV3地址1
#define CARDCAPTURE_AV3_DOORBELL_1					0x100A					// AV3门铃1
#define CARDCAPTURE_AV3_ADDRESS_2					0x0B800000				// AV3地址2
#define CARDCAPTURE_AV3_DOORBELL_2					0xEA6A					// AV3门铃2

#define CARDCAPTURE_AV4_ADDRESS_1					0x0C000000				// AV4地址1
#define CARDCAPTURE_AV4_DOORBELL_1					0x100B					// AV4门铃1
#define CARDCAPTURE_AV4_ADDRESS_2					0x0C800000				// AV4地址2
#define CARDCAPTURE_AV4_DOORBELL_2					0xEA6B					// AV4门铃2

#define SRIO_IBWIN_MAXNUM				8
#define SRIO_DMA_BUF_SIZE				(16*1024*1024)
#define SRIO_DMA_BUF_SIZE_HALF			(8*1024*1024)


#pragma pack(4)
struct SRIOData {
	UINT32 _fixed;
	UINT32 _type;
	UINT32 _encoding;
	UINT32 _dataSize;
	UINT32 _totalSize;
	PVOID  _buf;
	SRIOData(DWORD type, DWORD encoding, PVOID buf, DWORD size) : _fixed(0xffeebbaa), _type(type), _encoding(encoding), _dataSize(size)
	{
		_totalSize = _dataSize + 5 * sizeof(UINT32);
		_buf = malloc(_totalSize);
		if (_buf == NULL) {
			printf_s("ERROR(malloc) malloc SRIOData buf error\n");
			exit(0);
		}
		PUINT32 di = (PUINT32)_buf;
		*di = (UINT32)0xffeebbaa;; di++;
		*di = (UINT32)_type; di++;
		*di = (UINT32)_encoding; di++;
		*di = (UINT32)_dataSize; di++;
		*di = (UINT32)_totalSize; di++;
		memcpy((PVOID)di, buf, size);
	}
	~SRIOData() {
		free(_buf);
	}

	
};
#pragma pack()

struct SRIOBuffer {
	bool	_used;
	PVOID	_buf;
	DWORD	_bufSize;
	SRIOBuffer():_used(false), _buf(NULL), _bufSize(0) {}
	SRIOBuffer(bool used, PVOID buf, DWORD bufSize): _used(used), _buf(buf), _bufSize(bufSize){}
};

struct SRIODoorbell {
	DWORD _srcId;
	DWORD _dstId;
	DWORD _info;
	time_t _timeStamp;
	SRIODoorbell():_srcId(-1), _dstId(-1), _info(-1), _timeStamp(-1){}
	SRIODoorbell(DWORD srcId, DWORD dstId, DWORD doorbell) :
		_srcId(srcId), _dstId(dstId), _info(doorbell) {
		_timeStamp = time(NULL);
	}
};

struct SRIOR2pWin {
	DWORD _used;
	DWORD _winNum;
	DWORD _winBaseAddrLo;
	DWORD _winBaseAddrHi;
	DWORD _winSize;
	SRIOR2pWin() :_used(false), _winNum(0), _winBaseAddrLo(0), _winBaseAddrHi(0), _winSize(0) {}
	SRIOR2pWin(DWORD num, DWORD addr, DWORD size):_used(false), _winNum(num), _winBaseAddrLo(addr), _winBaseAddrHi(0), _winSize(size){}
};

struct SRIODBInfo {
	DWORD _doorbell;
	DWORD _srcId;
	DWORD _dstId;
	DWORD _dstAddr;
	DWORD _r2pWinOffset;
	DWORD _r2pWinNum;
	DWORD _channel;
};

static DWORD
SRIOR2pWinBaseAddr[SRIO_IBWIN_MAXNUM] = {
	SRIOCOM_COMMAND_ADDRESS_1,   // 控制
	CARDCAPTURE_SDI1_ADDRESS_1,  // 可见光
	CARDCAPTURE_SDI2_ADDRESS_1,  // 红外
	//CARDCAPTURE_SDI3_ADDRESS_1,
	//CARDCAPTURE_SDI4_ADDRESS_1,
	CARDCAPTURE_SDI5_ADDRESS_1,  // 无人机
	//CARDCAPTURE_SDI6_ADDRESS_1,
	//CARDCAPTURE_SDI7_ADDRESS_1,
};

// global function
static SRIODBInfo getDoorbellInfo(DWORD doorbell);
static DWORD getR2pWinInfo(DWORD doorbell);
static DWORD getChannel(DWORD doorbell);
static void initR2pWin(HANDLE device, DWORD r2pWinNum, DWORD addrLo, DWORD WinBufSize);
static unsigned char getCheckSum(const unsigned char* data, int length);

