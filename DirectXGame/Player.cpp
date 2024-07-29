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

	/*for (Sprite* sprite2DReticle : sprite2DReticle_) {
		delete sprite2DReticle;
	}

	for (Vector2* spritePosition : spritePosition_) {
		delete spritePosition;
	}

	for (Vector3* positionReticle : positionReticle_) {
		delete positionReticle;
	}*/
}

void Player::Initialize(Model* model, uint32_t font,Vector3 position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	font_ = font;

	worldTransformBlock.Initialize();

	worldTransformBlock.translation_ = position;

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	calculationMath_ = new CalculationMath;

	radius_ = 2.0f;
	
	// 3Dレティクル用のワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	//レティクル用テクスチャ取得
	textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	//for (Sprite* sprite2DReticle : sprite2DReticle_) {
		// sprite2DReticle = new Sprite;
		//  AddReticle(sprite2DReticle);
		sprite2DReticle_ = Sprite::Create(textureReticle, {}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	//}

	viewProjection_.Initialize();

	SetCollisionAttribute(0b1);

	SetCollisionMask(0b1 << 1);

};

void Player::Update(ViewProjection& viewProjection) {

	worldTransformBlock.UpdateMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	//デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {

		worldTransformBlock.rotation_.y += kRotSpeed;

	} else if (input_->PushKey(DIK_D)) {

		worldTransformBlock.rotation_.y -= kRotSpeed;
	}

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
	const float kMoveLimitX = 10;
	const float kMoveLimitY = 6;

	// 範囲を超えない処理
	worldTransformBlock.translation_.x = max(worldTransformBlock.translation_.x, -kMoveLimitX);
	worldTransformBlock.translation_.x = min(worldTransformBlock.translation_.x, kMoveLimitX);
	worldTransformBlock.translation_.y = max(worldTransformBlock.translation_.y, -kMoveLimitY);
	worldTransformBlock.translation_.y = min(worldTransformBlock.translation_.y, kMoveLimitY);

	// 座標移動（ベクトルの加算）
	worldTransformBlock.translation_ = calculationMath_->Add(worldTransformBlock.translation_, move);


	// ビューポート行列
	Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	

	//MouseReticle(matViewPort, viewProjection);

	PlayerReticle(matViewPort, viewProjection);

	/*if (!isRockon && preRockOn) {
		isLeave = true;
	}

	if (isLeave) {
		Vector3 pos = calculationMath_->Lerp({positionReticle.x, positionReticle.y, 0}, positionReticle, t);
		t += 1.0f / 50.0f;
		sprite2DReticle_->SetPosition({pos.x, pos.y});

		if (t > 1.0f) {
			B = false;
			t = 0;
		}
	} else {
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}*/

	preRockOn = isRockon;


	//if (isRockon)
	//{
		//for (Sprite* sprite2DReticle : sprite2DReticle_) {
			//sprite2DReticle = new Sprite;
			//AddReticle(sprite2DReticle);
		//}
	//}

	IsRockon(enemys_, viewProjection);


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
		// velocity = calculationMath_->TransformNormal(velocity, worldTransformBlock.matWorld_);

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

void Player::OnCollision() {
	// 何もしない
}

void Player::SetParent(const WorldTransform* parent) {
	worldTransformBlock.parent_ = parent;
}

void Player::DrawUI() {
	//for (Sprite* sprite2DReticle : sprite2DReticle_) {
		if (isRockon) {
			sprite2DReticle_->SetColor({1.0f, 0, 0, 1.0f});
		} /*else {
			sprite2DReticle->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		}*/
		sprite2DReticle_->Draw();
	//}
}

// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
/* void Player::MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = calculationMath_->Inverse(matVPV);
	
	
	 // スプライトの現在座標を取得
	for (Sprite* sprite2DReticle : sprite2DReticle_) {
		for (Vector2* spritePosition : spritePosition_) {
			spritePosition->x = sprite2DReticle->GetPosition().x;
			spritePosition->y = sprite2DReticle->GetPosition().y;
		}
	}

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
		 for (Sprite* sprite2DReticle : sprite2DReticle_) {
			 sprite2DReticle->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));
		 }

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
	 if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		 for (Sprite* sprite2DReticle : sprite2DReticle_) {
			 for (Vector2* spritePosition : spritePosition_) {
				 spritePosition->x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
				 spritePosition->y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
				 // スプライトの座標変更を反映
				 sprite2DReticle->SetPosition({spritePosition->x,spritePosition->y});
			 }
		 }
	 }

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

	 ImGui::End();

}*/

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
	/* for (Vector3* positionReticle : positionReticle_) {
		positionReticle->x = worldTransform3DReticle_.matWorld_.m[3][0];
		positionReticle->y = worldTransform3DReticle_.matWorld_.m[3][1];
		positionReticle->z = worldTransform3DReticle_.matWorld_.m[3][2];

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = calculationMath_->Multiply(calculationMath_->Multiply(viewProjection.matView, viewProjection.matProjection), matViewPort);

		// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		//positionReticle = calculationMath_->Transform(Vector3(positionReticle->x,positionReticle->y,positionReticle->z), matViewProjectionViewport);
		positionReticle->x = positionReticle->x * matViewProjectionViewport.m[0][0] + positionReticle->y * matViewProjectionViewport.m[1][0] + positionReticle->z * matViewProjectionViewport.m[2][0] +
		                     1.0f * matViewProjectionViewport.m[3][0];
		positionReticle->y = positionReticle->x * matViewProjectionViewport.m[0][1] + positionReticle->y * matViewProjectionViewport.m[1][1] + positionReticle->z * matViewProjectionViewport.m[2][1] +
		                     1.0f * matViewProjectionViewport.m[3][1];
		positionReticle->z = positionReticle->x * matViewProjectionViewport.m[0][2] + positionReticle->y * matViewProjectionViewport.m[1][2] + positionReticle->z * matViewProjectionViewport.m[2][2] +
		                     1.0f * matViewProjectionViewport.m[3][2];
		float w = positionReticle->x * matViewProjectionViewport.m[0][3] + positionReticle->y * matViewProjectionViewport.m[1][3] + positionReticle->z * matViewProjectionViewport.m[2][3] +
		          1.0f * matViewProjectionViewport.m[3][3];
		assert(w != 0.0f);
		positionReticle->x /= w;
		positionReticle->y /= w;
		positionReticle->z /= w;

		// スプライトのレティクルに座標設定
		for (Sprite* sprite2DReticle : sprite2DReticle_) {
			sprite2DReticle->SetPosition(Vector2(positionReticle->x, positionReticle->y));
		}
	}*/


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
	/* for (Sprite* sprite2DReticle : sprite2DReticle_) {

		for (Enemy* enemy : enemys) {
			enemyPos = enemy->ChangeScreenPos(viewProjection);

			length = calculationMath_->Length({sprite2DReticle->GetPosition().x, sprite2DReticle->GetPosition().y, 0}, enemyPos);

			if (length <= 20.0f) {
				sprite2DReticle->SetPosition({enemyPos.x, enemyPos.y});

				rockOnVelocity = enemy->GetWorldPosition() - GetWorldPosition();
				rockOnVelocity = kBulletSpeed * calculationMath_->Normalize(rockOnVelocity);

				return isRockon = true;
			}
		}
	}
	return isRockon = false;*/
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

//void Player::AddReticle(Sprite* sprite) { 
//	sprite2DReticle_.push_back(sprite);
//}
