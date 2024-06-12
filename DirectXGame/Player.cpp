#include "Player.h"
#include "ImGuiManager.h"
#include "cassert"
#include "TextureManager.h"
#include "WinApp.h"

Player::~Player() {
	delete calculationMath_;

	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;
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
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	//スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	viewProjection_.Initialize();

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
	
	// スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	//ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
		//スプライトの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition);
	}

	MouseReticle(matViewPort, viewProjection);


	/////// キャラクターの攻撃////////
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}


};

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		//velocity = calculationMath_->TransformNormal(velocity, worldTransformBlock.matWorld_);

		//自機から標準オブジェクトへのベクトル
		velocity = calculationMath_->Subtract(worldTransform3DReticle_.translation_, GetWorldPosition());
		velocity = calculationMath_->Multiply(kBulletSpeed, calculationMath_->Normalize(velocity));


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

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに合わせて回転させる
		// velocity = calculationMath_->TransformNormal(velocity, worldTransformBlock.matWorld_);

		// 自機から標準オブジェクトへのベクトル
		velocity = calculationMath_->Subtract(worldTransform3DReticle_.translation_, worldTransformBlock.rotation_);
		velocity = calculationMath_->Multiply(kBulletSpeed, calculationMath_->Normalize(velocity));

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
	sprite2DReticle_->Draw();
}

// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
void Player::MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection) {
	
	 POINT mousePosition;
	// マウス座標（スクリーン座標）を取得する
	 GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	 HWND hwnd = WinApp::GetInstance()->GetHwnd();
	 ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	 sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

	// ビュープロジェクションビューポート合成行列
	 Matrix4x4 matVPV =
		 calculationMath_->Multiply(viewProjection.matView, calculationMath_->Multiply(viewProjection.matProjection, matViewPort));

	// 合成行列の逆行列を計算する
	 Matrix4x4 matInverseVPV = calculationMath_->Inverse(matVPV);

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

	 ImGui::End();

}
