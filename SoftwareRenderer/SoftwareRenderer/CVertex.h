#pragma once
#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"

class CVertex
{
public:
	Eigen::Vector3f v;
	Eigen::Vector3f n;
	Eigen::Vector2f t;
	Gdiplus::Color c;

public:
	CVertex(Eigen::Vector3f position = Eigen::Vector3f::Zero(), Eigen::Vector3f normal = Eigen::Vector3f::Zero(), Eigen::Vector2f texture = Eigen::Vector2f::Zero(), Gdiplus::Color color = Gdiplus::Color(255, 255, 255));
	CVertex(Eigen::Vector3f position, Eigen::Vector2f texture, Gdiplus::Color color = Gdiplus::Color(255, 255, 255));
	CVertex(Eigen::Vector3f position, Gdiplus::Color color);
};

std::ostream& operator<<(std::ostream& os, CVertex vertex);