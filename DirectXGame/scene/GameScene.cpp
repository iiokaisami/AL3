#include "GameScene.h"
#include "TextureManager.h"
#include "AxisIndicator.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete debugCamera_;
	delete player_; 
	delete model_;
	delete enemy_;
	delete skydome_;
	delete modelSkydome_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	texture_ = TextureManager::Load("debugfont.png");
	model_ = Model::Create();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 300;
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	 player_->Initialize(model_, texture_);

	 enemy_ = new Enemy;
	 enemy_->Initialize(model_);
	 enemy_->SetPlayer(player_);

	 // 3Dモデルの生成
	 modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	 skydome_ = new Skydome;
	 skydome_->Initialize(modelSkydome_);
}

void GameScene::Update() { 

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = true;
	}
#endif // _DEBUG

	//カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
	//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}


	//自キャラの更新
	player_->Update();

	if (enemy_) {
		enemy_->Update();
	}

	CheckAllCollisions();

	skydome_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	skydome_->Draw(viewProjection_);
	player_->Draw(viewProjection_);

	if (enemy_) {
		enemy_->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定

	//自キャラの座標
	posA = player_->GetWorldPosition();

	//自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();

		float dir, rad, playerRad, enemyBltRad;
		dir = sqrtf((posB.x - posA.x) * (posB.x - posA.x) + 
			(posB.y - posA.y) * (posB.y - posA.y) +
			(posB.z - posA.z) * (posB.z - posA.z));

		playerRad = player_->GetRadius();
		enemyBltRad = bullet->GetRadius();

		rad = sqrtf((playerRad + enemyBltRad) * (playerRad + enemyBltRad));

		// 球と球の交差判定
		if (dir <= rad) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定

	// 敵キャラの座標
	posA = enemy_->GetWorldPosition();

	// 自弾と敵キャラ全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		posB = bullet->GetWorldPosition();

		float dir, rad, enemyRad, playerBltRad;
		dir = sqrtf((posB.x - posA.x) * (posB.x - posA.x) +
			(posB.y - posA.y) * (posB.y - posA.y) + 
			(posB.z - posA.z) * (posB.z - posA.z));

		enemyRad = enemy_->GetRadius();
		playerBltRad = bullet->GetRadius();

		rad = sqrtf((enemyRad + playerBltRad) * (enemyRad + playerBltRad));

		// 球と球の交差判定
		if (dir <= rad) {
			// 敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			// 自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

    #pragma endregion

	#pragma region 自弾と敵弾の当たり判定

	for (EnemyBullet* enemyBullet : enemyBullets) {
		// 敵弾の座標
		posA = enemyBullet->GetWorldPosition();

		// 自弾と敵弾全ての当たり判定
		for (PlayerBullet* playerBullet : playerBullets) {
			// 自弾の座標
			posB = playerBullet->GetWorldPosition();

			float dir, rad, enemyBltRad, playerBltRad;
			dir = sqrtf((posB.x - posA.x) * (posB.x - posA.x) + 
				(posB.y - posA.y) * (posB.y - posA.y) + 
				(posB.z - posA.z) * (posB.z - posA.z));

			enemyBltRad = enemyBullet->GetRadius();
			playerBltRad = playerBullet->GetRadius();

			rad = sqrtf((enemyBltRad + playerBltRad) * (enemyBltRad + playerBltRad));

			// 球と球の交差判定
			if (dir <= rad) {
				// 敵弾の衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
				// 自弾の衝突時コールバックを呼び出す
				playerBullet->OnCollision();
			}
		}
	}

    #pragma endregion
}