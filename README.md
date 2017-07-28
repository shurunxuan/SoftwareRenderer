# SoftwareRenderer
A very simple software renderer using GDI+ and [Eigen](http://eigen.tuxfamily.org/).

GDI+ is only used for displaying bitmap. The only functions used in GDI+ are `Bitmap::SetPixel`, `Graphics::FillRectangle` and `Graphics::DrawImage`.

Eigen is only used for calculation of matrices and vectors. It is licensed under [MPL2](https://www.mozilla.org/en-US/MPL/2.0/).

## Progress
Currently, this renderer can draw wire-framed and filled 2D triangles. It can also draw 3D models (in 2D) in form of obj files.

![CurrentProgress](https://raw.githubusercontent.com/shurunxuan/SoftwareRenderer/master/SoftwareRenderer/progress.png)
