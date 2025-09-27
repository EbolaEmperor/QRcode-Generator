#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

template<typename T>
class Matrix
{
private:
    std::vector<std::vector<T>> data;
    int rows, cols;

public:
    // 构造函数
    Matrix() : rows(0), cols(0) {}
    
    Matrix(int r, int c) : rows(r), cols(c)
    {
        data.resize(rows);
        for (int i = 0; i < rows; i++)
        {
            data[i].resize(cols);
            for (int j = 0; j < cols; j++)
                data[i][j] = T(0);  // 零元素
        }
    }
    
    Matrix(int r, int c, const T& value) : rows(r), cols(c)
    {
        data.resize(rows);
        for (int i = 0; i < rows; i++)
        {
            data[i].resize(cols);
            for (int j = 0; j < cols; j++)
                data[i][j] = value;
        }
    }
    
    // 拷贝构造函数
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(other.data) {}
    
    // 赋值运算符
    Matrix& operator=(const Matrix& other)
    {
        if (this != &other)
        {
            rows = other.rows;
            cols = other.cols;
            data = other.data;
        }
        return *this;
    }
    
    // 访问元素
    T& operator()(int i, int j)
    {
        if (i < 0 || i >= rows || j < 0 || j >= cols)
            throw std::out_of_range("Matrix index out of range");
        return data[i][j];
    }
    
    const T& operator()(int i, int j) const
    {
        if (i < 0 || i >= rows || j < 0 || j >= cols)
            throw std::out_of_range("Matrix index out of range");
        return data[i][j];
    }
    
    // 获取行数和列数
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    
    // 矩阵加法
    Matrix operator+(const Matrix& other) const
    {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for addition");
        
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result.data[i][j] = data[i][j] + other.data[i][j];
        return result;
    }
    
    // 矩阵减法
    Matrix operator-(const Matrix& other) const
    {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result.data[i][j] = data[i][j] - other.data[i][j];
        return result;
    }
    
    // 矩阵乘法
    Matrix operator*(const Matrix& other) const
    {
        if (cols != other.rows)
            throw std::invalid_argument("Matrix dimensions incompatible for multiplication");
        
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < other.cols; j++)
            {
                T sum = T(0);
                for (int k = 0; k < cols; k++)
                    sum = sum + data[i][k] * other.data[k][j];
                result.data[i][j] = sum;
            }
        return result;
    }
    
    // 标量乘法
    Matrix operator*(const T& scalar) const
    {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result.data[i][j] = data[i][j] * scalar;
        return result;
    }
    
    // 创建单位矩阵
    static Matrix identity(int n)
    {
        Matrix result(n, n);
        for (int i = 0; i < n; i++)
            result.data[i][i] = T(1);
        return result;
    }

    T trace() const
    {
        T tr = T(0);
        for (int i = 0; i < rows; i++)
            tr = tr + data[i][i];
        return tr;
    }
    
    // 矩阵快速幂
    Matrix power(int exp) const
    {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for power operation");
        if (exp < 0)
            throw std::invalid_argument("Negative exponent not supported");
        
        if (exp == 0) return identity(rows);
        Matrix result = identity(rows);
        Matrix base = *this;
        
        while (exp > 0)
        {
            if (exp % 2 == 1)
                result = result * base;
            base = base * base;
            exp = exp / 2;
        }
        return result;
    }
    
    // 高斯消元法解线性方程组 Ax = b
    std::vector<T> solve(const std::vector<T>& b) const
    {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for solving linear system");
        
        if (b.size() != rows)
            throw std::invalid_argument("Vector dimension must match matrix rows");
        
        Matrix augmented(rows, cols + 1);
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
                augmented.data[i][j] = data[i][j];
            augmented.data[i][cols] = b[i];
        }
        
        for (int i = 0; i < rows; i++)
        {
            int pivotRow = i;
            for (int k = i + 1; k < rows; k++)
                if (augmented.data[k][i] != T(0) && augmented.data[pivotRow][i] == T(0))
                    pivotRow = k;
            
            if (augmented.data[pivotRow][i] == T(0))
                throw std::runtime_error("Matrix is singular");
            
            if (pivotRow != i)
            {
                for (int j = 0; j <= cols; j++)
                {
                    T temp = augmented.data[i][j];
                    augmented.data[i][j] = augmented.data[pivotRow][j];
                    augmented.data[pivotRow][j] = temp;
                }
            }
            
            for (int k = 0; k < rows; k++)
                if (k != i && augmented.data[k][i] != T(0))
                {
                    T factor = augmented.data[k][i] / augmented.data[i][i];
                    for (int j = i; j <= cols; j++)
                        augmented.data[k][j] = augmented.data[k][j] - factor * augmented.data[i][j];
                }
        }
        
        // 回代求解
        std::vector<T> solution(rows);
        for (int i = 0; i < rows; i++)
            solution[i] = augmented.data[i][cols] / augmented.data[i][i];
        
        return solution;
    }
    
    // 转置
    Matrix transpose() const
    {
        Matrix result(cols, rows);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result.data[j][i] = data[i][j];
        return result;
    }
    
    // 输出矩阵
    void print() const
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                std::cout << data[i][j];
                if (j < cols - 1) std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
    
    // 友元函数：标量在左侧的乘法
    friend Matrix operator*(const T& scalar, const Matrix& matrix)
    {
        return matrix * scalar;
    }
};

// 输出流运算符
template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix)
{
    for (int i = 0; i < matrix.getRows(); i++)
    {
        for (int j = 0; j < matrix.getCols(); j++)
        {
            os << matrix(i, j);
            if (j < matrix.getCols() - 1) os << " ";
        }
        if (i < matrix.getRows() - 1) os << std::endl;
    }
    return os;
}

#endif // MATRIX_HPP