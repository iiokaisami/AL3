#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"
#include "EnemyBullet.h"
#include "BaseEnemyState.h"
#include "TimeCall.h"
#include <list>
#include "Collider.h"


class Player;

//GameSceneの前方宣言
class GameScene;

class Enemy : public Collider 
{
public:

	~Enemy();

	void Initialize(Model* model, Vector3 position);

	void Update();

	//弾発射
	void Fire();

	void Draw(ViewProjection& viewProjection_);

	// 発射間隔
	static const int32_t kFireInterval = 60;

	void SetPlayer(Player* player) { player_ = player; }

	Vector3 GetWorldPosition() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

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

	int32_t SetFireTimer() { return fireTimer_ = kFireInterval; }


	//コールバック関数
	void TimeReset();

	std::list<TimeCall*> GetTimeCalls() { return timeCalls_; }

	void ClearTimeCalls(std::list<TimeCall*> timeCalls);

	Vector3 ChangeScreenPos(ViewProjection& viewProjection_);

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t kamata_ = 0u;

	
	CalculationMath* calculationMath_ = nullptr;

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

	// HP
	int hitPoint_ = 10;

	const uint32_t damage_ = 1;

	// 弾速度
	Vector3 velocity_ = {0, 0, 0};

	//発射フラグ
	bool isFire = false;

	std::list<TimeCall*> timeCalls_;
};
