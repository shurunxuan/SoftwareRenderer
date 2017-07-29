#include <algorithm>
#include "CRenderer.h"
#include "Eigen/Dense"
#include "Eigen/Geometry"
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min
#define rint(v) int(round(v))

//Eigen::Vector3f barycentric(CPixel p1, CPixel p2, CPixel p3, int x, int y)
//{
//	//Eigen::Vector3f result;
//	//result(0) = float((p2.y() - p3.y()) * (x - p3.x()) + (p3.x() - p2.x()) * (y - p3.y())) / float((p2.y() - p3.y()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.y() - p3.y()));
//	//result(1) = float((p3.y() - p1.y()) * (x - p3.x()) + (p1.x() - p3.x()) * (y - p3.y())) / float((p2.y() - p3.y()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.y() - p3.y()));
//	//result(2) = 1 - result(0) - result(1);
//
//	Eigen::Matrix3f R;
//	R(0, 0) = p1.x(); R(0, 1) = p2.x(); R(0, 2) = p3.x();
//	R(1, 0) = p1.y(); R(1, 1) = p2.y(); R(1, 2) = p3.y();
//	R(2, 0) = 1;      R(2, 1) = 1;      R(2, 2) = 1;
//	Eigen::Vector3f P;
//	P(0) = x; P(1) = y; P(2) = 1;
//	Eigen::Vector3f result = R.inverse() * P;
//
//	return result;
//}

Eigen::Vector3f barycentric(CPixel p1, CPixel p2, CPixel p3, int x, int y) {
	
	Eigen::Vector3f u = Eigen::Vector3f(p3.x() - p1.x(), p2.x() - p1.x(), p1.x() - x).cross(Eigen::Vector3f(p3.y() - p1.y(), p2.y() - p1.y(), p1.y() - y));
	if (std::abs(u[2])<1) return Eigen::Vector3f(-1, 1, 1); // triangle is degenerate, in this case return smth with negative coordinates 
	return Eigen::Vector3f(1.f - (u(0) + u(1)) / u(2), u(1) / u(2), u(0) / u(2));
}

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

	z_buffer = new float*[width + 1];
	for (int i = 0; i <= width; ++i)
		z_buffer[i] = new float[height + 1];
}

void CRenderer::deinit() const
{
	for (int i = 0; i < width; ++i)
		delete z_buffer[i];
	delete z_buffer;

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
	if (gr == nullptr || buffer == nullptr || graphics == nullptr || z_buffer == nullptr) return;

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

void CRenderer::drawPixel(CPixel pixel, float z) const
{
	if (pixel.x() < 0 || pixel.x() > width) return;
	if (pixel.y() < 0 || pixel.y() > height) return;
	if (z_buffer[pixel.x()][pixel.y()] < z) return;
	buffer->SetPixel(pixel.x(), height - pixel.y(), pixel.color());
	z_buffer[pixel.x()][pixel.y()] = z;
}

void CRenderer::drawLine(CPixel p1, CPixel p2) const
{
	if (p2.y() < p1.y())
	{
		drawLine(p2, p1);
		return;
	}
	drawPixel(p2);
	bool reverse = p2.x() < p1.x();
	if (reverse)
	{
		p2.x() = -p2.x();
		p1.x() = -p1.x();
	}
	bool steep = p2.y() - p1.y() > p2.x() - p1.x();
	if (steep)
	{
		int temp = p2.y();
		p2.y() = p2.x();
		p2.x() = temp;
		temp = p1.y();
		p1.y() = p1.x();
		p1.x() = temp;
	}

	for (int i = p1.x(); i < p2.x(); ++i)
	{
		float ratio0 = float(i - p1.x()) / float(p2.x() - p1.x());
		float ratio1 = float(i + 1 - p1.x()) / float(p2.x() - p1.x());
		int y0 = int(ratio0 * float(p2.y() - p1.y()) + p1.y());
		int y1 = int(ratio1 * float(p2.y() - p1.y()) + p1.y());
		float r0 = ratio0 * float(p2.color().GetR() - p1.color().GetR()) + p1.color().GetR();
		float g0 = ratio0 * float(p2.color().GetG() - p1.color().GetG()) + p1.color().GetG();
		float b0 = ratio0 * float(p2.color().GetB() - p1.color().GetB()) + p1.color().GetB();
		float r1 = ratio1 * float(p2.color().GetR() - p1.color().GetR()) + p1.color().GetR();
		float g1 = ratio1 * float(p2.color().GetG() - p1.color().GetG()) + p1.color().GetG();
		float b1 = ratio1 * float(p2.color().GetB() - p1.color().GetB()) + p1.color().GetB();
		int r, g, b;
		for (int j = y0; j <= y1; ++j)
		{
			if (y1 == y0)
			{
				r = int(r0);
				g = int(g0);
				b = int(b0);
			}
			else {
				r = rint(float(j - y0) / float(y1 - y0) * (r1 - r0) + r0);
				g = rint(float(j - y0) / float(y1 - y0) * (g1 - g0) + g0);
				b = rint(float(j - y0) / float(y1 - y0) * (b1 - b0) + b0);
			}
			int i_ = (steep ? j : i) * (reverse ? -1 : 1);
			int j_ = steep ? i : j;

			CPixel p(i_, j_, Gdiplus::Color(r, g, b));
			drawPixel(p);

		}
	}
}

void CRenderer::drawLine(CPixel* pts) const
{
	drawLine(pts[0], pts[1]);
}

void CRenderer::drawTriangle(CPixel p1, CPixel p2, CPixel p3) const
{
	drawLine(p1, p2);
	drawLine(p2, p3);
	drawLine(p3, p1);
}

void CRenderer::drawTriangle(CPixel* pts) const
{
	drawTriangle(pts[0], pts[1], pts[2]);
}

void CRenderer::fillTriangle(CPixel p1, CPixel p2, CPixel p3) const
{
	int minX = std::min(p1.x(), std::min(p2.x(), p3.x()));
	int maxX = std::max(p1.x(), std::max(p2.x(), p3.x()));
	int minY = std::min(p1.y(), std::min(p2.y(), p3.y()));
	int maxY = std::max(p1.y(), std::max(p2.y(), p3.y()));

	for (int x = minX; x <= maxX; ++x)
		for (int y = minY; y <= maxY; ++y)
		{
			Eigen::Vector3f b = barycentric(p1, p2, p3, x, y);
			if (b(0) < -0.001 || b(1) < -0.001 || b(2) < -0.001) continue;
			int R = rint(p1.color().GetR() * b(0) + p2.color().GetR() * b(1) + p3.color().GetR() * b(2));
			int G = rint(p1.color().GetG() * b(0) + p2.color().GetG() * b(1) + p3.color().GetG() * b(2));
			int B = rint(p1.color().GetB() * b(0) + p2.color().GetB() * b(1) + p3.color().GetB() * b(2));
			drawPixel(CPixel(x, y, Gdiplus::Color(R, G, B)));
		}
}

void CRenderer::fillTriangle(CPixel* pts) const
{
	fillTriangle(pts[0], pts[1], pts[2]);
}

void CRenderer::fillTriangle(CVertex v1, CVertex v2, CVertex v3) const
{
	if ((v1.v - v2.v).cross(v2.v - v3.v).dot(Eigen::Vector3f(0, 0, 1)) < 0) return;
	const auto scale = 2.5f;
	CPixel p1(rint(v1.v(0) * scale) + 400, rint(v1.v(1) * scale) + 100, v1.c);
	CPixel p2(rint(v2.v(0) * scale) + 400, rint(v2.v(1) * scale) + 100, v2.c);
	CPixel p3(rint(v3.v(0) * scale) + 400, rint(v3.v(1) * scale) + 100, v3.c);

	int minX = std::min(p1.x(), std::min(p2.x(), p3.x()));
	int maxX = std::max(p1.x(), std::max(p2.x(), p3.x()));
	int minY = std::min(p1.y(), std::min(p2.y(), p3.y()));
	int maxY = std::max(p1.y(), std::max(p2.y(), p3.y()));

	for (int x = minX; x <= maxX; ++x)
		for (int y = minY; y <= maxY; ++y)
		{
			Eigen::Vector3f b = barycentric(p1, p2, p3, x, y);
			if (b(0) < 0 || b(1) < 0 || b(2) < 0) continue;
			int R = rint(p1.color().GetR() * b(0) + p2.color().GetR() * b(1) + p3.color().GetR() * b(2));
			int G = rint(p1.color().GetG() * b(0) + p2.color().GetG() * b(1) + p3.color().GetG() * b(2));
			int B = rint(p1.color().GetB() * b(0) + p2.color().GetB() * b(1) + p3.color().GetB() * b(2));
			float z = v1.v(2) * b(0) + v2.v(2) * b(1) + v3.v(2) * b(2);
			drawPixel(CPixel(x, y, Gdiplus::Color(R, G, B)), -z);
		}
}

void CRenderer::fillTriangle(CVertex* vtxs) const
{
	fillTriangle(vtxs[0], vtxs[1], vtxs[2]);
}

void CRenderer::clear() const
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	gr->FillRectangle(&brush, 0, 0, width, height);

	for (int i = 0; i <= width; ++i)
		for (int j = 0; j <= height; ++j)
			z_buffer[i][j] = 10000.0f;
}

void CRenderer::draw() const
{
	graphics->DrawImage(buffer, 0, 0);
}
