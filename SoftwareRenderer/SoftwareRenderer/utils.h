#pragma once

#include "CRenderer.h"
#include "Eigen/Dense"
#include "Eigen/Geometry"

Eigen::Vector3f barycentric(CPixel p1, CPixel p2, CPixel p3, int x, int y);
Gdiplus::Color getColorFromBitmap(float x, float y, Gdiplus::Bitmap* pBitmap);