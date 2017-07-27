#pragma once

#include "Eigen/Dense"

class CCamera
{
private:
	Eigen::Matrix4f ModelView;

	int width;
	int height;
	int depth;

	Eigen::Vector3f eye;
	Eigen::Vector3f center;
	Eigen::Vector3f up;

public:
	CCamera(int _width, int _height, int _depth) :
		width(_width), height(_height), depth(_depth) {}
	CCamera() :
		width(0), height(0), depth(255) {}
	~CCamera() {}

	void lookat(Eigen::Vector3f _eye, Eigen::Vector3f _center, Eigen::Vector3f _up);
	Eigen::Matrix4f modelView() const;
	Eigen::Matrix4f viewPort(int x = 0, int y = 0) const;
	Eigen::Matrix4f projection() const;

	void setWidth(int _width);
	void setHeight(int _height);
	void setDepth(int _depth);

	Eigen::Vector3f direction() const;
};