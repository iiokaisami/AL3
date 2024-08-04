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

	void SetPosition(Vector3 position) { worldTransform_.translation_ = position; }

	void SetRotation(Vector3 rotation) { worldTransform_.rotation_ = rotation; }

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

	Vector3 target = {0, 0, 0};
	Vector3 eye = {0, 0, 0};
	Vector3 forward = {0, 0, 0};

	const float segmentCountT = 1.0f / 600.0f;
	float targetT = 30.0f / 600.0f;
	float eyeT = 0;
};