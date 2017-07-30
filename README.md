# SoftwareRenderer
A very simple software renderer using GDI+ and [Eigen](http://eigen.tuxfamily.org/).

GDI+ is only used for displaying bitmap, setting pixels of a bitmap and getting pixels' color from a texture file.

Eigen is only used for calculation of matrices and vectors. It is licensed under [MPL2](https://www.mozilla.org/en-US/MPL/2.0/).

## Progress
Currently, this renderer can draw textured 3D models (.obj and .mtl files) with perspective projection.

As the renderer seems working correctly, there's still many things to be done (like Blinn-Phong shading model and normal mapping), and many bugs to be fixed.

![CurrentProgress](https://raw.githubusercontent.com/shurunxuan/SoftwareRenderer/master/SoftwareRenderer/progress.gif)
