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

	void Update(ViewProjection& viewProjection);

	void Draw(ViewProjection& viewProjection);

	void Attack();

	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

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
	void DrawUI();

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	void MouseReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	void SetEnemy(std::list<Enemy*> enemy) { enemys_ = enemy; }

	void PlayerReticle(Matrix4x4 matViewPort, ViewProjection& viewProjection);

	bool IsRockon(const std::list<Enemy*>& enemys, ViewProjection& viewProjection);

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

	//2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	ViewProjection viewProjection_;

	std::list<Enemy*> enemys_;

	bool isRockon = false;

	Vector3 enemyPos{0, 0, 0};

	bool A = false;
	bool B = false;
	float t = 0;
	Vector3 positionReticle = {0, 0, 0};
};
