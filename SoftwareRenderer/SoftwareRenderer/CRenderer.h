#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"
#include "CCamera.h"

#undef max
#undef min

class CRenderer
{
public:
	struct line_param
	{
		int x0;
		int y0;
		int x1;
		int y1;
		Gdiplus::Color color;
		bool steep;
	};
	struct pixel_param
	{
		int x;
		int y;
		int z;
		Gdiplus::Color color;
	};
private:
	HDC hdc_;
	HWND hwnd_;
	PAINTSTRUCT ps;
	Gdiplus::Color** zbuffer_color = nullptr;
	Gdiplus::Bitmap* buffer = nullptr;
	float* zbuffer = nullptr;
	Gdiplus::Graphics* gr = nullptr;
	Gdiplus::Graphics* graphics = nullptr;
	long width;
	long height;

	CCamera camera;

	void init();

	void deinit() const;
public:
	CRenderer(HWND hwnd);
	~CRenderer();

	void resizeBuffer();

	long getWidth() const;
	long getHeight() const;

	void drawPixel(int x, int y, Gdiplus::Color color);

	void drawLine(int x0, int y0, int x1, int y1, Gdiplus::Color color);
	void drawLine(Eigen::Vector2i v0, Eigen::Vector2i v1, Gdiplus::Color color);

	void drawTriangle(Eigen::Vector2i v0, Eigen::Vector2i v1, Eigen::Vector2i v2, Gdiplus::Color color);
	
	void fillTriangle(Eigen::Vector3f v0, Eigen::Vector3f v1, Eigen::Vector3f v2, Gdiplus::Color color);
	void fillTriangle(Eigen::Vector3f* coords, Gdiplus::Color color);
	void fillTriangle(Eigen::Vector3f* coords, Eigen::Vector3f* nor, Gdiplus::Color color);
	void fillTriangle(Eigen::Vector3f* coords, Eigen::Vector3f* nor, Gdiplus::Color color, bool shading);

	void draw();

	void cameraLookAt(Eigen::Vector3f _eye, Eigen::Vector3f _center, Eigen::Vector3f _up);
	Eigen::Matrix4f cameraMatrix() const;
	Eigen::Vector3f cameraDirection() const;
};