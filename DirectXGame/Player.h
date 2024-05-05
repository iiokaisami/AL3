#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "CalculationMath.h"

class Player {
public:

	~Player();

	void Initialize(Model*model,uint32_t font);

	void Updade();

	void Draw(ViewProjection& viewProjection_);

	private:
		//ワールド変換データ
	    WorldTransform worldTransform_;

		WorldTransform worldTransformBlock;

		//モデル
	    Model* model_ = nullptr;
		//テクスチャハンドル
	    uint32_t font_ = 0u;

		//キーボード入力
	    Input* input_ = nullptr;

		CalculationMath* calculationMath_ = nullptr;
};
