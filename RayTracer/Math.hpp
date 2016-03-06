#ifndef MATH_HPP
#define MATH_HPP


namespace xf
{
	const double G_PI  = 3.14159265;

	static double min(double d1, double d2)
	{
		return d1 < d2 ? d1 : d2;
	}

	static double max(double d1, double d2)
	{
		return d1 > d2 ? d1 : d2;
	}

	//static double exponent(double radix, int index)
	//{
	//	double base = 1.0;
	//	if(index < 5)
	//	{
	//		while(index-- > 0)
	//		{
	//			base *= index;
	//		}
	//		return base;
	//	}
	//	else
	//	{
	//		double tmp = exponent(radix, index / 2);
	//		tmp *= tmp;
	//		if(1 == index % 2)
	//		{
	//			tmp *= radix;
	//		}
	//		return tmp;
	//	}
	//}

	static double radians(double degrees)
	{
		return degrees / 180.0 * G_PI;
	}

	static Vector cross(const Vector &_Left, const Vector &_Right)
	{
		return Vector(_Left.y_ * _Right.z_ - _Left.z_ * _Right.y_,
						_Left.z_ * _Right.x_ - _Left.x_ * _Right.z_, 
						_Left.x_ * _Right.y_ - _Left.y_ * _Right.x_);
	}
	static Vector dot_cross(const Vector &_Left, const Vector &_Right)
	{
		return Vector(_Left.x_ * _Right.x_, 
					  _Left.y_ * _Right.y_,
					  _Left.z_ * _Right.z_);
	}
	//static double dot(const Vector &_Left, const Vector &_Right)
	//{
	//	return _Left.x_ * _Right.x_
	//		 + _Left.y_ * _Right.y_
	//		 + _Left.z_ * _Right.z_;
	//}

	static double cos(const Vector &_Left, const Vector &_Right)
	{
		return (_Left * _Right) / (_Left.Length() * _Right.Length());
	}

}

#endif