#include "draw.h"

void DrawImage(HWND hWnd, int width, int height, int bpp, const unsigned char *buffer)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HDC dc = GetDC(hWnd);

	if (bpp == 3) // BGR
	{
		BITMAPINFO bmpinfo;

		memset(&bmpinfo, 0, sizeof(bmpinfo));
		bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpinfo.bmiHeader.biBitCount = 24;
		bmpinfo.bmiHeader.biClrImportant = 0;
		bmpinfo.bmiHeader.biClrUsed = 0;
		bmpinfo.bmiHeader.biCompression = BI_RGB;
		bmpinfo.bmiHeader.biWidth = width;
		bmpinfo.bmiHeader.biHeight = -height;
		bmpinfo.bmiHeader.biPlanes = 1;
		bmpinfo.bmiHeader.biSizeImage = 0;
		bmpinfo.bmiHeader.biXPelsPerMeter = 100;
		bmpinfo.bmiHeader.biYPelsPerMeter = 100;

		::SetStretchBltMode(dc, COLORONCOLOR);
		::StretchDIBits(dc,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			0,
			0,
			width,
			height,
			buffer,
			&bmpinfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	else if (bpp == 1) // monochrome.
	{
		char bitmapInfoBuf[sizeof(BITMAPINFO) + 4 * 256];
		BITMAPINFO* pBmpInfo = (BITMAPINFO*)bitmapInfoBuf;

		memset(pBmpInfo, 0, sizeof(BITMAPINFO) + 4 * 256);
		pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBmpInfo->bmiHeader.biWidth = width;
		pBmpInfo->bmiHeader.biHeight = -height;
		pBmpInfo->bmiHeader.biCompression = BI_RGB;
		pBmpInfo->bmiHeader.biPlanes = 1;
		pBmpInfo->bmiHeader.biBitCount = 8;

		for (int i = 0; i < 256; i++)
		{
			pBmpInfo->bmiColors[i].rgbBlue = i;
			pBmpInfo->bmiColors[i].rgbGreen = i;
			pBmpInfo->bmiColors[i].rgbRed = i;
			pBmpInfo->bmiColors[i].rgbReserved = 255;
		}

		::SetStretchBltMode(dc, COLORONCOLOR);
		::StretchDIBits(dc,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			0,
			0,
			width,
			height,
			buffer,
			pBmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	ReleaseDC(hWnd, dc);
}

void DrawCVImage(cv::Mat image, HWND hWnd)
{
	if (image.cols % 4 == 0)
	{
		DrawImage(hWnd,
			image.cols,
			image.rows,
			image.channels() == 3 ? 3 : 1,
			image.data);
	}
	else
	{
		cv::Mat image2(image.rows, image.cols + (4 - image.cols % 4), image.type());
		image2 = 0;
		image.copyTo(image2(cv::Rect(0, 0, image.cols, image.rows)));

		DrawImage(hWnd,
			image2.cols,
			image2.rows,
			image2.channels() == 3 ? 3 : 1,
			image2.data);
	}
}