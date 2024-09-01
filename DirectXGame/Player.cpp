#include "Player.h"
#include "cassert"
#include "TextureManager.h"
#include "WinApp.h"
#include "Enemy.h"
#include <cmath>
#include <numbers>
#include "PlayerStateAttack.h"
#include "PlayerStateRoot.h"
#include "PlayerStateJump.h"
#include "Easing.h"


Player::~Player() {
	delete calculationMath_;

	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;

	for (Sprite* sprite2DReticle : lockOnSprite2DReticle_) {
		delete sprite2DReticle;
	}
}

void Player::Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm, Model* modelBullet, Model* model, uint32_t font) {

	// NULLポインタチェック
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);

	assert(model);
	model_ = model;
	font_ = font;

	modelBody_ = modelBody;
	modelHead_ = modelHead;
	modelL_arm_ = modelL_arm;
	modelR_arm_ = modelR_arm;

	modelBullet_ = modelBullet;


	worldTransform_.Initialize();
	
	worldTransformBody_.Initialize();

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

	ChangeState(std::make_unique<PlayerStateRoot>(this));

	InitializeFloatingGimick();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	calculationMath_ = new CalculationMath;

	radius_ = 2.0f;

	// 3Dレティクル用のワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
	worldTransform_.translation_ = {0, 1.0f, 0};

	// レティクル用テクスチャ取得
	textureReticle = TextureManager::Load("reticle.png");
	textureHitPoint1_ = TextureManager::Load("hitPoint1.png");
	textureHitPoint2_ = TextureManager::Load("hitPoint2.png");
	textureHitPoint3_ = TextureManager::Load("hitPoint3.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteHitPoint1_ = Sprite::Create(textureHitPoint1_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteHitPoint2_ = Sprite::Create(textureHitPoint2_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteHitPoint3_ = Sprite::Create(textureHitPoint3_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	SetCollisionAttribute(0b1);

	SetCollisionMask(0b1 << 1);

	 hitPoint = 4.0f;

	 damageCount = 61.0f;

	 isHit = false;

	 isDeath_ = false;

	 audioAttackSE_ = Audio::GetInstance();
	 soundAttackSE_ = audioAttackSE_->LoadWave("attackSE.wav");

	  audioDamageSE_ = Audio::GetInstance();
	 soundDamageSE_ = audioDamageSE_->LoadWave("playerDamageSE.wav");

};

void Player::Update(ViewProjection& viewProjection, const std::list<Enemy*>& enemys) {

	worldTransform_.UpdateMatrix();
	worldTransform3DReticle_.UpdateMatrix();

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// ゲームパッドの状態を得る変数（XINPUT）
	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	// ビューポート行列
	Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	state_->Update();

	MouseReticle(matViewPort, viewProjection);
	
	Damage();
	

	if (hitPoint <= 0){
		isDeath_ = true;
	}
	enemys;

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
};

void Player::Attack() {

	XINPUT_STATE joyState;

	//ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	//Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER){


		Vector3 toPlayer = calculationMath_->Subtract(GetWorld3DReticlePosition(), GetWorldPosition());

		// ベクトルを正規化する
		toPlayer = calculationMath_->Normalize(toPlayer);
		velocity = calculationMath_->Normalize(velocity);
		// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
		velocity = calculationMath_->Multiply(1.0f, calculationMath_->Slerp(velocity, toPlayer, 0.05f));

		// 進行方向に見た目の回転を合わせる(ex1)
		// Y軸周り角度(θy)
		worldTransform_.rotation_.y = std::atan2(velocity.x, velocity.z);


		// 自機から標準オブジェクトへのベクトル
		velocity = calculationMath_->Subtract(worldTransform3DReticle_.translation_, GetWorldPosition());
		velocity = calculationMath_->Multiply(kBulletSpeed, calculationMath_->Normalize(velocity));

		if (isRockon) {
			velocity = rockOnVelocity;
		}

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, GetWorldPosition(), velocity);

		// Y軸周り角度(θy)
		Vector3 num = {0, 0, 0};
		num.y = worldTransform_.rotation_.y; 

		newBullet->SetWorldRotation(num);

		// 弾を登録する
		bullets_.push_back(newBullet);

		isAttack_ = true;

		 playAttackSE_ = audioAttackSE_->PlayWave(soundAttackSE_, false, 0.2f);
	}
}

void Player::AttackMotion(){

	if (attackParameter_ < 1) {
		// １フレーム分
		float step = 1.0f / attackCycle_;

		// フレームを加算
		attackParameter_ += step;

		// 腕の角度の始点と終点
		float armOriginRotation = std::numbers::pi_v<float>;                           
		float armEndRotation = abs(armOriginRotation - std::numbers::pi_v<float> / 2); 

		// 腕の回転
		worldTransformL_arm_.rotation_.x = armOriginRotation + EaseInQuint<float>(attackParameter_) * armEndRotation;
		worldTransformR_arm_.rotation_.x = armOriginRotation + EaseInQuint<float>(attackParameter_) * armEndRotation;

	} else {
		isAttack_ = false;
	}
}

void Player::SetAttackParameter(float attackParameter, uint16_t attackCycle) {
	attackParameter_ = attackParameter;
	attackCycle_ = attackCycle;
}

void Player::Draw() {

	if (!GetIsDeath()) {
		modelBody_->Draw(worldTransformBody_, *viewProjection_);
		modelHead_->Draw(worldTransformHead_, *viewProjection_);
		modelL_arm_->Draw(worldTransformL_arm_, *viewProjection_);
		modelR_arm_->Draw(worldTransformR_arm_, *viewProjection_);
	}

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}

	//3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, *viewProjection_, font_);
}

void Player::Move() {
	XINPUT_STATE joyState;

	// コントローラが接続されているのなら
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		// 移動量
		velocity_ = {(float)joyState.Gamepad.sThumbLX, 0.0f, (float)joyState.Gamepad.sThumbLY};

		// 移動量に速さを反映
		// 0除算にならないようにする
		if (calculationMath_->Length(velocity_) > 0.0f) {
			velocity_ = calculationMath_->Multiply(speed, calculationMath_->Normalize(velocity_));
		}

		// 移動ベクトルをカメラの角度だけ回転させる
		velocity_ = calculationMath_->Transform(velocity_, calculationMath_->MakeRotateYMatrix(viewProjection_->rotation_.y)); 

		// 移動
		//calculationMath_->Add(worldTransformBlock.translation_, move_);
		worldTransform_.translation_.x += velocity_.x;
		worldTransform_.translation_.z += velocity_.z;

	}

	// 行列の計算
	worldTransform_.UpdateMatrix();
}

void Player::Turn(){
	//Y軸周り角度(0ｙ)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	float velocityX = calculationMath_->Length({velocity_.x, 0.0f, 0.0f});
	// X軸周り角度(0x)
	worldTransform_.rotation_.x = std::atan2(0.0f, velocityX);

	worldTransform_.UpdateMatrix();
};

void Player::InitializeFloatingGimick() {
	floatingParameter_ = 0.0f;
	floatingAmplitude_ = 0.4f;
	floatingCycle_ = 120;
}

void Player::UpdateFloatingGimick() {

	const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;

	// パラメータを１ステップ分加算
	floatingParameter_ += step;

	// 2πを超えたら０に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);

	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude_;

	// 手をぶらぶら揺らす
	worldTransformL_arm_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude_;
	worldTransformR_arm_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude_;

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Jump() {

	// 移動
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, velocity_);
	// 重力加速度
	const float kGravityAcceleration = -0.05f;
	// 加速度ベクトル
	Vector3 accelerationVector = {0.0f, kGravityAcceleration, 0.0f};
	// 加速する
	velocity_ = calculationMath_->Add(velocity_, accelerationVector);

	// 着地
	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;

		isJump_ = false;
	}

	UpdateMatrix();
}

void Player::SetJumpParameter() {

	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
		worldTransformBody_.translation_.y = 1.0f;
		worldTransformL_arm_.rotation_.x = 0.0f;
		worldTransformR_arm_.rotation_.x = 0.0f;

		velocity_.y = kJumpFirstSpeed;

		isJump_ = true;
	}
}

Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos;

	//ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetWorld3DReticlePosition() { 
	Vector3 reticlePos;
	reticlePos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	reticlePos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	reticlePos.z = worldTransform3DReticle_.matWorld_.m[3][2];
	return reticlePos;
}

void Player::OnCollision() {
	
	 playDamageSE_ = audioDamageSE_->PlayWave(soundDamageSE_, false, 0.2f);
	isHit = true;
	hitPoint -= kDamage;
}

void Player::ChangeState(std::unique_ptr<BasePlayerState> state) {
	state_ = std::move(state);
}

void Player::UpdateMatrix() {
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::SetParent(const WorldTransform* parent) { 
	worldTransform_.parent_ = parent; 
}

void Player::DrawUI(ViewProjection& viewProjection) {
	
	//sprite2DReticle_->Draw();

	if (!Damage()) {
		if (hitPoint >= 1) {
			spriteHitPoint1_->Draw();
			if (hitPoint >= 2) {
				spriteHitPoint2_->Draw();
				if (hitPoint >= 3) {
					spriteHitPoint3_->Draw();
				}
			}
		}
	}
	viewProjection;
}

void Player::MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {
	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = calculationMath_->Inverse(matVPV);
	
	 // スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	 XINPUT_STATE joyState;

	 // ゲームパッド未接続なら何もせずに抜ける
	 if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		 POINT mousePosition;
		 // マウス座標（スクリーン座標）を取得する
		 GetCursorPos(&mousePosition);

		 // クライアントエリア座標に変換する
		 HWND hwnd = WinApp::GetInstance()->GetHwnd();
		 ScreenToClient(hwnd, &mousePosition);

		 // マウス座標を2Dレティクルのスプライトに代入する
		 sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));


		 // スクリーン座標
		 Vector3 posNear = Vector3(640, 360, 0);
		 Vector3 posFar = Vector3(640 ,360, 1);

		 // スクリーン座標系からワールド座標系へ
		 posNear = calculationMath_->Transform(posNear, matInverseVPV);
		 posFar = calculationMath_->Transform(posFar, matInverseVPV);

		 // マウスレイの方向
		 Vector3 mouseDirection = calculationMath_->Subtract(posFar, posNear);
		 mouseDirection = calculationMath_->Normalize(mouseDirection);

		 // カメラから標準オブジェクトの距離
		 const float kDistanceTestObject = 60;
		 worldTransform3DReticle_.translation_ = calculationMath_->Add(calculationMath_->Multiply(kDistanceTestObject, mouseDirection), posNear);

		  worldTransform3DReticle_.UpdateMatrix();

		 return;
	 }


	 // ジョイスティック状態取得
	 //if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// for (Sprite* sprite2DReticle : sprite2DReticle_) {
		//	 for (Vector2* spritePosition : spritePosition_) {
		//		 spritePosition->x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		//		 spritePosition->y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
		//		 // スプライトの座標変更を反映
		//		 sprite2DReticle->SetPosition({spritePosition->x,spritePosition->y});
		//	 }
		// }
	 //}

	  // スクリーン座標
	 Vector3 cPosNear = Vector3((float)spritePosition.x, (float)spritePosition.y, 0);
	 Vector3 cPosFar = Vector3((float)spritePosition.x, (float)spritePosition.y, 1);

	 // スクリーン座標系からワールド座標系へ
	 cPosNear = calculationMath_->Transform(cPosNear, matInverseVPV);
	 cPosFar = calculationMath_->Transform(cPosFar, matInverseVPV);

	 // マウスレイの方向
	 Vector3 controlDirection = calculationMath_->Subtract(cPosFar, cPosNear);
	 controlDirection = calculationMath_->Normalize(controlDirection);

	 // カメラから標準オブジェクトの距離
	 const float kDistanceTestObject = 60;
	 worldTransform3DReticle_.translation_ = calculationMath_->Add(calculationMath_->Multiply(kDistanceTestObject, controlDirection), cPosNear);
	 worldTransform3DReticle_.UpdateMatrix();

}

void Player::LockOnRemove() {
	lockOnEnemys_.remove_if([](Enemy* enemy) {
		if (enemy == nullptr) {
			delete enemy;
			return true;
		}
		return false;
	});
}

bool Player::Damage() { 
	if (isHit) {
		damageCount -= 1.0f;
	}

	if ((damageCount <= 60.0f && damageCount > 50.0f) or
		(damageCount <= 30.0f && damageCount > 20.0f) or 
		(damageCount <= 10.0f && damageCount > 0.0f)) {
		return true;
	}

	if (damageCount <= 0)
	{
		isHit = false;
		damageCount = 61.0f;
		return false;
	}

	return false;
}

void Player::DeleteBullet() {
	for (PlayerBullet* bullet : bullets_) {
		bullet->SetIsDead();
	}
}
