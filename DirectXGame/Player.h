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

class Enemy;

class Player :public Collider
{
public:
	~Player();

	void Initialize(Model* model, uint32_t font,Vector3 position);

	void Update(ViewProjection& viewProjection, const std::list<Enemy*>& enemys);

	void Draw(ViewProjection& viewProjection);

	void Attack();

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

	Vector3 GetWorld3DReticlePosition();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	//弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	//半径
	float GetRadius() { return radius_; }

	void SetParent(const WorldTransform* parent);

    /// <summary>
    /// Ui描画
    /// </summary>
	void DrawUI(ViewProjection& viewProjection);

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	void MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	void SetEnemy(std::list<Enemy*> enemy) { enemys_ = enemy; }

	void PlayerReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	bool IsRockon(const std::list<Enemy*>& enemys, ViewProjection& viewProjection);

	void MultiLockOn(ViewProjection& viewProjection);

	void LockOnRemove(); 

private:
	// ワールド変換データ
	WorldTransform worldTransformBlock;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t font_ = 0u;

	// キーボード入力
	Input* input_ = nullptr;

	CalculationMath* calculationMath_ = nullptr;

	// 弾の速度
	const float kBulletSpeed = 1.0f;

	Vector3 velocity{0, 0, kBulletSpeed};

	Vector3 rockOnVelocity{0, 0, kBulletSpeed};

	// 弾
	std::list<PlayerBullet*> bullets_;

	//半径
	float radius_ = 0;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	uint32_t textureReticle = 0;

	//2Dレティクル用のスプライト
	std::list<Sprite*> lockOnSprite2DReticle_ ;
	std::list<Enemy*> lockOnEnemys_;

	Sprite* sprite2DReticle_ = nullptr;
	Vector2 spritePos_;
	Vector3 positionReticle_ = {0, 0, 0};

	ViewProjection viewProjection_;

	std::list<Enemy*> enemys_;

	bool isRockon = false;

	Vector3 enemyPos{0, 0, 0};
	Vector3 lockOnyPos{0, 0, 0};

	float length = 100.0f;
	float reticleRadius_ = 35.0f;



	Vector3 kSpeed{};
	Vector3 prePos = {0.0f,0.0f,0.0f};
	Vector3 preprepos{};
	Vector3 eneVel{};
	bool isHit = false;
};
