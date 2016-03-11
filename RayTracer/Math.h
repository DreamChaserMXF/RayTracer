#ifndef MATH_HPP
#define MATH_HPP

#include "Vector.h"

namespace xf
{
	const double PI  = 3.14159265;
	const double EPS = 1e-4;
	double min(double d1, double d2);
	double max(double d1, double d2);
	double radians(double degrees);
	Vector cross_product(const Vector &_Left, const Vector &_Right);
	Vector scale_product(const Vector &_Left, const Vector &_Right);
	double cos(const Vector &_Left, const Vector &_Right);
}

#endif