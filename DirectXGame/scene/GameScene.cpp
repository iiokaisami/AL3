#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>
#include "ImGuiManager.h"

#include "SceneStateTitle.h"
#include "SceneStatePlay.h"
#include "SceneStateClear.h"
#include "SceneStateGameOver.h"

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

	//texture_ = TextureManager::Load("sample.png");
	model_ = Model::Create();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 800;
	viewProjection_.Initialize();
	 

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);


	// モデル
	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));

	modelBullet_.reset(Model::CreateFromOBJ("hammer", true));

	modelFighterBody_E_.reset(Model::CreateFromOBJ("float_Body_E", true));
	modelFighterHead_E_.reset(Model::CreateFromOBJ("float_Head_E", true));
	modelFighterL_arm_E_.reset(Model::CreateFromOBJ("float_L_arm_E", true));
	modelFighterR_arm_E_.reset(Model::CreateFromOBJ("float_R_arm_E", true));

	modelEnemyBullet_.reset(Model::CreateFromOBJ("enemyBullet", true));



	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	Vector3 playerPosition(0, 0, 20.0f);
	player_->Initialize(modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(), modelFighterR_arm_.get(),modelBullet_.get());
	 
	LoadEnemyPopData();
	 

	 // 3Dモデルの生成 
	 modelSkydome_ = Model::CreateFromOBJ("skydome", true);
 	 modelGround_ = Model::CreateFromOBJ("ground", true);

	 skydome_ = new Skydome;
	 skydome_->Initialize(modelSkydome_);

	 ground_ = new Ground;
	 ground_->Initialize(modelGround_);

	 followCamera_ = new FollowCamera;
	 followCamera_->Initialize();


	 ChangeState(std::make_unique<SceneStateTitle>(this));


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

void GameScene::InitializeTitle() {
	// テクスチャ取得
	textureTitleLogo_ = TextureManager::Load("titleLogo.png");
	textureTitleUI_ = TextureManager::Load("titleUI.png");

	// スプライト生成
	spriteTitleLogo_ = Sprite::Create(textureTitleLogo_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteTitleUI_ = Sprite::Create(textureTitleUI_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	followCamera_->Initialize();

	audioTitleBGM_ = Audio::GetInstance();
	soundTitleBGM_ = audioTitleBGM_->LoadWave("bgm_title.wav");

	playTitleBGM_ = audioTitleBGM_->PlayWave(soundTitleBGM_, true, 0.1f);
}

void GameScene::InitializePlay() {
	// テクスチャ取得
	texturePlayUI_ = TextureManager::Load("playUI.png");
	// スプライト生成
	spritePlayUI_ = Sprite::Create(texturePlayUI_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	followCamera_->Initialize();
	AddEnemy({6.0f, 2.0f, 100.0f});
	player_->Initialize(modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(), modelFighterR_arm_.get(), modelBullet_.get());

	audioPlayBGM_ = Audio::GetInstance();
	soundPlayBGM_ = audioPlayBGM_->LoadWave("bgm_play.wav");

	playPlayBGM_ = audioPlayBGM_->PlayWave(soundPlayBGM_, true, 0.2f);
}

void GameScene::InitializeClear() {
	// テクスチャ取得
	textureClearUI_ = TextureManager::Load("clearUI.png");
	textureClearBG_ = TextureManager::Load("clearBG.png");
	// スプライト生成
	spriteClearUI_ = Sprite::Create(textureClearUI_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteClearBG_ = Sprite::Create(textureClearBG_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	
	audioClearSE_ = Audio::GetInstance();
	soundClearSE_ = audioClearSE_->LoadWave("clear.wav");

	playClearSE_ = audioClearSE_->PlayWave(soundClearSE_, false, 0.2f);
}

void GameScene::InitializeGameOver() {
	// テクスチャ取得
	textureGameOverUI_ = TextureManager::Load("gameOverUI.png");
	textureGameOverBG_ = TextureManager::Load("gameOverBG.png");
	// スプライト生成
	spriteGameOverUI_ = Sprite::Create(textureGameOverUI_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteGameOverBG_ = Sprite::Create(textureGameOverBG_, {640, 360}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	for (Enemy* enemy : enemys_) {
		enemy->DeathON();
	}

	audioGameOverSE_ = Audio::GetInstance();
	soundGameOverSE_ = audioGameOverSE_->LoadWave("gameOver.wav");

	playGameOverSE_ = audioGameOverSE_->PlayWave(soundGameOverSE_, false, 0.2f);
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


	player_->SetEnemy(enemys_);

	// 敵弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

	state_->Update();
	
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

	// コライダーをリストに登録
	colliderManager_->UpData(player_, playerBullets, GetBullets(), GetEnemy());

	ground_->Update();

	skydome_->Update();
}

void GameScene::UpdateTitle() { 

	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {}
	
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		audioTitleBGM_->StopWave(playTitleBGM_);
	}

}

void GameScene::UpdatePlay() {

	UpdateEnemyPopCommands();

	// 自キャラの更新
	player_->Update(viewProjection_, enemys_);

	// 敵更新
	for (Enemy* enemy : enemys_) {

		if (enemy) {
			enemy->Update();

			if (enemy->GetIsFire()) {

				// 弾を生成し、初期化
				EnemyBullet* newBullet = new EnemyBullet();
				newBullet->Initialize(modelEnemyBullet_.get(), enemy->GetWorldPosition(), enemy->GetVelocity());
				newBullet->SetPlayer(player_);

				// 弾を登録する
				AddEnemyBullet(newBullet);

				enemy->SetIsFire(false);
			}
		}
	}
}

void GameScene::UpdateClear() { 

	audioTitleBGM_->StopWave(playPlayBGM_);
	player_->DeleteBullet();

}

void GameScene::UpdateGameOver() {

	audioTitleBGM_->StopWave(playPlayBGM_);
	player_->DeleteBullet();

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
	

	state_->Draw();
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	state_->Draw2D();
	
	
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::DrawTitle() {

}

void GameScene::DrawTitle2D() { 
	spriteTitleLogo_->Draw();
	spriteTitleUI_->Draw();
}

void GameScene::DrawPlay() {
	player_->Draw();

	// 敵描画
	for (Enemy* enemy : enemys_) {
		if (enemy) {
			enemy->Draw(viewProjection_);
		}
	}

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}
}

void GameScene::DrawPlay2D() { 
	player_->DrawUI(viewProjection_); 
	spritePlayUI_->Draw();
}

void GameScene::DrawClear() { 
	player_->Draw(); 
}

void GameScene::DrawClear2D() {
	//spriteClearBG_->Draw();
	spriteClearUI_->Draw();
}

void GameScene::DrawGameOver() {
	player_->Draw();
	for (Enemy* enemy : enemys_) {
		if (enemy) {
			enemy->Draw(viewProjection_);
		}
	}
}

void GameScene::DrawGameOver2D() {
	spriteGameOverBG_->Draw();
	spriteGameOverUI_->Draw();
}



void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(Vector3 position) {
	
	Enemy* enemy = new Enemy;
	enemy->Initialize(model_, position, modelFighterBody_E_.get(),modelFighterHead_E_.get(), modelFighterL_arm_E_.get(), modelFighterR_arm_E_.get());

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
			//AddEnemy(Vector3(x, y, z));
			x;
			y;
			z;

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

void GameScene::ChangeState(std::unique_ptr<BaseSceneState> state) { 
	state_ = std::move(state); }

bool GameScene::ToPlay() {
	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return false;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		return true;
	}

	return false;
}

bool GameScene::PlayToClear() {
	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return false;
	}

		

	for (Enemy* enemy : enemys_) {
		if (enemy->GetIsDeath()){
			return true;
		}
	}


	return false;
}

bool GameScene::PlayToGameOver() {
	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return false;
	}

	

	if (player_->GetIsDeath()) {
		return true;
	}


	return false;
}

bool GameScene::ToTitle() {
	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせずに抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return false;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		return true;
	}
	return false;
}
