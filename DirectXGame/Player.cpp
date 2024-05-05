#include "Player.h"
#include "cassert"
#include "ImGuiManager.h"

Player::~Player() {
	delete calculationMath_;
}

void Player::Initialize(Model* model, uint32_t font){

//NULLポインタチェック
	assert(model);
    model_ = model;
	font_ = font;

	worldTransform_.Initialize();

	worldTransformBlock.Initialize();

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	calculationMath_ = new CalculationMath;
};

void Player::Updade() { 
	worldTransformBlock.matWorld_ = 
		calculationMath_->MakeAffineMatrix(
			worldTransformBlock.scale_,
			worldTransformBlock.rotation_, 
			worldTransformBlock.translation_);

	// 定数バッファに転送する
	worldTransformBlock.TransferMatrix();

	worldTransform_.TransferMatrix();

	//キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	//キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}

	//移動限界座標
	const float kMoveLimitX = 34;
	const float kMoveLimitY = 18;

	//範囲を超えない処理
	worldTransformBlock.translation_.x = max(worldTransformBlock.translation_.x, -kMoveLimitX);
	worldTransformBlock.translation_.x = min(worldTransformBlock.translation_.x, kMoveLimitX);
	worldTransformBlock.translation_.y = max(worldTransformBlock.translation_.y, -kMoveLimitY);
	worldTransformBlock.translation_.y = min(worldTransformBlock.translation_.y, kMoveLimitY);

	//座標移動（ベクトルの加算）
	worldTransformBlock.translation_ = calculationMath_->Add(worldTransformBlock.translation_, move);


	#ifdef DEBUG

	//キャラクターの座標を画面表示する処理
	ImGui::Begin(" ");
	//ImGui::SliderFloat3("Player", worldTransformBlock.translation_, 0.0f, 1.0f);
	ImGui::Text("Player %.3f.%.3f.%.3f",
		worldTransformBlock.translation_.x,
		worldTransformBlock.translation_.y, 
		worldTransformBlock.translation_.z);
	ImGui::End();

	#endif // DEBUG

	//スケーリング行列の生成
	//worldTransform_.scale_=calculationMath_
};

void Player::Draw(ViewProjection& viewProjection_) { 
	model_->Draw(worldTransformBlock, viewProjection_, font_);

};