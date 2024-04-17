#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Player {
public:

	void Initialize(Model*model,uint32_t font);

	void Updade();

	void Draw(ViewProjection& viewProjection_);

	private:
		//ワールド変換データ
	    WorldTransform worldTransform_;

		//モデル
	    Model* model_ = nullptr;
		//テクスチャハンドル
	    uint32_t font_ = 0u;
};
