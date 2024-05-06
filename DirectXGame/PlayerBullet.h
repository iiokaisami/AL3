#pragma once
#include "Model.h"
#include "WorldTransform.h"

class PlayerBullet {
public:

	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	private:

	//ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* bulletModel_ = nullptr;
	// テクスチャハンドル
	uint32_t sample_ = 0u;
};
