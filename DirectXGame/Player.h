#pragma once
#include "CalculationMath.h"
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <list>
#include "Sprite.h"

class Player {
public:
	~Player();

	void Initialize(Model* model, uint32_t font,Vector3 position);

	void Update(ViewProjection& viewProjection);

	void Draw(ViewProjection& viewProjection);

	void Attack();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	//弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	//半径
	float GetRadius() { return radius_; }

	void SetParent(const WorldTransform* parent);

    /// <summary>
    /// Ui描画
    /// </summary>
	void DrawUI();

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

	// 弾
	std::list<PlayerBullet*> bullets_;

	//半径
	float radius_;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	//2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	ViewProjection viewProjection_;

};
