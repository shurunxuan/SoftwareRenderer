#include <fstream>
#include "CModel.h"

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


CModel::CModel(std::string obj_file)
{
	std::ifstream fin(obj_file);
	std::string line;
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
				face.push_back(vertex);
			}
			faces.push_back(face);
		}
		else
		{		}
	}
}
