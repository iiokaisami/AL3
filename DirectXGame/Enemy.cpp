#include "Enemy.h"
#include "cassert"
#include "TextureManager.h"
#include "Player.h"

Enemy::~Enemy() { 
	delete calculationMath_;
}

void Enemy::Initialize(Model* model, Vector3 position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	approachSpeed_ = {0, 0, -0.05f};

	leaveSpeed_ = {-0.2f, 0.1f, -0.2f};

	calculationMath_ = new CalculationMath;

	ApproachInitialize();

	radius_ = 2.0f;

	isDeath_ = false;
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

void Enemy::ApproachInitialize() {
	//発射タイマーを初期化
	fireTimer_ = kFireInterval;
}

void Enemy::ApproachUpdate(){
	//発射タイマーカウントダウン
	fireTimer_--;

	// 指定時間に達した
	if (fireTimer_ <= 0)
	{
		isFire = true;

		//弾を発射
		Fire();

		fireTimer_ = kFireInterval;
	}
	
	// 移動(ベクトルを加算)
	//worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, approachSpeed_);

	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < -20.0f) {
		phase_ = Enemy::Phase::Leave;
	}

}

void Enemy::LeaveUpdate() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, leaveSpeed_);
}

void Enemy::Fire() {
	// 弾の速度
	const float kBulletSpeed = 1.0f;
	velocity_ = {0, 0, kBulletSpeed};

	// 自キャラのワールド座標を取得する
	Vector3 startPos = player_->GetWorldPosition();
	// 敵キャラのワールド座標を取得する
	Vector3 goalPos = GetWorldPosition();
	// 敵キャラ->自キャラの差分ベクトルを求める
	Vector3 differenceVector = calculationMath_->Subtract(startPos, goalPos);
	// ベクトルの正規化
	Vector3 normalVec = calculationMath_->Normalize(differenceVector);
	// ベクトルの長さを、速さに合わせる
	velocity_ = calculationMath_->Multiply(kBulletSpeed, normalVec);
}

void Enemy::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, kamata_);
}

Vector3 Enemy::GetWorldPosition() { 
	//ワールド座標を取得
	Vector3 worldPos;

	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision(){ 
	isDeath_ = true;
}