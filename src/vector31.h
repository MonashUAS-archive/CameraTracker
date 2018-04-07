#pragma once

// A 3x1 column vector class for use on Arduino devices
class Vector31
{
public:
    Vector31();
    Vector31( float v1, float v2, float v3);
    Vector31( const Vector31 &v);

    float &operator()( int i) { return val_[i];}
    float operator()( int i) const { return val_[i];}
    Vector31 &operator+=( const Vector31 &m);
    Vector31 &operator-=( const Vector31 &m);
    Vector31 &operator*=( const float &v);
    Vector31 &operator/=( const float &v);
    Vector31 operator+( const Vector31 &m) const;
    Vector31 operator-( const Vector31 &m) const;
    Vector31 operator*( const float &v) const;
    Vector31 operator/( const float &v) const;

private:
    float val_[3];
};