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

	worldTransform_.translation_ = {0, 2, 20};

	approachSpeed_ = {0, 0, -0.3f};

	leaveSpeed_ = {-0.2f, 0.1f, -0.2f};

	calculationMath_ = new CalculationMath;
}

void Enemy::Update(){

	switch (phase_) {
	case Enemy::Phase::Approach:
	default:
		
		//接近フェーズ更新関数
		Enemy::ApproachUpdate();
		
		break;
	case Enemy::Phase::Leave:

		// 離脱フェーズ更新関数
		Enemy::LeaveUpdate();
		
		break;
	}

	
	worldTransform_.UpdateMatrix();
}

void Enemy::ApproachUpdate(){
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, approachSpeed_);

	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Enemy::Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, leaveSpeed_);
}


void Enemy::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, kamata_);
}