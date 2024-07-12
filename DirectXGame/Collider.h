#pragma once
#include "Vector3.h"
#include "CalculationMath.h"

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

	// 衝突属性（自分）を取得
	uint32_t GetCollisionAttribute() { return collisionAttribute_; }

	// 衝突属性（自分）を設定
	uint32_t SetCollisionAttribute(const uint32_t& collisionAttribute) {return  collisionAttribute_ = collisionAttribute; }

	// 衝突マスク（相手）を取得
	uint32_t GetCollisionMask() { return collisionMask_; }

	// 衝突マスク（相手）を設定
	uint32_t SetCollisionMask(const uint32_t& collisionMask) {return  collisionMask_ = collisionMask; }


private:

	// 衝突半径
	float radius_ = 2;

	// 衝突属性
	uint32_t collisionAttribute_ = 0xffffffff;

	// 衝突マスク(自分)
	uint32_t collisionMask_ = 0xffffffff;

};