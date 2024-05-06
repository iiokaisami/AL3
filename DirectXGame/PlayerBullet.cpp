#include "PlayerBullet.h"
#include "cassert"
#include "TextureManager.h"

void PlayerBullet::Initialize(Model* bulletModel, const Vector3& position) {
	// NULLポインタチェック
	assert(bulletModel);

	bulletModel_ = bulletModel;
	//テクスチャ読み込み
	sample_ = TextureManager::Load("debugfont.png");

	//引数で受け取った初期座標をリセット
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {
	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	bulletModel_->Draw(worldTransform_, viewProjection, sample_);
}