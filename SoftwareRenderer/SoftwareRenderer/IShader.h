#pragma once
#include <vector>
#include "CVertex.h"
#include "CModel.h"

class IShader
{
public:
	IShader();
	virtual ~IShader();
	virtual std::vector<CVertex> vertex(CModel::TFace& input) = 0;
	virtual Gdiplus::Color pixel(Eigen::Vector3f barycentric) = 0;
};
