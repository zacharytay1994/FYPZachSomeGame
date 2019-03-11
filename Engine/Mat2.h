#pragma once

#include "Vec2.h"

template <typename T>
class Mat2 {
public:
	// Set up operator overload
	Mat2& operator=(const Mat2& rhs) {
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}

	Mat2& operator*=(T rhs) {
		for (auto& row : elements) {
			for (T& e : row) {
				e *= rhs;
			};
		};
		return *this;
	}

	Mat2& operator*(T rhs) const {
		Mat2 result = *this;
		return result *= rhs;
	}
	// Matrix-matrix multiplication
	Mat2& operator*(const Mat2& rhs) const {
		Mat2 result;
		for (size_t j = 0; i < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				T sum = (T)0.0;
				for (size_t i = 0; i < 2; i++) {
					sum += elements[j][i] * rhs.elements[i][k];
				}
				result.elements[j][k] = sum;
			}
		}
		return result;
	}

	// Functions
	// Generate matrix identity
	static Mat2 Identity() {
		Mat2 temp = { (T)1.0,(T)0.0,(T)0.0,(T)1.0 };
		return temp;
	}

	// Rotation matrix
	static Mat2 Rotation(T theta) {
		const T cosTheta = cos(theta);
		const T sinTheta = sin(theta);
		Mat2 rotMat = {
			cosTheta, sinTheta,
			-sinTheta, cosTheta
		};
		return rotMat;
	}

	// Scaling matrix
	static Mat2 Scaling(T scale) {
		Mat2 scaleMat = {
			scale, (T)0.0,
			(T)0.0, scale
		};
		return scaleMat;
	}

	// Attributes
	// [row][column]
	T elements[2][2];
};

template <typename T>
// Vector matrix multiplication overload, row vector multiplication
Vec2<T> operator*(const Vec2<T>& lhs, const Mat2<T>& rhs) {
	return {
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1]
	};
}

template <typename T>
Vec2<T> operator*=(Vec2<T>& lhs, const Mat2<T>& rhs) {
	return lhs = lhs * rhs;
}

typedef Mat2<float> Matf2;
typedef Mat2<double> Matd2;