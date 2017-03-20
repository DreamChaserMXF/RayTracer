#include "Math.h"

namespace xf
{
	double min(double d1, double d2)
	{
		return d1 < d2 ? d1 : d2;
	}

	double max(double d1, double d2)
	{
		return d1 > d2 ? d1 : d2;
	}

	double radians(double degrees)
	{
		return degrees / 180.0 * PI;
	}

	Vector cross_product(const Vector &_Left, const Vector &_Right)
	{
		return Vector(_Left.y_ * _Right.z_ - _Left.z_ * _Right.y_,
						_Left.z_ * _Right.x_ - _Left.x_ * _Right.z_, 
						_Left.x_ * _Right.y_ - _Left.y_ * _Right.x_);
	}
	Vector scale_product(const Vector &_Left, const Vector &_Right)
	{
		return Vector(_Left.x_ * _Right.x_, 
					  _Left.y_ * _Right.y_,
					  _Left.z_ * _Right.z_);
	}
	double cos(const Vector &_Left, const Vector &_Right)
	{
		return (_Left * _Right) / (_Left.Length() * _Right.Length());
	}
}