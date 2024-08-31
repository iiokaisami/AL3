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
	delete followCamera_;
	delete ground_;
	delete modelGround_;
	delete colliderManager_;
	delete calculationMath_; 

	//for (Enemy* enemy : enemys_) {
		delete enemy_;
	//}

	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	delete enemy123;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//texture_ = TextureManager::Load("sample.png");
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


	// モデル
	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));

	modelBullet_.reset(Model::CreateFromOBJ("hammer", true));

	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	Vector3 playerPosition(0, 0, 20.0f);
	player_->Initialize(modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(), modelFighterR_arm_.get(),modelBullet_.get());
	 
	//LoadEnemyPopData();
	 

	 // 3Dモデルの生成 
	 modelSkydome_ = Model::CreateFromOBJ("skydome", true);
 	 modelGround_ = Model::CreateFromOBJ("ground", true);

	 skydome_ = new Skydome;
	 skydome_->Initialize(modelSkydome_);

	 ground_ = new Ground;
	 ground_->Initialize(modelGround_);

	 followCamera_ = new FollowCamera;
	 followCamera_->Initialize();

	 enemy123 = new Enemy123();
	 enemy123->Initialize(model_, {0, 10.0f, 40.0f});

	 AddEnemy({0,10.0f,40.0f});

	 //  自キャラのワールドトランスフォームを追従カメラにセット
	 followCamera_->SetTarget(&player_->GetWorldTransform());

	 //向いている方向を合わせる
	 player_->SetViewProjection(&followCamera_->GetViewProjection());


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

		//  追従カメラ
		followCamera_->Update();

		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

		viewProjection_.TransferMatrix();
	}

	
	

	//自キャラの更新
	player_->Update(viewProjection_, enemy_/*enemys_*/);

	//UpdateEnemyPopCommands();

	// デスフラグの立った敵を削除
	//enemys_.remove_if([](Enemy* enemy) {
		if (enemy_->isDeath()) {
			delete enemy_;
			//return true;
		}
		//return false;
	//});


	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead() == true) {
			delete bullet;
			return true;
		}
		return false;
	});

	
	//敵更新
	//for (Enemy* enemy : enemys_) {

		if (enemy_) {
			enemy_->Update();

			if (enemy_->GetIsFire()) {

				// 弾を生成し、初期化
				EnemyBullet* newBullet = new EnemyBullet();
				newBullet->Initialize(model_, enemy_->GetWorldPosition(), enemy_->GetVelocity());
				newBullet->SetPlayer(player_);

				// 弾を登録する
				AddEnemyBullet(newBullet);

				enemy_->SetIsFire(false);
			}
		}
	//}

	enemy123->Update();

	player_->SetEnemy(enemy_ /*enemys_*/);

	// 敵弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}


	
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

	// コライダーをリストに登録
	colliderManager_->UpData(player_, playerBullets, GetBullets(),enemy_/*GetEnemy()*/);

	ground_->Update();

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
	ground_->Draw(viewProjection_);
	player_->Draw();

	
	// 敵描画
	//for (Enemy* enemy : enemys_) {
		if (enemy_) 
		{
			enemy_->Draw(viewProjection_);
		}
	//}
	
	//enemy123->Draw(viewProjection_);

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

	player_->DrawUI(viewProjection_);
	
	
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
	
	//Enemy* enemy_ = new Enemy;
	enemy_ = new Enemy;
	enemy_->Initialize(model_, position);

	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	enemy_->SetPlayer(player_);

	//リストに登録
	//enemys_.push_back(enemy);
}
/*
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
*/