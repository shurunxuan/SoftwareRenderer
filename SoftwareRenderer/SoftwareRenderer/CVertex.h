#pragma once
#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"

class CVertex
{
private:
	Eigen::Vector3f v;
	Eigen::Vector3f n;
	Gdiplus::Color c;

public:
	CVertex(Eigen::Vector3f position, Eigen::Vector3f normal = Eigen::Vector3f::Zero(), Gdiplus::Color color = Gdiplus::Color(255, 255, 255));
	CVertex(Eigen::Vector3f position, Gdiplus::Color color);
};