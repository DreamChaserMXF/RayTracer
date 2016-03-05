#ifndef MATH_HPP
#define MATH_HPP


namespace xf
{
	const double G_PI  = 3.14159265;

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