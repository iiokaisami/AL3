#pragma once
#include "Model.h"
#include "Input.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"

class Enemy123 {
public:

	~Enemy123();

	void Initialize(Model* model, Vector3 position);

	void Update();

	void Draw(ViewProjection& viewProjection_);

private:

	// ワールド変換データ
    WorldTransform worldTransform_;
	WorldTransform worldTransform1_;
	WorldTransform worldTransform2_;

    // モデル
    Model* model_ = nullptr;

	// テクスチャハンドル
    uint32_t kamata_ = 0u;

	// キーボード入力
	Input* input_ = nullptr;

    CalculationMath* calculationMath_ = nullptr;

	bool U1 = false;
	bool Y2 = false;

	float Us = 100.0f;
	float Ys = 150.0f;

	float U = 2.0f;
	float Y = 2.0f;

	bool U10 = false;
	bool Y20 = false;

	bool isDed = false;
	int deds = 30;
	bool isSHI = false;
};
