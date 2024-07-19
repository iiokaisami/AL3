#include "Operator.h"

Vector3 operator+(const Vector3& v1, const Vector3& v2) { 
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}
Vector3 operator*(float scalar, const Vector3& v) { 
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}
Vector3 operator*(const Vector3& v, float s) { 
	return s * v; 
}
Vector3 operator/(const Vector3& v, float s) { 
	return 1.0f / s * v;
}

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { 
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { 
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { 
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][0] * m2.m[0][column] + m1.m[row][1] * m2.m[1][column] + m1.m[row][2] * m2.m[2][column] + m1.m[row][3] * m2.m[3][column];
		}
	}
	return result;
}

Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
Vector3 operator+(const Vector3& v) { return v; }