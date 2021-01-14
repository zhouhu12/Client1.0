#include"videoEncoderManager.h"

#include "spdlog/spdlog.h"

#define DEBUG(x) spdlog::get("srio")->debug("[{}] {}", "VideoEncodeManager", x);
#define ERROR(x) spdlog::get("srio")->error("[{}] {}", "VideoEncodeManager", x);

videoEncoderManager::videoEncoderManager()
{
	DEBUG("Encoder initialized.");
    encoderCount = 0;
}

videoEncoderManager::~videoEncoderManager()
{
	DEBUG("Encoder destructor called.");
	stopRecord();
}

int videoEncoderManager::paraSet(int srcColsPara, int srcRowsPara, int dstColsPara, int dstRowsPara, int FPSPara, int segmentationTimePara, int bitratePara, std::string _frameType, std::string _savePath)
{
	srcCols = srcColsPara;
	srcRows = srcRowsPara;
	dstCols = dstColsPara;
	dstRows = dstRowsPara;

	srcColsEven = srcColsPara / 2 * 2;//偶数化
	srcRowsEven = srcRowsPara / 2 * 2;
	dstColsEven = dstColsPara / 2 * 2;
	dstRowsEven = dstRowsPara / 2 * 2;

	//std::cout << "dstRowsEven= " << dstRowsEven;

	FPS = FPSPara;
	segmentationTime = segmentationTimePara;
	savePath = _savePath;
	bitrate = bitratePara;
	if (_frameType == std::string("GRAY"))
	{
		srcIsGRAY = true;
		//srcColsSwsScale = srcColsPara;
		//src_pix_fmt = AV_PIX_FMT_GRAY8;
		//dst_pix_fmt = AV_PIX_FMT_GRAY8;

	}
	else if (_frameType == std::string("BGR"))
	{
		srcIsGRAY = false;
		//srcColsSwsScale = srcColsPara * 3;
		//src_pix_fmt = AV_PIX_FMT_BGR24;
		//dst_pix_fmt = AV_PIX_FMT_YUV420P;
	}
	else
	{
		std::cerr << "[Encode] Frame type error" << std::endl;
		return 1;
	}
	int offset = savePath.rfind('/');
	if (offset == -1) offset = savePath.rfind('\\');
	string folderName = savePath.substr(0, offset);
	if (createSavePath(folderName) == false)
	{
		std::cerr << "[Encode] Create savePath failed" << std::endl;
		return 1;
	}
	return -1;
}

mutex * videoEncoderManager::getEncoderMutex()
{
	return &encoderQueueLock;
}

//push之后立即处理，不再单独线程
int videoEncoderManager::push(unsigned char *buf)//
{
    if (flagRunEncoder == false)//如果没有开启，那么return 1；
	{
		ERROR("error:ffmpeg is not working");
        return 1;
	}

    if (encoderCount >= segmentationTime*FPS)//达到计数值，重新开启
    {
        Encoder1.EncodeEndFrame();
        Encoder1.closeEncoder();
        encoderCount = 0;
        //deleteEarlyVideo();

		DEBUG("encoder reached target frame count");
		//改
		++num;
    }
    //编码器启动，（到0重启）
    if (encoderCount == 0)
    {
        int encoderOpenState = 0;
        if (srcIsGRAY)
            encoderOpenState= Encoder1.openEncoder(srcColsEven, srcRowsEven, dstColsEven, dstRowsEven, FPS, bitrate, "GRAY", savePath.substr(0, savePath.size() - 4), num);
        else
            encoderOpenState = Encoder1.openEncoder(srcColsEven, srcRowsEven, dstColsEven, dstRowsEven, FPS, bitrate, "BGR", savePath.substr(0, savePath.size() - 4), num);

        if(encoderOpenState)
        {
            ERROR(fmt::format("openEncoderFailed,errorNum ={}", encoderOpenState));
            return 2;
        }
    }

    if (!srcIsGRAY)
    {
        int ffmpegState = 0;//编码是否成功
        cv::Mat bgr(srcRows, srcCols, CV_8UC3, buf);//
        cv::Mat bgrEven;
        if (srcRows % 2 != 0 || srcCols % 2 != 0)
        {
            //
            cv::Rect rect(0, 0, srcColsEven, srcRowsEven);
            bgr(rect).copyTo(bgrEven);
            ffmpegState = Encoder1.EncoderPerFrame(bgrEven.data);
        }
        else
        {
            ffmpegState = Encoder1.EncoderPerFrame(buf);
        }

        if (ffmpegState != 0)
        {
            flagRunEncoder = false;
			ERROR(fmt::format("encoder frame failed,errorNUm = {}", ffmpegState));
            return 3;
        }

    }
    else
    {
        int ffmpegState = 0;//编码是否成功
        cv::Mat infrare(srcRows, srcCols, CV_8UC1, buf);
        cv::Mat infrareEven;

        if (srcRows % 2 != 0 || srcCols % 2 != 0)
        {
            //
            cv::Rect rect(0, 0, srcColsEven, srcRowsEven);
            infrare(rect).copyTo(infrareEven);
            ffmpegState = Encoder1.EncoderPerFrame(infrareEven.data);
        }
        else
        {
            ffmpegState = Encoder1.EncoderPerFrame(infrare.data);
        }

        if (ffmpegState != 0)
        {
            flagRunEncoder = false;
            ERROR(fmt::format("encoder frame failed,errorNUm = {}", ffmpegState));
            return 3;
        }
    }

    encoderCount++;//count++
    //cout<<"encoderCount="<<encoderCount<<endl;
    return 0;
}


int videoEncoderManager::startRecord()
{
	//deleteEarlyVideo();
    encoderCount =0;
	flagRunEncoder = true;
	num = 0;

	DEBUG("startRecord");

	return 0;
}

int videoEncoderManager::stopRecord()
{
	flagRunEncoder = false;
    encoderCount =0;
    Encoder1.closeEncoder();

	DEBUG("stopRecord");

	return 0;
}

void videoEncoderManager::getFiles(std::string path, std::string exd, std::vector<std::string>& files)
{
//	//文件句柄
//	long   hFile = 0;
//	//文件信息
//	struct _finddata_t fileinfo;
//	std::string pathName, exdName;

//	if (0 != strcmp(exd.c_str(), ""))
//	{
//		exdName = "\\*." + exd;
//	}
//	else
//	{
//		exdName = "\\*";
//	}

//	if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
//	{
//		do
//		{
//			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
//				files.push_back(fileinfo.name);
//		} while (_findnext(hFile, &fileinfo) == 0);
//		_findclose(hFile);
//	}
}

int videoEncoderManager::deleteEarlyVideo()
{

//	std::tm timeCurrent = { 0 };
//	std::tm timeRecord = { 0 };

//	SYSTEMTIME sys;
//	GetLocalTime(&sys);
//	char currentTime[100];
//	sprintf_s(currentTime, 100, "%d%02d%02d%02d%02d%02d%03d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

//	std::string sCurrentTime = currentTime;
//	std::string currentTimeYear = sCurrentTime.substr(0, 4);
//	std::string currentTimeMonth = sCurrentTime.substr(4, 2);
//	std::string currentTimeDays = sCurrentTime.substr(6, 2);
//	std::string currentTimeHours = sCurrentTime.substr(8, 2);
//	std::string currentTimeMinutes = sCurrentTime.substr(10, 2);
//	std::string currentTimeSeconds = sCurrentTime.substr(12, 2);

//	timeCurrent.tm_year = std::stoi(currentTimeYear) - 1900;
//	timeCurrent.tm_mon = std::stoi(currentTimeMonth);
//	timeCurrent.tm_mday = std::stoi(currentTimeDays);
//	timeCurrent.tm_hour = std::stoi(currentTimeHours);
//	timeCurrent.tm_min = std::stoi(currentTimeMinutes);
//	timeCurrent.tm_sec = std::stoi(currentTimeSeconds);

//	std::vector<std::string> existFiles;//读文件中所有文件的名称

//	getFiles(savePath, "mp4", existFiles);

//	int size = existFiles.size();

//	double recordTime;//记录文件的创建时间
//	for (int i = 0; i < size; i++)

//	{
//		std::string srecordTime = existFiles[i].c_str();

//		std::string recordTimeYear = srecordTime.substr(0, 4);
//		std::string recordTimeMonth = srecordTime.substr(4, 2);
//		std::string recordTimeDays = srecordTime.substr(6, 2);
//		std::string recordTimeHours = srecordTime.substr(8, 2);
//		std::string recordTimeMinutes = srecordTime.substr(10, 2);
//		std::string recordTimeSeconds = srecordTime.substr(12, 2);

//		timeRecord.tm_year = std::stoi(recordTimeYear) - 1900;
//		timeRecord.tm_mon = std::stoi(recordTimeMonth);
//		timeRecord.tm_mday = std::stoi(recordTimeDays);
//		timeRecord.tm_hour = std::stoi(recordTimeHours);
//		timeRecord.tm_min = std::stoi(recordTimeMinutes);
//		timeRecord.tm_sec = std::stoi(recordTimeSeconds);

//		double diffseconds = difftime(mktime(&timeCurrent), mktime(&timeRecord));

//		if (diffseconds >= 86400)//一日·
//		{
//			char deleteVideoFullName[100];
//			sprintf_s(deleteVideoFullName, 100, "%s\\%s", savePath.c_str(), existFiles[i].c_str());

//			printf("%s\n", deleteVideoFullName);

//			if (remove(deleteVideoFullName) == 0)
//				std::cout << "delete success" << std::endl;
//			else
//				std::cout << "delete failed" << std::endl;
//		}
//	}
	return 0;
}

bool videoEncoderManager::createSavePath(std::string savePath)
{

        //判断文件夹是否存在，不存在则创建
    if (_access(savePath.c_str(), 0) == -1)	//如果文件夹不存在,则递归创建
    {
        if (strlen(savePath.c_str())>200)
        {
            return false;
        }
        int ipathLength = strlen(savePath.c_str());
        std::cout << "ipathLength = " << ipathLength << std::endl;
        int ileaveLength = 0;
        int iCreatedLength = 0;
        char szPathTemp[200] = { 0 };
        for (int i = 0; (NULL != strchr(savePath.c_str() + iCreatedLength, '\\')); i++)
        {
            ileaveLength = strlen(strchr(savePath.c_str() + iCreatedLength, '\\')) - 1;
            iCreatedLength = ipathLength - ileaveLength;
            strncpy_s(szPathTemp, savePath.c_str(), iCreatedLength);
            if (_access(savePath.c_str(), 0) == -1)//不存在，创建
            {
                _mkdir(szPathTemp);
            }
        }
        if (iCreatedLength<ipathLength)
        {
            _mkdir(savePath.c_str());
        }
    }
    return true;


}




