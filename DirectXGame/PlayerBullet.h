#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Collider.h"



#include "Input.h"


class PlayerBullet : public Collider 
{
public:

	~PlayerBullet();

	void Initialize(Model* model, const Vector3& position,const Vector3 velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	void SetIsDead() { isDead_ = true; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

	// ワールド変換データの取得
	void SetWorldRotation(Vector3 num) { worldTransform_.rotation_ = num; }

	// 半径
	float GetRadius() { return radius_; }


private:

	//ワールド変換データ
	WorldTransform worldTransform_;

	//数学関数
	CalculationMath* calculationMath_ = nullptr;

	// モデル
	Model* model_ = nullptr;

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


	// キーボード入力
	Input* input_ = nullptr;
};
