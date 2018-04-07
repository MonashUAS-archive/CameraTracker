#include "matrix33.h"

Matrix33::Matrix33( float val)
{
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = val;
        }
    }
}

Matrix33::Matrix33( float v11, float v12, float v13, 
                    float v21, float v22, float v23,
                    float v31, float v32, float v33)
{
    val_[0][0] = v11;
    val_[0][1] = v12;
    val_[0][2] = v13;
    val_[1][0] = v21;
    val_[1][1] = v22;
    val_[1][2] = v23;
    val_[2][0] = v31;
    val_[2][1] = v32;
    val_[2][2] = v33;
}

Matrix33::Matrix33( const Matrix33 &m)
{
    val_[0][0] = m(0,0);
    val_[0][1] = m(0,1);
    val_[0][2] = m(0,2);
    val_[1][0] = m(1,0);
    val_[1][1] = m(1,1);
    val_[1][2] = m(1,2);
    val_[2][0] = m(2,0);
    val_[2][1] = m(2,1);
    val_[2][2] = m(2,2);
}

Matrix33 &Matrix33::operator+=( const Matrix33 &m)
{
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = (*this)(i,j) + m(i,j);
        }
    }
    return *this;
}

Matrix33 &Matrix33::operator-=( const Matrix33 &m)
{
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = (*this)(i,j) - m(i,j);
        }
    }
    return *this;
}

Matrix33 &Matrix33::operator*=( const Matrix33 &m)
{
    Matrix33 tmp = (*this);
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = 0;
            for ( int k = 0; k < 3; ++k) {
                val_[i][j] += tmp(i,k) * m(k,j);
            }
        }
    }
    return *this;
}

Matrix33 &Matrix33::operator*=( const float &v)
{
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = (*this)(i,j) * v;
        }
    }
    return *this;
}

Matrix33 &Matrix33::operator/=( const float &v)
{
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            val_[i][j] = (*this)(i,j) / v;
        }
    }
    return *this;
}

Matrix33 Matrix33::operator+( const Matrix33 &m) const
{
    return Matrix33( val_[0][0] + m(0,0), val_[0][1] + m(0,0), val_[0][2] + m(0,0),
                     val_[1][0] + m(1,0), val_[1][1] + m(1,1), val_[1][2] + m(1,2),
                     val_[2][0] + m(2,0), val_[2][1] + m(2,1), val_[2][2] + m(2,2));
}

Matrix33 Matrix33::operator-( const Matrix33 &m) const
{
    return Matrix33( val_[0][0] - m(0,0), val_[0][1] - m(0,0), val_[0][2] - m(0,0),
                     val_[1][0] - m(1,0), val_[1][1] - m(1,1), val_[1][2] - m(1,2),
                     val_[2][0] - m(2,0), val_[2][1] - m(2,1), val_[2][2] - m(2,2));
}

Matrix33 Matrix33::operator*( const Matrix33 &m) const
{
    Matrix33 ret = (*this);
    ret *= m;
    return ret;
}

Vector31 Matrix33::operator*( const Vector31 &v)
{
    Vector31 ret = { 0, 0, 0};
    for ( int i = 0; i < 3; ++i) {
        for ( int j = 0; j < 3; ++j) {
            ret(i) += val_[i][j] * v(j);
        }
    }
    return ret;
}

Matrix33 Matrix33::operator*( const float &v) const
{
    return Matrix33( val_[0][0] * v, val_[0][1] * v, val_[0][2] * v,
                     val_[1][0] * v, val_[1][1] * v, val_[1][2] * v,
                     val_[2][0] * v, val_[2][1] * v, val_[2][2] * v);
}

Matrix33 Matrix33::operator/( const float &v) const
{
    return Matrix33( val_[0][0] / v, val_[0][1] / v, val_[0][2] / v,
                     val_[1][0] / v, val_[1][1] / v, val_[1][2] / v,
                     val_[2][0] / v, val_[2][1] / v, val_[2][2] / v);
}

Matrix33 Matrix33::transpose() const
{
    Matrix33 ret = { val_[0][0], val_[1][0], val_[2][0],
                     val_[0][1], val_[1][1], val_[2][1],
                     val_[0][2], val_[1][2], val_[2][2]};
    return ret;
}