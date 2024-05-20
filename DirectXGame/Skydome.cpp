#include "Skydome.h"

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0, 0, 0};

	worldTransform_.scale_ = {300, 300, 300};
}

void Skydome::Update() { 
}

void Skydome::Draw(const ViewProjection& viewProjection) { 
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}