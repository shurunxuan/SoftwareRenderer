// include the basic windows header file
#include <time.h>
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "CRenderer.h"
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib,"Gdiplus.lib")

CRenderer* p_renderer = nullptr;

VOID paint_main(CRenderer& renderer)
{

	
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// this struct holds information for the window class
	WNDCLASSEX wc;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = TEXT("SoftwareRenderer");

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	HWND hWnd = CreateWindowEx(NULL,
		TEXT("SoftwareRenderer"),    // name of the window class
		TEXT("Software Renderer"),    // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		CW_USEDEFAULT,    // x-position of the window
		CW_USEDEFAULT,    // y-position of the window
		800,    // width of the window
		800,    // height of the window
		nullptr,    // we have no parent window, NULL
		nullptr,    // we aren't using menus, NULL
		hInstance,    // application handle
		nullptr);    // used with multiple windows, NULL

					 // display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg;



	CRenderer renderer(hWnd);
	p_renderer = &renderer;
	TCHAR title[100];
	// Enter the infinite message loop
	while (true)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);


		}
		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
			break;
		DWORD start = timeGetTime();
		paint_main(renderer);
		renderer.draw();
		DWORD end = timeGetTime();
		double fps = 1000.0 / (end - start);
		wsprintf(title, TEXT("FPS: %d.%d%d, Resolution: %d x %d"), int(fps), int(fps * 10) % 10, int(fps * 100) % 10, renderer.getWidth(), renderer.getHeight());
		SetWindowText(hWnd, title);
	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
	case WM_SIZE:
	{
		if (p_renderer != nullptr)
			p_renderer->resizeBuffer();
	}
	break;
	// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely

		// FIXME: The program won't exit when close button 
		// is hit, so an ugly fix is applied. If you know 
		// how to fix this, please give me a PR. Thanks.
		exit(0);

		//PostQuitMessage(0);
		//return 0;
	}
	default: break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}