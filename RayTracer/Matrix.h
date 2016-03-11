#ifndef MATRIX_HPP
#define MATRIX_HPP

#pragma warning(disable:4290)

#include <stdexcept>
#include <string>
#include "Math.h"
#include "Vector.h"

namespace xf
{
	
	class Matrix
	{
	public:
		Matrix(int dimension) throw(std::bad_alloc);
		Matrix(int row, int column) throw(std::bad_alloc);
		template<class T>
		Matrix(int row, int column, T *data) throw(std::bad_alloc) : row_(row), column_(column), data_(new double [row * column])
		{
			for(int i = 0; i < row; ++i)
			{
				for(int j = 0; j < column; ++j)
				{
					data_[i * column + j] = data[i * column + j];
				}
			}
		}
		Matrix(const Matrix &_Right) throw(std::bad_alloc);

		~Matrix();
		Matrix& Transpose() throw(std::bad_alloc);
		Matrix GetTranspose() const throw(std::bad_alloc);
		

		// 高斯-若当消元法求逆矩阵
		Matrix GetInverse() const throw(std::bad_alloc, std::runtime_error);
		Matrix GetHomogeneousMatrix() const throw(std::bad_alloc, std::bad_cast);
		Matrix& operator = (const Matrix &_Right) throw(std::exception);
		Matrix& operator += (const Matrix &_Right) throw(std::exception);
		Matrix operator + (const Matrix &_Right) const throw(std::exception);
		Matrix& operator -= (const Matrix &_Right) throw(std::exception);
		Matrix operator - (const Matrix &_Right) const throw(std::exception);
		Matrix operator * (const Matrix &_Right) const throw(std::exception);

		Vector TransformDirection(const Vector &_Right) const;
		Vector TransformVector(const Vector &_Right) const;
		Vector operator * (const Vector &_Right) const;

		double* operator [](int n) const throw();

		int row_;
		int column_;
		double  *data_;

	private:
		// 对第row_num行进行放缩
		void ScaleRow(int row_num, double scale);
		// 交换第i j行
		void ExchangeRow(int i, int j);
		void AddRow(int i, int j, double factor);
	};


	static Matrix operator *(double factor, Matrix mat)
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
	static Matrix operator *(Matrix mat, double factor)
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

	static std::ostream& operator << (std::ostream &out, const Matrix &mat)
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