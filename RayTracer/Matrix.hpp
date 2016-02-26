#ifndef MATRIX_HPP
#define MATRIX_HPP

#pragma warning(disable:4290)

#include <stdexcept>
#include <string>
#include "Math.hpp"
#include "Vector.hpp"

namespace xf
{

	template<class T>
	class Matrix
	{
	public:
		Matrix(int dimension) : row_(dimension), column_(dimension), data_(new T[dimension * dimension])
		{
			for(int i = 0; i < dimension; ++i)
			{
				for(int j = 0; j < dimension; ++j)
				{
					data_[i * dimension + j] = 0;
				}
			}
		}
		Matrix(int row, int column) throw(std::bad_alloc) : row_(row), column_(column), data_(new T[row * column])
		{
			for(int i = 0; i < row; ++i)
			{
				for(int j = 0; j < column; ++j)
				{
					data_[i * column + j] = 0;
				}
			}
		}
		template<class OtherType>
		Matrix(int row, int column, OtherType *data) throw(std::bad_alloc) : row_(row), column_(column), data_(new T[row * column])
		{
			for(int i = 0; i < row; ++i)
			{
				for(int j = 0; j < column; ++j)
				{
					data_[i * column + j] = data[i * column + j];
				}
			}
		}
		Matrix(const Matrix<T> &_Right) throw(std::bad_alloc) : row_(_Right.row_), column_(_Right.column_), data_(new T[_Right.row_ * _Right.column_])
		{
			for(int i = 0; i < row_; ++i)
			{
				for(int j = 0; j < column_; ++j)
				{
					data_[i * column_ + j] = _Right.data_[i * column_ + j];
				}
			}
		}
		template<class OtherType>
		Matrix(const Matrix<OtherType> &_Right) throw(std::bad_alloc) : row_(_Right.row_), column_(_Right.column_), data_(new T[_Right.row_ * _Right.column_])
		{
			for(int i = 0; i < row; ++i)
			{
				for(int j = 0; j < column; ++j)
				{
					data_[i * column + j] = _Right.data_[i * column + j];
				}
			}
		}

		

		~Matrix()
		{
			delete[] data_;
		}

		Matrix<T>& Tranpose()
		{
			if(row_ == column_)
			{
				for(int i = 0; i < row_; ++i)
				{
					for(int j = 0; j < i; ++j)
					{
						T tmp = data_[i * column + j];
						data_[i * column + j] = data_[j * column + i];
						data_[j * column + i] = tmp;
					}
				}
			}
			else
			{
				T *new_data = new T[row_ * column_];
				for(int i = 0; i < row_; ++i)
				{
					for(int j = 0; j < column_; ++j)
					{
						new_data_[i * column + j] = data_[j * column + i];
					}
				}
				int tmp = row_;
				row_ = column_;
				column_ = tmp;
				delete[] data_;
				data_ = new_data;
			}
			return *this;
		}

		Matrix<T> ToHomogeneousMatrix() const throw(std::bad_alloc, std::bad_cast)
		{
			if(3 != row_ || 3 != column_)
			{
				throw std::bad_cast("only can get homogeneous matrix from 3*3 matrix");
			}
			Matrix<T> ret_mat(4, 4);
			for(int i = 0; i < 3; ++i)
			{
				for(int j = 0; j < 3; ++j)
				{
					ret_mat.data_[i * 4 + j] = data_[i * 3 + j];
				}
			}
			ret_mat.data_[15] = 1;
			return ret_mat;
		}

		Matrix<T>& operator = (const Matrix<T> &_Right) throw(std::exception)
		{
			if(this != &_Right)
			{
				if(row_ != _Right.row_
					|| column_ != _Right.column_)
				{
					throw std::exception("matrix size not match");
				}
				for(int i = 0; i < row_; ++i)
				{
					for(int j = 0; j < column_; ++j)
					{
						data_[i * column_ + j] = _Right.data_[i * column_ + j];
					}
				}
			}
			return *this;
		}

		template<class OtherType>
		Matrix<T>& operator = (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			if(this != &_Right)
			{
				if(row_ != _Right.row_
					|| column_ != _Right.column_)
				{
					throw std::exception("matrix size not match");
				}
				for(int i = 0; i < row_; ++i)
				{
					for(int j = 0; j < column_; ++j)
					{
						data_[i * column_ + j] = _Right.data_[i * column_ + j];
					}
				}
			}
			return *this;
		}

		template<class OtherType>
		Matrix<T>& operator += (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			if(row_ != _Right.row_
				|| column_ != _Right.column_)
			{
				throw std::exception("matrix size not match");
			}
			for(int i = 0; i < row_; ++i)
			{
				for(int j = 0; j < column_; ++j)
				{
					data_[i * column_ + j] += _Right.data_[i * column_ + j];
				}
			}	
			return *this;
		}
		template<class OtherType>
		Matrix<T> operator + (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			Matrix ret_mat(*this);
			return ret_mat += _Right;
		}

		template<class OtherType>
		Matrix<T>& operator -= (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			if(row_ != _Right.row_
				|| column_ != _Right.column_)
			{
				throw std::exception("matrix size not match");
			}
			for(int i = 0; i < row_; ++i)
			{
				for(int j = 0; j < column_; ++j)
				{
					data_[i * column + j] -= _Right.data_[i * column + j];
				}
			}
			return *this;
		}
		template<class OtherType>
		Matrix<T> operator - (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			Matrix ret_mat(*this);
			return ret_mat -= _Right;
		}

		template<class OtherType>
		Matrix<T> operator * (const Matrix<OtherType> &_Right) throw(std::exception)
		{
			if(column_ != _Right.row_)
			{
				throw std::exception("matrix size not match");
			}
			Matrix<T> ret_mat(row_, _Right.column_);
			for(int i = 0; i < row_; ++i)
			{
				for(int j = 0; j < _Right.column_; ++j)
				{
					T sum = 0;
					for(int k = 0; k < column_; ++k)
					{
						sum += data_[i * column_ + k] * _Right.data_[k * _Right.column_ + j];
					}
					ret_mat.data_[i * _Right.column_ + j] = sum;
				}
			}
			return ret_mat;
		}

		T* operator [](int n) const throw()
		{
			return data_ + n * column_;
		}



		static Matrix<T> IdentityMatrix(int dimension)
		{
			Matrix<T> ret_mat(dimension);
			for(int i = 0; i < ret_mat.row_; ++i)
			{
				ret_mat.data_[i * ret_mat.column_ + i] = 1;
			}
			return ret_mat;
		}

		template<class OtherType>
		static Matrix<T> TranslateMatrix(const Vector<OtherType> &translate_vector)
		{
			Matrix<T> ret_mat = IdentityMatrix(4);
			ret_mat[0][3] = translate_vector.x_;
			ret_mat[1][3] = translate_vector.y_;
			ret_mat[2][3] = translate_vector.z_;
			return ret_mat;
		}

		template<class OtherType>
		static Matrix<T> ScaleMatrix(const Vector<OtherType> &scale_vector)
		{
			Matrix<T> ret_mat(4);
			ret_mat[0][0] = scale_vector.x_;
			ret_mat[1][1] = scale_vector.y_;
			ret_mat[2][2] = scale_vector.z_;
			ret_mat[3][3] = 1;
			return ret_mat;
		}

		static Matrix<T> RotationMatrix(const Vector<T> vec, double degrees)
		{
			//Matrix<T> ret_mat(3, 3);
			Vector<T> direction = vec;
			direction.Normalize();
			T data1[9] = {
				direction.x_ * direction.x_, direction.x_ * direction.y_, direction.x_ * direction.z_,
				direction.y_ * direction.x_, direction.y_ * direction.y_, direction.y_ * direction.z_,
				direction.z_ * direction.x_, direction.z_ * direction.y_, direction.z_ * direction.z_
						};
			Matrix<T> mProjection(3, 3, data1);
			Matrix<T> mIdentity = IdentityMatrix(3);
			T data2[9] = {
				0.0f, -direction.z_, direction.y_,
				direction.z_, 0.0f, -direction.x_,
				-direction.y_, direction.x_, 0.0f
			};	// 行主序！！
			Matrix<T> mCross(3, 3, data2);
			const double radians = xf::radians(degrees);
			Matrix<T> ret_mat = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
			return ret_mat.ToHomogeneousMatrix();

			//mat3 ret;
			//// YOUR CODE FOR HW2 HERE
			//// Please implement this.  Likely the same as in HW 1.  
			//vec3 base = glm::normalize(axis);
			//mat3 mProjection(
			//	base.x * base.x, base.y * base.x, base.z * base.x,
			//	base.x * base.y, base.y * base.y, base.z * base.y,
			//	base.x * base.z, base.y * base.z, base.z * base.z
			//	);
			//mat3 mIdentity(1.0f);
			//mat3 mCross(
			//	0.0f, base.z, -base.y,
			//	-base.z, 0.0f, base.x,
			//	base.y, -base.x, 0.0f
			//	);	// 列主序！！
			//const float radians = glm::radians(degrees);
	
			//ret = (1 - cos(radians)) * mProjection + cos(radians) * mIdentity + sin(radians) * mCross;
			//return ret;
		}


		int row_;
		int column_;
		T *data_;
	};


	template<class T1, class T2>
	Matrix<T2> operator *(T1 factor, Matrix<T2> mat)
	{
		for(int i = 0; i < mat.row_; ++i)
		{
			for(int j = 0; j < mat.column_; ++j)
			{
				mat.data_[i * mat.column_ + j] *= factor;
			}
		}
		return mat;
	}
	template<class T1>
	Matrix<T1> operator *(Matrix<T1> mat, double factor)
	{
		for(int i = 0; i < mat.row_; ++i)
		{
			for(int j = 0; j < mat.column_; ++j)
			{
				mat.data_[i * mat.column_ + j] *= factor;
			}
		}
		return mat;
	}
	template<class T1>
	Matrix<T1> operator *(Matrix<T1> mat, int factor)
	{
		for(int i = 0; i < mat.row_; ++i)
		{
			for(int j = 0; j < mat.column_; ++j)
			{
				data_[i * mat.column_ + j] *= factor;
			}
		}
		return mat;
	}

	template<class T>
	std::ostream& operator << (std::ostream &out, const Matrix<T> &mat)
	{
		for(int i = 0; i < mat.row_; ++i)
		{
			for(int j = 0; j < mat.column_; ++j)
			{
				out << mat[i][j] << "  ";
			}
			out << '\n';
		}
		return out;
	}
}

#endif