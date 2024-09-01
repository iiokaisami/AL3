#include "FollowCamera.h"
#include "Input.h"
#include "ImGuiManager.h"


FollowCamera::~FollowCamera() {
	delete calculationMath_;
}

void FollowCamera::Initialize() {
	viewProjection_.Initialize();
	viewProjection_.rotation_ = {0, 0, 0};
	calculationMath_ = new CalculationMath;
}

void FollowCamera::Update() {
	// 追従
	Follow();

	// 旋回
	Turn();
}

void FollowCamera::Follow() {

	// 追従対象がいれば
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 5.0f, -20.0f};

		// 回転行列を求める
		Matrix4x4 rotateXMatrix = calculationMath_->MakeRotateXMatrix(viewProjection_.rotation_.x); // X軸の回転
		Matrix4x4 rotateYMatrix = calculationMath_->MakeRotateYMatrix(viewProjection_.rotation_.y); // Y軸の回転
		Matrix4x4 rotateZMatrix = calculationMath_->MakeRotateZMatrix(viewProjection_.rotation_.z); // Z軸の回転
		Matrix4x4 rotateMatrix = calculationMath_->Multiply(rotateXMatrix, calculationMath_->Multiply(rotateYMatrix, rotateZMatrix));

		// オフセットをカメラの回転に合わせて回転させる
		offset = calculationMath_->TransformNormal(offset, rotateMatrix);

		// 座標をコピーしてオフセット文ずらす
		viewProjection_.translation_ = target_->translation_ + offset;
	}

	// ビュー行列の更新と転送
	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();
}

void FollowCamera::Turn() {
	// ジョイスティックの状態
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float rotationSpeed = 0.000001f;

		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX * rotationSpeed;
		viewProjection_.rotation_.x -= (float)joyState.Gamepad.sThumbRY * rotationSpeed;
	}
}