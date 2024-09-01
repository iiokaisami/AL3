#include "EnemyBullet.h"
#include "cassert"
#include <cmath>
#include <iostream>

EnemyBullet::~EnemyBullet() { delete calculationMath_; }

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3 velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	velocity_ = velocity;

	calculationMath_ = new CalculationMath;

	worldTransform_.Initialize();

	// 引数で受け取った初期座標をリセット
	worldTransform_.translation_ = position;

	

	radius_ = 2.0f;

	SetCollisionAttribute(0b1 << 1);

	SetCollisionMask(0b1);
}

void EnemyBullet::Update() {

	// 敵弾から自キャラへのベクトルを計算
	Vector3 toPlayer = calculationMath_->Subtract(player_->GetWorldPosition(),player_->GetWorld3DReticlePosition());

	// ベクトルを正規化する
	toPlayer = calculationMath_->Normalize(toPlayer);
	velocity_ = calculationMath_->Normalize(velocity_);
	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	velocity_ = calculationMath_->Multiply(1.0f, calculationMath_->Slerp(velocity_, toPlayer, 0.1f));


	// 進行方向に見た目の回転を合わせる(ex1)
	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	double velocityXZ = sqrt(pow(velocity_.x, 2) + pow(velocity_.z, 2));

	// X軸周り角度(θx)
	worldTransform_.rotation_.x = (float)std::atan2(-velocity_.y, velocityXZ);



	// 座標を移動させる(フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--fireTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}

void EnemyBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}