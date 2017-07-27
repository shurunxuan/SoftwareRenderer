#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"

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
public:
	CRenderer(HWND hwnd);
	~CRenderer();

	void resizeBuffer();

	long getWidth() const;
	long getHeight() const;

	void draw();

};