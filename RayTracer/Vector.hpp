#ifndef VECTOR_HPP
#define VECTOR_HPP

#pragma warning(disable:4290)

namespace xf
{

	class Vector
	{
	public:
		Vector() : x_(0), y_(0), z_(0)
		{
			;
		}
		Vector(double x_, double y_, double z_) : x_(x_), y_(y_), z_(z_)
		{
			;
		}

		double Length() const throw()
		{
			return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
		}

		Vector& Normalize() throw(std::bad_cast)
		{
			double length = Length();
			if(length < std::numeric_limits<double>::min())
			{
				throw(std::bad_cast("cannot normalize zero vector"));
			}
			x_ /= length;
			y_ /= length;
			z_ /= length;
			return *this;
		}


		//Vector& Homogeneous() throw(std::bad_cast)
		//{
		//	if(0 == w_)
		//	{
		//		throw(std::bad_cast("cannot get hemogeneous vector with w = 0"))
		//	}
		//	x_ /= w_;
		//	y_ /= w_;
		//	z_ /= w_;
		//	return *this;
		//}
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

		Vector& operator += (const Vector &_Right)
		{
			x_ += _Right.x_;
			y_ += _Right.y_;
			z_ += _Right.z_;
			return *this;
		}
		Vector operator + (const Vector &_Right) const
		{
			Vector retVector = *this;
			retVector += _Right;
			return retVector;
		}
		Vector& operator -= (const Vector &_Right)
		{
			x_ -= _Right.x_;
			y_ -= _Right.y_;
			z_ -= _Right.z_;
			return *this;
		}
		Vector operator - (const Vector &_Right) const
		{
			Vector retVector = *this;
			retVector -= _Right;
			return retVector;
		}

		Vector operator + () const throw()
		{
			return Vector(*this);
		}
		Vector operator - () const throw()
		{
			return Vector(-x_, -y_, -z_);
		}
		double operator *(const Vector &_Right) const
		{
			return x_ * _Right.x_
				 + y_ * _Right.y_
				 + z_ * _Right.z_;
		}

		Vector& operator /=(const Vector &_Right)
		{
			x_ /= _Right.x_;
			y_ /= _Right.y_;
			z_ /= _Right.z_;
			return *this;
		}
		Vector operator /(const Vector &_Right) const
		{
			Vector ret_vec(*this);
			return ret_vec /= _Right;
		}
	};

	static Vector& operator *=(Vector &vec, double factor)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}

	static Vector operator *(double factor, Vector vec)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}

	static Vector operator *(Vector vec, double factor)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}

}
#endif