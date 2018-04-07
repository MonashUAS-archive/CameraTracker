#pragma once

#include "vector31.h"

// 3x3 Matrix class without external dependencies for use on Arduino devices
class Matrix33
{
public:
    Matrix33();
    Matrix33( float val);
    Matrix33( float v11, float v12, float v13, 
              float v21, float v22, float v23,
              float v31, float v32, float v33);
    Matrix33( const Matrix33 &m);

    float &operator()( int i, int j) { return val_[i][j];}
    float operator()( int i, int j) const { return val_[i][j];}
    Matrix33 &operator+=( const Matrix33 &m);
    Matrix33 &operator-=( const Matrix33 &m);
    Matrix33 &operator*=( const Matrix33 &m);
    Matrix33 &operator*=( const float &v);
    Matrix33 &operator/=( const float &v);
    Matrix33 operator+( const Matrix33 &m) const;
    Matrix33 operator-( const Matrix33 &m) const;
    Matrix33 operator*( const Matrix33 &m) const;
    Vector31 operator*( const Vector31 &v);
    Matrix33 operator*( const float &v) const;
    Matrix33 operator/( const float &v) const;

    Matrix33 transpose() const;

private:
    float val_[3][3];
};