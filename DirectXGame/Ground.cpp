#include "Ground.h"

Ground::~Ground() { 
	delete calculationMath_;
}

void Ground::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	worldTransform_.Initialize();

	calculationMath_ = new CalculationMath;
}

void Ground::Update() { 
	worldTransform_.UpdateMatrix();
}
　
void Ground::Draw(const ViewProjection& viewProjection) {
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}
