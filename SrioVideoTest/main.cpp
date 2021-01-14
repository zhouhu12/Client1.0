#include "SrioVideoCtl.h"

#include "opencv2/highgui/highgui_c.h"
#include "opencv2/opencv.hpp"
#include <thread>



int main(int argc, char *argv[])
{
	cv::namedWindow("test");
	auto h = (HWND)cvGetWindowHandle("test");
    
	BoardCapture *p = createBoardCapture(0, 0, 0, 1, 0, 0, h);
	p->start();


	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//p->snapshot("D:/pano/test.jpg");



	//ExtStruct control;
	//control.type = RECORD_START;
	//control.para.recordSetting.sourceType = INFRARED;
	//control.para.recordSetting.timeDuration = 1;
	//control.para.recordSetting.encodePara.fRate = 30;
	//control.para.recordSetting.encodePara.MBitRate = 8;
	//control.para.recordSetting.encodePara.moiveScale.wScale = 1920;
	//control.para.recordSetting.encodePara.moiveScale.hScale = 1080;
	//sprintf(control.para.recordSetting.path, "D:\\panowww\\123.mp4");
	//p->extInterface(control);
	//std::this_thread::sleep_for(std::chrono::seconds(100));
	//control.type = RECORD_STOP;
	//p->extInterface(control);
	cv::waitKey(0);
	destroyBoardCapture(p);
	return 0;
}
