#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"
#include "EnemyBullet.h"
#include "BaseEnemyState.h";
#include <list>


class Player;

//GameSceneの前方宣言
class GameScene;

class Enemy {
public:

	~Enemy();

	void Initialize(Model* model, Vector3 position);

	void Update();

	//接近フェーズ初期化
	void ApproachInitialize();

	// 接近フェーズ更新関数
	//void ApproachUpdate();

	// 離脱フェーズ更新関数
	//void LeaveUpdate();

	//弾発射
	void Fire();

	void Draw(ViewProjection& viewProjection_);

	// 発射間隔
	static const int32_t kFireInterval = 60;

	void SetPlayer(Player* player) { player_ = player; }

	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 半径
	float GetRadius() { return radius_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool isDeath() { return isDeath_; }

	Vector3 GetVelocity() { return velocity_; }

	bool GetIsFire() { return isFire; }

	bool SetIsFire(bool fire) { return isFire = fire; }


	//StatePattern
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	void SetVel(const Vector3& vel) { vel_ = vel; }


private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t kamata_ = 0u;

	
	CalculationMath* calculationMath_ = nullptr;


	//行動フェーズ
	/* enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

	//フェーズ
	Phase phase_ = Enemy::Phase::Approach;

	static void (Enemy::*p[])();*/

	std::unique_ptr<BaseEnemyState> state_;

	
    Vector3 vel_ = {0, 0, 0};


	// 発射タイマー
	int32_t fireTimer_ = 0;

	// 自キャラ
	Player* player_ = nullptr;

	// 半径
	float radius_ = 0.0f;

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//デスフラグ
	bool isDeath_ = false;

	// 弾速度
	Vector3 velocity_ = {0, 0, 0};

	//発射フラグ
	bool isFire = false;
};
