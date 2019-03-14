#pragma once

#include "Vec2.h"
#include "MathStuff.h"
#include <algorithm>

template <typename T>
class Vec3 : public Vec2<T> {
public:
	Vec3()
	{}
	Vec3(T x, T y, T z)
		:
		Vec2(x, y),
		z(z)
	{}
	Vec3(const Vec3& vec3) 
		:
		Vec3(vec3.x, vec3.y, vec3.z)
	{}
	template <typename T2>
	explicit operator Vec3<T2>() const
	{
		return{ (T2)x,(T2)y, (T2)z };
	}
	T		LenSq() const
	{
		return sq(*this);
	}
	T		Len() const
	{
		return sqrt(LenSq());
	}
	Vec3&	Normalize()
	{
		const T length = Len();
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
	Vec3	GetNormalized() const
	{
		Vec3 norm = *this;
		norm.Normalize();
		return norm;
	}
	Vec3	operator-() const
	{
		return Vec3(-x, -y, -z);
	}
	Vec3&	operator=(const Vec3 &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	Vec3&	operator+=(const Vec3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Vec3&	operator-=(const Vec3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	Vec3	operator+(const Vec3 &rhs) const
	{
		return Vec3(*this) += rhs;
	}
	Vec3	operator-(const Vec3 &rhs) const
	{
		return Vec3(*this) -= rhs;
	}
	Vec3&	operator*=(const T &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Vec3	operator*(const T &rhs) const
	{
		return Vec3(*this) *= rhs;
	}
	float operator*(const Vec3& rhs) const {
		float result = x * rhs.x + y * rhs.y + z * rhs.z;
		return result;
	}
	Vec3 operator%(const Vec3& rhs) const {
		return { y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x };
	}
	Vec3&	operator/=(const T &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Vec3	operator/(const T &rhs) const
	{
		return Vec3(*this) /= rhs;
	}
	bool	operator==(const Vec3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool	operator!=(const Vec3 &rhs) const
	{
		return !(*this == rhs);
	}
public:
	T z;
};

typedef Vec3<float> Vecf3;
typedef Vec3<double> Vecd3;
typedef Vec3<int> Veci3;