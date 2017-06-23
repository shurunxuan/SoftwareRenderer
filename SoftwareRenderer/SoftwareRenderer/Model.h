#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include "geometry.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
	std::vector<Vec3f> norms_;
	std::vector<Vec2f> uv_;
	Color** diffusemap_;
	//void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f norm(int iface, int nvert);
	Vec3f vert(int i);
	Vec2i uv(int iface, int nvert);
	Color diffuse(Vec2i uv);
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__