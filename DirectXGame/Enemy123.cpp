#include "Enemy123.h"
#include "cassert"
#include "TextureManager.h"
#include "Player.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"
#include "WinApp.h"

Enemy123::~Enemy123() {
	delete calculationMath_; 
}

void Enemy123::Initialize(Model* model, Vector3 position) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	worldTransform_.Initialize();
	worldTransform1_.Initialize();
	worldTransform2_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform1_.translation_ = {0, 0.2f, 35.0f};
	worldTransform2_.translation_ = {15.0f, 1.0f, 30.0f};

	worldTransform_.scale_ = {5.0f, 5.0f, 2.0f};
	worldTransform1_.scale_ = {10.0f, 0.5f, 0.5f};
	worldTransform2_.scale_ = {2.0f, 2.0f, 2.0f};

	calculationMath_ = new CalculationMath;
}

void Enemy123::Update() {

	if (input_->TriggerKey(DIK_U)) {
		U = Us;

		U1 = true;
	}
	if (input_->TriggerKey(DIK_Y)) {
		Y = Ys;

		Y2 = true;
	}

	if (input_->TriggerKey(DIK_J)) {
		U1 = false;
		worldTransform1_.translation_ = {-15.0f, 0.2f, 20.0f};
		worldTransform1_.scale_ = {0.5f, 0.5f, 10.0f};
		U10 = true;
	}
	if (input_->TriggerKey(DIK_H)) {
		Y2 = false;

		worldTransform2_.translation_ = {-5.0f, 0.2f, 35.0f};
		Y20 = true;
	}

	if (U1) {

		U -= 1.2f;

		if (U <= 0.0f &&!U10) {
			worldTransform1_.translation_.z -= 0.5f;
		} else if (U <= 0.0f && U10) {
			worldTransform1_.translation_.x += 0.5f;
		}
	}

	if (Y2) {

		Y -= 1.2f;

		if (Y <= 0.0f&&!Y20) {

			worldTransform2_.translation_.x -= 0.5f;
		} else if (Y <= 0.0f && Y20)
		{
			worldTransform2_.translation_.z -= 0.5f;
		}
	}



	if (input_->TriggerKey(DIK_RETURN)){
		isDed = true;
	}

	if (isDed&&deds>=0)
	{
		deds -= 1;
		worldTransform_.rotation_.z -= 0.05f;
		worldTransform_.translation_.x += 0.1f;
		worldTransform_.translation_.y -= 0.1f;
	}

	if (deds <= 0)
	{
		isSHI = true;
	}

	worldTransform_.UpdateMatrix();
	worldTransform1_.UpdateMatrix();
	worldTransform2_.UpdateMatrix();
}

void Enemy123::Draw(ViewProjection& viewProjection_) {
	if (!isSHI) {
		model_->Draw(worldTransform_, viewProjection_, kamata_);
	}
	if (U1) {
		model_->Draw(worldTransform1_, viewProjection_, kamata_);
	}
	if (Y2) {
		model_->Draw(worldTransform2_, viewProjection_, kamata_);
	}
}
