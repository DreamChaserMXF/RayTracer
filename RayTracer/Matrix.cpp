#include "Matrix.h"

namespace xf
{
	Matrix::Matrix(int dimension) throw(std::bad_alloc) : row_(dimension), column_(dimension), data_(new double [dimension * dimension])
	{
		for(int i = 0; i < dimension; ++i)
		{
			for(int j = 0; j < dimension; ++j)
			{
				data_[i * dimension + j] = 0;
			}
		}
	}
	Matrix::Matrix(int row, int column) throw(std::bad_alloc) : row_(row), column_(column), data_(new double [row * column])
	{
		for(int i = 0; i < row; ++i)
		{
			for(int j = 0; j < column; ++j)
			{
				data_[i * column + j] = 0;
			}
		}
	}
	
	Matrix::Matrix(const Matrix &_Right) throw(std::bad_alloc) : row_(_Right.row_), column_(_Right.column_), data_(new double [_Right.row_ * _Right.column_])
	{
		for(int i = 0; i < row_; ++i)
		{
			for(int j = 0; j < column_; ++j)
			{
				data_[i * column_ + j] = _Right.data_[i * column_ + j];
			}
		}
	}
		

	Matrix::~Matrix()
	{
		delete[] data_;
	}

	Matrix& Matrix::Transpose() throw(std::bad_alloc)
	{
		double  *new_data = new double[row_ * column_];
		for(int i = 0; i < row_; ++i)
		{
			for(int j = 0; j < column_; ++j)
			{
				new_data[i * column_ + j] = data_[j * column_ + i];
			}
		}
		int tmp = row_;
		row_ = column_;
		column_ = tmp;
		delete[] data_;
		data_ = new_data;
		return *this;
	}

	Matrix Matrix::GetTranspose() const throw(std::bad_alloc)
	{
		return Matrix(*this).Transpose();
	}

	// 对第row_num行进行放缩
	void Matrix::ScaleRow(int row_num, double scale)
	{
		for(int j = 0; j < column_; ++j)
		{
			data_[row_num * column_ + j] *= scale;
		}
	}

	// 交换第i j行
	void Matrix::ExchangeRow(int i, int j)
	{
		assert(i < row_ && j < row_);
		for(int k = 0; k < column_; ++k)
		{
			double tmp = data_[i * column_ + k];
			data_[i * column_ + k] = data_[j * column_ + k];
			data_[j * column_ + k] = tmp;
		}
	}

	// row[i] += row[j] * factor
	void Matrix::AddRow(int i, int j, double factor)
	{
		assert(i < row_ && j < row_);
		for(int k = 0; k < column_; ++k)
		{
			data_[i * column_ + k] += data_[j * column_ + k] * factor;
		}
	}

	// 高斯-若当消元法求逆矩阵
	Matrix Matrix::GetInverse() const throw(std::bad_alloc, std::runtime_error)
	{
		if(row_ != column_)
		{
			throw std::exception("cannot inverse a matrix which is not square");
		}
		// 建立增广矩阵
		Matrix augmented_mat(row_, column_ * 2);
		for(int i = 0; i < row_; ++i)
		{
			for(int j = 0; j < column_; ++j)
			{
				augmented_mat.data_[i * augmented_mat.column_ + j] = data_[i * column_ + j];
			}
			for(int j = 0; j < column_; ++j)
			{
				augmented_mat.data_[i * augmented_mat.column_ + column_ + j] = (i == j ? 1 : 0);
			}
		}
		// 进行高斯-若当消元法
		for(int i = 0; i < row_; ++i)
		{
			// 找到主元
			if(abs(augmented_mat.data_[i * augmented_mat.column_ + i]) <= EPS)
			{
				int j = i + 1;
				while(j < row_)
				{
					if(abs(augmented_mat.data_[i * augmented_mat.column_ + j]) > EPS)
					{
						break;
					}
					++j;
				}
				if(j == row_)	// 矩阵为奇异矩阵
				{
					throw std::runtime_error("singular matrix cannot be inversed");
				}
				// 交换第i列和第j列
				augmented_mat.ExchangeRow(i, j);
			}
			// 主元归一
			double pivot_value = augmented_mat.data_[i * augmented_mat.column_ + i];
			augmented_mat.ScaleRow(i, 1 / pivot_value);
			// 消去主元所在列的其他位置的元素
			for(int j = 0; j < row_; ++j)
			{
				if(j != i && abs(augmented_mat.data_[j * augmented_mat.column_ + i]) > DBL_MIN)
				{
					// 用第i行，消去第j行第i列的元素
					augmented_mat.AddRow(j, i, -augmented_mat.data_[j * augmented_mat.column_ + i]);
				}
			}
		}
		// 分离增广矩阵的右半边
		Matrix inverse_mat(row_, column_);
		for(int i = 0; i < row_; ++i)
		{
			for(int j = 0; j < column_; ++j)
			{
				inverse_mat.data_[i * column_ + j] = augmented_mat.data_[i * column_ * 2 + column_ + j];
			}
		}
		return inverse_mat;
	}

		

	Matrix Matrix::GetHomogeneousMatrix() const throw(std::bad_alloc, std::bad_cast)
	{
		if(3 != row_ || 3 != column_)
		{
			throw std::bad_cast("only can get homogeneous matrix from 3*3 matrix");
		}
		Matrix ret_mat(4, 4);
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

	Vector Matrix::TransformDirection(const Vector &_Right) const
	{
		return Vector( _Right.x_ * data_[0 * column_ + 0] + _Right.y_ * data_[0 * column_ + 1] + _Right.z_ * data_[0 * column_ + 2],
					   _Right.x_ * data_[1 * column_ + 0] + _Right.y_ * data_[1 * column_ + 1] + _Right.z_ * data_[1 * column_ + 2],
					   _Right.x_ * data_[2 * column_ + 0] + _Right.y_ * data_[2 * column_ + 1] + _Right.z_ * data_[2 * column_ + 2]);
	}

	Vector Matrix::TransformVector(const Vector &_Right) const
	{
		Vector ret_vec( _Right.x_ * data_[0 * column_ + 0] + _Right.y_ * data_[0 * column_ + 1] + _Right.z_ * data_[0 * column_ + 2] + data_[0 * column_ + 3],
						_Right.x_ * data_[1 * column_ + 0] + _Right.y_ * data_[1 * column_ + 1] + _Right.z_ * data_[1 * column_ + 2] + data_[1 * column_ + 3],
						_Right.x_ * data_[2 * column_ + 0] + _Right.y_ * data_[2 * column_ + 1] + _Right.z_ * data_[2 * column_ + 2] + data_[2 * column_ + 3]);
		double homogeneous_ratio = _Right.x_ * data_[3 * column_ + 0] + _Right.y_ * data_[3 * column_ + 1] + _Right.z_ * data_[3 * column_ + 2] + data_[3 * column_ + 3];
		if(abs(homogeneous_ratio - 1.0) > DBL_MIN)
		{
			if(abs(homogeneous_ratio) < std::numeric_limits<double>::min())
			{
				throw std::exception("cannot transform a vector to infinity");
			}
			assert(abs(homogeneous_ratio) > DBL_MIN);	// 这里真是0的话怎么办？
			ret_vec.x_ /= homogeneous_ratio;
			ret_vec.y_ /= homogeneous_ratio;
			ret_vec.z_ /= homogeneous_ratio;
		}
		return ret_vec;
	}

	Matrix& Matrix::operator = (const Matrix &_Right) throw(std::exception)
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

		

	Matrix& Matrix::operator += (const Matrix &_Right) throw(std::exception)
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
	Matrix Matrix::operator + (const Matrix &_Right) const throw(std::exception)
	{
		Matrix ret_mat(*this);
		return ret_mat += _Right;
	}

	Matrix& Matrix::operator -= (const Matrix &_Right) throw(std::exception)
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
				data_[i * column_ + j] -= _Right.data_[i * column_ + j];
			}
		}
		return *this;
	}
	Matrix Matrix::operator - (const Matrix &_Right) const throw(std::exception)
	{
		Matrix ret_mat(*this);
		return ret_mat -= _Right;
	}

	Matrix Matrix::operator * (const Matrix &_Right) const throw(std::exception)
	{
		if(column_ != _Right.row_)
		{
			throw std::exception("matrix size not match");
		}
		Matrix ret_mat(row_, _Right.column_);
		for(int i = 0; i < row_; ++i)
		{
			for(int j = 0; j < _Right.column_; ++j)
			{
				double  sum = 0;
				for(int k = 0; k < column_; ++k)
				{
					sum += data_[i * column_ + k] * _Right.data_[k * _Right.column_ + j];
				}
				ret_mat.data_[i * _Right.column_ + j] = sum;
			}
		}
		return ret_mat;
	}

	

	Vector Matrix::operator * (const Vector &_Right) const
	{
		return TransformVector(_Right);
	}

	double* Matrix::operator [](int n) const throw()
	{
		return data_ + n * column_;
	}

}