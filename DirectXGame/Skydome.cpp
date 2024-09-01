#include "Skydome.h"

Skydome::~Skydome(){
	delete calculationMath_;
}

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0, 0, 0};

	worldTransform_.scale_ = {1500, 1500, 1500};

	calculationMath_ = new CalculationMath;

}

void Skydome::Update() {

}

void Skydome::Draw(const ViewProjection& viewProjection) { 
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}