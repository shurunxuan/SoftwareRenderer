#pragma once

#include <queue>
#include <windows.h>
#include <gdiplus.h>
#include "geometry.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

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
private:
	std::queue<line_param> DrawQueue;
	HDC hdc_;
	HWND hwnd_;
public:
	CDrawer(HDC hdc, HWND hwnd) 
		: hdc_(hdc), hwnd_(hwnd)
	{
		
	}

	long getWidth() const;
	long getHeight() const;

	void drawLine(int x0, int y0, int x1, int y1, Color color);
	void drawLine(Vec2i v0, Vec2i v1, Color color);

	void drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color);

	void draw();
};