#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "TextureManager.h"

class EnemyBullet {
public:

	~EnemyBullet();

	void Initialize(Model* model, const Vector3& position, const Vector3 velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// 数学関数
	CalculationMath* calculationMath_ = nullptr;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t uv_ = 0u;

	// 速度
	Vector3 velocity_;

	// 発射間隔
	static const int32_t kFireInterval = 60;

	// 発射タイマー
	int32_t fireTimer_ = kFireInterval;

	// デスグラフ
	bool isDead_ = false;
};
