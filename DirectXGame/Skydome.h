#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"

class Skydome {
public:

	~Skydome();

	void Initialize(Model* model);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	CalculationMath* calculationMath_ = nullptr;
};
