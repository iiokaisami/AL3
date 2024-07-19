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
	    {0,  0,  -10},
        {10, 10,  -10},
        {10, 15,  -10},
        {20, 15,  -10},
        {20, 0, -10 },
        {30, 0, -10 },
	};
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
}

void RailCamera::Update() { 
	//カメラの座標を画面表示する処理
//#ifdef DEBUG

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translation", &worldTransform_.translation_.x, 0.0f, 300.0f);
	ImGui::SliderFloat3("rotation", &worldTransform_.rotation_.x, 0.0f, 180.0f);
	ImGui::End();

//#endif // DEBUG

	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = calculationMath_->CatmullRomPosition(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);

		target = calculationMath_->CatmullRomPosition(controlPoints_, t + 1);
	}

	Vector3 railCameraSpeed = {0, 0, 0.05f};
	Vector3 rotation = {0, 0.000f, 0};

	// ワールドトランスフォームの角度の数値を加算したりする(回転)
	// worldTransform_.rotation_ = calculationMath_->Add(worldTransform_.rotation_, rotation);

	eye = GetWorldPosition();
	forward = target - eye;

	// ベクトルを正規化する
	forward = calculationMath_->Normalize(forward);
	railCameraSpeed = calculationMath_->Normalize(railCameraSpeed);
	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	railCameraSpeed = calculationMath_->Slerp(railCameraSpeed, forward, 0.1f) * 0.05f;

	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(forward.x, forward.z);
	double velocityXZ = sqrt(pow(forward.x, 2) + pow(forward.z, 2));

	// X軸周り角度(θx)
	worldTransform_.rotation_.x = (float)std::atan2(-forward.y, velocityXZ);

	//ワールドトランスフォームの座標の数値を加算したりする(移動)
	//worldTransform_.translation_ = calculationMath_->Add(worldTransform_.translation_, railCameraSpeed);
	//worldTransform_.translation_ = calculationMath_->Subtract(worldTransform_.translation_, railCameraSpeed);
	worldTransform_.translation_ = eye + railCameraSpeed;

	//ワールドトランスフォームのワールド行列再計算
	worldTransform_.UpdateMatrix();

	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = calculationMath_->Inverse(worldTransform_.matWorld_);
}
void RailCamera::Draw() {
	for (size_t i = 0; i < segmentCount; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], {1.0f, 0.0f, 0.0f, 1.0f});
	}
}

Vector3 RailCamera::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}