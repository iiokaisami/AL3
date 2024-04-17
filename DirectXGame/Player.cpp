#include "Player.h"
#include "cassert"

void Player::Initialize(Model* model, uint32_t font){

//NULLポインタチェック
	assert(model);
    model_ = model;
	font_ = font;

	worldTransform_.Initialize();
};

void Player::Updade() { 
	worldTransform_.TransferMatrix();
};

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, font_); 
};