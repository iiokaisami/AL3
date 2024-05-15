#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"
#include "EnemyBullet.h"
#include <list>

class Player;

class Enemy {
public:

	~Enemy();

	void Initialize(Model* model);

	void Update();

	//接近フェーズ初期化
	void ApproachInitialize();

	// 接近フェーズ更新関数
	void ApproachUpdate();

	// 離脱フェーズ更新関数
	void LeaveUpdate();

	//弾発射
	void Fire();

	void Draw(ViewProjection& viewProjection_);

	// 発射間隔
	static const int32_t kFireInterval = 60;

	void SetPlayer(Player* player) { player_ = player; }

	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return enemyBullets_; }

	// 半径
	float GetRadius() { return radius_; }


private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t kamata_ = 0u;

	
	CalculationMath* calculationMath_ = nullptr;

	//接近フェーズ速度
	Vector3 approachSpeed_;

	//離脱フェーズ速度
	Vector3 leaveSpeed_;

	//行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

	//フェーズ
	Phase phase_ = Enemy::Phase::Approach;

	//弾
	std::list<EnemyBullet*> enemyBullets_;

	// 発射タイマー
	int32_t fireTimer_ = 0;

	// 自キャラ
	Player* player_ = nullptr;

	// 半径
	float radius_;
};
