#include <algorithm>
#include <fstream>
#include "CRenderer.h"
#include "Eigen/Dense"
#include "Eigen/Geometry"
#include "utils.h"
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min
#define rint(v) int(round(v))

//Eigen::Vector3f barycentric(CPixel p1, CPixel p2, CPixel p3, int x, int y)
//{
//	//Eigen::Vector3f result;
//	//result(0) = float((p2.y() - p3.y()) * (x - p3.x()) + (p3.x() - p2.x()) * (y - p3.y())) / float((p2.y() - p3.y()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.y() - p3.y()));
//	//result(1) = float((p3.y() - p1.y()) * (x - p3.x()) + (p1.x() - p3.x()) * (y - p3.y())) / float((p2.y() - p3.y()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.y() - p3.y()));
//	//result(2) = 1 - result(0) - result(1);
//
//	Eigen::Matrix3f R;
//	R(0, 0) = p1.x(); R(0, 1) = p2.x(); R(0, 2) = p3.x();
//	R(1, 0) = p1.y(); R(1, 1) = p2.y(); R(1, 2) = p3.y();
//	R(2, 0) = 1;      R(2, 1) = 1;      R(2, 2) = 1;
//	Eigen::Vector3f P;
//	P(0) = x; P(1) = y; P(2) = 1;
//	Eigen::Vector3f result = R.inverse() * P;
//
//	return result;
//}



void CRenderer::init()
{
	hdc_ = BeginPaint(hwnd_, &ps_);
	graphics_ = new Gdiplus::Graphics(hdc_);
	RECT rect;
	GetClientRect(hwnd_, &rect);
	width_ = rect.right - rect.left;
	height_ = rect.bottom - rect.top;

	buffer_ = new Gdiplus::Bitmap(width_, height_);
	gr_ = new Gdiplus::Graphics(buffer_);

	z_buffer_ = new float*[width_ + 1];
	for (int i = 0; i <= width_; ++i)
		z_buffer_[i] = new float[height_ + 1];
}

void CRenderer::deinit() const
{
	for (int i = 0; i < width_; ++i)
		delete z_buffer_[i];
	delete z_buffer_;

	delete gr_;
	delete buffer_;
	delete graphics_;
	EndPaint(hwnd_, &ps_);
}

CRenderer::CRenderer(HWND hwnd)
	: hwnd_(hwnd)
{
	init();
}

CRenderer::~CRenderer()
{
	deinit();
}

void CRenderer::resizeBuffer()
{
	if (gr_ == nullptr || buffer_ == nullptr || graphics_ == nullptr || z_buffer_ == nullptr) return;

	deinit();
	init();

	resetPerspectiveCamera();
}

long CRenderer::getWidth() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto width = rect.right - rect.left;
	return width;
}

long CRenderer::getHeight() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto height = rect.bottom - rect.top;
	return height;
}

void CRenderer::drawPixel(CPixel pixel, float z) const
{
	if (pixel.x() < 0 || pixel.x() > width_) return;
	if (pixel.y() < 0 || pixel.y() > height_) return;
	// Z-buffering
	if (z_buffer_[pixel.x()][pixel.y()] < z) return;
	buffer_->SetPixel(pixel.x(), height_ - pixel.y(), pixel.color());
	z_buffer_[pixel.x()][pixel.y()] = z;
}

void CRenderer::drawLine(CPixel p1, CPixel p2) const
{
	if (p2.y() < p1.y())
	{
		drawLine(p2, p1);
		return;
	}
	drawPixel(p2);
	bool reverse = p2.x() < p1.x();
	if (reverse)
	{
		p2.x() = -p2.x();
		p1.x() = -p1.x();
	}
	bool steep = p2.y() - p1.y() > p2.x() - p1.x();
	if (steep)
	{
		int temp = p2.y();
		p2.y() = p2.x();
		p2.x() = temp;
		temp = p1.y();
		p1.y() = p1.x();
		p1.x() = temp;
	}

	for (int i = p1.x(); i < p2.x(); ++i)
	{
		float ratio0 = float(i - p1.x()) / float(p2.x() - p1.x());
		float ratio1 = float(i + 1 - p1.x()) / float(p2.x() - p1.x());
		int y0 = int(ratio0 * float(p2.y() - p1.y()) + p1.y());
		int y1 = int(ratio1 * float(p2.y() - p1.y()) + p1.y());
		float r0 = ratio0 * float(p2.color().GetR() - p1.color().GetR()) + p1.color().GetR();
		float g0 = ratio0 * float(p2.color().GetG() - p1.color().GetG()) + p1.color().GetG();
		float b0 = ratio0 * float(p2.color().GetB() - p1.color().GetB()) + p1.color().GetB();
		float r1 = ratio1 * float(p2.color().GetR() - p1.color().GetR()) + p1.color().GetR();
		float g1 = ratio1 * float(p2.color().GetG() - p1.color().GetG()) + p1.color().GetG();
		float b1 = ratio1 * float(p2.color().GetB() - p1.color().GetB()) + p1.color().GetB();
		int r, g, b;
		for (int j = y0; j <= y1; ++j)
		{
			if (y1 == y0)
			{
				r = int(r0);
				g = int(g0);
				b = int(b0);
			}
			else {
				r = rint(float(j - y0) / float(y1 - y0) * (r1 - r0) + r0);
				g = rint(float(j - y0) / float(y1 - y0) * (g1 - g0) + g0);
				b = rint(float(j - y0) / float(y1 - y0) * (b1 - b0) + b0);
			}
			int i_ = (steep ? j : i) * (reverse ? -1 : 1);
			int j_ = steep ? i : j;

			CPixel p(i_, j_, Gdiplus::Color(r, g, b));
			drawPixel(p);

		}
	}
}

void CRenderer::drawLine(CPixel* pts) const
{
	drawLine(pts[0], pts[1]);
}

void CRenderer::drawTriangle(CPixel p1, CPixel p2, CPixel p3) const
{
	drawLine(p1, p2);
	drawLine(p2, p3);
	drawLine(p3, p1);
}

void CRenderer::drawTriangle(CPixel* pts) const
{
	drawTriangle(pts[0], pts[1], pts[2]);
}

void CRenderer::fillTriangle(CPixel p1, CPixel p2, CPixel p3) const
{
	int minX = std::min(p1.x(), std::min(p2.x(), p3.x()));
	int maxX = std::max(p1.x(), std::max(p2.x(), p3.x()));
	int minY = std::min(p1.y(), std::min(p2.y(), p3.y()));
	int maxY = std::max(p1.y(), std::max(p2.y(), p3.y()));

	for (int x = minX; x <= maxX; ++x)
		for (int y = minY; y <= maxY; ++y)
		{
			Eigen::Vector3f b = barycentric(p1, p2, p3, x, y);
			if (b(0) < 0 || b(1) < 0 || b(2) < 0) continue;
			int R = rint(p1.color().GetR() * b(0) + p2.color().GetR() * b(1) + p3.color().GetR() * b(2));
			int G = rint(p1.color().GetG() * b(0) + p2.color().GetG() * b(1) + p3.color().GetG() * b(2));
			int B = rint(p1.color().GetB() * b(0) + p2.color().GetB() * b(1) + p3.color().GetB() * b(2));
			drawPixel(CPixel(x, y, Gdiplus::Color(R, G, B)));
		}
}

void CRenderer::fillTriangle(CPixel* pts) const
{
	fillTriangle(pts[0], pts[1], pts[2]);
}

void CRenderer::fillTriangle(CVertex v1, CVertex v2, CVertex v3) const
{
	// Backface culling
	if ((v1.v - v2.v).cross(v2.v - v3.v).dot(Eigen::Vector3f(0, 0, 1)) < 0) return;

	// View transformation
	const auto scale = 2.5f;
	CPixel p1(rint(v1.v(0) * scale) + 400, rint(v1.v(1) * scale) + 100, v1.c);
	CPixel p2(rint(v2.v(0) * scale) + 400, rint(v2.v(1) * scale) + 100, v2.c);
	CPixel p3(rint(v3.v(0) * scale) + 400, rint(v3.v(1) * scale) + 100, v3.c);

	// Find bounding box
	int minX = std::min(p1.x(), std::min(p2.x(), p3.x()));
	int maxX = std::max(p1.x(), std::max(p2.x(), p3.x()));
	int minY = std::min(p1.y(), std::min(p2.y(), p3.y()));
	int maxY = std::max(p1.y(), std::max(p2.y(), p3.y()));

	for (int x = minX; x <= maxX; ++x)
		for (int y = minY; y <= maxY; ++y)
		{
			// See if the pixel is in the triangle
			Eigen::Vector3f b = barycentric(p1, p2, p3, x, y);
			if (b(0) < 0 || b(1) < 0 || b(2) < 0) continue;
			// Interpolate color
			int R = rint(p1.color().GetR() * b(0) + p2.color().GetR() * b(1) + p3.color().GetR() * b(2));
			int G = rint(p1.color().GetG() * b(0) + p2.color().GetG() * b(1) + p3.color().GetG() * b(2));
			int B = rint(p1.color().GetB() * b(0) + p2.color().GetB() * b(1) + p3.color().GetB() * b(2));
			// Interpolate z coordinate
			float z = v1.v(2) * b(0) + v2.v(2) * b(1) + v3.v(2) * b(2);
			drawPixel(CPixel(x, y, Gdiplus::Color(R, G, B)), -z);
		}
}

void CRenderer::fillTriangle(CVertex* vtxs) const
{
	fillTriangle(vtxs[0], vtxs[1], vtxs[2]);
}

void CRenderer::fillTriangle(CModel::TFace face)
{
	// Vertex shader
	auto vs_out = shader_->vertex(face);
	CVertex v1(vs_out[0]);
	CVertex v2(vs_out[1]);
	CVertex v3(vs_out[2]);

	// Backface culling
	if ((v1.v - v2.v).cross(v2.v - v3.v)(2) < 0) return;

	// Just clip all the triangle if one vertex is outside the viewport
	//if (v1.v(0) > width_ || v1.v(0) < 0) return;
	//if (v2.v(0) > width_ || v2.v(0) < 0) return;
	//if (v3.v(0) > width_ || v3.v(0) < 0) return;
	//if (v1.v(1) > height_ || v1.v(1) < 0) return;
	//if (v2.v(1) > height_ || v2.v(1) < 0) return;
	//if (v3.v(1) > height_ || v3.v(1) < 0) return;
	//const auto scale = 2.5f;
	//CPixel p1(rint(v1.v(0) * scale) + 400, rint(v1.v(1) * scale) + 100, v1.c);
	//CPixel p2(rint(v2.v(0) * scale) + 400, rint(v2.v(1) * scale) + 100, v2.c);
	//CPixel p3(rint(v3.v(0) * scale) + 400, rint(v3.v(1) * scale) + 100, v3.c);
	CPixel p1(rint(v1.v(0)), rint(v1.v(1)), v1.c);
	CPixel p2(rint(v2.v(0)), rint(v2.v(1)), v2.c);
	CPixel p3(rint(v3.v(0)), rint(v3.v(1)), v3.c);

	// Find bounding box
	int minX = std::min(p1.x(), std::min(p2.x(), p3.x()));
	int maxX = std::max(p1.x(), std::max(p2.x(), p3.x()));
	int minY = std::min(p1.y(), std::min(p2.y(), p3.y()));
	int maxY = std::max(p1.y(), std::max(p2.y(), p3.y()));

	for (int x = minX; x <= maxX; ++x)
		for (int y = minY; y <= maxY; ++y)
		{
			// See if the pixel is in the triangle
			Eigen::Vector3f b = barycentric(p1, p2, p3, x, y);
			if (b(0) < 0 || b(1) < 0 || b(2) < 0) continue;
			// Interpolate z coordinate
			float z = v1.v(2) * b(0) + v2.v(2) * b(1) + v3.v(2) * b(2);

			// Pixel shader
			const Gdiplus::Color pixel_color(shader_->pixel(b));
			drawPixel(CPixel(x, y, pixel_color), -z);
		}
}

void CRenderer::setPerspectiveCamera(float near, float far, float fov)
{
	camera_.n = near;
	camera_.f = far;
	camera_.theta = fov;
	resetPerspectiveCamera();
}

void CRenderer::cameraLookat(Eigen::Vector3f e, Eigen::Vector3f g, Eigen::Vector3f t)
{
	camera_.e = e;
	camera_.g = g;
	camera_.w = -g / abs(g.norm());
	auto temp = t.cross(camera_.w);
	camera_.u = temp / abs(temp.norm());
	camera_.v = camera_.w.cross(camera_.u);
}

void CRenderer::resetPerspectiveCamera()
{
	camera_.t = tanf(camera_.theta / 2.0f) * abs(camera_.n);
	camera_.r = float(width_) / float(height_) * camera_.t;
	camera_.l = -camera_.r;
	camera_.b = -camera_.t;
}

Eigen::Vector3f CRenderer::getCameraPosition()
{
	return camera_.e;
}

Eigen::Matrix4f CRenderer::viewport()
{
	Eigen::Matrix4f Mvp(Eigen::Matrix4f::Zero());
	Mvp(0, 0) = width_ / 2.0f;
	Mvp(1, 1) = height_ / 2.0f;
	Mvp(2, 2) = Mvp(3, 3) = 1.0f;
	Mvp(0, 3) = (width_ - 1) / 2.0f;
	Mvp(1, 3) = (height_ - 1) / 2.0f;
	return Mvp;
}

Eigen::Matrix4f CRenderer::orthographic()
{
	Eigen::Matrix4f Morth(Eigen::Matrix4f::Zero());
	Morth(0, 0) = 2.0f / (camera_.r - camera_.l);
	Morth(1, 1) = 2.0f / (camera_.t - camera_.b);
	Morth(2, 2) = 2.0f / (camera_.n - camera_.f);
	Morth(3, 3) = 1.0f;
	Morth(0, 3) = -(camera_.r + camera_.l) * (camera_.r - camera_.l);
	Morth(1, 3) = -(camera_.t + camera_.b) * (camera_.t - camera_.b);
	Morth(2, 3) = -(camera_.n + camera_.f) * (camera_.n - camera_.f);
	return Morth;
}

Eigen::Matrix4f CRenderer::P()
{
	Eigen::Matrix4f p(Eigen::Matrix4f::Zero());
	p(0, 0) = p(1, 1) = camera_.n;
	p(2, 2) = camera_.n + camera_.f;
	p(2, 3) = -camera_.f * camera_.n;
	p(3, 2) = 1.0f;
	return p;
}


Eigen::Matrix4f CRenderer::cameraTrans()
{
	Eigen::Matrix4f Mcam(Eigen::Matrix4f::Zero());
	Mcam(0, 0) = camera_.u(0); Mcam(0, 1) = camera_.u(1); Mcam(0, 2) = camera_.u(2);
	Mcam(1, 0) = camera_.v(0); Mcam(1, 1) = camera_.v(1); Mcam(1, 2) = camera_.v(2);
	Mcam(2, 0) = camera_.w(0); Mcam(2, 1) = camera_.w(1); Mcam(2, 2) = camera_.w(2);
	Mcam(3, 3) = 1.0f;
	Eigen::Matrix4f Mcam2(Eigen::Matrix4f::Identity());
	Mcam2(0, 3) = -camera_.e(0);
	Mcam2(1, 3) = -camera_.e(1);
	Mcam2(2, 3) = -camera_.e(2);
	return Mcam * Mcam2;
}


void CRenderer::clear() const
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	gr_->FillRectangle(&brush, 0, 0, width_, height_);

	for (int i = 0; i <= width_; ++i)
		for (int j = 0; j <= height_; ++j)
			z_buffer_[i][j] = 10000.0f;
}

void CRenderer::draw() const
{
	graphics_->DrawImage(buffer_, 0, 0);
}

CVertex CRenderer::trans(CVertex v)
{
	auto v_ = viewport();
	auto o = orthographic();
	auto p = P();
	auto c = cameraTrans();
	auto M = v_ * o * p * c;
	auto M_it = M.inverse().transpose();

	Eigen::Vector4f v_homo(v.v(0), v.v(1), v.v(2), 1.0f);
	Eigen::Vector4f n_homo(v.n(0), v.n(1), v.n(2), 0.0f);


	Eigen::Vector4f rv_homo = M * v_homo;
	Eigen::Vector4f rn_homo = M_it * n_homo;

	Eigen::Vector3f rv(rv_homo(0) / rv_homo(3), rv_homo(1) / rv_homo(3), rv_homo(2) / rv_homo(3));
	Eigen::Vector3f rn(rn_homo(0), rn_homo(1), rn_homo(2));

	CVertex result(rv, rn.normalized(), v.t, v.c);
	//std::ofstream fout("log.txt");
	//fout << result << std::endl << std::endl;
	//fout << v << std::endl;
	//fout.close();
	return result;
}

void CRenderer::setShader(IShader* shader)
{
	shader_ = shader;
}
