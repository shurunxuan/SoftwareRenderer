#include <fstream>
#include <map>
#include <locale>
#include <codecvt>
#include "CModel.h"
#pragma comment (lib, "Gdiplus.lib")

std::vector<std::string> split(std::string str, char ch)
{
	std::vector<std::string> result;
	std::string temp(str);
	size_t pos;

	while ((pos = temp.find(ch)) != std::string::npos)
	{
		std::string s(temp.begin(), temp.begin() + pos);
		result.push_back(s);
		temp = std::string(temp.begin() + pos + 1, temp.end());
	}
	result.push_back(temp);

	return result;
}

template <typename T>
T str2num(std::string str, T& num)
{
	std::stringstream ss;
	ss << str;
	ss >> num;
	return num;
}


CModel::SFace::SFace(CVertex v, SMtl m)
	:vertex(v), material(m)
{ }

CModel::CModel(std::string obj_file, std::string mtl_file)
{
	std::map<std::string, SMtl> mtl_map;
	if (mtl_file != "")
	{
		std::ifstream fin(mtl_file);
		std::string line;
		SMtl* current_mtl = nullptr;
		while (getline(fin, line))
		{
			auto s = split(line, ' ');
			std::string first_token(s[0]);
			if (first_token == "newmtl")
			{
				SMtl newmtl;
				newmtl.name = s[1];
				mtl_map[s[1]] = newmtl;
				current_mtl = &mtl_map[s[1]];
			}
			else if (first_token == "Kd")
			{
				float r, g, b;
				str2num(s[1], r);
				str2num(s[2], g);
				str2num(s[3], b);
				current_mtl->diffuse = Eigen::Vector3f(r, g, b);
			}
			else if (first_token == "Ka")
			{
				float r, g, b;
				str2num(s[1], r);
				str2num(s[2], g);
				str2num(s[3], b);
				current_mtl->ambient = Eigen::Vector3f(r, g, b);
			}
			else if (first_token == "Ks")
			{
				float r, g, b;
				str2num(s[1], r);
				str2num(s[2], g);
				str2num(s[3], b);
				current_mtl->specular = Eigen::Vector3f(r, g, b);
			}
			else if (first_token == "Ke")
			{
				float r, g, b;
				str2num(s[1], r);
				str2num(s[2], g);
				str2num(s[3], b);
				current_mtl->emission = Eigen::Vector3f(r, g, b);
			}
			else if (first_token == "Ns")
			{
				str2num(s[1], current_mtl->shininess);
			}
			else if (first_token == "map_Kd")
			{
				// string -> wstring
				std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
				std::wstring name = cv.from_bytes(s[1]);
				current_mtl->texture = Gdiplus::Bitmap::FromFile(name.c_str());
			}
			else
			{

			}
		}
	}
	std::ifstream fin(obj_file);
	std::string line;
	std::string current_mtl = "";
	while (getline(fin, line))
	{
		auto s = split(line, ' ');
		std::string first_token(s[0]);
		if (first_token == "v")
		{
			Eigen::Vector3f v;
			for (int i = 0; i < 3; ++i)
				str2num(s[i + 1], v(i));
			vertices.push_back(v);

		}
		else if (first_token == "vn")
		{
			Eigen::Vector3f n;
			for (int i = 0; i < 3; ++i)
				str2num(s[i + 1], n(i));
			normals.push_back(n);
		}
		else if (first_token == "vt")
		{
			Eigen::Vector2f t;
			for (int i = 0; i < 2; ++i)
				str2num(s[i + 1], t(i));
			texture.push_back(t);
		}
		else if (first_token == "f")
		{
			TFace face;
			for (int i = 0; i < 3; ++i)
			{
				std::string p(s[i + 1]);
				auto p_detail = split(p, '/');
				int index = -1;
				str2num(p_detail[0], index);
				Eigen::Vector3f v_f(vertices[index - 1]);
				str2num(p_detail[1], index);
				Eigen::Vector3f n_f(normals[index - 1]);
				str2num(p_detail[2], index);
				Eigen::Vector2f t_f(texture[index - 1]);
				CVertex vertex(v_f, n_f, t_f);
				if (mtl_file != "")
				{
					SFace this_face(vertex, mtl_map[current_mtl]);
					face.push_back(this_face);
				}
				else
				{
					SFace this_face(vertex, SMtl());
					face.push_back(this_face);
				}
			}
			faces.push_back(face);
		}
		else if (first_token == "usemtl" && mtl_file != "")
		{
			current_mtl = s[1];
		}
	}
}
