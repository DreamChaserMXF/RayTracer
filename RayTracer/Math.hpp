#ifndef MATH_HPP
#define MATH_HPP


namespace xf
{
	const double G_PI  = 3.14159265;

	double radians(double degrees)
	{
		return degrees / 180.0 * G_PI;
	}
}

#endif