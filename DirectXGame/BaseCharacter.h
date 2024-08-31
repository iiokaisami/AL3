#pragma once
#include <vector>
#include "Model.h"
#include "WorldTransform.h"

class BaseCharacter 
{
protected: // メンバ変数
	// モデルデータ配列
	std::vector<Model*> models_;
	// ワールド変換データ
	WorldTransform worldTransform_;

public:
	// 初期化
	// models モデルデータ配列
	virtual void Initialize(const std::vector<Model*>& models);

	// 更新
	virtual void Update();

	// 描画
	// viewProjection ビュープロジェクション(参照渡し)
	virtual void Draw(const ViewProjection& viewProjection);

	
	// ワールド変換データを取得
	// ワールド変換データ
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
};
