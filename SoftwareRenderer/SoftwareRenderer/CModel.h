#pragma once

#include <vector>
#include "CVertex.h"

class CModel
{
public:
	struct SMtl
	{
		std::string name = "";
		Eigen::Vector3f ambient = Eigen::Vector3f::Zero();
		Eigen::Vector3f diffuse = Eigen::Vector3f::Zero();
		Eigen::Vector3f specular = Eigen::Vector3f::Zero();
		Eigen::Vector3f emission = Eigen::Vector3f::Zero();
		float shininess = 0.0f;
		Gdiplus::Bitmap* texture = nullptr;
	};
	struct SFace
	{
		CVertex vertex;
		SMtl material;

		SFace(CVertex v, SMtl m);
	};
	typedef std::vector<SFace> TFace;
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> normals;
	std::vector<Eigen::Vector2f> texture;
	std::vector<TFace> faces;
public:
	CModel(std::string base_path, std::string obj_file, std::string mtl_file = "");
};

