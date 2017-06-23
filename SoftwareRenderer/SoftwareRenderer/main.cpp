// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "Model.h"
#include "CDrawer.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

Model model("head.obj");
CDrawer* p_drawer = nullptr;

VOID OnPaint(CDrawer& drawer)
{
	int width = drawer.getWidth();
	int height = drawer.getHeight();

	for (int i = 0; i<model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		Vec3f pts[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model.vert(face[j]);
			world_coords[j] = v;
			int c = 4;
			v.x = v.x / (1 - v.z / c);
			v.y = v.y / (1 - v.z / c);
			v.z = v.z / (1 - v.z / c);
			pts[j] = Vec3f(int((v.x + 1.) * width / 2. + 0.5), int((v.y + 1.) * height / 2. + 0.5), v.z);
		}
		Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
		Vec3f light_dir = { -1, -1, -1 };
		light_dir.normalize();
		n.normalize();
		float intensity = n * light_dir;
		Vec3f cam_dir = { 0, 0, -1 };
		cam_dir.normalize();
		if (n * cam_dir < 0) continue;
		if (intensity < 0) intensity = 0;
		if (intensity > 1) intensity = 1;
		drawer.fillTriangle(pts[0], pts[1], pts[2], Color(intensity * 255, intensity * 255, intensity * 255));
	}

	//Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	//Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	//Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//drawer.fillTriangle(t0[0], t0[1], t0[2], Color(255, 0, 0));
	//drawer.drawTriangle(t1[0], t1[1], t1[2], Color(255, 255, 255));
	//drawer.drawTriangle(t2[0], t2[1], t2[2], Color(0, 255, 0));

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



	CDrawer drawer(hWnd);
	p_drawer = &drawer;

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
	case WM_SIZE:
	{
		if (p_drawer != nullptr)
			p_drawer->resizeBuffer();
	}
	break;
	// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely

		// The program won't exit when close button is hit,
		// so an ugly fix is applied. If you know how to fix
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