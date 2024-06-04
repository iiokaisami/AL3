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
	sprite2DReticle_ = Sprite::Create(textureReticle, {640,360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	viewProjection_.Initialize();

};

void Player::Update() {

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


	/////自機のワールド座標から3Dレティクルのワールド座標を計算////////

	//自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;

	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = {0, 0, 1.0f};

	//自機のワールド行列の回転を反映
	offset = calculationMath_->TransformNormal(offset, worldTransformBlock.matWorld_);

	//ベクトルの長さを整える
	offset = calculationMath_->Multiply(kDistancePlayerTo3DReticle, calculationMath_->Normalize(offset));

	//3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = calculationMath_->Transform(offset, worldTransformBlock.matWorld_);
	worldTransform3DReticle_.UpdateMatrix();

	///////////////////////////////////////////////////////////////////



	/////自機のワールド座標から3Dレティクルのワールド座標を計算////////

	Vector3 positionReticle = worldTransform3DReticle_.translation_;

	//ビューポート行列
	Matrix4x4 matViewPort = calculationMath_->MakeViewportMatrix(0, 0,WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport =
		calculationMath_->Multiply(viewProjection_.matView, calculationMath_->Multiply(viewProjection_.matProjection, matViewPort));

	//ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle = calculationMath_->Transform(positionReticle, matViewProjectionViewport);

	//スプライトのレティクルに座標設定
	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	///////////////////////////////////////////////////////////////////



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
	//model_->Draw(worldTransform3DReticle_, viewProjection);
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