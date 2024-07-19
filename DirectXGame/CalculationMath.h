#pragma once
#include <cmath>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Matrix4x4.h"
#include "assert.h"
#include "Vector3.h"
#include "Operator.h"

class CalculationMath {

	Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
	Vector3 operator+(const Vector3& v) { return v; }


public:

	void Initialize();

	void Updade();

	void Draw();

	// 加算
	Vector3 Add(const Vector3& v1, const Vector3& v2);

	// 減算
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);

	// スカラー倍
	Vector3 Multiply(float scalar, const Vector3& v);

	// ベクトル積
	Vector3 Multiply(const Vector3& v1, const Vector3& v2);

	// 内積
	float Dot(const Vector3& v1, const Vector3& v2);

	// 長さ(ノルム)
	float Length(const Vector3& v);

	// 正規化 v/||v||
	Vector3 Normalize(const Vector3& v);

	// クロス値
	Vector3 Cross(const Vector3& v1, const Vector3& v2);

	// X軸回転行列
	Matrix4x4 MakeRotateXMatrix(float radian);

	// Y軸回転行列
	Matrix4x4 MakeRotateYMatrix(float radian);

	// Z軸回転行列
	Matrix4x4 MakeRotateZMatrix(float radian);

	// 行列の加法
	Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

	// 行列の減法
	Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

	// 行列の積
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	// 3次元のアフィン変換行列
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	// 逆行列
	Matrix4x4 Inverse(const Matrix4x4& m);

	// 転置行列
	Matrix4x4 Transpose(const Matrix4x4& m);

	// 単位行列の作成
	Matrix4x4 MakeIdentity4x4();

	// 透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	// 正射影行列
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	// ビューポート変換行列
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	// 座標変換
	Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	// ベクトル変換
	Vector3 TransformNormal(const Vector3& vel, const Matrix4x4& matrix);

	float Length(const Vector3& v1, const Vector3& v2);

	float Lerp(const float& v1, const float& v2, float t);

	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

	Vector3 CatmullRomInterpolation(const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3, float t);

	Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);
};