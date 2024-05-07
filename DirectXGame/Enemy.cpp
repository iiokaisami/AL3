#include "Enemy.h"
#include "cassert"
#include "TextureManager.h"

Enemy::~Enemy() { 
	delete calculationMath_;
}

void Enemy::Initialize(Model* model) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	worldTransform_.Initialize();

	worldTransform_.translation_ = {0, 2, 10};

	velocity_ = {0, 0, -0.5f};

	calculationMath_ = new CalculationMath;
}

void Enemy::Update(){
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, kamata_);
}