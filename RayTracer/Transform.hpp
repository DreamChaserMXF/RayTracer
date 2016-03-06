#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Primitive.h"
#include "Matrix.hpp"
using xf::Matrix;

static Sphere Transform(const xf::Matrix &transform_mat, Sphere sphere)
{
	//sphere.center_ = transform_mat * sphere.center_;
	sphere.transform_mat_ = transform_mat;
	return sphere;
}

static Triangle Transform(const xf::Matrix &transform_mat, Triangle triangle)
{
	triangle.v1_ = transform_mat * triangle.v1_;
	triangle.v2_ = transform_mat * triangle.v2_;
	triangle.v3_ = transform_mat * triangle.v3_;
	triangle.normal_ = transform_mat.Tranpose().Inverse() * triangle.normal_;
	return triangle;
}

static Matrix IdentityMatrix(int dimension)
{
	Matrix ret_mat(dimension);
	for(int i = 0; i < ret_mat.row_; ++i)
	{
		ret_mat.data_[i * ret_mat.column_ + i] = 1;
	}
	return ret_mat;
}

static Matrix TranslateMatrix(const Vector &translate_vector)
{
	Matrix ret_mat = IdentityMatrix(4);
	ret_mat[0][3] = translate_vector.x_;
	ret_mat[1][3] = translate_vector.y_;
	ret_mat[2][3] = translate_vector.z_;
	return ret_mat;
}

static Matrix ScaleMatrix(const Vector &scale_vector)
{
	Matrix ret_mat(4);
	ret_mat[0][0] = scale_vector.x_;
	ret_mat[1][1] = scale_vector.y_;
	ret_mat[2][2] = scale_vector.z_;
	ret_mat[3][3] = 1;
	return ret_mat;
}

static Matrix RotateMatrix(const Vector vec, double degrees)
{
	Vector direction = vec;
	direction.Normalize();
	double  data1[9] = {
		direction.x_ * direction.x_, direction.x_ * direction.y_, direction.x_ * direction.z_,
		direction.y_ * direction.x_, direction.y_ * direction.y_, direction.y_ * direction.z_,
		direction.z_ * direction.x_, direction.z_ * direction.y_, direction.z_ * direction.z_
				};
	Matrix mProjection(3, 3, data1);
	Matrix mIdentity = IdentityMatrix(3);
	double  data2[9] = {
		0.0f, -direction.z_, direction.y_,
		direction.z_, 0.0f, -direction.x_,
		-direction.y_, direction.x_, 0.0f
	};	// 行主序！！
	Matrix mCross(3, 3, data2);
	const double radians = xf::radians(degrees);
	Matrix ret_mat = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
	return ret_mat.GetHomogeneousMatrix();

	//mat3 ret;
	//// YOUR CODE FOR HW2 HERE
	//// Please implement this.  Likely the same as in HW 1.  
	//vec3 base = glm::normalize(axis);
	//mat3 mProjection(
	//	base.x_ * base.x_, base.y_ * base.x_, base.z_ * base.x_,
	//	base.x_ * base.y_, base.y_ * base.y_, base.z_ * base.y_,
	//	base.x_ * base.z_, base.y_ * base.z_, base.z_ * base.z_
	//	);
	//mat3 mIdentity(1.0f);
	//mat3 mCross(
	//	0.0f, base.z_, -base.y_,
	//	-base.z_, 0.0f, base.x_,
	//	base.y_, -base.x_, 0.0f
	//	);	// 列主序！！
	//const float radians = glm::radians(degrees);
	
	//ret = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
	//return ret;
}

#endif

