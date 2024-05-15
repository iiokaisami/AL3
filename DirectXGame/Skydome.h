#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Skydome {
public:

	void Initialize();

	void Update();

	void Draw();

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
};
