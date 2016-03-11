#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Primitive.h"
#include "Matrix.h"
using xf::Matrix;
using xf::Vector;

//Sphere Transform(const xf::Matrix &transform_mat, Sphere sphere);

Triangle& Transform(const xf::Matrix &transform_mat, Triangle &triangle);

Matrix IdentityMatrix(int dimension);

Matrix TranslateMatrix(const Vector &translate_vector);

Matrix ScaleMatrix(const Vector &scale_vector);

Matrix RotateMatrixD(Vector direction, double degrees);
Matrix RotateMatrixR(Vector direction, double radians);

#endif

