#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "Vector.h"
#include "Matrix.h"
#include "Material.h"

using xf::Vector;
using xf::Matrix;
using std::vector;

enum PrimitiveType {NONE, SPHERE, TRIANGLE};

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

class Primitive
{
public:
	Primitive(){}
	virtual bool IntersectTest(const Ray &ray, double &t) const = 0;
	virtual Vector GetNormal(const Vector& intersection_pt) const = 0;

	Material material_;
};


class Sphere : public Primitive
{
public:
	Sphere() : center_(), radius_(0.0), inv_transform_mat_(4)
	{
		;
	}

	virtual bool IntersectTest(const Ray &ray, double &t) const
	{
		// 把视点和方向变换回去
		Vector origin = inv_transform_mat_ * ray.origin_;
		Vector direction = inv_transform_mat_.TransformDirection(ray.direction_);	// 这个方向只需要变换回去就可以了，不需要归一化，否则参数t在变换前后的意义就不同了

		double a = direction * direction;
		double b = 2.0 * direction * (origin - center_);
		Vector tmp = origin - center_;
		double c = tmp * tmp - radius_ * radius_;
		double delta = b * b - 4.0 * a * c;
		if(abs(delta) < DBL_MIN)	// 直线与球只有一个交点
		{
			// 计算交点的参数t
			t = -b / 2.0 / a;
			if(t >= ray.tmin_ && t <= ray.tmax_)	// 线段与球相交
			{
				return true;
			}
		}
		else if(delta > 0.0)	// 有两个交点，计算出两个t来
		{
			// t1 < t2
			double t1 = (-b - sqrt(delta)) / 2.0 / a;
			double t2 = (-b + sqrt(delta)) / 2.0 / a;
			// 如果t1符合要求
			if(t1 >= ray.tmin_ && t1 <= ray.tmax_)
			{
				t = t1;
				return true;
			}
			else if(t2 >= ray.tmin_ && t2 <= ray.tmax_)
			{
				t = t2;
				return true;
			}
		}
		return false;
	}

	virtual Vector GetNormal(const Vector& intersection_pt) const
	{
		Vector original_point = inv_transform_mat_ * intersection_pt;
		Vector original_normal = (original_point - center_).Normalize();	// parse的时候检查过了，球的半径不可能为0，故一定不会有异常
		Vector normal = inv_transform_mat_.GetTranspose().TransformDirection(original_normal).Normalize();	// 方向变换之后，也一定不会为0向量
		return normal;
	}

	Vector center_;
	double radius_;
	Matrix inv_transform_mat_;
	
};
class Triangle : public Primitive
{
public:
	Triangle() : va_(), vb_(), vc_(), is_const_normal_(true), normal_(), v1_(), v2_()
	{
		
	}

	void FormAssistVector()
	{
		Vector a_minus_c(va_ - vc_);
		Vector b_minus_c(vb_ - vc_);
		// 确定v1
		v1_ = cross_product(normal_, b_minus_c);
		v1_ /= (v1_ * a_minus_c);
		// 确定v2
		v2_ = cross_product(normal_, a_minus_c);
		v2_ /= (v2_ * b_minus_c);
	}


	virtual bool IntersectTest(const Ray &ray, double &t) const
	{
		// 先判断直线的方向与三角形是否共面
		double cos_angle = ray.direction_ * normal_;
		if(abs(cos_angle) > DBL_MIN)
		{
			// 不共面的情况
			// 则直线一定和三角形所在的平面相交
			// 求交点的参数t，以计算出交点是否在三角形内
			t = ((va_ - ray.origin_) * normal_) / cos_angle;
			if(t >= ray.tmin_ && t <= ray.tmax_)
			{
				// 求交点
				Vector intersection_point = ray.origin_ + t * ray.direction_;
				// 判断该点是否在三角形内
				// 有角度法、面积法、同向法、重心法等算法，这里采用面积法
				// 先计算三角形的面积
				//double tri_aera = cross_product(vb_ - va_, vc_ - va_).Length();
				//// 再计算以intersection_point为顶点的三个小三角形的面积
				//double aera1 = cross_product(intersection_point - va_, intersection_point - vb_).Length();
				//double aera2 = cross_product(intersection_point - vb_, intersection_point - vc_).Length();
				//double aera3 = cross_product(intersection_point - vc_, intersection_point - va_).Length();
				//// 比较看三个小三角形的面积和是否等于大三角形的面积
				//if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// 相等，说明在里面
				//{
				//	return true;
				//}

				// 有角度法、面积法、同向法、重心法等算法，这里采用重心法，即推算交点用ABC点所表示的系数，求它们是否大于0
				// 见 http://www.tuicool.com/articles/rUrMJvi
				double alpha = (intersection_point - vc_) * v1_;
				double beta = (intersection_point - vc_) * v2_;
				if(alpha >= 0.0 && beta >= 0.0 && alpha + beta <= 1.0)
				{
					return true;
				}
			}
		}
		return false;
	}

	virtual Vector GetNormal(const Vector& intersection_pt) const
	{
		return normal_;
	}

	Vector va_;
	Vector vb_;
	Vector vc_;
	bool is_const_normal_;
	Vector normal_;

	// 辅助变量
	Vector v1_;
	Vector v2_;
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


#endif