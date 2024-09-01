#include "Enemy.h"
#include "cassert"
#include "TextureManager.h"
#include "Player.h"
#include <cmath>
#include <numbers>
#include "EnemyStateRoot.h"
#include "EnemyStateLeave.h"
#include "WinApp.h"


#include "ImGuiManager.h"

Enemy::~Enemy() { 
	delete calculationMath_;

	for (TimeCall* timeCall : timeCalls_) {
		delete timeCall;
	}
}

void Enemy::Initialize(Model* model, Vector3 position, Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm) {

	// NULLポインタチェック
	assert(model);
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);

	model_ = model;
	kamata_ = TextureManager::Load("kamata.ico");

	modelBody_ = modelBody;
	modelHead_ = modelHead;
	modelL_arm_ = modelL_arm;
	modelR_arm_ = modelR_arm;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {5.0f, 5.0f, 5.0f};

	worldTransformBody_.Initialize();
	worldTransformBody_.translation_ = {0, 8, 10};

	worldTransformHead_.Initialize();
	worldTransformHead_.translation_ = {0.0f, 1.5f, 0.0f};

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation_ = {-0.35f, 1.35f, 0.0f};

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation_ = {0.35f, 1.35f, 0.0f};


	// パーツ同士の親子関係
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;


	vel_ = {0, 0, 0};
	velocity = {0.1f, 0.1f, 0.1f};

	calculationMath_ = new CalculationMath;

	radius_ = 8.0f;

	isDeath_ = false;

	ChangeState(std::make_unique<EnemyStateRoot>(this));

	SetCollisionAttribute(0b1 << 1);

	SetCollisionMask(0b1);

	InitializeFloatingGimick();
}

void Enemy::Update() {
	
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

	if (hitPoint_ == 0)
	{
		isDeath_ = true;
	}

	state_->Update();

	UpdateFloatingGimick();

	// 移動(ベクトルを加算)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, vel_);

	Move();

	worldTransform_.UpdateMatrix();

	ImGui::Begin("enemy Model");

	ImGui::SliderFloat3("Head Translation", &worldTransform_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("Head ratate", &worldTransform_.rotation_.x, -20.0f, 20.0f);

	ImGui::SliderFloat3("ToPlayer", &toPlayer.x, -20.0f, 20.0f);
	ImGui::DragInt("HP", &hitPoint_, 1);

	

	ImGui::End();
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
	//model_->Draw(worldTransform_, viewProjection_, kamata_);
	modelHead_->Draw(worldTransform_, viewProjection_);
}

void Enemy::InitializeFloatingGimick() {
	floatingParameter_ = 0.0f;
	floatingAmplitude_ = 0.4f;
	floatingCycle_ = 120;

}

void Enemy::UpdateFloatingGimick() {
	const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;

	// パラメータを１ステップ分加算
	floatingParameter_ += step;

	// 2πを超えたら０に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);

	// 浮遊を座標に反映
	worldTransform_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude_;


	worldTransform_.UpdateMatrix();
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
	hitPoint_ -= damage_;
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

Vector3 Enemy::ChangeScreenPos(ViewProjection& viewProjection_) { 
	// ワールド->スクリーン
	Vector3 position = GetWorldPosition();

	// ビューポート行列
	Matrix4x4 matViewport = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matVPV = viewProjection_.matView * viewProjection_.matProjection * matViewport;

	// ワールド->スクリーン座標系（ここで3Dから2Dになる）
	position = calculationMath_->Transform(position, matVPV);

	// 座標設定
	return position;
}

void Enemy::Move() {
	toPlayer = calculationMath_->Subtract(player_->GetWorldPosition(), GetWorldPosition());

	// ベクトルを正規化する
	toPlayer = calculationMath_->Normalize(toPlayer);
	velocity = calculationMath_->Normalize(velocity);
	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	velocity = calculationMath_->Multiply(1.0f, calculationMath_->Slerp(velocity, toPlayer, 0.05f));

	// 進行方向に見た目の回転を合わせる(ex1)
	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(velocity.x, velocity.z);
	// 座標を移動させる(フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity/5);

	worldTransform_.UpdateMatrix();
}
