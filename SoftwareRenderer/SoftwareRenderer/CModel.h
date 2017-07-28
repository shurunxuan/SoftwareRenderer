#pragma once

#include <vector>
#include "CVertex.h"

class CModel
{
public:
	typedef std::vector<CVertex> TFace;
	std::string filename;
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> normals;
	std::vector<Eigen::Vector2f> texture;
	std::vector<TFace> faces;
public:
	CModel(std::string obj_file);
};

