#include "RailCamera.h"
#include "ImGuiManager.h"

RailCamera::~RailCamera(){
	delete calculationMath_;
}

void RailCamera::Initialize(Vector3 worldMatrix,Vector3 rotation) {
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = worldMatrix;
	worldTransform_.rotation_ = rotation;

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 800;
	viewProjection_.Initialize();

	//数字関数
	calculationMath_ = new CalculationMath;
}

void RailCamera::Update() { 
	//カメラの座標を画面表示する処理
//#ifdef DEBUG

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translation", &worldTransform_.translation_.x, 0.0f, 300.0f);
	ImGui::SliderFloat3("rotation", &worldTransform_.rotation_.x, 0.0f, 180.0f);
	ImGui::End();

//#endif // DEBUG

	

	Vector3 railCameraSpeed = {0, 0, 0.05f};
	Vector3 rotation = {0, 0.000f, 0};

	//ワールドトランスフォームの座標の数値を加算したりする(移動)
	worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, railCameraSpeed);
	//worldTransform_.translation_ = calculationMath_->Subtract(worldTransform_.translation_, railCameraSpeed);

	//ワールドトランスフォームの角度の数値を加算したりする(回転)
	worldTransform_.rotation_ = calculationMath_->Add(worldTransform_.rotation_, rotation);

	//ワールドトランスフォームのワールド行列再計算
	worldTransform_.UpdateMatrix();

	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = calculationMath_->Inverse(worldTransform_.matWorld_);

}