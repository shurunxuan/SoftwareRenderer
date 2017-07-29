#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "CPixel.h"
#include "CVertex.h"
#include "CModel.h"

#undef max
#undef min

class CRenderer
{
public:

private:
	HDC hdc_;
	HWND hwnd_;
	PAINTSTRUCT ps;

	Gdiplus::Bitmap* buffer = nullptr;

	Gdiplus::Graphics* gr = nullptr;
	Gdiplus::Graphics* graphics = nullptr;
	long width;
	long height;

	void init();

	void deinit() const;

	float** z_buffer = nullptr;
public:
	CRenderer(HWND hwnd);
	~CRenderer();

	void resizeBuffer();

	long getWidth() const;
	long getHeight() const;

	// 2D
	void drawPixel(CPixel pixel, float z = 0) const;
	void drawLine(CPixel p1, CPixel p2) const;
	void drawLine(CPixel* pts) const;
	void drawTriangle(CPixel p1, CPixel p2, CPixel p3) const;
	void drawTriangle(CPixel* pts) const;
	void fillTriangle(CPixel p1, CPixel p2, CPixel p3) const;
	void fillTriangle(CPixel* pts) const;

	// 3D
	void fillTriangle(CVertex v1, CVertex v2, CVertex v3) const;
	void fillTriangle(CVertex* vtxs) const;
	void fillTriangle(CModel::TFace face) const;

	void clear() const;
	void draw() const;

};
