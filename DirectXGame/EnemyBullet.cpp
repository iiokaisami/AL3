#include "EnemyBullet.h"
#include "cassert"

EnemyBullet::~EnemyBullet() { delete calculationMath_; }

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3 velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	velocity_ = velocity;

	calculationMath_ = new CalculationMath;

	// テクスチャ読み込み
	uv_ = TextureManager::Load("uvChecker.png");

	worldTransform_.Initialize();

	// 引数で受け取った初期座標をリセット
	worldTransform_.translation_ = position;
}

void EnemyBullet::Update() {

	// 座標を移動させる(フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--fireTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, uv_);
}