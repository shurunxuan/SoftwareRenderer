#pragma once

#include <queue>
#include <windows.h>
#include <gdiplus.h>
#include "geometry.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min

class CDrawer
{
public:
	struct line_param
	{
		int x0;
		int y0;
		int x1;
		int y1;
		Color color;
		bool steep;
	};
	struct pixel_param
	{
		int x;
		int y;
		int z;
		Color color;
	};
private:
	std::queue<pixel_param> DrawQueue;
	HDC hdc_;
	HWND hwnd_;
	PAINTSTRUCT ps;
	Color** vertex_buffer = nullptr;
	Bitmap* buffer = nullptr;
	float* zbuffer = nullptr;
	Graphics* gr = nullptr;
	Graphics* graphics = nullptr;
	long width;
	long height;
public:
	CDrawer(HWND hwnd) 
		: hwnd_(hwnd)
	{
		
		hdc_ = BeginPaint(hwnd_, &ps);
		graphics = new Graphics(hdc_);
		RECT rect;
		GetClientRect(hwnd_, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		zbuffer = new float[width * height];
		for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

		vertex_buffer = new Color*[width];
		for (int i = 0; i < width; ++i) vertex_buffer[i] = new Color[height];

		buffer = new Bitmap(width, height);
		gr = new Graphics(buffer);
	}

	~CDrawer()
	{
		delete gr;
		delete buffer;
		delete zbuffer;
		delete graphics;
		for (int i = 0; i < width; ++i) delete vertex_buffer[i];
		delete vertex_buffer;
		EndPaint(hwnd_, &ps);
	}

	void resizeBuffer();
	

	long getWidth() const;
	long getHeight() const;

	void drawPixel(int x, int y, Color color);

	void drawLine(int x0, int y0, int x1, int y1, Color color);
	void drawLine(Vec2i v0, Vec2i v1, Color color);

	void drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color);

	void fillTriangle(Vec3f v0, Vec3f v1, Vec3f v2, Color color);
	void CDrawer::fillTriangle(Vec3f* pts, Color color);

	void draw();
};