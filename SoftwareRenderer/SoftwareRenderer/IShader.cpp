#include "IShader.h"



IShader::IShader()
{
}


IShader::~IShader()
{
}

void IShader::setLight(Eigen::Vector3f light)
{
	light_ = light;
}
