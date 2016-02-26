#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Primitive.h"
#include "Matrix.hpp"

template<class T>
Sphere Transform(const xf::Matrix<T> &transform_mat, Sphere sphere)
{
	sphere.x = static_cast<int>(transform_mat[0][0] * sphere.x + transform_mat[0][1] * sphere.y + transform_mat[0][2] * sphere.z + transform_mat[0][3]);
	sphere.y = static_cast<int>(transform_mat[1][0] * sphere.x + transform_mat[1][1] * sphere.y + transform_mat[1][2] * sphere.z + transform_mat[1][3]);
	sphere.z = static_cast<int>(transform_mat[2][0] * sphere.x + transform_mat[2][1] * sphere.y + transform_mat[2][2] * sphere.z + transform_mat[2][3]);
	// 先暂时不管置换矩阵的第四行
	return sphere;
}

template<class T>
Triangle Transform(const xf::Matrix<T> &transform_mat, Triangle triangle)
{
	triangle.x = static_cast<int>(transform_mat[0][0] * triangle.x + transform_mat[0][1] * triangle.y + transform_mat[0][2] * triangle.z + transform_mat[0][3]);
	triangle.y = static_cast<int>(transform_mat[1][0] * triangle.x + transform_mat[1][1] * triangle.y + transform_mat[1][2] * triangle.z + transform_mat[1][3]);
	triangle.z = static_cast<int>(transform_mat[2][0] * triangle.x + transform_mat[2][1] * triangle.y + transform_mat[2][2] * triangle.z + transform_mat[2][3]);
	return triangle;
}

#endif

