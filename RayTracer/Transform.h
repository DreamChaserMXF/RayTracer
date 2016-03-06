#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Primitive.h"
#include "Matrix.hpp"
using xf::Matrix;

//Sphere Transform(const xf::Matrix &transform_mat, Sphere sphere);

Triangle Transform(const xf::Matrix &transform_mat, Triangle triangle);

Matrix IdentityMatrix(int dimension);

Matrix TranslateMatrix(const Vector &translate_vector);

Matrix ScaleMatrix(const Vector &scale_vector);

Matrix RotateMatrix(const Vector vec, double degrees);

#endif

