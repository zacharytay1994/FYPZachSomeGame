#pragma once

#include "Vec3.h"

template <typename T>
class Mat3 {
public:
	Mat3& operator=(const Mat3& rhs) {
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}

	Mat3& operator*=(T rhs) {
		for (auto& row : elements) {
			for (T& e : row) {
				r *= rhs;
			};
		};
		return *this;
	}

	Mat3& operator*(T rhs) {
		return *this * rhs;
	}

	Mat3& operator*=(const Mat3& rhs) {
		return *this = *this * rhs;
	}

	Mat3 operator*(const Mat3& rhs) const {
		Mat3 result;
		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 3; k++) {
				T sum = (T)0.0;
				for(size_t i = 0; i < 3; i++) {
					sum += elements[j][i] * rhs.elements[i][k];
				};
				result.elements[j][k] = sum;
			};
		};
		return result;
	}

	static Mat3 Identity() {
		return {
			(T)1.0, (T)0.0, (T)0.0,
			(T)0.0, (T)1.0, (T)0.0,
			(T)0.0, (T)0.0, (T)1.0
		};
	}

	static Mat3 Scaling(T scale) {
		return {
			scale, (T)0.0, (T)0.0,
			(T)0.0, scale, (T)0.0,
			(T)0.0, (T)0.0, scale
		};
	}

	static Mat3 RotationZ(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		return {
			cosTheta, sinTheta, (T)0.0,
			-sinTheta, cosTheta, (T)0.0,
			(T)0.0, (T)0.0, (T)1.0
		};
	}

	static Mat3 RotationY(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		return {
			cosTheta, (T)0.0, -sinTheta,
			(T)0.0, (T)1.0, (T)0.0,
			sinTheta, (T)0.0, cosTheta
		};
	}
	
	static Mat3 RotationX(T theta) {
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		return {
			(T)1.0, (T)0.0, (T)0.0,
			(T)0.0, cosTheta, sinTheta,
			(T)0.0, -sinTheta, cosTheta
		};
	}
	T elements[3][3];
};

template <typename T>
Vec3<T> operator*(const Vec3<T>& lhs, const Mat3<T>& rhs) {
	return {
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
	};
}

template <typename T>
Vec3<T> operator*=(Vec3<T>& lhs, const Mat3<T>& rhs) {
	return lhs = lhs * rhs;
}

typedef Mat3<float> Matf3;
typedef Mat3<double> Matd3;
typedef Mat3<int> Mati3;
