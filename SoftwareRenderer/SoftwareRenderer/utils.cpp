#include "utils.h"
#pragma comment (lib,"Gdiplus.lib")

Eigen::Vector3f barycentric(CPixel p1, CPixel p2, CPixel p3, int x, int y) 
{
	Eigen::Vector3f u = Eigen::Vector3f(p3.x() - p1.x(), p2.x() - p1.x(), p1.x() - x).cross(Eigen::Vector3f(p3.y() - p1.y(), p2.y() - p1.y(), p1.y() - y));
	if (std::abs(u(2)) < 1) return Eigen::Vector3f(-1, 1, 1); // triangle is degenerate, in this case return smth with negative coordinates 
	return Eigen::Vector3f(1.f - (u(0) + u(1)) / u(2), u(1) / u(2), u(0) / u(2));
}

Gdiplus::Color getColorFromBitmap(float x, float y, Gdiplus::Bitmap* pBitmap)
{
	Gdiplus::Color color;
	float x_ = (x - int(x)) * pBitmap->GetWidth();
	float y_ = (1.0f - (y - int(y))) * pBitmap->GetHeight();
	pBitmap->GetPixel(int(x_), int(y_), &color);
	return color;
}