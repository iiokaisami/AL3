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

	void Draw(ViewProjection& viewProjection_);

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t kamata_ = 0u;

	
	CalculationMath* calculationMath_ = nullptr;

	Vector3 velocity_;

};
