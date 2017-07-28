#include "CVertex.h"
#pragma comment (lib, "Gdiplus.lib")
CVertex::CVertex(Eigen::Vector3f position, Eigen::Vector3f normal, Eigen::Vector2f texture, Gdiplus::Color color)
	: v(position), n(normal), t(texture), c(color)
{}

CVertex::CVertex(Eigen::Vector3f position, Eigen::Vector2f texture, Gdiplus::Color color)
	: v(position), n(Eigen::Vector3f::Zero()), t(texture), c(color)
{}

CVertex::CVertex(Eigen::Vector3f position, Gdiplus::Color color)
	: v(position), n(Eigen::Vector3f::Zero()), t(Eigen::Vector2f::Zero()), c(color)
{}

std::ostream& operator<<(std::ostream& os, CVertex vertex)
{
	os << "v: " << std::endl;
	os << vertex.v << std::endl;
	os << "n: " << std::endl;
	os << vertex.n << std::endl;
	os << "t: " << std::endl;
	os << vertex.t << std::endl;
	return os;
}


