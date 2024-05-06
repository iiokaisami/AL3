#include "Player.h"
#include "ImGuiManager.h"
#include "cassert"

Player::~Player() {
	delete calculationMath_;
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(Model* model, uint32_t font) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	font_ = font;

	worldTransformBlock.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	calculationMath_ = new CalculationMath;
};

void Player::Update() {

	worldTransformBlock.UpdateMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransformBlock.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransformBlock.rotation_.y -= kRotSpeed;
	}

	// 押した方向で移動ベクトルを変更（左右）
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

	// 移動限界座標
	const float kMoveLimitX = 34;
	const float kMoveLimitY = 18;

	// 範囲を超えない処理
	worldTransformBlock.translation_.x = max(worldTransformBlock.translation_.x, -kMoveLimitX);
	worldTransformBlock.translation_.x = min(worldTransformBlock.translation_.x, kMoveLimitX);
	worldTransformBlock.translation_.y = max(worldTransformBlock.translation_.y, -kMoveLimitY);
	worldTransformBlock.translation_.y = min(worldTransformBlock.translation_.y, kMoveLimitY);

	// 座標移動（ベクトルの加算）
	worldTransformBlock.translation_ = calculationMath_->Add(worldTransformBlock.translation_, move);

	// キャラクターの攻撃
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
};

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransformBlock.translation_);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
};

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransformBlock, viewProjection_, font_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
};