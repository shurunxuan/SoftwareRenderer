// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "Model.h"
#include "CDrawer.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

Model model("head.obj");

VOID OnPaint(CDrawer& drawer)
{
	//int width = drawer.getWidth();
	//int height = drawer.getHeight();
	//for (int i = 0; i<model.nfaces(); i++) {
	//	std::vector<int> face = model.face(i);
	//	for (int j = 0; j<3; j++) {
	//		Vec3f v0 = model.vert(face[j]);
	//		Vec3f v1 = model.vert(face[(j + 1) % 3]);
	//		int x0 = (-v0.x + 1.) * width / 2.;
	//		int y0 = (-v0.y + 1.) * height / 2.;
	//		int x1 = (-v1.x + 1.) * width / 2.;
	//		int y1 = (-v1.y + 1.) * height / 2.;
	//		drawer.drawLine(x0, y0, x1, y1, Color(255, 255, 255));
	//	}
	//}

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	drawer.drawTriangle(t0[0], t0[1], t0[2], Color(255, 0, 0));
	drawer.drawTriangle(t1[0], t1[1], t1[2], Color(255, 255, 255));
	drawer.drawTriangle(t2[0], t2[1], t2[2], Color(0, 255, 0));

	drawer.draw();
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

	GdiplusStartupInput gdiplusStartupInput;
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
		CW_USEDEFAULT,    // width of the window
		CW_USEDEFAULT,    // height of the window
		nullptr,    // we have no parent window, NULL
		nullptr,    // we aren't using menus, NULL
		hInstance,    // application handle
		nullptr);    // used with multiple windows, NULL

					 // display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg;

	PAINTSTRUCT  ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	CDrawer drawer(hdc, hWnd);

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

		OnPaint(drawer);
	}

	EndPaint(hWnd, &ps);
	GdiplusShutdown(gdiplusToken);
	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely

		// The program won't exit when close button is hit,
		// so a bad fix is applied. If you know how to fix
		// this, please give me a PR. Thanks.
		exit(0);

		//PostQuitMessage(0);
		//return 0;
	}
	default: break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}