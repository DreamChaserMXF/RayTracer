#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Material.h"

using xf::Vector;
using xf::Matrix;
using std::vector;

enum Primitive {NONE, SPHERE, TRIANGLE};

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


// ��İ뾶�������洢����Ϊ���ܻ�������ά���Ͻ�������
class Sphere
{
public:
	Sphere() : center_(), radius_(0.0), inv_transform_mat_(4), ambient_(0.2, 0.2, 0.2), material_(), mirror_coefficient_(0.0)
	{
		;
	}

	bool IntersectTest(const Ray &ray, double &t) const
	{
		// ���ӵ�ͷ���任��ȥ
		Vector origin = inv_transform_mat_ * ray.origin_;
		Vector direction = inv_transform_mat_.TransformDirection(ray.direction_);	// �������ֻ��Ҫ�任��ȥ�Ϳ����ˣ�����Ҫ��һ�����������t�ڱ任ǰ�������Ͳ�ͬ��

		double a = direction * direction;
		double b = 2.0 * direction * (origin - center_);
		Vector tmp = origin - center_;
		double c = tmp * tmp - radius_ * radius_;
		double delta = b * b - 4.0 * a * c;
		if(abs(delta) < xf::EPS)	// ֱ������ֻ��һ������
		{
			// ���㽻��Ĳ���t
			t = -b / 2.0 / a;
			if(t >= ray.tmin_ && t <= ray.tmax_)	// �߶������ཻ���ҽ������
			{
				return true;
			}
		}
		else if(delta > 0.0)	// ���������㣬���������t��
		{
			// t1 < t2
			double t1 = (-b - sqrt(delta)) / 2.0 / a;
			double t2 = (-b + sqrt(delta)) / 2.0 / a;
			// ���t1����Ҫ��
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

	Vector center_;
	double radius_;
	Matrix inv_transform_mat_;
	Vector ambient_;
	Material material_;
	double mirror_coefficient_;
};
class Triangle
{
public:
	Triangle() : v1_(), v2_(), v3_(), is_const_normal_(true), normal_(), ambient_(0.2, 0.2, 0.2), material_(), mirror_coefficient_(0.0)
	{
		;
	}
	bool IntersectTest(const Ray &ray, double &t) const
	{
		// ���ж�ֱ�ߵķ������������Ƿ���
		if(abs(cos(ray.direction_, normal_)) > std::numeric_limits<double>::min())
		{
			// ����������
			// ��ֱ��һ�������������ڵ�ƽ���ཻ
			// �󽻵�Ĳ���t���Լ���������Ƿ�����������
			t = ((v1_ - ray.origin_) * normal_) / (ray.direction_ * normal_);
			if(t >= ray.tmin_ && t <= ray.tmax_)
			{
				// �󽻵�
				Vector intersection_point = ray.origin_ + t * ray.direction_;
				// �жϸõ��Ƿ�����������
				// �нǶȷ����������ͬ�򷨡����ķ����㷨��������������
				// �ȼ��������ε����
				double tri_aera = cross(v2_ - v1_, v3_ - v1_).Length() / 2.0;
				// �ټ�����intersection_pointΪ���������С�����ε����
				double aera1 = cross(intersection_point - v1_, intersection_point - v2_).Length() / 2.0;
				double aera2 = cross(intersection_point - v2_, intersection_point - v3_).Length() / 2.0;
				double aera3 = cross(intersection_point - v3_, intersection_point - v1_).Length() / 2.0;
				// �ȽϿ�����С�����ε�������Ƿ���ڴ������ε����
				if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// ��ȣ�˵��������
				{
					return true;
				}
			}
		}
		return false;
	}
	Vector v1_;
	Vector v2_;
	Vector v3_;
	bool is_const_normal_;
	Vector normal_;
	Vector ambient_;
	Material material_;
	double mirror_coefficient_;
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