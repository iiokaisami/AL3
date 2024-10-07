#include "RailCamera.h"
#include "ImGuiManager.h"

RailCamera::~RailCamera() {
	delete calculationMath_; 
}

void RailCamera::Initialize(Vector3 worldMatrix, Vector3 rotation, ViewProjection& viewProjection) {
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = worldMatrix;
	worldTransform_.rotation_ = rotation;

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 800;
	viewProjection_.Initialize();

	//数字関数
	calculationMath_ = new CalculationMath;

	controlPoints_ = {
	    {0,  0,  0},
        {10, 10, 0},
        {10, 15, 0},
        {20, 15, 10},
        {20, 0,  30},
        {30, 0,  0},
	};
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
}

void RailCamera::Update() { 
	//カメラの座標を画面表示する処理
#ifdef DEBUG

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translation", &worldTransform_.translation_.x, 0.0f, 300.0f);
	ImGui::SliderFloat3("rotation", &worldTransform_.rotation_.x, 0.0f, 180.0f);

	ImGui::SliderFloat("eyeT", &eyeT, 0.0f, 180.0f);
	ImGui::SliderFloat("targetT", &targetT, 0.0f, 180.0f);
	ImGui::SliderFloat3("forward", &forward.x, 0.0f, 180.0f);

	ImGui::End();

#endif // DEBUG

	Vector3 railCameraSpeed = {0, 0, 0.01f};

	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = calculationMath_->CatmullRomPosition(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	eyeT = eyeT + segmentCountT;
	targetT = targetT + segmentCountT;

	eye = calculationMath_->CatmullRomPosition(controlPoints_, eyeT);
	eye.y += 1;

	target = calculationMath_->CatmullRomPosition(controlPoints_, targetT);
	target.y += 1;

	/*if (targetT < 1.0f) {

		worldTransform_.translation_ = eye;

		forward = target - eye;


		Vector3 cameraRotate{};

		cameraRotate.y = std::atan2(forward.x, forward.z);
		float length = calculationMath_->Length({forward.x, 0, forward.z});

		cameraRotate.x = std::atan2(-forward.y, length);
		worldTransform_.rotation_ = cameraRotate;
	}*/

	//worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, railCameraSpeed);

	//ワールドトランスフォームのワールド行列再計算
	worldTransform_.UpdateMatrix();
	;
	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = calculationMath_->Inverse(worldTransform_.matWorld_);
}

void RailCamera::Draw() {
	/*for (size_t i = 0; i < segmentCount; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], {1.0f, 0.0f, 0.0f, 1.0f});
	}*/
}