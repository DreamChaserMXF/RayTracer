#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "Vector.h"
#include "Matrix.h"
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
	Sphere() : center_(), radius_(0.0), inv_transform_mat_(4), material_()
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
		if(abs(delta) < DBL_MIN)	// ֱ������ֻ��һ������
		{
			// ���㽻��Ĳ���t
			t = -b / 2.0 / a;
			if(t >= ray.tmin_ && t <= ray.tmax_)	// �߶������ཻ
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
	Material material_;
};
class Triangle
{
public:
	Triangle() : va_(), vb_(), vc_(), is_const_normal_(true), normal_(), material_(), v1_(), v2_()
	{
		
	}

	void FormAssistVector()
	{
		Vector a_minus_c(va_ - vc_);
		Vector b_minus_c(vb_ - vc_);
		// ȷ��v1
		v1_ = cross_product(normal_, b_minus_c);
		v1_ /= (v1_ * a_minus_c);
		// ȷ��v2
		v2_ = cross_product(normal_, a_minus_c);
		v2_ /= (v2_ * b_minus_c);
	}


	bool IntersectTest(const Ray &ray, double &t) const
	{
		// ���ж�ֱ�ߵķ������������Ƿ���
		double cos_angle = ray.direction_ * normal_;
		if(abs(cos_angle) > DBL_MIN)
		{
			// ����������
			// ��ֱ��һ�������������ڵ�ƽ���ཻ
			// �󽻵�Ĳ���t���Լ���������Ƿ�����������
			t = ((va_ - ray.origin_) * normal_) / cos_angle;
			if(t >= ray.tmin_ && t <= ray.tmax_)
			{
				// �󽻵�
				Vector intersection_point = ray.origin_ + t * ray.direction_;
				// �жϸõ��Ƿ�����������
				// �нǶȷ����������ͬ�򷨡����ķ����㷨��������������
				// �ȼ��������ε����
				//double tri_aera = cross_product(vb_ - va_, vc_ - va_).Length();
				//// �ټ�����intersection_pointΪ���������С�����ε����
				//double aera1 = cross_product(intersection_point - va_, intersection_point - vb_).Length();
				//double aera2 = cross_product(intersection_point - vb_, intersection_point - vc_).Length();
				//double aera3 = cross_product(intersection_point - vc_, intersection_point - va_).Length();
				//// �ȽϿ�����С�����ε�������Ƿ���ڴ������ε����
				//if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// ��ȣ�˵��������
				//{
				//	return true;
				//}

				// �нǶȷ����������ͬ�򷨡����ķ����㷨������������ķ�
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
	Vector va_;
	Vector vb_;
	Vector vc_;
	bool is_const_normal_;
	Vector normal_;
	Material material_;

	// ��������
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