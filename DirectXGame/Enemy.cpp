#include "Enemy.h"
#include "cassert"
#include "TextureManager.h"
#include "Player.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"


Enemy::~Enemy() { 
	delete calculationMath_;

	for (TimeCall* timeCall : timeCalls_) {
		delete timeCall;
	}
}

void Enemy::Initialize(Model* model, Vector3 position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	vel_ = {0, 0, 0};

	calculationMath_ = new CalculationMath;

	radius_ = 2.0f;

	isDeath_ = false;

	ChangeState(std::make_unique<EnemyStateApproach>(this));

	SetCollisionAttribute(0b1 << 1);

	SetCollisionMask(0b1);
}

void Enemy::Update(){
	
	fireTimer_--;

	timeCalls_.remove_if([](TimeCall* timeCall) {
		if (timeCall->IsFinished() == true) {
			delete timeCall;
			return true;
		}
		return false;
	});

	for (TimeCall* timeCall : timeCalls_)
	{
		timeCall->Update();
	}


	// 指定時間に達した
	if (fireTimer_ <= 0) {

		TimeReset();

		SetFireTimer();
	}

	

	state_->Update();

	// 移動(ベクトルを加算)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, vel_);



	worldTransform_.UpdateMatrix();

	
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


//StatePattern
void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state); 
}

void Enemy::TimeReset() {

	isFire = true;

	// 弾を発射
	Fire();

	//timeCalls_.push_back(new TimeCall(std::bind(&Enemy::TimeReset, this, kFireInterval)));

    std::function<void(void)> callback = std::bind(&Enemy::TimeReset, this);

	TimeCall* timedCall = new TimeCall(callback, kFireInterval);

	timeCalls_.push_back(timedCall);

}

void Enemy::ClearTimeCalls(std::list<TimeCall*> timeCalls) { 
	timeCalls.clear();
}
