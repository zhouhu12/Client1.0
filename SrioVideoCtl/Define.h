/***********************************************
*文件名：define.h
*
*版本号：version 1.00.00
*
*功能功能说明：
*   基本数据类型重新定义
*
*更新说明：
*
*
*设计约束：
*
*更新日期：2019-07-26
*
*作者：周燕
*************************************************/
#ifndef DEFINE_H
#define DEFINE_H
//#include "uiprototypes.h"
#include "Prototyse.h"
typedef void* PVOID;
//typedef void* HWND;
//typedef unsigned int  ULONG;
//typedef unsigned char BYTE ;
//typedef unsigned long long ULLONG;

/*********************
*上报姿态角的信息
* 惯导数据
**********************/
struct GuanDaoAttiangleInfoT
{
    float orientation;     //方位角  单位：度
    float picht;           //俯仰角  单位：度
    float roll;            //横滚角  单位：度
    GuanDaoAttiangleInfoT()
    {
        orientation = 0.0;
        picht =0.0;
        roll = 0.0;
    }

};
/*********************
*上报码盘值的信息
* 稳台数据
**********************/
struct CoderAttiangleInfoT
{
    float orientation;    //方位角       报码盘值，软件需转换成实际度数
    float picht;          //俯仰角       报码盘值，软件需转换成实际度数
    CoderAttiangleInfoT()
    {
        orientation = 0.0;
        picht = 0.0;
    }

};
/*********************
*设置脱靶量的参数
**********************/
struct OfftargetInfo
{
    float x;          //偏离中心点（中心点）x  单位：像素
    float y;          //偏离中心点y            单位：像素
    OfftargetInfo()
    {
        x = 0.0;
        y = 0.0;
    }
};
//如何体现多目标？
typedef struct TargetInfoT
{
    unsigned int targetID; //目标编号
    BYTE  type;//目标类型
    GuanDaoAttiangleInfoT guanDaoAttiangle ;//惯导姿态信息
    CoderAttiangleInfoT coderAttiangle;//稳台码盘值信息
    OfftargetInfo offTarget;//脱靶量
    float  confidenceLevel;//置信度           置信度标准？
    ULONG x;      //目标中心横坐标
    ULONG y;      //目标中心纵坐标
    ULONG w;      //目标区域的宽度
    ULONG h;      //目标区域的高度
    TargetInfoT()
    {
        targetID = 0;
        type = 0;
        confidenceLevel = 0.0;
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }
}TargetInfo;


#endif // DEFINE_H
