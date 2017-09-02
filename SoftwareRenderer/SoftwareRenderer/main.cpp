// include the basic windows header file
#include <time.h>
#include <windows.h>
#include <windowsx.h>
#include <ShellScalingAPI.h>
#include <gdiplus.h>
#include "CRenderer.h"
#include "CModel.h"
#include "utils.h"
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "Shcore.lib")

class CGouraudShader : public IShader
{
public:
	CGouraudShader(CRenderer* renderer_ptr) : renderer(renderer_ptr) {}
	std::vector<CVertex> vertex(CModel::TFace& input) override
	{
		vertex_output.resize(0);
		vertex_output.reserve(3);
		// View transformation
		vertex_output.push_back(renderer->trans(input[0].vertex));
		vertex_output.push_back(renderer->trans(input[1].vertex));
		vertex_output.push_back(renderer->trans(input[2].vertex));
		texture = input[0].material.texture;

		return vertex_output;
	}
	Gdiplus::Color pixel(Eigen::Vector3f barycentric) override
	{
		// Interpolate color
		float R = vertex_output[0].c.GetR() * barycentric(0) + vertex_output[1].c.GetR() * barycentric(1) + vertex_output[2].c.GetR() * barycentric(2);
		float G = vertex_output[0].c.GetG() * barycentric(0) + vertex_output[1].c.GetG() * barycentric(1) + vertex_output[2].c.GetG() * barycentric(2);
		float B = vertex_output[0].c.GetB() * barycentric(0) + vertex_output[1].c.GetB() * barycentric(1) + vertex_output[2].c.GetB() * barycentric(2);
		// Interpolate texture coordinate
		Eigen::Vector2f tex = vertex_output[0].t * barycentric(0) + vertex_output[1].t * barycentric(1) + vertex_output[2].t * barycentric(2);
		// Get texture color
		Gdiplus::Color t_color = getColorFromBitmap(tex(0), tex(1), texture);
		const Gdiplus::Color pixel_color(R / 255.0f * t_color.GetR(), G / 255.0f * t_color.GetG(), B / 255.0f * t_color.GetB());
		return pixel_color;
	}
private:
	CRenderer* renderer;
	std::vector<CVertex> vertex_output;
	Gdiplus::Bitmap* texture;
};

CRenderer* p_renderer = nullptr;
CModel* p_model = nullptr;
CGouraudShader* p_shader = nullptr;
float p_v = 0.0f;
bool dir = false;
int rotate = 0;



VOID paint_main(CRenderer& renderer)
{
	//if (dir) p_v += 10.0f;
	//else p_v -= 10.0f;
	//if (p_v > 100.0f || p_v < 0.0f) dir = !dir;
	float r_rotate = 3.14159f / 180.f * rotate;
	rotate = (rotate + 10) % 360;
	Eigen::Vector3f light = { cosf(r_rotate), 0, sinf(r_rotate) };
	light.normalize();
	//Eigen::Vector3f light = { 0, 0, 1 };
	//renderer.cameraLookat(Eigen::Vector3f(0, p_v + 50, 400), Eigen::Vector3f(0, 0, -1), Eigen::Vector3f(0, 1, 0));
	renderer.cameraLookat(Eigen::Vector3f(0, 20, 0) + 600.0f * light, -light, Eigen::Vector3f(0, 1, 0));
	for (CModel::TFace face : p_model->faces)
	{
		for (int i = 0; i < 3; ++i)
		{
			float intensity = face[i].vertex.n.normalized().dot(light);
			intensity = intensity > 1.0f ? 1.0f : intensity < 0.0f ? 0.0f : intensity;
			//float intensity = 1.0f;
			face[i].vertex.c = Gdiplus::Color(255 * intensity, 255 * intensity, 255 * intensity);
		}
		renderer.fillTriangle(face);
		//renderer.fillTriangle(face[0].vertex, face[1].vertex, face[2].vertex);
	}

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
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// this struct holds information for the window class
	WNDCLASSEX wc;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	CModel model("Models\\382_00_0\\", "0.obj", "0.mtl");
	p_model = &model;

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
		1946,    // width of the window
		1151,    // height of the window
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
	CGouraudShader shader(p_renderer);
	p_shader = &shader;
	renderer.setShader(p_shader);
	renderer.setPerspectiveCamera(-1.f, -2.f, 3.14159f / 3.0f);

	TCHAR title[100];
	// Enter the infinite message loop
	while (true)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
		renderer.clear();
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