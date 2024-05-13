#include "Enemy.h"
#include "cassert"
#include "TextureManager.h"

Enemy::~Enemy() { 
	delete calculationMath_;

	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(Model* model) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	worldTransform_.Initialize();

	worldTransform_.translation_ = {5, 0, 20};

	approachSpeed_ = {0, 0, -0.1f};

	leaveSpeed_ = {-0.2f, 0.1f, -0.2f};

	calculationMath_ = new CalculationMath;

	ApproachInitialize();
}

void Enemy::Update(){

	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead() == true) {
			delete bullet;
			return true;
		}
		return false;
	});

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

	// 弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}
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
		//弾を発射
		Fire();

		fireTimer_ = kFireInterval;
	}
	
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

void Enemy::Fire() {
		// 弾の速度
		const float kBulletSpeed = -1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
	
		// 弾を生成し、初期化
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);

		// 弾を登録する
		enemyBullets_.push_back(newBullet);	
}

void Enemy::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, kamata_);

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}
}