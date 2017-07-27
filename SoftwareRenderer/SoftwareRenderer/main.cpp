// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "Model.h"
#include "CRenderer.h"
using namespace Gdiplus;
using namespace Eigen;
#pragma comment (lib,"Gdiplus.lib")

Model model("head.obj");
CRenderer* p_renderer = nullptr;

VOID OnPaint(CRenderer& renderer)
{
	int width = renderer.getWidth();
	int height = renderer.getHeight();

	renderer.cameraLookAt(Vector3f(0, 0, -4), Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	for (int i = 0; i<model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		Vector3f world_coords[3];
		Vector3f norm[3];
		for (int j = 0; j < 3; j++)
		{
			world_coords[j] = model.vert(face[j]);
			norm[j] = model.norm(i, j);
		}

		renderer.fillTriangle(world_coords, norm, Color(255, 255, 255), false);
	}

	//Vector2i t0[3] = { Vector2i(10, 70),   Vector2i(50, 160),  Vector2i(70, 80) };
	//Vector2i t1[3] = { Vector2i(180, 50),  Vector2i(150, 1),   Vector2i(70, 180) };
	//Vector2i t2[3] = { Vector2i(180, 150), Vector2i(120, 160), Vector2i(130, 180) };
	//renderer.fillTriangle(t0[0], t0[1], t0[2], Color(255, 0, 0));
	//renderer.drawTriangle(t1[0], t1[1], t1[2], Color(255, 255, 255));
	//renderer.drawTriangle(t2[0], t2[1], t2[2], Color(0, 255, 0));

	renderer.draw();
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



	CRenderer renderer(hWnd);
	p_renderer = &renderer;

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

		OnPaint(renderer);
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