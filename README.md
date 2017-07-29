# SoftwareRenderer
A very simple software renderer using GDI+ and [Eigen](http://eigen.tuxfamily.org/).

GDI+ is only used for displaying bitmap, setting pixels of a bitmap and getting pixels' color from a texture file.

Eigen is only used for calculation of matrices and vectors. It is licensed under [MPL2](https://www.mozilla.org/en-US/MPL/2.0/).

## Progress
Currently, this renderer can draw wire-framed and filled 2D triangles. It can also draw textured 3D models (in 2D) saved as obj files and mtl files with backface culling, z-buffer and Gouraud shading.

![CurrentProgress](https://raw.githubusercontent.com/shurunxuan/SoftwareRenderer/master/SoftwareRenderer/progress.png)
