#include <algorithm>
#include "CRenderer.h"
#include "Eigen/Geometry"
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min

void CRenderer::init()
{
	hdc_ = BeginPaint(hwnd_, &ps);
	graphics = new Gdiplus::Graphics(hdc_);
	RECT rect;
	GetClientRect(hwnd_, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	buffer = new Gdiplus::Bitmap(width, height);
	gr = new Gdiplus::Graphics(buffer);
}

void CRenderer::deinit() const
{
	delete gr;
	delete buffer;
	delete graphics;
	EndPaint(hwnd_, &ps);
}

CRenderer::CRenderer(HWND hwnd)
	: hwnd_(hwnd)
{
	init();
}

CRenderer::~CRenderer()
{
	deinit();
}

void CRenderer::resizeBuffer()
{
	if (gr == nullptr || buffer == nullptr || graphics == nullptr) return;

	deinit();
	init();
}

long CRenderer::getWidth() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto width = rect.right - rect.left;
	return width;
}

long CRenderer::getHeight() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto height = rect.bottom - rect.top;
	return height;
}

void CRenderer::draw()
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	gr->FillRectangle(&brush, 0, 0, width, height);

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			buffer->SetPixel(i, height - j, Gdiplus::Color(rand() % 256, rand() % 256, rand() % 256));
		}

	graphics->DrawImage(buffer, 0, 0);
}
