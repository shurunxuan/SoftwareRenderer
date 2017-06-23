#include "CDrawer.h"
#undef max
#undef min

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void CDrawer::init()
{
	hdc_ = BeginPaint(hwnd_, &ps);
	graphics = new Graphics(hdc_);
	RECT rect;
	GetClientRect(hwnd_, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	zbuffer = new float[width * height];
	for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	vertex_buffer = new Color*[width];
	for (int i = 0; i < width; ++i) vertex_buffer[i] = new Color[height];

	buffer = new Bitmap(width, height);
	gr = new Graphics(buffer);
}

void CDrawer::deinit()
{
	delete gr;
	delete buffer;
	delete zbuffer;
	delete graphics;
	for (int i = 0; i < width; ++i) delete vertex_buffer[i];
	delete vertex_buffer;
	EndPaint(hwnd_, &ps);
}

CDrawer::CDrawer(HWND hwnd)
	: hwnd_(hwnd)
{
	init();
}

CDrawer::~CDrawer()
{
	deinit();
}

void CDrawer::resizeBuffer()
{
	if (gr == nullptr || buffer == nullptr || graphics == nullptr || zbuffer == nullptr) return;

	deinit();
	init();
}

long CDrawer::getWidth() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto width = rect.right - rect.left;
	return width;
}

long CDrawer::getHeight() const
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	auto height = rect.bottom - rect.top;
	return height;
}

void CDrawer::drawPixel(int x, int y, Color color)
{
	vertex_buffer[x][y] = color;
}

void CDrawer::drawLine(int x0, int y0, int x1, int y1, Color color)
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

void CDrawer::drawLine(Vec2i v0, Vec2i v1, Color color)
{
	drawLine(v0.x, v0.y, v1.x, v1.y, color);
}

void CDrawer::drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color)
{
	drawLine(v0, v1, color);
	drawLine(v1, v2, color);
	drawLine(v2, v0, color);
}

void CDrawer::fillTriangle(Vec3f* pts, Color color)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(getWidth() - 1, getHeight() - 1);
	for (int i = 0; i<3; i++) {
		for (int j = 0; j<2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
			P.z = 0;
			for (int i = 0; i<3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y*width)]<P.z) {
				zbuffer[int(P.x + P.y*width)] = P.z;
				drawPixel(P.x, P.y, color);
			}
		}
	}
}

void CDrawer::fillTriangle(Vec3f v0, Vec3f v1, Vec3f v2, Color color)
{
	Vec3f pts[3] = { v0, v1, v2 };
	fillTriangle(pts, color);
}

void CDrawer::draw()
{
	SolidBrush brush(Color(0, 0, 0));
	gr->FillRectangle(&brush, 0, 0, width, height);
	for (int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			buffer->SetPixel(i, getHeight() - j, vertex_buffer[i][j]);
			vertex_buffer[i][j] = Color(0, 0, 0);
		}

	graphics->DrawImage(buffer, 0, 0);
}
