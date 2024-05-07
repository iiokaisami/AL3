#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"

class Enemy {
public:

	~Enemy();

	void Initialize(Model* model);

	void Update();

	// 接近フェーズ更新関数
	void ApproachUpdate();

	// 離脱フェーズ更新関数
	void LeaveUpdate();

	void Draw(ViewProjection& viewProjection_);

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t kamata_ = 0u;

	
	CalculationMath* calculationMath_ = nullptr;

	//接近フェーズ速度
	Vector3 approachSpeed_;

	//離脱フェーズ速度
	Vector3 leaveSpeed_;

	//行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

	//フェーズ
	Phase phase_ = Enemy::Phase::Approach;

};
