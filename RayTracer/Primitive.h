#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Material.h"

using xf::Vector;
using xf::Matrix;
using std::vector;

// 球的半径用向量存储，因为可能会在三个维度上进行缩放
class Sphere
{
public:
	Sphere() : center_(), radius_(0.0), transform_mat_(4), ambient_(0.2, 0.2, 0.2), material_()
	{
		;
	}
	Vector center_;
	double radius_;
	Matrix transform_mat_;
	Vector ambient_;
	Material material_;
};
class Triangle
{
public:
	Triangle() : v1_(), v2_(), v3_(), is_const_normal_(true), normal_(), ambient_(0.2, 0.2, 0.2), material_()
	{
		;
	}
	Vector v1_;
	Vector v2_;
	Vector v3_;
	bool is_const_normal_;
	Vector normal_;
	Vector ambient_;
	Material material_;
};

//class Polygon
//{
//public:
//	Polygon() : vertices_(), ambient_(), material_()
//	{
//		;
//	}
//	vector<Vector> vertices_;
//	Vector ambient_;
//	Material material_;
//};

class Light
{
public:
	Light() : origin_(), color_()
	{
		;
	}
	Vector origin_;
	Vector color_;
};

// Ray: origin_ + t * direction_, tmin_ <= t <= tmax_
class Ray
{
public:
	Ray(Vector position, Vector direction, double tmin, double tmax) : origin_(position), direction_(direction), tmin_(tmin), tmax_(tmax)
	{
		;
	}
	Vector origin_;
	Vector direction_;
	double tmin_;
	double tmax_;
};

#endif