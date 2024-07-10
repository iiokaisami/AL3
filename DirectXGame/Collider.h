#pragma once
#include "Vector3.h"

class Collider {
public:

	// 半径を取得
	float GetRadius() { return radius_; }

	// 半径を設定
	float SetRadius(const float& radius) { radius_ = radius; }

	// 衝突時に呼ばれる関数
	virtual void OnCollision();

	// ワールド座標を取得
	virtual Vector3 GetWorldPosition() = 0;

private:

	// 衝突半径
	float radius_ = 2;

};