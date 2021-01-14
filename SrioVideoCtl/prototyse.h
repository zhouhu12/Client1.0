/*
 * V1.00.00.01
 * 1、修正UI操作指令 统一为UI_INTERACT
 * 2、添加推流结构体参数
 *    添加视频源类型、存储的视频源类型、存储目标数据格式、端口号、文件名、编码参数设置
 * 3、修改周扫拼接和扇扫拼接选择警戒区时，鼠标点的位置由原来的左上角（0，0）点变更为中心点
 * 4、添加启动存储结构体参数
 *    添加编码参数设置、视频源类型设置
 * 5、停止存储添加视频源类型设置
 * 6、停止推流添加视频源类型设置
*/
#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__
#ifdef _WIN32
#include <Windows.h>
#include <minwindef.h>
typedef unsigned long long ULLONG;
#else
typedef void* PVOID;
typedef void* HWND;
typedef unsigned int  ULONG;
typedef unsigned char BYTE;
typedef unsigned long long ULLONG;
#endif
//拼接视场等级
#define FOCUS_STAGE_0 0  //大视场
#define FOCUS_STAGE_1 1  //中视场
#define FOCUS_STAGE_2 2  //小视场

//注：所有的点的坐标均为实际像素的坐标，以左上角为（0,0）同比到1920*1080的界面大小
//定义界面操作时，界面上的点坐标
struct Point
{
    int x;//界面上横坐标
    int y;//界面上纵坐标
};

//定义界面操作时，界面上的起点、终点坐标
struct Line
{
    Point pt1;//起始点坐标
    Point pt2;//终止点坐标
};

/*********************
*扩展接口数据结构中定义不同的工作模式
*用于区分扩展接口的功能
**********************/
enum CmdType
{
    //视频处理模式指令： 直通、拼接、融合 三者互斥
    MODE_RAW_VIDEO = 100,          //直通模式
    MODE_PANORAMA_STCHING_VIDEO,   //周扫拼接
    MODE_SECTOR_SCAN_STCHING_VIDEO, //扇扫拼接
    MODE_DUAL_SOURCE_FUSION_VIDEO, //融合

    MODE_TARGET_TRACKING_VIDEO,    //跟踪


    //跟踪、检测
    TARGET_DETECT_START = 150,       //启动检测
    TARGET_DETECT_STOP,             //结束检测


    //消旋，保留。
    IMAGE_STABILIZATION_START = 200, //消旋开启  通知综合图像处理软件消旋的状态
    IMAGE_STABILIZATION_STOP,       //消旋关闭  通知综合图像处理软件消旋的状态

    //编码参数设置
    ENCODEPARAM_SET=300,
    //压缩编码
    RECORD_START = 350,             //启动存储
    RECORD_STOP,              //结束存储

    //网络传输	(压缩编码后的视频进行推流)
    PUSH_STREAM_START = 400,            //启动推流
    PUSH_STREAM_STOP,                   //关闭推流

    //UI交互
    UI_INTERACT = 500,

    //  猫眼业务 （可见光变成近红外）
    CATEYE_OPEN = 600,     //猫眼开启状态 通知综合图像处理软件当前猫眼的状态
    CATEYE_TARGET_INFO,    //（周扫拼接时）猫眼告警信息：方位、俯仰、距离、目标编号
    CATEYE_CLOSE,         //猫眼关闭状态 通知综合图像处理软件当前猫眼的状态

    //无人机业务
    WRJ_SOUTCE_CHANGE = 700     //无人机视频源切换时
};

/*********************
*视频源种类 
**********************/
enum SourcePara
{
    VISIBLE = 0x00000001,//可见光视频  分辨率：1920*1080
    INFRARED = 0x00000002,//红外视频   分辨率： 640*512
    UAV = 0x00000003     //无人机视频 	//新增
};
/*********************
*人机交互的内容
**********************/
enum UIInteractType
{	
    RADAR_SHOW_REGION_SELECT_SET = 1000,
    //周扫拼接下在雷达图上通过点击操作进行全景态势与观测区的显示区域设置,下发鼠标点坐标
    PANO_POINT,           //周扫拼接下在全景态势图上进行观测区显示内容调整设置，下发鼠标坐标点
    PANO_SCROLL,          //周扫拼接下全景态势图上进行全景态势显示内容设置 下发起点与终点
    OBSERVE_POINT,        //周扫拼接下在观测区上进行警戒区显示设置 下发 警戒区编号与鼠标坐标
    OBSERVE_SCROLL,       //周扫拼接下观测区上进行观测区显示内容设置 下发起点与终点
    ALERT_ZOOM,           //周扫拼接下双击警戒区将警戒区内容显示到/取消显示到观测区 下发警戒区编号
    // 扇扫拼接
    SECTOR_ALERT_SET = 1500//左扇区对应警戒区1/右扇区对应警戒区2 上设置警戒区设置 下发点坐
};

/*********************
*设置警戒区参数
**********************/
struct ObserverAlertRegionPara
{
    SourcePara sourceType;  //视频源
    int alertNum;//警戒区编号
    Point centerInfo;//警戒区中心
};

/*********************
*点操作
**********************/
struct UIInteractPoint
{
    SourcePara sourceType;  //视频源
    Point pointInfo;
};

/*********************
*滑动操作
**********************/
struct UIInteractLine
{
    SourcePara sourceType;  //视频源
    Line moveInfo;
};
/*********************
*人机交互结构体
**********************/
struct UIInteract
{
    UIInteractType interactType;
    union
    {
        UIInteractPoint uiPointInfo;//界面点操作RADAR_SHOW_REGION_SELECT_SET、PANO_POINT、SECTOR_ALERT_SET 、跟踪MODE_TARGET_TRACKING_VIDEO
        UIInteractLine uiMoveInfo;//界面滑动操作PANO_SCROLL、OBSERVE_SCROLL
        ObserverAlertRegionPara alertRegionParaSetting;  //警戒区设置 OBSERVE_POINT、ALERT_ZOOM
    }para;
};

/*********************
*设置周扫拼接时的参数           
**********************/
struct StitchPara
{                                                       
    SourcePara sourceType;  //视频源  仅支持红外IFRARED
    int isc;                //拼接对应的视场  0 大视场  1 中视场  2 小视场
};

/*********************
*设置扇扫拼接时的参数           
**********************/
struct SectorScanStichingPara
{
    SourcePara sourceType;    //视频源 (扇扫时是可见IFRARED和红外VISIBLE)
    int scanStichingMode; //1 模式1 可见光大视场和红外大视场以20°/s的速度，120°范围做扇扫
};

/*********************
*启动目标检测参数
**********************/
struct DetectPara
{
    SourcePara sourceType;  //视频源
    int algorithm;  //不同场景模式  0：沙漠
};

/*********************
*启动目标跟踪参数   (预留)
**********************/
struct TargetTrackingPara
{
    SourcePara sourceType;                       //视频源种类
    BYTE mode;                                   //跟踪模式 0点击跟踪  1自动跟踪
    unsigned int targetID;                       //目标编号[0-9]
};

/*********************
*融合开始参数
**********************/
struct FusionPara
{
    SourcePara sourceType;  //视频源种类  只支持红外视频IFRARED、可见光视频VISIBLE
    float visibleFocal;    //可见光焦距值 单位：mm
    float infraredFocal;   //红外焦距值  单位：mm
    int isc;//fusion       //档位
};

/*********************
*存储数据源
**********************/
enum VideoType
{
    CURRENT_SHOW_VIDEO ,    //当前界面显示的内容
    NON_PROCESSED_VIDEO   //原始视频（未经处理视频）
};

/*********************
*开始录制视频的长宽
**********************/
struct RecordScale
{
    int wScale;//视频的宽 单位：像素
    int hScale;//视频的高 单位：像素
};

/*********************
*编解码标准 默认H264
**********************/
enum EncodeType
{
    H264,
    H265
};

/*********************
*编解码参数设置
**********************/
struct EncodePara
{
    ULLONG MBitRate;       //码率            单位Mbps
    RecordScale moiveScale;//视频长、宽      单位：像素
    EncodeType encodeType;    //编码标准
    unsigned int fRate;    //帧率

};

/*********************
*存储目标数据格式
**********************/
enum RecordFileType
{
    REC_TS,  //存储为TS视频流
    REC_MP4, //存储为MP4视频流
    REC_RAW  //存储为裸流
};

/*********************
*开始记录数据的参数
**********************/
struct RecordPara
{
    SourcePara sourceType;     //视频源类型 可见光 红外 无人机
    char path[256];            //存储路径 单个文件的文件名[150字节] ["路径"+"/"+"文件名（不带后缀）"]
    ULONG timeDuration;        //存储时长      单位分钟
    RecordFileType fileType;   //视频存储格式
    VideoType videoType;      //存储的视频源类型
    EncodePara encodePara;    //编码参数设置
};

/*********************
*设置外部标注的参数
**********************/
struct CatEyePara
{
    int num;// 猫眼告警目标编号
    float distance;          //距离               单位：米
    float orientatio;        //方位角             单位：mrad
    float picht;             //俯仰角             单位：mrad
};

/*********************
*设置推流的参数
**********************/
struct PushStreamPara
{
    SourcePara sourceType;   // 视频源类型
    VideoType videoType;      //存储的视频源类型
    unsigned int port;      //端口号
    char filename[64];     //文件名
    RecordFileType fileType;//存储目标数据格式
    EncodePara encodePara;//编码参数设置
};

/*********************
*扩展接口数据结构定义
**********************/

union ParaT
{
    StitchPara stitchSetting;  // MODE_PANORAMA_STCHING_VIDEO  周扫拼接参数
    SectorScanStichingPara sectorSetting;// MODE_SECTOR_SCAN_STCHING_VIDEO 扇扫拼接参数
    FusionPara fusionSetting; //MODE_DUAL_SOURCE_FUSION_VIDEO 融合参数
    DetectPara detectSetting;//TARGET_DETECT_START 目标检测开启参数
    SourcePara GDSourceType;//视频源参数          目标检测结束TARGET_DETECT_STOP、直通MODE_RAW_VIDEO（视频源不包含无人机）、RECORD_STOP（停止存储）、PUSH_STREAM_STOP（停止推流）
    TargetTrackingPara targetTrackSetting;//启动目标跟踪TARGET_TRACKING_START
    // EncodePara encodeSetting;     // ENCODEPARAM_SET视频编码设置
    RecordPara recordSetting;//RECORD_START 视频存储设置
    PushStreamPara pushStreamSetting;//PUSH_STREAM_START  推流参数
    UIInteract uiInteract;    //界面操作
    CatEyePara  catEyeInfo;   //猫眼信息标注CATEYE_TARGET_INFO
    int WRJSourceType ;  //WRJ_SOUTCE_CHANGE    0:可见光  1:红外  2:画中画
};
struct ExtStruct
{
    CmdType type;       //工作模式
    ParaT para;
};
//class BoardCapture
//{
//
//};
//class CapBoardListner
//{
//
//};
#endif
