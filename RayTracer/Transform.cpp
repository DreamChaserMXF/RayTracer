#include "Transform.h"

//Sphere Transform(const xf::Matrix &transform_mat, Sphere sphere)
//{
//	//sphere.center_ = transform_mat * sphere.center_;
//	sphere.inv_transform_mat_ = transform_mat.GetInverse();
//	return sphere;
//}

Triangle& Transform(const xf::Matrix &transform_mat, Triangle &triangle)
{
	triangle.va_ = transform_mat * triangle.va_;
	triangle.vb_ = transform_mat * triangle.vb_;
	triangle.vc_ = transform_mat * triangle.vc_;
	triangle.normal_ = transform_mat.GetTranspose().GetInverse().TransformDirection(triangle.normal_).Normalize();
	triangle.FormAssistVector();
	return triangle;
}

Matrix IdentityMatrix(int dimension)
{
	Matrix ret_mat(dimension);
	for(int i = 0; i < ret_mat.row_; ++i)
	{
		ret_mat.data_[i * ret_mat.column_ + i] = 1;
	}
	return ret_mat;
}

Matrix TranslateMatrix(const Vector &translate_vector)
{
	Matrix ret_mat = IdentityMatrix(4);
	ret_mat[0][3] = translate_vector.x_;
	ret_mat[1][3] = translate_vector.y_;
	ret_mat[2][3] = translate_vector.z_;
	return ret_mat;
}

Matrix ScaleMatrix(const Vector &scale_vector)
{
	Matrix ret_mat(4);
	ret_mat[0][0] = scale_vector.x_;
	ret_mat[1][1] = scale_vector.y_;
	ret_mat[2][2] = scale_vector.z_;
	ret_mat[3][3] = 1;
	return ret_mat;
}

Matrix RotateMatrixD(Vector direction, double degrees)
{
	return RotateMatrixR(direction, xf::radians(degrees));	// ?? 这样也可以吧？不知道为什么当初写了两个版本。
	//direction.Normalize();
	//double  data1[9] = {
	//	direction.x_ * direction.x_, direction.x_ * direction.y_, direction.x_ * direction.z_,
	//	direction.y_ * direction.x_, direction.y_ * direction.y_, direction.y_ * direction.z_,
	//	direction.z_ * direction.x_, direction.z_ * direction.y_, direction.z_ * direction.z_
	//			};
	//Matrix mProjection(3, 3, data1);
	//Matrix mIdentity = IdentityMatrix(3);
	//double  data2[9] = {
	//	0.0f, -direction.z_, direction.y_,
	//	direction.z_, 0.0f, -direction.x_,
	//	-direction.y_, direction.x_, 0.0f
	//};	// 行主序！！
	//Matrix mCross(3, 3, data2);
	//const double radians = xf::radians(degrees);
	//Matrix ret_mat = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
	//return ret_mat.GetHomogeneousMatrix();
}

Matrix RotateMatrixR(Vector direction, double radians)
{
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
	Matrix ret_mat = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
	return ret_mat.GetHomogeneousMatrix();
}