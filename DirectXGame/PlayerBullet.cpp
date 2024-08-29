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

	worldTransform_.Initialize();

	//引数で受け取った初期座標をリセット
	worldTransform_.translation_ = position;

	radius_ = 2.0f;

	SetCollisionAttribute(0b1);

	SetCollisionMask(0b1 << 1);


	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void PlayerBullet::Update() {

	//座標を移動させる(フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();

	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	/*if (input_->TriggerKey(DIK_RETURN))
	{
		isDead_ = true;
	}*/
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection);
}

void PlayerBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

Vector3 PlayerBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}