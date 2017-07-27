#include <algorithm>
#include "CRenderer.h"
#include "Eigen/Geometry"
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min

Eigen::Vector3f barycentric(Eigen::Vector3f A, Eigen::Vector3f B, Eigen::Vector3f C, Eigen::Vector3f P) {
	Eigen::Vector3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Eigen::Vector3f u = s[0].cross(s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
	{
		Eigen::Vector3f result(1.f - (u(0) + u(1)) / u(2), u(1) / u(2), u(0) / u(2));
		return result;
	}
	return Eigen::Vector3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void CRenderer::init()
{
	hdc_ = BeginPaint(hwnd_, &ps);
	graphics = new Gdiplus::Graphics(hdc_);
	RECT rect;
	GetClientRect(hwnd_, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	camera.setWidth(width);
	camera.setHeight(height);

	zbuffer = new float[width * height];
	for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	zbuffer_color = new Gdiplus::Color*[width];
	for (int i = 0; i < width; ++i) zbuffer_color[i] = new Gdiplus::Color[height];

	buffer = new Gdiplus::Bitmap(width, height);
	gr = new Gdiplus::Graphics(buffer);
}

void CRenderer::deinit() const
{
	delete gr;
	delete buffer;
	delete zbuffer;
	delete graphics;
	for (int i = 0; i < width; ++i) delete zbuffer_color[i];
	delete zbuffer_color;
	EndPaint(hwnd_, &ps);
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
	if (gr == nullptr || buffer == nullptr || graphics == nullptr || zbuffer == nullptr) return;

	deinit();
	init();
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

void CRenderer::drawPixel(int x, int y, Gdiplus::Color color)
{
	zbuffer_color[x][y] = color;
}

void CRenderer::drawLine(int x0, int y0, int x1, int y1, Gdiplus::Color color)
{
	line_param lp;

	lp.steep = (std::abs(x0 - x1) < std::abs(y0 - y1));
	if (lp.steep) { // if the line is steep, we transpose the image 
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	lp.x0 = x0;
	lp.y0 = y0;
	lp.x1 = x1;
	lp.y1 = y1;
	lp.color = color;
	int dx = lp.x1 - lp.x0;
	int dy = lp.y1 - lp.y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = lp.y0;
	for (int x = lp.x0; x <= lp.x1; ++x) {
		if (lp.steep) {
			drawPixel(y, x, lp.color);
		}
		else {
			drawPixel(x, y, lp.color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (lp.y1 > lp.y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void CRenderer::drawLine(Eigen::Vector2i v0, Eigen::Vector2i v1, Gdiplus::Color color)
{
	drawLine(v0(0), v0(1), v1(0), v1(1), color);
}

void CRenderer::drawTriangle(Eigen::Vector2i v0, Eigen::Vector2i v1, Eigen::Vector2i v2, Gdiplus::Color color)
{
	drawLine(v0, v1, color);
	drawLine(v1, v2, color);
	drawLine(v2, v0, color);
}

void CRenderer::fillTriangle(Eigen::Vector3f* coords, Gdiplus::Color color)
{
	fillTriangle(coords, nullptr, color, false);
}

void CRenderer::fillTriangle(Eigen::Vector3f* coords, Eigen::Vector3f* nor, Gdiplus::Color color)
{
	fillTriangle(coords, nor, color, true);
}

void CRenderer::fillTriangle(Eigen::Vector3f* coords, Eigen::Vector3f* nor, Gdiplus::Color color, bool shading)
{
	// Backface culling
	Eigen::Vector3f n = ((coords[2] - coords[0]).cross(coords[1] - coords[0]));
	n.normalize();
	if (n.dot(cameraDirection()) < 0) return;


	// Lighting 
	Eigen::Vector3f light_dir = { 0, 0, -1 };
	light_dir.normalize();

	float intensity = n.dot(light_dir);
	if (intensity < 0) intensity = 0;
	if (intensity > 1) intensity = 1;
	Gdiplus::Color flat_color = Gdiplus::Color(color.GetR() * intensity, color.GetG() * intensity, color.GetB() * intensity);

	// Color of pts
	Gdiplus::Color pts_color[3];
	// Affine transformed coords
	Eigen::Vector3f pts[3];
	// Affine transforEigen::Matrix4fion
	for (int j = 0; j < 3; j++)
	{
		Eigen::Vector3f v = coords[j];
		Eigen::Vector4f v_homo(v(0), v(1), v(2), 0);
		v = (cameraMatrix() * v_homo).head<3>();

		pts[j] = v;

		if (shading)
		{
			Eigen::Vector3f pts_norm = nor[j];
			pts_norm.normalize();
			float gouraud_intensity = -1 * pts_norm.dot(light_dir);
			if (gouraud_intensity < 0) gouraud_intensity = 0;
			if (gouraud_intensity > 1) gouraud_intensity = 1;
			pts_color[j] = Gdiplus::Color(color.GetR() * gouraud_intensity, color.GetG() * gouraud_intensity, color.GetB() * gouraud_intensity);
		}
	}


	Eigen::Vector2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Eigen::Vector2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Eigen::Vector2f clamp(getWidth() - 1, getHeight() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Eigen::Vector3f P;
	for (P(0) = bboxmin(0); P(0) <= bboxmax(0); P(0)++) {
		for (P(1) = bboxmin(1); P(1) <= bboxmax(1); P(1)++) {
			Eigen::Vector3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen(0) < 0 || bc_screen(1) < 0 || bc_screen(2) < 0) continue;
			P(2) = 0;
			Gdiplus::Color pixel_color = Gdiplus::Color(0, 0, 0);
			if (!shading) pixel_color = flat_color;
			for (int i = 0; i < 3; i++)
			{
				P(2) += pts[i][2] * bc_screen[i];
				if (shading)
					pixel_color = Gdiplus::Color(pts_color[i].GetR() * bc_screen[i] + pixel_color.GetR(),
						pts_color[i].GetG() * bc_screen[i] + pixel_color.GetG(),
						pts_color[i].GetB() * bc_screen[i] + pixel_color.GetB());
			}
			if (zbuffer[int(P(0) + P(1)*width)] < P(2)) {
				zbuffer[int(P(0) + P(1)*width)] = P(2);
				drawPixel(P(0), P(1), pixel_color);
			}
		}
	}
}


void CRenderer::fillTriangle(Eigen::Vector3f v0, Eigen::Vector3f v1, Eigen::Vector3f v2, Gdiplus::Color color)
{
	Eigen::Vector3f pts[3] = { v0, v1, v2 };
	fillTriangle(pts, color);
}

void CRenderer::draw()
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	gr->FillRectangle(&brush, 0, 0, width, height);
	for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			buffer->SetPixel(i, getHeight() - j, zbuffer_color[i][j]);
			zbuffer_color[i][j] = Gdiplus::Color(0, 0, 0);
		}

	graphics->DrawImage(buffer, 0, 0);
}

void CRenderer::cameraLookAt(Eigen::Vector3f _eye, Eigen::Vector3f _center, Eigen::Vector3f _up)
{
	camera.lookat(_eye, _center, _up);
}

Eigen::Matrix4f CRenderer::cameraMatrix() const
{
	return camera.viewPort() * camera.projection() * camera.modelView();
}

Eigen::Vector3f CRenderer::cameraDirection() const
{
	return camera.direction();
}
