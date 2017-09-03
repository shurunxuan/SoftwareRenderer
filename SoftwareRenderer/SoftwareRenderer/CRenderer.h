#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "CPixel.h"
#include "CVertex.h"
#include "CModel.h"
#include "IShader.h"

#undef max
#undef min

class CRenderer
{
public:

private:
	HDC hdc_;
	HWND hwnd_;
	PAINTSTRUCT ps_;

	Gdiplus::Bitmap* buffer_ = nullptr;

	Gdiplus::Graphics* gr_ = nullptr;
	Gdiplus::Graphics* graphics_ = nullptr;
	long width_;
	long height_;

	IShader* shader_ = nullptr;

	void init();

	void deinit() const;

	float** z_buffer_ = nullptr;
	struct camera
	{
		float n;
		float f;
		float t;
		float r;
		float l;
		float b;
		float theta;
		Eigen::Vector3f e;
		Eigen::Vector3f g;
		Eigen::Vector3f u;
		Eigen::Vector3f v;
		Eigen::Vector3f w;
	} camera_;
public:
	explicit CRenderer(HWND hwnd);
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
	void fillTriangle(CModel::TFace face);
#undef near
#undef far
	// Camera
	void setPerspectiveCamera(float near, float far, float fov);
	void cameraLookat(Eigen::Vector3f e, Eigen::Vector3f g, Eigen::Vector3f t);
	void resetPerspectiveCamera();

	Eigen::Vector3f getCameraPosition();

	Eigen::Matrix4f viewport();
	Eigen::Matrix4f orthographic();
	Eigen::Matrix4f P();
	Eigen::Matrix4f cameraTrans();

	CVertex trans(CVertex v);

	void setShader(IShader* shader);



	void clear() const;
	void draw() const;

};
