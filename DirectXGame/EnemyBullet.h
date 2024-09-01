#pragma once
#include "Collider.h"
#include "Model.h"
#include "Player.h"
#include "TextureManager.h"
#include "WorldTransform.h"

class EnemyBullet : public Collider {
public:
	~EnemyBullet();

	void Initialize(Model* model, const Vector3& position, const Vector3 velocity);

	void Update();
	
	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

	// 半径
	float GetRadius() { return radius_; }

	void SetPlayer(Player* player) { player_ = player; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// 数学関数
	CalculationMath* calculationMath_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// 速度
	Vector3 velocity_ = {0, 0, 0};

	// 発射間隔
	static const int32_t kFireInterval = 60;

	// 発射タイマー
	int32_t fireTimer_ = kFireInterval;

	// デスグラフ
	bool isDead_ = false;

	// 半径
	float radius_ = 0;

	Player* player_ = nullptr;
};