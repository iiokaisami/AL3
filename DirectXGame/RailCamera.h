#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"
#include "PrimitiveDrawer.h"

class RailCamera {
public:
	~RailCamera();

	void Initialize(Vector3 worldMatrix, Vector3 rotation, ViewProjection& viewProjection);

	void Update();

	void Draw();

	ViewProjection& GetViewProjection() { return viewProjection_; }

	WorldTransform& GetWorldTransform() { return worldTransform_; }

	Vector3 GetWorldPosition(); 

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 数学関数
	CalculationMath* calculationMath_ = nullptr;

	// スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing;
	// 線分の数
	const size_t segmentCount = 100;

	Vector3 target;
	Vector3 eye;
	Vector3 forward;
};