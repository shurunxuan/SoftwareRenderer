#include "CDrawer.h"

long CDrawer::getWidth() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto width = rect.right - rect.left;
	return width;
}

long CDrawer::getHeight() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto height = rect.bottom - rect.top;
	return height;
}

void CDrawer::drawLine(int x0, int y0, int x1, int y1, Color color)
{
	line_param lp;

	lp.steep = (std::abs(x0 - x1) < std::abs(y0 - y1));
	if (lp.steep) { // if the line is steep, we transpose the image 
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	lp.x0 = x0;
	lp.y0 = y0;
	lp.x1 = x1;
	lp.y1 = y1;
	lp.color = color;
	DrawQueue.push(lp);
}

void CDrawer::drawLine(Vec2i v0, Vec2i v1, Color color)
{
	drawLine(v0.x, v0.y, v1.x, v1.y, color);
}

void CDrawer::drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color)
{
	drawLine(v0, v1, color);
	drawLine(v1, v2, color);
	drawLine(v2, v0, color);
}

void CDrawer::draw()
{
	Graphics graphics(hdc_);
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto width = rect.right - rect.left;
	auto height = rect.bottom - rect.top;

	Bitmap bitmap(width, height);
	Graphics gr(&bitmap);

	SolidBrush brush(Color(0, 0, 0));
	gr.FillRectangle(&brush, 0, 0, width, height);

	while (!DrawQueue.empty())
	{
		line_param lp = DrawQueue.front();
		DrawQueue.pop();
		int dx = lp.x1 - lp.x0;
		int dy = lp.y1 - lp.y0;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = lp.y0;
		for (int x = lp.x0; x <= lp.x1; ++x) {
			if (lp.steep) {
				bitmap.SetPixel(y, getHeight() - x, lp.color);
			}
			else {
				bitmap.SetPixel(x, getHeight() - y, lp.color);
			}
			error2 += derror2;
			if (error2 > dx) {
				y += (lp.y1 > lp.y0 ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}


	graphics.DrawImage(&bitmap, 0, 0);
}
