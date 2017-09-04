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
	explicit CGouraudShader(CRenderer* renderer_ptr) : renderer_(renderer_ptr) {}
	std::vector<CVertex> vertex(CModel::TFace& input) override
	{
		vertex_output_.resize(0);
		vertex_output_.reserve(3);
		// View transformation
		vertex_output_.push_back(renderer_->trans(input[0].vertex));
		vertex_output_.push_back(renderer_->trans(input[1].vertex));
		vertex_output_.push_back(renderer_->trans(input[2].vertex));
		material_ = input[0].material;

		for (int i = 0; i < 3; ++i)
		{
			intensity_[i] = input[i].vertex.n.normalized().dot(light_);
			clip(intensity_[i], 0.0f, 1.0f);
		}

		return vertex_output_;
	}
	Gdiplus::Color pixel(Eigen::Vector3f barycentric) override
	{
		// Interpolate color
		float R = vertex_output_[0].c.GetR() * barycentric(0) + vertex_output_[1].c.GetR() * barycentric(1) + vertex_output_[2].c.GetR() * barycentric(2);
		float G = vertex_output_[0].c.GetG() * barycentric(0) + vertex_output_[1].c.GetG() * barycentric(1) + vertex_output_[2].c.GetG() * barycentric(2);
		float B = vertex_output_[0].c.GetB() * barycentric(0) + vertex_output_[1].c.GetB() * barycentric(1) + vertex_output_[2].c.GetB() * barycentric(2);

		float i = intensity_[0] * barycentric(0) + intensity_[1] * barycentric(1) + intensity_[2] * barycentric(2);
		// Interpolate texture coordinate
		Eigen::Vector2f tex = vertex_output_[0].t * barycentric(0) + vertex_output_[1].t * barycentric(1) + vertex_output_[2].t * barycentric(2);
		// Get texture color
		Gdiplus::Color t_color = getColorFromBitmap(tex(0), tex(1), material_.texture);
		const Gdiplus::Color pixel_color(R / 255.0f * i * t_color.GetR(), G / 255.0f * i * t_color.GetG(), B / 255.0f * i * t_color.GetB());
		return pixel_color;
	}
	//void setLight(Eigen::Vector3f light) override
	//{
	//	light_ = renderer->trans(light).normalized();
	//}
private:
	CRenderer* renderer_;
	std::vector<CVertex> vertex_output_;
	float intensity_[3];
	CModel::SMtl material_;
};

class CPhongShader : public IShader
{
public:
	explicit CPhongShader(CRenderer* renderer_ptr, bool blinn_phong)
		: renderer_(renderer_ptr), blinn_phong_(blinn_phong) {}
	std::vector<CVertex> vertex(CModel::TFace& input) override
	{
		vertex_output_.resize(0);
		vertex_output_.reserve(3);
		// View transformation
		vertex_output_.push_back(renderer_->trans(input[0].vertex));
		vertex_output_.push_back(renderer_->trans(input[1].vertex));
		vertex_output_.push_back(renderer_->trans(input[2].vertex));

		vertex_input_.resize(0);
		vertex_input_.reserve(3);
		vertex_input_.push_back(input[0].vertex);
		vertex_input_.push_back(input[1].vertex);
		vertex_input_.push_back(input[2].vertex);

		material_ = input[0].material;

		return vertex_output_;
	}
	Gdiplus::Color pixel(Eigen::Vector3f barycentric) override
	{
		// vector l
		Eigen::Vector3f l = light_.normalized();
		// normal
		Eigen::Vector3f n = vertex_input_[0].n * barycentric(0) + vertex_input_[1].n * barycentric(1) + vertex_input_[2].n * barycentric(2);
		n.normalize();
		// position
		Eigen::Vector3f pos = vertex_input_[0].v * barycentric(0) + vertex_input_[1].v * barycentric(1) + vertex_input_[2].v * barycentric(2);
		// view
		Eigen::Vector3f v = renderer_->getCameraPosition() - pos;
		v.normalize();

		// diffuse factor l dot n
		float fd = l.dot(n);
		fd = fd < 0.0f ? 0.0f : fd;
		// specular factor r dot v / n dot h
		float sd;
		if (blinn_phong_)
		{// halfway vector
			Eigen::Vector3f h = l + v;
			h.normalize();
			sd = n.dot(h);
		}
		else
		{
			// r vector
			Eigen::Vector3f rv = 2 * l.dot(n) * n - l;
			sd = rv.dot(v);
		}
		sd = sd < 0.0f ? 0.0f : sd;

		// Interpolate diffuse color
		float R = vertex_output_[0].c.GetR() * barycentric(0) + vertex_output_[1].c.GetR() * barycentric(1) + vertex_output_[2].c.GetR() * barycentric(2);
		float G = vertex_output_[0].c.GetG() * barycentric(0) + vertex_output_[1].c.GetG() * barycentric(1) + vertex_output_[2].c.GetG() * barycentric(2);
		float B = vertex_output_[0].c.GetB() * barycentric(0) + vertex_output_[1].c.GetB() * barycentric(1) + vertex_output_[2].c.GetB() * barycentric(2);
		// Interpolate texture coordinate
		Eigen::Vector2f tex = vertex_output_[0].t * barycentric(0) + vertex_output_[1].t * barycentric(1) + vertex_output_[2].t * barycentric(2);
		// Get texture color
		Gdiplus::Color t_color = getColorFromBitmap(tex(0), tex(1), material_.texture);

		// diffuse (0 ~ 255)
		float dR = R / 255.0f * fd * t_color.GetR() * material_.diffuse(0);
		float dG = G / 255.0f * fd * t_color.GetG() * material_.diffuse(1);
		float dB = B / 255.0f * fd * t_color.GetB() * material_.diffuse(2);
		clip(dR, 0.0f, 255.0f);
		clip(dG, 0.0f, 255.0f);
		clip(dB, 0.0f, 255.0f);

		// ambient (0 ~ 255)
		float aR = material_.ambient(0) * ambient_.GetR();
		float aG = material_.ambient(1) * ambient_.GetG();
		float aB = material_.ambient(2) * ambient_.GetB();
		clip(aR, 0.0f, 255.0f);
		clip(aG, 0.0f, 255.0f);
		clip(aB, 0.0f, 255.0f);

		// specular (0 ~ 255)
		float sR = material_.specular(0) * pow(sd, material_.shininess) * specular_.GetR();
		float sG = material_.specular(1) * pow(sd, material_.shininess) * specular_.GetG();
		float sB = material_.specular(2) * pow(sd, material_.shininess) * specular_.GetB();
		clip(sR, 0.0f, 255.0f);
		clip(sG, 0.0f, 255.0f);
		clip(sB, 0.0f, 255.0f);

		// all
		int r = round(aR + dR + sR);
		int g = round(aG + dG + sG);
		int b = round(aB + dB + sB);
		clip(r, 0, 255);
		clip(g, 0, 255);
		clip(b, 0, 255);

		const Gdiplus::Color pixel_color(r, g, b);
		return pixel_color;
	}

	void setLightProperties(Gdiplus::Color ambient = Gdiplus::Color(0, 0, 0), Gdiplus::Color diffuse = Gdiplus::Color(255, 255, 255), Gdiplus::Color specular = Gdiplus::Color(255, 255, 255))
	{
		ambient_ = ambient;
		diffuse_ = diffuse;
		specular_ = specular;
	}
private:
	CRenderer* renderer_;
	std::vector<CVertex> vertex_input_;
	std::vector<CVertex> vertex_output_;
	CModel::SMtl material_;
	Gdiplus::Color ambient_;
	Gdiplus::Color diffuse_;
	Gdiplus::Color specular_;
	bool blinn_phong_;
};

CRenderer* p_renderer = nullptr;
CModel* p_model = nullptr;
IShader* p_shader = nullptr;
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
	renderer.cameraLookat(Eigen::Vector3f(0, 100, 0) + 500.0f * light, -light, Eigen::Vector3f(0, 1, 0));
	p_shader->setLight(light + Eigen::Vector3f(0, 20, 0));
	for (const CModel::TFace face : p_model->faces)
	{
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

	CModel model("Models\\003_01_0\\", "0.obj", "0.mtl");
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
	//CGouraudShader shader(p_renderer);
	CPhongShader shader(p_renderer, true);
	shader.setLightProperties(/*Gdiplus::Color(63, 63, 63)*/);
	p_shader = &shader;
	renderer.setShader(&shader);
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