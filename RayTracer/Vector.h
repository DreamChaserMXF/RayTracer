#ifndef VECTOR_HPP
#define VECTOR_HPP

#pragma warning(disable:4290)

#include <cassert>
#include <exception>
#include <typeinfo>

namespace xf
{
	
	class Vector
	{
	public:
		Vector();
		Vector(double x_, double y_, double z_);

		double Length() const throw();
		Vector& Normalize() throw(std::bad_cast);
		Vector GetNormalize() const throw(std::bad_cast);
		// 向量加减
		Vector& operator += (const Vector &_Right);
		Vector operator + (const Vector &_Right) const;
		Vector& operator -= (const Vector &_Right);
		Vector operator - (const Vector &_Right) const;
		Vector operator + () const throw();
		Vector operator - () const throw();
		// 向量乘除
		double operator *(const Vector &_Right) const;
		Vector& operator /=(const Vector &_Right);
		Vector operator /(const Vector &_Right) const;
		// 向量的数乘
		Vector& operator *=(double factor);
		Vector operator *(double factor) const;
		Vector& operator /=(double factor);
		Vector operator /(double factor) const;

		union{
		double  x_;
		double  r_;
		};
		union{
		double  y_;
		double  g_;
		};
		union{
		double  z_;
		double  b_;
		};
	};

	Vector operator *(double factor, Vector vec);
	Vector operator /(double numerator, Vector vec);
}
#endif