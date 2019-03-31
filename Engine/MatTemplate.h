#pragma once

#include "Vec3.h"
#include "Vec4.h"

template <typename T, size_t S>

class MatTemplate {
	typedef MatTemplate Mat;
public:
	// operators
	Mat& operator=(const Mat& rhs) {
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}
	Mat& operator*=(T rhs) {
		for (T row : elements) {
			for (T& e : row) {
				e *= rhs;
			}
		}
		return *this;
	}
	Mat operator*(T rhs) const {
		Mat result = *this;
		return result *= rhs;
	}
	Mat& operator*=(const Mat& rhs) {
		return *this = *this * rhs;
	}
	Mat operator*(const Mat& rhs) const {
		Mat result;
		for (size_t i = 0; i < S; i++) {
			for (size_t j = 0; j < S; j++) {
				T sum = (T)0;
				for (size_t k = 0; k < S; k++) {
					sum += elements[i][k] * rhs.elements[k][j];
				}
				result.elements[i][j] = sum;
			}
		}
		return result;
	}
	// inverse orthographic matrix for rotation matrix
	Mat operator!() const {
		Mat xp;
		for (size_t i = 0; i < S; i++) {
			for (size_t j = 0; j < S; j++) {
				xp.elements[i][j] = elements[j][i];
			}
		}
		return xp;
	}
	// matrix identities
	static Mat Identity() {
		// if 3 by 3, compile time check constexpr
		if constexpr (S == 3) {
			return {
			(T)1.0,(T)0.0,(T)0.0,
			(T)0.0,(T)1.0,(T)0.0,
			(T)0.0,(T)0.0,(T)1.0
			};
		}
		// if 4 by 4
		else if constexpr (S == 4) {
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0
			};
		}
		else {
			// assert message
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	// scaling matrix
	static Mat Scaling(T scale) {
		// if 3 by 3, compile time check constexpr
		if constexpr (S == 3) {
			return {
			(T)scale,(T)0.0,(T)0.0,
			(T)0.0,(T)scale,(T)0.0,
			(T)0.0,(T)0.0,(T)scale
			};
		}
		// if 4 by 4
		else if constexpr (S == 4) {
			return {
				(T)scale,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)scale,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)scale,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0
			};
		}
		else {
			// assert message
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	// rotation matrices
	static Mat RotationZ(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		// if 3 by 3, compile time check constexpr
		if constexpr (S == 3) {
			return {
			cosTheta,sinTheta,(T)0.0,
			-sinTheta,cosTheta,(T)0.0,
			(T)0.0,(T)0.0,(T)1.0
			};
		}
		// if 4 by 4
		else if constexpr (S == 4) {
			return {
				cosTheta,sinTheta,(T)0.0,(T)0.0,
				-sinTheta,cosTheta,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0
			};
		}
		else {
			// assert message
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	static Mat RotationY(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3) {
			return {
			cosTheta,(T)0.0,-sinTheta,
			(T)0.0,(T)1.0,(T)0.0,
			sinTheta,(T)0.0,cosTheta
			};
		}
		// if 4 by 4
		else if constexpr (S == 4) {
			return {
				cosTheta,(T)0.0,-sinTheta,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				sinTheta,(T)0.0,cosTheta,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0
			};
		}
		else {
			// assert message
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	static Mat RotationX(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3) {
			return {
			(T)1.0,(T)0.0,(T)0.0,
			(T)0.0,cosTheta,sinTheta,
			(T)0.0,-sinTheta,cosTheta
			};
		}
		// if 4 by 4
		else if constexpr (S == 4) {
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,cosTheta,sinTheta,(T)0.0,
				(T)0.0,-sinTheta,cosTheta,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0
			};
		}
		else {
			// assert message
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	// translation matrix
	// taking in vec
	template <class C>
	static Mat Translation(const C& vector) {
		return Translation(vector.x, vector.y, vector.z);
	}
	static Mat Translation(T x, T y, T z) {
		if constexpr (S == 4) {
			return {
			(T)1.0,(T)0.0,(T)0.0,(T)0.0,
			(T)0.0,(T)1.0,(T)0.0,(T)0.0,
			(T)0.0,(T)0.0,(T)1.0,(T)0.0,
			x,y,z,(T)1.0
			};
		}
		else {
			static_assert(false, "Wrong matrix dimensions");
		}
	}
	static Mat Projection(T w, T h, T n, T f) {
		if constexpr (S == 4) {
			return {
				(T)2.0 * n / w, (T)0.0, (T)0.0, (T)0.0,
				(T)0.0, (T)2.0 * n / h, (T)0.0, (T)0.0,
				(T)0.0, (T)0.0, f / (f - n), (T)1.0,
				(T)0.0, (T)0.0, (-n * f) / (f - n), (T)0.0
			};
		}
		else {
			static_assert(false, "Wrong matrix dimensions");
		}
	}
public:
	// [row][column]
	T elements[S][S];
};

template<typename T>
Vec4<T>& operator*=(Vec4<T>& lhs, const MatTemplate<T, 4>& rhs) {
	return lhs = lhs * rhs;
}

template<typename T>
Vec4<T> operator*(const Vec4<T>& lhs, const MatTemplate<T, 4>& rhs) {
	return {
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0] + lhs.w * rhs.elements[3][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1] + lhs.w * rhs.elements[3][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2] + lhs.w * rhs.elements[3][2],
		lhs.x * rhs.elements[0][3] + lhs.y * rhs.elements[1][3] + lhs.z * rhs.elements[2][3] + lhs.w * rhs.elements[3][3]
	};
}

typedef MatTemplate<float, 3> Matf3;
typedef MatTemplate<float, 4> Matf4;
typedef MatTemplate<double, 3> Matd3;
typedef MatTemplate<double, 4> Matd4;