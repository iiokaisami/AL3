#include "CalculationMath.h"


void CalculationMath::Initialize(){};

void CalculationMath::Updade(){};

void CalculationMath::Draw(){};

// 加算
Vector3 CalculationMath::Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

// 減算
Vector3 CalculationMath::Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

// スカラー倍
Vector3 CalculationMath::Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

// ベクトル積
Vector3 CalculationMath::Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

// 内積
float CalculationMath::Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

// 長さ(ノルム)
float CalculationMath::Length(const Vector3& v) {
	float result;
	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

// 正規化 v/||v||
Vector3 CalculationMath::Normalize(const Vector3& v) {
	Vector3 result;
	result.x = v.x / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.y = v.y / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.z = v.z / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

// クロス値
Vector3 CalculationMath::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};

	return result;
}

// X軸回転行列
Matrix4x4 CalculationMath::MakeRotateXMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, std::cos(radian), std::sin(radian), 0.0f, 0.0f, -std::sin(radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}

// Y軸回転行列
Matrix4x4 CalculationMath::MakeRotateYMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    std::cos(radian), 0.0f, -std::sin(radian), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, std::sin(radian), 0.0f, std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}

// Z軸回転行列
Matrix4x4 CalculationMath::MakeRotateZMatrix(float radian) {
	Matrix4x4 result;
	result = {
	    std::cos(radian), std::sin(radian), 0.0f, 0.0f, -std::sin(radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}

// 行列の加法
Matrix4x4 CalculationMath::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}

// 行列の減法
Matrix4x4 CalculationMath::Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}

// 行列の積
Matrix4x4 CalculationMath::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][0] * m2.m[0][column] + m1.m[row][1] * m2.m[1][column] + m1.m[row][2] * m2.m[2][column] + m1.m[row][3] * m2.m[3][column];
		}
	}
	return result;
}

// 3次元のアフィン変換行列
Matrix4x4 CalculationMath::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix)); // R

	result = {
	    scale.x * rotateXYZMatrix.m[0][0],
	    scale.x * rotateXYZMatrix.m[0][1],
	    scale.x * rotateXYZMatrix.m[0][2],
	    0.0f,
	    scale.y * rotateXYZMatrix.m[1][0],
	    scale.y * rotateXYZMatrix.m[1][1],
	    scale.y * rotateXYZMatrix.m[1][2],
	    0.0f,
	    scale.z * rotateXYZMatrix.m[2][0],
	    scale.z * rotateXYZMatrix.m[2][1],
	    scale.z * rotateXYZMatrix.m[2][2],
	    0.0f,
	    translate.x,
	    translate.y,
	    translate.z,
	    1.0f};

	return result;
}

// 逆行列
Matrix4x4 CalculationMath::Inverse(const Matrix4x4& m) {
	Matrix4x4 result;
	float a;
	Matrix4x4 b;

	a = 1 /
	    (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	     m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	     m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	     m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	     m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	     m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	b.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	            m.m[1][1] * m.m[2][3] * m.m[3][2];
	b.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	            m.m[0][1] * m.m[2][3] * m.m[3][2];
	b.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	            m.m[0][1] * m.m[1][3] * m.m[3][2];
	b.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	            m.m[0][1] * m.m[1][3] * m.m[2][2];

	b.m[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	            m.m[1][0] * m.m[2][3] * m.m[3][2];
	b.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	            m.m[0][0] * m.m[2][3] * m.m[3][2];
	b.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	            m.m[0][0] * m.m[1][3] * m.m[3][2];
	b.m[1][3] = +m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	            m.m[0][0] * m.m[1][3] * m.m[2][2];

	b.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	            m.m[1][0] * m.m[2][3] * m.m[3][1];
	b.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	            m.m[0][0] * m.m[2][3] * m.m[3][1];
	b.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	            m.m[0][0] * m.m[1][3] * m.m[3][1];
	b.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	            m.m[0][0] * m.m[1][3] * m.m[2][1];

	b.m[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	            m.m[1][0] * m.m[2][2] * m.m[3][1];
	b.m[3][1] = +m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	            m.m[0][0] * m.m[2][2] * m.m[3][1];
	b.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	            m.m[0][0] * m.m[1][2] * m.m[3][1];
	b.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	            m.m[0][0] * m.m[1][2] * m.m[2][1];

	result.m[0][0] = a * b.m[0][0];
	result.m[0][1] = a * b.m[0][1];
	result.m[0][2] = a * b.m[0][2];
	result.m[0][3] = a * b.m[0][3];
	result.m[1][0] = a * b.m[1][0];
	result.m[1][1] = a * b.m[1][1];
	result.m[1][2] = a * b.m[1][2];
	result.m[1][3] = a * b.m[1][3];
	result.m[2][0] = a * b.m[2][0];
	result.m[2][1] = a * b.m[2][1];
	result.m[2][2] = a * b.m[2][2];
	result.m[2][3] = a * b.m[2][3];
	result.m[3][0] = a * b.m[3][0];
	result.m[3][1] = a * b.m[3][1];
	result.m[3][2] = a * b.m[3][2];
	result.m[3][3] = a * b.m[3][3];

	return result;
}

// 転置行列
Matrix4x4 CalculationMath::Transpose(const Matrix4x4& m) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			if (row == column) {
				result.m[row][column] = m.m[row][column];
			} else {
				result.m[row][column] = m.m[column][row];
			}
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 CalculationMath::MakeIdentity4x4() {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			if (row == column) {
				result.m[row][column] = 1;
			} else {
				result.m[row][column] = 0;
			}
		}
	}
	return result;
}

// 透視投影行列
Matrix4x4 CalculationMath::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result;
	result = {
	    1 / aspectRatio * (1 / std::tan(fovY / 2)), 0.0f, 0.0f, 0.0f, 0.0f, 1 / std::tan(fovY / 2), 0.0f, 0.0f, 0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f, 0.0f, 0.0f,
	    -nearClip * farClip / (farClip - nearClip), 0.0f,
	};

	return result;
}

// 正射影行列
Matrix4x4 CalculationMath::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;
	result = {
	    2 / (right - left),0.0f,0.0f,0.0f,
	    0.0f,2 / (top - bottom),0.0f,0.0f,
	    0.0f,0.0f,1 / (farClip - nearClip),0.0f,
	    (left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1.0f,
	};

	return result;
}

// ビューポート変換行列
Matrix4x4 CalculationMath::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;
	result = {width / 2, 0.0f, 0.0f, 0.0f, 0.0f, -height / 2, 0.0f, 0.0f, 0.0f, 0.0f, maxDepth - minDepth, 0.0f, left + (width / 2), top + (height / 2), minDepth, 1.0f};

	return result;
}

// 座標変換
Vector3 CalculationMath::Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

//ベクトル変換
Vector3 CalculationMath::TransformNormal(const Vector3& vel, const Matrix4x4& matrix) {

	Vector3 result{
	    vel.x * matrix.m[0][0] + vel.y * matrix.m[1][0] + vel.z * matrix.m[2][0], 
		vel.x * matrix.m[0][1] + vel.y * matrix.m[1][1] + vel.z * matrix.m[2][1],
	    vel.x * matrix.m[0][2] + vel.y * matrix.m[1][2] + vel.z * matrix.m[2][2]};

	return result;
}

float CalculationMath::Length(const Vector3& v1, const Vector3& v2) {
	Vector3 distance;
	float d;

	distance = Subtract(v2, v1);
	d = sqrtf(powf(distance.x, 2) + powf(distance.y, 2) + powf(distance.z, 2));

	return d;
}

float CalculationMath::Lerp(const float& v1, const float& v2, float t) {

	float result;
	result = v1 + t * (v2 - v1);
	return result;
}

Vector3 CalculationMath::Lerp(const Vector3& v1, const Vector3& v2, float t){

	Vector3 result;
	result = Add(v1, Multiply(t, Subtract(v2, v1)));
	return result;
}

Vector3 CalculationMath::Slerp(const Vector3& v1, const Vector3& v2, float t) {

	Vector3 Nv1 = Normalize(v1);
	Vector3 Nv2 = Normalize(v2);

	float dot = Dot(Nv1, Nv2);

	dot = (std::min)(dot, 1.0f);
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1 - t) * theta);
	float sinThetaTo = std::sin(t * theta);

	Vector3 man;

	if (sinTheta < 1.0e-5) {
		man = Nv1;
	} else {
		man = Multiply(1 / sinTheta, Add(Multiply(sinThetaFrom, Nv1), Multiply(sinThetaTo, Nv2)));
	}

	float length1 = Length(v1);
	float length2 = Length(v2);

	float length = Lerp(length1, length2, t);

	man = Multiply(length, man);

	return man;
}

Vector3 CalculationMath::CatmullRomInterpolation(const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3, float t) { 

	const float s = 0.5f; // 1/2のこと

	float t2 = t * t; // tの2乗
	float t3 = t2 * t; // tの3乗

	Vector3 e3 = -p0 + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2 * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

Vector3 CalculationMath::CatmullRomPosition(const std::vector<Vector3>& points, float t) { 
	
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区画数は制御点の数-1
	size_t division = points.size() - 1;
	// 1区画の長さ（全体を1,0fとしたときの現在位置）
	float areaWith = 1.0f / division;

	// 区画内の始点を0.0f、終点を1.0fをしたときの現在位置
	float t_2 = std::fmod(t, areaWith) * division;
	// 下段(0.0f)と上段(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区画番号
	size_t index = static_cast<size_t>(t / areaWith);
	// 区画番号が上段を超えないように収める
	index = std::min(index, division - 1);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区画のp0はp1を重複使用する
	if (index == 0){
		index0 = index1;
	}

	// 最後の区画のp3はp2を重複使用する
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 4点を指定してCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}