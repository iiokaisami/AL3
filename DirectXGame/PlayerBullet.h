#pragma once
#include "Model.h"
#include "WorldTransform.h"

class PlayerBullet {
public:

	~PlayerBullet();

	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	private:

	//ワールド変換データ
	WorldTransform* worldTransform_ = nullptr;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t sample_ = 0u;
};
