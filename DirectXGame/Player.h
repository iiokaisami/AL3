#pragma once
#include "CalculationMath.h"
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <list>
#include "Sprite.h"
#include "Collider.h"
#include "BasePlayerState.h"

class Enemy;

class Player :public Collider
{
public:
	~Player();

	void Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm, Model* modelBullet);

	void Update(ViewProjection& viewProjection, Enemy* enemy/*const std::list<Enemy*>& enemys*/);

	void Draw();

	// 移動
	void Move();

	// 旋回
	void Turn();

	// 浮遊ギミック初期化
	void InitializeFloatingGimick();

	// 浮遊ギミック更新
	void UpdateFloatingGimick();

	// カメラのセット
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	// ワールド変換データの取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	void Attack();
	bool GetIsAttack() const { return isAttack_; }
	void AttackMotion();
	void SetAttackParameter(float attackParameter, uint16_t attackCycle);

	void Jump();
	bool GetIsJump() const { return isJump_; }
	void SetJumpParameter();

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

	Vector3 GetWorld3DReticlePosition();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	//弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	// StatePattern
	void ChangeState(std::unique_ptr<BasePlayerState> state);

	void UpdateMatrix();

	//半径
	float GetRadius() { return radius_; }

	void SetParent(const WorldTransform* parent);

    /// <summary>
    /// Ui描画
    /// </summary>
	void DrawUI(ViewProjection& viewProjection);

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	void MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	void SetEnemy(Enemy* enemy/* std::list<Enemy*> enemy*/) { enemy_ = enemy; }

	void PlayerReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	bool IsRockon(Enemy* enemy/* const std::list<Enemy*>& enemys*/, ViewProjection& viewProjection);

	void LockOnRemove(); 

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	// モデル
	Model* modelHead_ = nullptr;
	Model* modelBody_ = nullptr;
	Model* modelL_arm_ = nullptr;
	Model* modelR_arm_ = nullptr;
	// 弾
	Model* modelBullet_ = nullptr;
	
	// テクスチャハンドル
	//uint32_t font_ = 0u;

	// キーボード入力
	Input* input_ = nullptr;

	CalculationMath* calculationMath_ = nullptr;

	// 弾の速度
	const float kBulletSpeed = 1.0f;

	Vector3 velocity{0, 0, kBulletSpeed};

	Vector3 rockOnVelocity{0, 0, kBulletSpeed};

	std::unique_ptr<BasePlayerState> state_;

	// 弾
	std::list<PlayerBullet*> bullets_;

	bool isAttack_ = false;

	// 攻撃ギミックの媒介変数
	float attackParameter_;
	// 攻撃モーション
	uint16_t attackCycle_;

	//半径
	float radius_ = 0;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	uint32_t textureReticle = 0;

	//2Dレティクル用のスプライト
	std::list<Sprite*> lockOnSprite2DReticle_ ;

	Sprite* sprite2DReticle_ = nullptr;
	Vector2 spritePos_;
	Vector3 positionReticle_ = {0, 0, 0};

	const ViewProjection* viewProjection_ = nullptr;

	//std::list<Enemy*> enemys_;
	Enemy* enemy_;

	bool isRockon = false;

	Vector3 enemyPos{0, 0, 0};
	Vector3 lockOnyPos{0, 0, 0};

	float length = 100.0f;
	float reticleRadius_ = 35.0f;

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 速度
	Vector3 velocity_ = {};
	// 速さ
	float speed = 0.5f;

	// ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;

	bool isJump_ = false;

	// 浮遊ギミックの媒介変数
	float floatingParameter_;
	// 浮遊の振幅<m>
	float floatingAmplitude_;
	// 浮遊移動のサイクル
	uint16_t floatingCycle_;
};
