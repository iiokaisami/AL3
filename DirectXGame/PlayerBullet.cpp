#include "PlayerBullet.h"
#include "cassert"
#include "TextureManager.h"

PlayerBullet::~PlayerBullet() {
	delete calculationMath_;
}

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3 velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	velocity_ = velocity;

	calculationMath_ = new CalculationMath;

	//テクスチャ読み込み
	sample_ = TextureManager::Load("sample.png");

	worldTransform_.Initialize();

	//引数で受け取った初期座標をリセット
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {

	//座標を移動させる(フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();

	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, sample_);
}