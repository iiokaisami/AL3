#include "Player.h"
#include "ImGuiManager.h"
#include "cassert"
#include "TextureManager.h"
#include "WinApp.h"
#include "Enemy.h"

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

void Player::Initialize(Model* model, uint32_t font, Vector3 position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	font_ = font;

	worldTransformBlock.Initialize();

	worldTransformBlock.translation_ = position;

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	calculationMath_ = new CalculationMath;

	radius_ = 1.0f;

	// 3Dレティクル用のワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	viewProjection_.Initialize();

	SetCollisionAttribute(0b1);

	SetCollisionMask(0b1 << 1);

	
};

void Player::Update(ViewProjection& viewProjection, const std::list<Enemy*>& enemys) {

	preprepos = prePos;
	prePos = GetWorldPosition();

	ImGui::Begin("Player");

	ImGui::Text("w:(%+.2f,%+.2f,%+.2f)", GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z);
	ImGui::Text("cSpeed:(%+.2f,%+.2f,%+.2f)", kSpeed.x, kSpeed.y, kSpeed.z);
	ImGui::Text("EnemyVel:(%+.2f,%+.2f,%+.2f)", eneVel.x, eneVel.y, eneVel.z);

		if (isHit == true) {
			ImGui::Text("true");
	    } else if (isHit == false) {
		    ImGui::Text("false");
	    }

	ImGui::Text("timer:(%d)",hitTimer);

		
	ImGui::End();


	worldTransformBlock.UpdateMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 回転速さ[ラジアン/frame]
	// const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {

		kSpeed.x = -0.1f;
		// worldTransformBlock.rotation_.y += kRotSpeed;

	} else if (input_->PushKey(DIK_D)) {

		kSpeed.x = 0.1f;
		// worldTransformBlock.rotation_.y -= kRotSpeed;
	}
	if (input_->PushKey(DIK_W)) {

		kSpeed.y = 0.1f;
		// worldTransformBlock.rotation_.y += kRotSpeed;

	} else if (input_->PushKey(DIK_S)) {

		kSpeed.y = -0.1f;
		// worldTransformBlock.rotation_.y -= kRotSpeed;
	}

	if (input_->PushKey(DIK_RETURN)) {
		kSpeed = {0, 0, 0};
	}

	if (isHit /*&& (eneVel.x != 0.0f || eneVel.y != 0.0f)*/) {
		kSpeed = calculationMath_->Add(kSpeed, eneVel * 0.4f);
		eneVel = {0.0f, 0.0f, 0.0f};
		isHit = false;
	}

	if (hitTimer >= 0)
	{
		hitTimer--;
	}

	if (kSpeed.x < -0.15f or kSpeed.x > 0.15f or kSpeed.y < -0.15f or kSpeed.y > 0.15f) {
		kSpeed.x = max(kSpeed.x, 0.15f);
		kSpeed.x = min(kSpeed.x, -0.15f);
		kSpeed.y = max(kSpeed.y, 0.15f);
		kSpeed.y = min(kSpeed.y, -0.15f);
	}

	move.x += kSpeed.x;
	move.y += kSpeed.y;


	// 押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {

		move.x -= kCharacterSpeed;

	} else if (input_->PushKey(DIK_RIGHT)) {
		
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_DOWN)) {

		move.y -= kCharacterSpeed;
	
	} else if (input_->PushKey(DIK_UP)) {

		move.y += kCharacterSpeed;
	}


	// ゲームパッドの状態を得る変数（XINPUT）
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	

	// 移動限界座標
	const float kMoveLimitX = 17;
	const float kMoveLimitY = 10;

	// 範囲を超えない処理
	/*worldTransformBlock.translation_.x = max(worldTransformBlock.translation_.x, -kMoveLimitX);
	worldTransformBlock.translation_.x = min(worldTransformBlock.translation_.x, kMoveLimitX);
	worldTransformBlock.translation_.y = max(worldTransformBlock.translation_.y, -kMoveLimitY);
	worldTransformBlock.translation_.y = min(worldTransformBlock.translation_.y, kMoveLimitY);*/

	if (worldTransformBlock.translation_.x <= -kMoveLimitX or worldTransformBlock.translation_.x >= kMoveLimitX)
	{
		worldTransformBlock.translation_ = preprepos;
		kSpeed.x *= -1.0f;
	}
	if (worldTransformBlock.translation_.y <= -kMoveLimitY or worldTransformBlock.translation_.y >= kMoveLimitY) {
		worldTransformBlock.translation_ = preprepos;
		kSpeed.y *= -1.0f;
	}


	// 座標移動（ベクトルの加算）
	worldTransformBlock.translation_ = calculationMath_->Add(worldTransformBlock.translation_, move);


	// ビューポート行列
	Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	

	MouseReticle(matViewPort, viewProjection);

	//PlayerReticle(matViewPort, viewProjection);


	//IsRockon(enemys, viewProjection);
	enemys;

	/////// キャラクターの攻撃////////
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
};

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = calculationMath_->TransformNormal(velocity, worldTransformBlock.matWorld_);

		// 自機から標準オブジェクトへのベクトル
		//velocity = calculationMath_->Subtract(worldTransform3DReticle_.translation_, GetWorldPosition());
		//velocity = calculationMath_->Multiply(kBulletSpeed, calculationMath_->Normalize(velocity));
		if (!isRockon) {
			velocity = GetWorld3DReticlePosition() - GetWorldPosition();

			velocity = calculationMath_->Normalize(velocity) * kBulletSpeed;

		    // 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(model_, GetWorldPosition(), velocity);

			// 弾を登録する
			bullets_.push_back(newBullet);

		} else {
			for (Enemy* lockEnemy : lockOnEnemys_) {
				if (lockEnemy->isDeath() == false) {
					velocity = lockEnemy->GetWorldPosition() - GetWorldPosition();
					velocity = calculationMath_->Normalize(velocity) * kBulletSpeed;

					// 弾を生成し、初期化
					PlayerBullet* newBullet = new PlayerBullet();
					newBullet->Initialize(model_, GetWorldPosition(), velocity);

					// 弾を登録する
					bullets_.push_back(newBullet);

				}
			}
			lockOnEnemys_.clear();
		}

		
	}

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
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
};

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransformBlock, viewProjection, font_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	//3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection);
};

Vector3 Player::GetWorldPosition(){
	//ワールド座標を入れる変数
	Vector3 worldPos;

	//ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransformBlock.matWorld_.m[3][0];
	worldPos.y = worldTransformBlock.matWorld_.m[3][1];
	worldPos.z = worldTransformBlock.matWorld_.m[3][2];

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

	//worldTransformBlock.translation_ = preprepos;

	//kSpeed.x *= -1.0f;
	//kSpeed.y *= -1.0f;

	if (!isHit && hitTimer <= 0) {
		for (Enemy* enemy : enemys_) {

			eneVel = enemy->GetVel();
		}
		hitTimer = 40;
	
		isHit = true;
	}
	
}

void Player::SetParent(const WorldTransform* parent) {
	worldTransformBlock.parent_ = parent;
}

void Player::DrawUI(ViewProjection& viewProjection) {
	sprite2DReticle_->Draw();
	for (Enemy* enemy : lockOnEnemys_) {
		if (enemy->isDeath() == false) {
			Vector3 pos = enemy->GetWorldPosition();
			Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
			Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

			pos = calculationMath_->Transform(pos, matVPV);
			Sprite* reticle = Sprite::Create(textureReticle, {pos.x, pos.y}, {1, 0, 0, 1}, {0.5f, 0.5f});
			reticle->Draw();
		}
	}
}

// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
void Player::MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {
	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = calculationMath_->Inverse(matVPV);
	
	 MultiLockOn(viewProjection);
	
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

		  #ifdef DEBUG


		 ImGui::Begin("Player");

		 ImGui::Text("2DReticle:(%.2f,%.2f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
		 ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		 ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		 ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);

		 ImGui::Text("w:(%+.2f,%+.2f,%+.2f)", GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z);

		 ImGui::End();

#endif // DEBUG		 
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

	 #ifdef DEBUG


	 ImGui::Begin("Player");

	 ImGui::Text("2DReticle:(%.2f,%.2f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	 ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", cPosNear.x, cPosNear.y, cPosNear.z);
	 ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", cPosFar.x, cPosFar.y, cPosFar.z);
	 ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);

	 ImGui::End();

#endif // DEBUG
}

void Player::PlayerReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {
	////////////////自機のワールド座標から3Dレティクルのワールド座標を計算///////////////////

	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;

	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = {0, 0, kDistancePlayerTo3DReticle};

	// 自機のワールド行列の回転を反映
	offset = calculationMath_->Transform(offset, worldTransformBlock.matWorld_);

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

bool Player::IsRockon(const std::list<Enemy*>& enemys, ViewProjection& viewProjection) {
	Vector2 reticlePos = sprite2DReticle_->GetPosition();

	for (Enemy* enemy : enemys) {
		enemyPos = enemy->ChangeScreenPos(viewProjection);

		length = calculationMath_->Length({reticlePos.x, reticlePos.y, 0}, enemyPos);

		if (length <= 20.0f) {
			sprite2DReticle_->SetPosition({enemyPos.x, enemyPos.y});

			rockOnVelocity = enemy->GetWorldPosition() - GetWorldPosition();
			rockOnVelocity = kBulletSpeed * calculationMath_->Normalize(rockOnVelocity);

			return isRockon = true;
		}
	}

	return isRockon = false;
}

void Player::MultiLockOn(ViewProjection& viewProjection) { 
	sprite2DReticle_->SetColor({1, 1, 1, 1});

	if (lockOnEnemys_.size() == 0) {
		isRockon = false;
	}

	for (Enemy* enemy : enemys_){
		Vector3 pos = enemy->GetWorldPosition();
		Vector2 spritePosition = sprite2DReticle_->GetPosition();
		Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
		Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

		pos = calculationMath_->Transform(pos, matVPV);

		if (calculationMath_->Length(Vector2{pos.x, pos.y} - spritePosition) <= reticleRadius_) {
			sprite2DReticle_->SetColor({1, 0, 0, 1});

			bool isAdd = true;
			for (Enemy* lockOnenemy : lockOnEnemys_) {
				if (lockOnenemy == enemy) {
					isAdd = false;
					break;
				}
			}

			if (isAdd) {
				isRockon = true;
				lockOnEnemys_.push_back(enemy);
			}
			break;
		}
	}
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
