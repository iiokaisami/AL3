#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "FollowCamera.h"
#include "CollisionManager.h"
#include "Ground.h"
#include <sstream>
#include <memory>

#include "Enemy2.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//void AddEnemyBullet(EnemyBullet* enemyBullet);

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return enemyBullets_; }

	//void AddEnemy(Vector3 position);
	
	// 敵リストを取得
	const std::list<Enemy*>& GetEnemy() const { return enemys_; }

	//敵発生コマンド
	std::stringstream enemyPopCommands;

	/*
    /// <summary>
    /// 敵発生データの読み込み
    /// </summary>
	void LoadEnemyPopData();

    /// <summary>
    /// 敵発生のコマンド更新
    /// </summary>
	void UpdateEnemyPopCommands();
	*/

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	uint32_t texture_ = 0;
	Model* model_ = nullptr;

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	ViewProjection viewProjection_;
	Skydome* skydome_ = nullptr;
	//RailCamera* railCamera_ = nullptr;
	FollowCamera* followCamera_ = nullptr;
	Ground* ground_ = nullptr;

	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	//敵
	std::list<Enemy*> enemys_;

	Enemy123* enemy123 = nullptr;

	//3Dモデル
	Model* modelSkydome_ = nullptr;
	Model* modelGround_ = nullptr;

	// パーツ
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	// 自弾
	std::unique_ptr<Model> modelBullet_;

	//待機中フラグ
	bool isWaiting = true;

	//待機タイマー
	int32_t waitingTimer = 0;

	CollisionManager* colliderManager_ = nullptr;

	CalculationMath* calculationMath_ = nullptr;
};