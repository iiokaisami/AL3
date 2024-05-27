#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"

class RailCamera {
public:

	~RailCamera();

	void Initialize(Vector3 worldMatrix, Vector3 rotation);

	void Update();

	ViewProjection& GetViewProjection() { return viewProjection_; }

	WorldTransform& GetWorldTransform() { return worldTransform_; }

private:

	//ワールド変換データ
	WorldTransform worldTransform_;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//数学関数
	CalculationMath* calculationMath_ = nullptr;

};
