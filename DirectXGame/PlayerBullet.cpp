#include "PlayerBullet.h"
#include "cassert"
#include "TextureManager.h"

PlayerBullet::~PlayerBullet() { 
	delete worldTransform_;
}

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	//テクスチャ読み込み
	sample_ = TextureManager::Load("sample.png");

	worldTransform_->Initialize();

	//引数で受け取った初期座標をリセット
	worldTransform_->translation_ = position;
}

void PlayerBullet::Update() {
	worldTransform_->UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, sample_);
}