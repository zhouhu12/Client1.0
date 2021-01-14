#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include<QLabel>
#include<QDebug>
#include"srio.h"
#include<QImage>
#include"opencv2/opencv.hpp"
//#include"Swidget.h"
#include"display.h"
#include "prototyse.h"

struct RecordInfo {
	int videoWidth = 1920;
	int videoHeight = 1080;
	int targetWidth = 1920;
	int targetHeight = 1080;
	int FPS = 30;
	int time = 30;
	int rate = 8;
	std::string picType = "BGR";
	std::string path = "D:/pano";
};

class Screen
{

public:
	explicit Screen();

	void showPix(int category);

	//    void record( QImage& img,bool isRecord,const RecordInfo& info );

	int extInterface(ExtStruct extPara);

	QPixmap pix1, pix2, pix3;

	//QLabel *screen1;

	int sendSize = 0;
	std::thread t1, t2, t3;
	unsigned char * screenBuf1 = nullptr;
	unsigned char * screenBuf2 = nullptr;
	unsigned char * screenBuf3 = nullptr;

	const int maxOriBufsize = 8 * 1024 * 1024;

	bool isVISDisplay = true;
	bool isINFARDisplay = true;
	bool isDRODisplay = true;

	bool isVISRecord = false;
	bool isINFARRecord = false;
	bool isDRORecord = false;

	RecordInfo info1;
	RecordInfo info2;
	RecordInfo info3;

	void run();

	SRIO& srio;
	display *infar;
	display *vis;
	display *dro;

	std::mutex pixMtx;

signals:

	public slots :
};
#endif // SCREEN_H
