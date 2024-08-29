#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CalculationMath.h"


class FollowCamera {
public:

	~FollowCamera();

	void Initialize();

	void Update();

	void Follow();

	void Turn();

	
	void SetTarget(const WorldTransform* target) { target_ = target; }
		
	ViewProjection& GetViewProjection() { return viewProjection_; }

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 追従対象
	const WorldTransform* target_ = nullptr;

	CalculationMath* calculationMath_ = nullptr;
};
