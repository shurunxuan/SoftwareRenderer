#include "CCamera.h"
#include "Eigen/Geometry"

void CCamera::lookat(Eigen::Vector3f _eye, Eigen::Vector3f _center, Eigen::Vector3f _up)
{
	eye = _eye;
	center = _center;
	up = _up;
	Eigen::Vector3f z = (_eye - _center);
	z.normalize();
	Eigen::Vector3f x = _up.cross(z);
	x.normalize();
	Eigen::Vector3f y = z.cross(x);
	y.normalize();
	Eigen::Matrix4f Minv = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f Tr = Eigen::Matrix4f::Identity();
	for (int i = 0; i < 3; i++) {
		Minv(0, i) = x(i);
		Minv(1, i) = y(i);
		Minv(2, i) = z(i);
		Tr(i, 3) = -_center(i);
	}
	ModelView = Minv * Tr;
}

Eigen::Matrix4f CCamera::modelView() const
{
	return ModelView;
}

Eigen::Matrix4f CCamera::viewPort(int x, int y) const
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();

	m(0, 3) = x + width / 2.f;
	m(1, 3) = y + height / 2.f;
	m(2, 3) = depth / 2.f;

	m(0, 0) = width / 2.f;
	m(1, 1) = height / 2.f;
	m(2, 2) = depth / 2.f;
	return m;
}

Eigen::Matrix4f CCamera::projection() const
{
	Eigen::Matrix4f Projection = Eigen::Matrix4f::Identity();
	Projection(3, 2) = 1.f / (eye - center).norm();
	return Projection;
}

void CCamera::setWidth(int _width)
{
	width = _width;
}

void CCamera::setHeight(int _height)
{
	height = _height;
}

void CCamera::setDepth(int _depth)
{
	depth = _depth;
}

Eigen::Vector3f CCamera::direction() const
{
	Eigen::Vector3f result = eye - center;
	result.normalize();
	return result;
}
