#pragma once

#ifdef SRIO_EXPORT

#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif


/***********************************************
*文件名：SrioVideoCtl.h
*
*版本号：version 1.00.01
*
*功能说明：
*    本接口用于侦察监视软件与综合图像处理软件对接
* *更新说明：
*      此文件相较于V1.00.01
*      (1)新增上报扩展接口
*      (3)新增指令下发扩展接口
*      (4)修正链路状态上报接口linkStatusProc
*      (5)指令性接口输出增加超时与失败返回值
*      (6)修改拼接接口、融合接口
*
*设计约束：
*      暂无
*
*更新日期：2019-12-4
*
*作者：周燕
*************************************************/
#ifndef SRIO_COMMUNICATE_PARSER_H_
#define SRIO_COMMUNICATE_PARSER_H_

#include "Prototyse.h"
#include "Define.h"
class CapBoardListner
{
public:
	/***********************************************
	*函数名：linkStatusReportProc
	*函数功能：SRIO链路状态上报 视频信号状态上报 服务状态上报
	*        上报时机：(1)SRIO链路状态上报 start后 视频信号到达之前 首先上报SRIO链路状态   之后有变化则上报
	*                (2)视频信号状态上报  start后 视频数据上送或显示之前 应首先上报信号状态  之后有变化则上报
	*                (3)服务状态上报 由服务端向客户端1s报一次，启动立即上报
	*
	*参数：
	*    输入：
	*         destId 目的设备编号
	*         channelID 通道编号
	*         state
	*               -1: SRIO链路异常
	*                1: SRIO链路通畅
	*               -2: 无视频信号
	*                2：有视频信号
	*                3：服务客户端1S上报一次，启动即发
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int linkStatusReportProc(unsigned int destId, unsigned int channelID, int state) = 0;

	/***********************************************
	*函数名：videoFormatDetectedProc
	*函数功能：视频格式上报接口
	*        上报时机：视频采集启动后视频数据上送/显示之前首先上报视频格式信息
	*参数：
	*    输入：
	*         destID 目的设备编号
	*         channelID 通道编号
	*         nVideoWidth 视频宽
	*         nVideoHeight 视频高
	*         frameStoreType YUV数据存储方式
	*                        1 YUV422 YUYV
	*                        2 YUV420 YV12)
	*         duration 备用字段
	*         frameRate  视频播放帧率 实时视频可不填，回放通道时必填，实时通道可不填
	*
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int VideoFormatDetectedProc(unsigned int destID, unsigned int channelID, ULONG nVideoWidth, ULONG nVideoHeight, ULONG frameStoreType,
		ULLONG duration, ULONG frameRate) = 0;

	/***********************************************
	*函数名：frameReachedProc
	*函数功能：帧数据上报
	*参数：
	*    输入：
	*         destID 目的设备编号
	*         channelID 通道编号
	*         pFrameBuffer 帧数据
	*         nFrameBufferLen数据长度
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int frameReachedProc(unsigned int destID, unsigned int channelID, char*pFrameBuffer, ULONG nFrameBufferLen) = 0;

	/***********************************************
	*函数名：stopCapFinishedProc
	*函数功能：停止完成回调
	*参数：
	*    输入：
	*         destID 目的设备编号
	*         channelID 通道编号
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int stopCapFinishedProc(unsigned int destID, unsigned int channelID) = 0;

	/***********************************************
	*函数名：recvTargetRecognizationResultProc
	*函数功能：目标识别结果上报
	*参数：
	*    输入：
	*         destId  目的设备编号
	*         channelID 通道编号
	*         targetNum 目标识别数量
	*         targetInfo 目标信息
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int recvTargetRecognizationResultProc(unsigned int destId, unsigned int channelID, BYTE targetNum, TargetInfo *targetInfo) = 0;

	/***********************************************
	*函数名：freshTrackStatusProc
	*函数功能：目标跟踪状态刷新上报
	*参数：
	*    输入：
	*         destId  目的设备编号
	*         channelID 通道编号
	*         x,y 坐标
	*         trackStatus 跟踪状态
	*                     1 跟踪
	*                     2 重搜索
	*                     3 丢失
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int freshTrackStatusProc(unsigned int destId, unsigned int channelID, unsigned int x, unsigned int y, unsigned int trackStatus) = 0;

	/***********************************************
	*函数名：callBackMsg
	*函数功能：上报扩展接口
	*         消息上报的扩展接口，如有需要使用，需图像组提供协议
	*参数：
	*    输入：
	*         destID 目的设备编号
	*         channelID 通道编号
	*         messageType 消息类型
	*         messageLegnth 消息长度
	*         messageData  消息数据
	*    输出：正常返回0 其他返回错误码
	*************************************************/
	virtual int callBackMsg(unsigned int destID, unsigned int channelID, unsigned int messageType, unsigned int messageLegnth, char *messageData) = 0;

};


class BoardCapture
{
public:
	/***********************************************
	*函数名：getLocalDevice
	*函数功能：获取本地设备编号和端口号
	*参数：
	*    输入：
	*    输出：0xXXXXXXXX
	*         前两个字节本地设备编号localID，后两个字节本地设备端口号 localPort
	*************************************************/
	virtual unsigned int getLocalDevice() = 0;

	/***********************************************
	*函数名：getDestDevice
	*函数功能：获取目的设备编号
	*参数：
	*    输入：
	*    输出：目的设备编号
	*************************************************/
	virtual unsigned int getDestDevice() = 0;

	/***********************************************
	*函数名：init
	*函数功能：初始化操作
	*参数：
	*    输入：
	*    输出：  0 成功
	*          -1 失败
	*          -2 超时
	*
	*************************************************/
	virtual int init() = 0;
	/***********************************************
	*函数名：uninit
	*函数功能：去初始化操作，用于状态复位
	*参数：
	*    输入：
	*
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int uninit() = 0;

	/***********************************************
	*函数名：swtichVideoType
	*函数功能：切换要显示的视频内容类型
	*参数：
	*    输入：
	*          videoType 显示类型
	*                   0 原始视频
	*                   1 处理之后的视频
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int swtichVideoType(BYTE videoType) = 0;

	/***********************************************
	*函数名：setOverlapCharacters
	*函数功能：设置叠加字符串
	*参数：
	*    输入：
	*         index 叠加区域编号 1-4
	*         bOn  开关：
	*              true 开
	*              false 关
	*         x，y  左上角起始点坐标 单位像素
	*         characters 字符串
	*         length 字符串长度
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int setOverlapCharacters(ULONG index, bool bOn, ULONG x, ULONG y, char* characters, ULONG length) = 0;

	/***********************************************
	*函数名：setImageEnhancementSwitch
	*函数功能：设置图像增强开关
	*参数：
	*    输入：
	*         on 开关：
	*            true 开
	*            false 关
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int setImageEnhancementSwitch(bool on) = 0;

	/***********************************************
	*函数名：start
	*函数功能：启动运行，开始采集显示视频
	*参数：
	*    输入：
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int start() = 0;

	/***********************************************
	*函数名：stop
	*函数功能：停止采集与视频处理任务
	*参数：
	*    输入：
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int stop() = 0;

	/***********************************************
	*函数名：clone
	*函数功能：实现多窗口显示同一通道视频
	*参数：
	*    输入：
	*         winId 待显示的窗口ID 显示需支持跨平台(linux，windows)
	*         cloneId 克隆编号 [1-3] 克隆窗口最多支持3个
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*
	*************************************************/
	virtual int clone(HWND winId, BYTE cloneId) = 0;

	/***********************************************
	*函数名：unClone
	*函数功能：取消通道的多窗口显示
	*参数：
	*    输入：
	*         cloneId 克隆ID [1-3]
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int unClone(BYTE cloneId) = 0;

	/***********************************************
	*函数名：setMagnification
	*函数功能：设置电子放大倍数
	*参数：
	*    输入：
	*         magnification 放大倍数：
	*                       0 x1
	*                       1 x2;
	*                       2 x4
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setMagnification(BYTE magnification) = 0;

	/***********************************************
	*函数名：setFogProcSwitch
	*函数功能：设置电子透雾开关
	*参数：
	*    输入：
	*         on 开关：
	*            true 开
	*            false 关
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setFogProcSwitch(bool on) = 0;

	/***********************************************
	*函数名：setViedoStabilizationSwitch
	*函数功能：设置电子稳像开关
	*参数：
	*    输入：
	*         on 开关：
	*           true 开
	*           false 关
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setViedoStabilizationSwitch(bool on) = 0;

	/***********************************************
	*函数名：setImageStitchingSwitch
	*函数功能：设置图像拼接开关 只对通道1响应
	*参数：
	*    输入：
	*         on 开关：
	*           true 开
	*           false 关
	* 		  isc 0 大视场
	*             1 中视场
	*             2 小视场
	*         ext 备用
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setImageStitchingSwitch(bool on, int isc, unsigned int ext) = 0;

	/***********************************************
	*函数名：setTargetDetectSwitch
	*函数功能：设置目标检测开关
	*参数：
	*    输入：
	*        on 开关：
	*          true 开
	*          false 关
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setTargetDetectSwitch(bool on) = 0;

	/***********************************************
	*函数名：setTargetRecognizeSwitch
	*函数功能：设置目标识别开关
	*参数：
	*    输入：
	*         on 开关：
	*           true 开
	*           false 关
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setTargetRecognizeSwitch(bool on) = 0;

	/***********************************************
	*函数名：setTargetTrackMode
	*函数功能：设置目标跟踪模式
	*参数：
	*    输入：
	*         mode 跟踪模式：0点击跟踪  1自动跟踪
	*               0 填坐标x,y
	*               1 填目标编号
	*          on 跟踪开关 true 开启 false 关闭
	*          x,y 目标的在图像中起始点左上角的像素坐标
	*          targetID 目标编号[0-9]
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setTargetTrackMode(BYTE mode, bool on, unsigned int x, unsigned int y, unsigned int targetID) = 0;

	/***********************************************
	*函数名：setImageFusionSwitch
	*函数功能：设置图像融合开关
	*参数：
	*    输入：
	*         on 开关：
	*           true 开
	*           false 关
	*         KJGFocalDistance 可见光通道焦距 单位：mm
	*         HWFocalDistance  红外通道焦距  单位: mm
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setImageFusionSwitch(bool on, float KJGFocalDistance, float HWFocalDistance) = 0;

	/***********************************************
	*函数名：setRecordSettings
	*函数功能：录像设置
	*参数：
	*    输入：
	*         videoType
	*                  0 送显视频
	*                  1 原始视频
	*         MByteRate 码率 单位Mbps
	*         timeDuration 时长 单位分钟
	*         WScale  存储视频的width 单位像素
	*         HScale  存储视频的height 单位像素
	*         codecType: 0 H264  1 H265 默认H264
	*		  fileType : 文件格式
	*                           0 Ts
	*                           1 裸流
	*                           2 MP4
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setRecordSettings(BYTE videoType, ULLONG MBitRate, ULONG timeDuration, int WScale, int HScale, int codecType = 0, int fileType = 0) = 0;


	/***********************************************
	*函数名：startRecord
	*函数功能：开始录像
	*               格式支持 裸流 ts mp4(需支持seek)
	*参数：
	*    输入：
	*          path   存储路径 单个文件的文件名[150字节] ["路径"+"/"+"文件名（不带后缀）"]
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int startRecord(char* path) = 0;

	/***********************************************
	*函数名：stopRecord
	*函数功能：停止记录
	*参数：
	*    输入：
	*
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int stopRecord() = 0;

	/***********************************************
	*函数名：snapshot
	*函数功能：截图（支持.JPG .BMP .JP2 .PNG等常用格式）以后缀名区分
	*参数：
	*    输入：
	*         path    存储路径 [最大长度待定]
	*    输出：
	*         0 成功
	*        -1 失败
	************************************************/
	virtual int snapshot(char* path) = 0;

	/***********************************************
	*函数名：setCross
	*函数功能：设置十字丝
	*参数：
	*    输入：
	*         on  开关：
	*            true 开
	*            false 关
	*         centerX 十字丝中心水平坐标
	*         centerY 十字丝中心垂直坐标
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int setCross(bool on, unsigned int centerX, unsigned int centerY) = 0;

	/***********************************************
	*函数名：selectEth
	*函数功能：处理板以太网设置
	*参数：
	*    输入：
	*         srcIp  视频源IP
	*         srcPort 视频源Port
	*         destIp 目的IP
	*         destPort:目的端口号
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int selectEth(unsigned int srcIp, unsigned int srcPort, unsigned int destIp, unsigned int destPort) = 0;

	/***********************************************
	*函数名：extInterface
	*函数功能：扩展指令下发或参数设置接口
	*参数：
	*    输入：
	*         extPara  扩展指令或参数
	*    输出：
	*         0 成功
	*        -1 失败
	*        -2 超时
	*************************************************/
	virtual int extInterface(ExtStruct extPara) = 0;
	virtual ~BoardCapture() {}
};

extern "C"
{
	/***********************************************
	*函数名：createBoardCapture
	*函数功能：创建BoardCapture对象
	*参数：
	*    输入：
	*          localId  本地设备编号
	*          localPort 本地端口号
	*          destId 目的设备编号
	*          channelID 通道编号：0可见光，1 红外, 4 无人机
	*          listner 消息侦听器
	*          videoType 备用扩展
	*          winId    显示窗口ID 显示需支持跨平台(linux,windows)
	*    输出：
	*           BoardCapture对象
	*           失败返回 nullptr
	*************************************************/
	EXPORT_API BoardCapture *createBoardCapture(int localId, unsigned int localPort, unsigned int destId, unsigned int channelID, unsigned int videoType, CapBoardListner *listner, PVOID winId);

	/***********************************************
	*函数名：destroyBoardCapture
	*函数功能：销毁BoardCapture对象
	*参数：
	*    输入：
	*        处理板图像采集器
	*    输出：
	*        0 成功
	*        -1 失败
	*************************************************/
	EXPORT_API int destroyBoardCapture(BoardCapture *boardCap);
}
#endif /* SRIO_COMMUNICATE_PARSER_H_ */
