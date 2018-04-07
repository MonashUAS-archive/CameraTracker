#include "vector31.h"

Vector31::Vector31( float v1, float v2, float v3)
{
    val_[0] = v1;
    val_[1] = v2;
    val_[2] = v3;
}

Vector31::Vector31( const Vector31 &v)
{
    val_[0] = v(0);
    val_[1] = v(1);
    val_[2] = v(2);
}

Vector31 &Vector31::operator+=( const Vector31 &v)
{
    for ( int i = 0; i < 3; ++i) {
        val_[i] = (*this)(i) + v(i);
    }
    return *this;
}

Vector31 &Vector31::operator-=( const Vector31 &v)
{
    for ( int i = 0; i < 3; ++i) {
        val_[i] = (*this)(i) - v(i);
    }
    return *this;
}

Vector31 &Vector31::operator*=( const float &v)
{
    for ( int i = 0; i < 3; ++i) {
        val_[i] = (*this)(i) * v;
    }
    return *this;
}

Vector31 &Vector31::operator/=( const float &v)
{
    for ( int i = 0; i < 3; ++i) {
        val_[i] = (*this)(i) / v;
    }
    return *this;
}

Vector31 Vector31::operator+( const Vector31 &v) const 
{
    return Vector31( val_[0] + v(0), val_[1] + v(1), val_[2] + v(2));
}

Vector31 Vector31::operator-( const Vector31 &v) const
{
    return Vector31( val_[0] - v(0), val_[1] - v(1), val_[2] - v(2));
}

Vector31 Vector31::operator*( const float &v) const
{
    return Vector31( val_[0] * v, val_[1] * v, val_[2] * v);
}

Vector31 Vector31::operator/( const float &v) const
{
    return Vector31( val_[0] / v, val_[1] / v, val_[2] / v);
}
