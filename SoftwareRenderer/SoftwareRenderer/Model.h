#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include "Eigen/Dense"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class Model {
private:
	std::vector<Eigen::Vector3f> verts_;
	std::vector<std::vector<Eigen::Vector3i> > faces_; // attention, this Eigen::Vector3i means vertex/uv/normal
	std::vector<Eigen::Vector3f> norms_;
	std::vector<Eigen::Vector2f> uv_;
	Color** diffusemap_;
	//void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts() const;
	int nfaces() const;
	Eigen::Vector3f norm(int iface, int nvert);
	Eigen::Vector3f vert(int i);
	Eigen::Vector2i uv(int iface, int nvert);
	Color diffuse(Eigen::Vector2i uv) const;
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__