#include "Vector.h"
#include <cmath>
#include <cfloat>
#include <exception>

namespace xf
{

	Vector::Vector() : x_(0), y_(0), z_(0)
	{
		;
	}
	Vector::Vector(double x_, double y_, double z_) : x_(x_), y_(y_), z_(z_)
	{
		;
	}

	double Vector::Length() const throw()
	{
		return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
	}

	Vector& Vector::Normalize() throw(std::bad_cast)
	{
		double length = Length();
		if(length > DBL_MIN)
		{
			x_ /= length;
			y_ /= length;
			z_ /= length;
		}
		else
		{
			throw(std::bad_cast("cannot normalize zero vector"));
		}
		return *this;
	}

	Vector Vector::GetNormalize() const throw(std::bad_cast)
	{
		return Vector(*this).Normalize();
	}

	Vector& Vector::operator += (const Vector &_Right)
	{
		x_ += _Right.x_;
		y_ += _Right.y_;
		z_ += _Right.z_;
		return *this;
	}
	Vector Vector::operator + (const Vector &_Right) const
	{
		Vector retVector = *this;
		retVector += _Right;
		return retVector;
	}
	Vector& Vector::operator -= (const Vector &_Right)
	{
		x_ -= _Right.x_;
		y_ -= _Right.y_;
		z_ -= _Right.z_;
		return *this;
	}
	Vector Vector::operator - (const Vector &_Right) const
	{
		Vector retVector = *this;
		retVector -= _Right;
		return retVector;
	}

	Vector Vector::operator + () const throw()
	{
		return Vector(*this);
	}
	Vector Vector::operator - () const throw()
	{
		return Vector(-x_, -y_, -z_);
	}
	// 两向量之间的乘/除
	double Vector::operator *(const Vector &_Right) const
	{
		return x_ * _Right.x_
				+ y_ * _Right.y_
				+ z_ * _Right.z_;
	}
	Vector& Vector::operator /=(const Vector &_Right)
	{
		x_ /= _Right.x_;
		y_ /= _Right.y_;
		z_ /= _Right.z_;
		return *this;
	}
	Vector Vector::operator /(const Vector &_Right) const
	{
		Vector ret_vec(*this);
		return ret_vec /= _Right;
	}

	// 向量与数的乘除
	Vector& Vector::operator *=(double factor)
	{
		x_ *= factor;
		y_ *= factor;
		z_ *= factor;
		return *this;
	}
	Vector Vector::operator *(double factor) const
	{
		return Vector(x_ * factor, y_ * factor, z_ * factor);
	}

	Vector& Vector::operator /=(double factor)
	{
		assert(abs(factor) > DBL_MIN);
		x_ /= factor;
		y_ /= factor;
		z_ /= factor;
		return *this;
	}
	Vector Vector::operator /(double factor) const
	{
		return Vector(*this) /= factor;
	}

	Vector operator *(double factor, Vector vec)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}
	Vector operator /(double numerator, Vector vec)
	{
		assert(abs(vec.x_) > DBL_MIN && abs(vec.y_) > DBL_MIN && abs(vec.z_) > DBL_MIN);
		return Vector(numerator / vec.x_, numerator / vec.y_, numerator / vec.z_);
	}
}