#include "GameScene.h"
#include "TextureManager.h"
#include "AxisIndicator.h"
#include <cassert>
#include <fstream>
#include "ImGuiManager.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete debugCamera_;
	delete player_; 
	delete model_;
	delete skydome_;
	delete modelSkydome_;
	delete railCamera_;
	delete colliderManager_;
	delete calculationMath_; 

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}

	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	texture_ = TextureManager::Load("debugfont.png");
	model_ = Model::Create();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 800;
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
	Vector3 playerPosition(0, 0, 30.0f);
	player_->Initialize(model_, texture_,playerPosition);

	 
	LoadEnemyPopData();
	 

	 // 3Dモデルの生成
	 modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	 skydome_ = new Skydome;
	 skydome_->Initialize(modelSkydome_);

	 railCamera_ = new RailCamera;
	 railCamera_->Initialize({0, 0, 0}, {0, 0, 0},viewProjection_);

	 //自キャラとレールカメラの親子関係を結ぶ
	 player_->SetParent(&railCamera_->GetWorldTransform());

	 //レティクルのテクスチャ
	 TextureManager::Load("reticle.png");

	 colliderManager_ = new CollisionManager();
	 colliderManager_->Initialize();

	  
	 calculationMath_ = new CalculationMath;
}

void GameScene::Update() { 

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
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

		railCamera_->Update();

		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}

	
	

	//自キャラの更新
	player_->Update(viewProjection_);

	UpdateEnemyPopCommands();

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->isDeath() == true) {
			delete enemy;
			return true;
		}
		return false;
	});


	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead() == true) {
			delete bullet;
			return true;
		}
		return false;
	});

	
	//敵更新
	for (Enemy* enemy : enemys_) {

		if (enemy) {
			enemy->Update();

			if (enemy->GetIsFire()) {

				// 弾を生成し、初期化
				EnemyBullet* newBullet = new EnemyBullet();
				newBullet->Initialize(model_, enemy->GetWorldPosition(), enemy->GetVelocity());
				newBullet->SetPlayer(player_);

				// 弾を登録する
				AddEnemyBullet(newBullet);

				enemy->SetIsFire(false);
			}
		}
	}
	player_->SetEnemy(enemys_);

	// 敵弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}


	
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

	// コライダーをリストに登録
	colliderManager_->UpData(player_, playerBullets, GetBullets(), GetEnemy());

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

	
	// 敵描画
	for (Enemy* enemy : enemys_) {
		if (enemy) 
		{
			enemy->Draw(viewProjection_);
		}
	}
	

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	player_->DrawUI();
	
	railCamera_->Draw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}



void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(Vector3 position) {
	
	Enemy* enemy = new Enemy;
	enemy->Initialize(model_, position);

	// 敵キャラにゲームシーンを渡す
	enemy->SetGameScene(this);
	enemy->SetPlayer(player_);

	//リストに登録
	enemys_.push_back(enemy);
}

void GameScene::LoadEnemyPopData() {

	//ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	//待機処理
	if (isWaiting) {
		waitingTimer--;

		if (waitingTimer <= 0) {
		    //待機完了
			isWaiting = false;
		}
		return;
	}


	//1行分の文字列を入れる変数
	std::string line;

	//コマンドループ
	while (getline(enemyPopCommands, line)){
		//1行分の文字列を入れる変数
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭列を取得
		getline(line_stream, word, ',');
		
		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
		    //コメント行を飛ばす
			continue;
		}

		//POPコマンド
		if (word.find("POP") == 0) {
			// X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// Y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// Z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			AddEnemy(Vector3(x, y, z));
			

		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {

			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機時間
			isWaiting = true;
			waitingTimer = waitTime;
			

			//コマンドループを抜ける
			break;
		}
	}
}