
#include <QtWidgets/QApplication>
#include <qdialog.h>

#include "SrioVideoCtl.h"

//int main(int argc, char *argv[])
//{
//    /*QApplication a(argc, argv);
//
//
//	BoardCapture *p = createBoardCapture(0, 0, 0, 0, 0, 0, 0);
//	p->start();
//
//    return a.exec();*/
//
//	const wchar_t CLASS_NAME[] = L"Sample Window Class";
//
//	WNDCLASS wc = {};
//
//	//wc.lpfnWndProc = WindowProc;
//	//wc.hInstance = hInstance;
//	wc.lpszClassName = CLASS_NAME;
//
//	RegisterClass(&wc);
//
//	// Create the window.
//
//	HWND hwnd = CreateWindowEx(
//		0,                              // Optional window styles.
//		CLASS_NAME,                     // Window class
//		L"Learn to Program Windows",    // Window text
//		WS_OVERLAPPEDWINDOW,            // Window style
//
//										// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		NULL,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//
//	if (hwnd == NULL)
//	{
//		return 0;
//	}
//
//	ShowWindow(hwnd, nCmdShow);
//
//}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	//wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

										// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	return 0;
}

