#ifndef VECTOR_HPP
#define VECTOR_HPP

#pragma warning(disable:4290)

namespace xf
{

	template<class T>
	class Vector
	{
	public:
		Vector() : x_(0), y_(0), z_(0), w_(0)
		{
			;
		}
		Vector(const T &x, const T &y, const T &z) : x_(x), y_(y), z_(z), w_(1)
		{
			;
		}
		template<class OtherType>
		Vector(const Vector<OtherType> &_Right) : x_(_Right.x_), y_(_Right.y_), z_(_Right.z_), w_(_Right.w_)
		{
			;
		}

		Vector<T>& Normalize() throw(std::bad_cast)
		{
			if(0 == x_ + y_ + z_)
			{
				throw(std::bad_cast("cannot normalize zero vector"));
			}
			T sum = x_ + y_ + z_;
			x_ /= sum;
			y_ /= sum;
			z_ /= sum;
			return *this;
		}


		Vector<T>& Homogeneous() throw(std::bad_cast)
		{
			if(0 == w_)
			{
				throw(std::bad_cast("cannot get hemogeneous vector with w = 0"))
			}
			x_ /= w_;
			y_ /= w_;
			z_ /= w_;
			return *this;
		}
		union{
		T x_;
		T r_;
		};
		union{
		T y_;
		T g_;
		};
		union{
		T z_;
		T b_;
		};
		union{
		T w_;
		T a_;
		};
		//T y_;
		//T z_;
		//T w_;
	};

	template<class T1, class T2>
	Vector<T2> operator *(T1 factor, Vector<T2> vec)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}

	template<class T1, class T2>
	Vector<T1> operator *(Vector<T1> vec, T2 factor)
	{
		vec.x_ *= factor;
		vec.y_ *= factor;
		vec.z_ *= factor;
		return vec;
	}

}
#endif