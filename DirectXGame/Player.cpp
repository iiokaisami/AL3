#include "Player.h"
#include "ImGuiManager.h"
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

void Player::Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm,Model* modelBullet) {

	// NULLポインタチェック
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);

	//assert(model);
	//model_ = model;
	//font_ = font;

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

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	SetCollisionAttribute(0b1);

	SetCollisionMask(0b1 << 1);

	
};

void Player::Update(ViewProjection& viewProjection, Enemy* enemy /*const std::list<Enemy*>& enemys*/) {

	worldTransform_.UpdateMatrix();

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

	//PlayerReticle(matViewPort, viewProjection);


	//IsRockon(enemys, viewProjection);
	enemy;

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

		// 自機から標準オブジェクトへのベクトル
		velocity = calculationMath_->Subtract(worldTransform3DReticle_.translation_, GetWorldPosition());
		velocity = calculationMath_->Multiply(kBulletSpeed, calculationMath_->Normalize(velocity));

		if (isRockon) {
			velocity = rockOnVelocity;
		}

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, GetWorldPosition(), velocity);

		//// Y軸周り角度(θy)
		//Vector3 num = {0, 0, 0};
		//num.y = std::atan2(velocity_.x, velocity_.z);

		//worldTransform_.rotation_ = num;
		//newBullet->SetWorldRotation(num);

		// 弾を登録する
		bullets_.push_back(newBullet);

		isAttack_ = true;
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
	modelBody_->Draw(worldTransformBody_, *viewProjection_);
	modelHead_->Draw(worldTransformHead_, *viewProjection_);
	modelL_arm_->Draw(worldTransformL_arm_, *viewProjection_);
	modelR_arm_->Draw(worldTransformR_arm_, *viewProjection_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(*viewProjection_);
	}

	//3Dレティクルを描画
	modelBullet_->Draw(worldTransform3DReticle_, *viewProjection_);
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
	ImGui::Begin("Floating Model");
	ImGui::SliderFloat3("Base Translation", &worldTransform_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("Head Translation", &worldTransformHead_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("ArmR Translation", &worldTransformR_arm_.translation_.x, -20.0f, 20.0f);
	int cycle = static_cast<int>(floatingCycle_);
	ImGui::SliderInt("Cycle", &cycle, 0, 600);
	floatingCycle_ = static_cast<uint16_t>(cycle);

	ImGui::SliderFloat("Amplitude", &floatingAmplitude_, 0.0f, 10.0f);

	ImGui::End();

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
	// 何もしない
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
	if (isRockon) {
		sprite2DReticle_->SetColor({1.0f, 0, 0, 1.0f});
	} else {
		sprite2DReticle_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	}
	sprite2DReticle_->Draw();

	viewProjection;
	//sprite2DReticle_->Draw();
	////for (Enemy* enemy : lockOnEnemys_) {
	//	if (enemy_->isDeath() == false) {
	//		Vector3 pos = enemy_->GetWorldPosition();
	//		Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	//		Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

	//		pos = calculationMath_->Transform(pos, matVPV);
	//		Sprite* reticle = Sprite::Create(textureReticle, {pos.x, pos.y}, {1, 0, 0, 1}, {0.5f, 0.5f});
	//		reticle->Draw();
	//	}
	////}
}

// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
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
		 Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
		 Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

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

		 ImGui::Begin("Player");

		 ImGui::Text("2DReticle:(%.2f,%.2f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
		 ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		 ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		 ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);

		 ImGui::Text("w:(%+.2f,%+.2f,%+.2f)", GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z);

		 ImGui::End();
		 
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


	 ImGui::Begin("Player");

	 ImGui::Text("2DReticle:(%.2f,%.2f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	 ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", cPosNear.x, cPosNear.y, cPosNear.z);
	 ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", cPosFar.x, cPosFar.y, cPosFar.z);
	 ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);

	 ImGui::Checkbox("Jump", &isJump_);
	 if (isJump_) {
		 Jump();
	 }

	 ImGui::End();

}

void Player::PlayerReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {
	////////////////自機のワールド座標から3Dレティクルのワールド座標を計算///////////////////

	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;

	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = {0, 0, kDistancePlayerTo3DReticle};

	// 自機のワールド行列の回転を反映
	offset = calculationMath_->Transform(offset, worldTransform_.matWorld_);

	// ベクトルの長さを整える
	offset = calculationMath_->Multiply(kDistancePlayerTo3DReticle, calculationMath_->Normalize(offset));

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = calculationMath_->Add(GetWorldPosition(), offset);
	worldTransform3DReticle_.UpdateMatrix();

	/////////////////////////////////////////////////////////////////////////////////////

	///////3Dレコードレティクルのワールド座標から2Dレティクルのスクリーン座標を計算//////

	positionReticle_.x = worldTransform3DReticle_.matWorld_.m[3][0];
	positionReticle_.y = worldTransform3DReticle_.matWorld_.m[3][1];
	positionReticle_.z = worldTransform3DReticle_.matWorld_.m[3][2];

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = calculationMath_->Multiply(calculationMath_->Multiply(viewProjection.matView, viewProjection.matProjection), matViewPort);

	// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle_ = calculationMath_->Transform(positionReticle_, matViewProjectionViewport);

	// スプライトのレティクルに座標設定
	sprite2DReticle_->SetPosition(Vector2(positionReticle_.x, positionReticle_.y));

	/////////////////////////////////////////////////////////////////////////////////////
}

bool Player::IsRockon(Enemy* enemy /* const std::list<Enemy*>& enemys*/, ViewProjection& viewProjection) {
	Vector2 reticlePos = sprite2DReticle_->GetPosition();

	//for (Enemy* enemy : enemys) {
		enemyPos = enemy->ChangeScreenPos(viewProjection);

		length = calculationMath_->Length({reticlePos.x, reticlePos.y, 0}, enemyPos);

		if (length <= 20.0f) {
			sprite2DReticle_->SetPosition({enemyPos.x, enemyPos.y});

			rockOnVelocity = enemy->GetWorldPosition() - GetWorldPosition();
			rockOnVelocity = kBulletSpeed * calculationMath_->Normalize(rockOnVelocity);

			return isRockon = true;
		}
	//}

	return isRockon = false;
}

void Player::LockOnRemove() {
	//lockOnEnemys_.remove_if([](Enemy* enemy) {
		if (enemy_ == nullptr) {
			delete enemy_;
			//return true;
		}
		//return false;
	//});
}
