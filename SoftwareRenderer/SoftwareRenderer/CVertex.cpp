#include "CVertex.h"
#pragma comment (lib, "Gdiplus.lib")

CVertex::CVertex(Eigen::Vector3f position, Eigen::Vector3f normal, Gdiplus::Color color)
	:v(position), n(normal), c(color)
{}

CVertex::CVertex(Eigen::Vector3f position, Gdiplus::Color color)
	: v(position), n(Eigen::Vector3f::Zero()), c(color)
{}
