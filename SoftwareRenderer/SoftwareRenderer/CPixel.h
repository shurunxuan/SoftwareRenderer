#pragma once
#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"

class CPixel
{
public:
	Eigen::Vector2i p;
	Gdiplus::Color c;
public:
	CPixel(Eigen::Vector2i position = Eigen::Vector2i::Zero(), Gdiplus::Color color = Gdiplus::Color(255, 255, 255));
	CPixel(int x, int y, Gdiplus::Color color = Gdiplus::Color(255, 255, 255));

	int& x();
	int& y();
	Gdiplus::Color color() const;
};

