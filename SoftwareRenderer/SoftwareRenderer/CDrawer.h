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
	HDC hdc_;
	HWND hwnd_;
	PAINTSTRUCT ps;
	Color** zbuffer_color = nullptr;
	Bitmap* buffer = nullptr;
	float* zbuffer = nullptr;
	Graphics* gr = nullptr;
	Graphics* graphics = nullptr;
	long width;
	long height;

	void init();

	void deinit();
public:
	CDrawer(HWND hwnd);
	~CDrawer();

	void resizeBuffer();

	long getWidth() const;
	long getHeight() const;

	void drawPixel(int x, int y, Color color);

	void drawLine(int x0, int y0, int x1, int y1, Color color);
	void drawLine(Vec2i v0, Vec2i v1, Color color);

	void drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color);
	
	void fillTriangle(Vec3f v0, Vec3f v1, Vec3f v2, Color color);
	void fillTriangle(Vec3f* coords, Color color);
	void fillTriangle(Vec3f* coords, Vec3f* nor, Color color);
	void fillTriangle(Vec3f* coords, Vec3f* nor, Color color, bool shading);

	void draw();
};