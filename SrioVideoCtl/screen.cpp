#include "screen.h"
#include<iostream>
#include<opencv2/opencv.hpp>
#include"videoEncoder.h"
#include"videoEncoderManager.h"

Screen::Screen() : srio(SRIO::getSrio())
{
    dro=new display("Drone");
    dro->move(960,540);
    dro->setWindowFlags(Qt::FramelessWindowHint);
    dro->show();
    infar=new display("Infared");
    infar->move(960,0);
    infar->setWindowFlags(Qt::FramelessWindowHint);
    infar->show();
    vis=new display("Visiual");
    vis->setWindowFlags(Qt::FramelessWindowHint);
    vis->move(0,0);
    vis->show();
    screenBuf1=new unsigned char[1920*1080*4];
    memset(screenBuf1, 100, 1920*1080*4);
    screenBuf2=new unsigned char[1920*1080*4];
    memset(screenBuf2, 150, 1920*1080*4);
    screenBuf3=new unsigned char[1920*1080*4];
    memset(screenBuf3, 200, 1920*1080*4);
    //run();
//    showPix();

}
void Screen::run()
{
    srio.bindHostId(0x06);
    srio.listenDoorbell();
    srio.connect(0x0a);
    t1 = std::thread(&Screen::showPix, this, 1);
    t2 = std::thread(&Screen::showPix, this, 2);
    t3 = std::thread(&Screen::showPix, this, 3);

}
void Screen::showPix(int category)
{
    videoEncoderManager videoEncoderManager;
    switch (category) {
    case 1:
        videoEncoderManager.paraSet(960,540,info1.targetWidth,info1.targetHeight,info1.FPS,info1.time,info1.rate,info1.picType,info1.path);
        break;
    case 2:
        videoEncoderManager.paraSet(960,540,info2.targetWidth,info2.targetHeight,info2.FPS,info2.time,info2.rate,info2.picType,info3.path);
        break;
    case 3:
        videoEncoderManager.paraSet(960,540,info3.targetWidth,info3.targetHeight,info3.FPS,info3.time,info3.rate,info3.picType,info3.path);
        break;
    default:
        break;
    }
    videoEncoderManager.startRecord();
    while (true) {
        switch (category) {
        case 1:
        {
            sendSize=srio.acceptImage(1,screenBuf1,maxOriBufsize);
            QImage img=QImage(screenBuf1+16,960,540,QImage::Format_RGB888);
            if(isVISRecord)
            {
                videoEncoderManager.push(img.bits());
            }
            if(isVISDisplay)
            {
                //std::lock_guard<std::mutex> lck(pixMtx);
                pix1=QPixmap::fromImage(img);
//                qDebug()<<"vis";
                vis->screen->setPixmap(pix1.scaled(vis->screen->width(),vis->screen->height()));
            }
            break;
        }
        case 2:
        {
//            qDebug()<<"infar";
            sendSize=srio.acceptImage(2,screenBuf2,maxOriBufsize);
            QImage img=QImage(screenBuf2+16,960,540,QImage::Format_RGB888);
            if(isINFARRecord)
            {
                videoEncoderManager.push(img.bits());
            }
            if(isINFARDisplay)
            {
                //std::lock_guard<std::mutex> lck(pixMtx);
                pix2=QPixmap::fromImage(img);
                infar->screen->setPixmap(pix2.scaled(infar->screen->width(),infar->screen->height()));
            }
            break;
        }
        case 3:
        {
//            qDebug()<<"dro";
            sendSize=srio.acceptImage(3,screenBuf3,maxOriBufsize);
            QImage img=QImage(screenBuf3+16,960,540,QImage::Format_RGB888);
            if(isDRORecord)
            {
                videoEncoderManager.push(img.bits());
            }
            if(isDRODisplay)
            {
                //std::lock_guard<std::mutex> lck(pixMtx);
                pix3=QPixmap::fromImage(img);
                dro->screen->setPixmap(pix3.scaled(dro->screen->width(),dro->screen->height()));
            }
            break;
        }
        default:
            break;
        }
//        cv::waitKey(30);
        //std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    videoEncoderManager.stopRecord();
}


//void Screen::record( QImage& img,bool isRecord,const RecordInfo& info)
//{
//    videoEncoderManager videoEncoderManager;
//    videoEncoderManager.paraSet(info.videoWidth,info.videoHeight,info.targetWidth,info.targetHeight,info.FPS,info.time,info.rate,info.picType,info.path);
//    videoEncoderManager.startRecord();

//    videoEncoderManager.push(img.bits());

//    videoEncoderManager.stopRecord();
//}

int Screen::extInterface(ExtStruct extPara) {
	    if(extPara.type==RECORD_START)
	    {
	        switch (extPara.para.recordSetting.sourceType) {
	        case VISIBLE:
	            this->isVISRecord=true;
	            this->info1.time=extPara.para.recordSetting.timeDuration;
	            this->info1.path=extPara.para.recordSetting.path;
	            this->info1.FPS=extPara.para.recordSetting.encodePara.fRate;
	            this->info1.rate=extPara.para.recordSetting.encodePara.MBitRate;
	            this->info1.targetWidth=extPara.para.recordSetting.encodePara.moiveScale.wScale;
	            this->info1.targetHeight=extPara.para.recordSetting.encodePara.moiveScale.hScale;
	            break;
	        case INFRARED:
	            this->isINFARRecord=true;
	            this->info2.time=extPara.para.recordSetting.timeDuration;
	            this->info2.path=extPara.para.recordSetting.path;
	            this->info2.FPS=extPara.para.recordSetting.encodePara.fRate;
	            this->info2.rate=extPara.para.recordSetting.encodePara.MBitRate;
	            this->info2.targetWidth=extPara.para.recordSetting.encodePara.moiveScale.wScale;
	            this->info2.targetHeight=extPara.para.recordSetting.encodePara.moiveScale.hScale;
	            break;
	        case UAV:
	            this->isDRORecord=true;
	            this->info3.time=extPara.para.recordSetting.timeDuration;
	            this->info3.path=extPara.para.recordSetting.path;
	            this->info3.FPS=extPara.para.recordSetting.encodePara.fRate;
	            this->info3.rate=extPara.para.recordSetting.encodePara.MBitRate;
	            this->info3.targetWidth=extPara.para.recordSetting.encodePara.moiveScale.wScale;
	            this->info3.targetHeight=extPara.para.recordSetting.encodePara.moiveScale.hScale;
	            break;
	        default:
	            break;
	        }
	    }else if(extPara.type==RECORD_STOP)
	    {
	        this->isVISRecord=false;
	        this->isINFARRecord=false;
	        this->isDRORecord=false;
	    }
	    return 0;
}

