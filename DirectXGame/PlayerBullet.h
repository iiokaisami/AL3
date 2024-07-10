#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Collider.h"

class PlayerBullet : public Collider 
{
public:

	~PlayerBullet();

	void Initialize(Model* model, const Vector3& position,const Vector3 velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

	// 半径
	float GetRadius() { return radius_; }


private:

	//ワールド変換データ
	WorldTransform worldTransform_;

	//数学関数
	CalculationMath* calculationMath_ = nullptr;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t sample_ = 0u;

	//速度
	Vector3 velocity_ = {0, 0, 0};

	//寿命<frm>
	static const int32_t kLifeTime = 60 * 5;

	//デスタイマー
	int32_t deathTimer_ = kLifeTime;

	//デスグラフ
	bool isDead_ = false;

	// 半径
	float radius_ = 0;

};
