#pragma once

#include "Vec3.h"
#include "MathStuff.h"

#include <algorithm>

template <typename T>
class Vec4 : public Vec3<T> {
public:
	Vec4() = default;
	Vec4(T x, T y, T z, T w) 
		:
		Vec3(x, y, z),
		w(w)
	{}
	Vec4(const Vec3& v3, float w = 1.0f) 
		:
		Vec3(v3),
		w(w)
	{}
	// type convert operator
	template <typename T2>
	explicit operator Vec4<T2>() const {
		return { (T2)x, (T2)y, (T2)z, (T2)w };
	}
	// operators
	Vec4 operator-() const {
		return Vec4(-x, -y, -z, -w);
	}
	Vec4& operator=(const Vec4& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}
	Vec4& operator+=(const Vec4& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}
	Vec4& operator-=(const Vec4& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}
	Vec4 operator+(const Vec4& rhs) const {
		return Vec4(*this) += rhs;
	}
	Vec4 operator-(const Vec4& rhs) const {
		return Vec4(*this) -= rhs;
	}
	Vec4 & operator*=(const T& rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		w *= rhs;
		return *this;
	}
	Vec4 operator*(const T& rhs) const {
		return Vec4(*this) *= rhs;
	}
	Vec4& operator/=(const T& rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}
	Vec4 operator/(const T& rhs) const {
		return Vec4(*this) /= rhs;
	}
	bool operator==(const Vec4& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}
	bool operator!=(const Vec4& rhs) const {
		return !(*this == rhs);
	}
public:
	T w;
};

typedef Vec4<float> Vecf4;
typedef Vec4<double> Vecd4;
typedef Vec4<int> Veci4;