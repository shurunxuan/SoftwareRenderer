#include "CPixel.h"
#pragma comment (lib, "Gdiplus.lib")

CPixel::CPixel(Eigen::Vector2i position, Gdiplus::Color color)
	:p(position), c(color)
{}

CPixel::CPixel(int x, int y, Gdiplus::Color color)
	: p(Eigen::Vector2i(x, y)), c(color)
{}

int& CPixel::x()
{
	return p(0);
}

int& CPixel::y()
{
	return p(1);
}

Gdiplus::Color CPixel::color() const
{
	return c;
}
