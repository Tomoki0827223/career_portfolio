#include "CameraController.h"
#include "Player.h"
#include <algorithm>
#include <iostream>

// Lerp、Camera、Vector3などが定義されているKamataEngine名前空間を開きます。
using namespace KamataEngine; // 他の型（Vector3、WorldTransformなど）のために残します

void CameraController::Initialize() {
	camera_.Initialize();
	// ★修正点: 初期化をVector3コンストラクタを使用して行う (C3079対策)
	targetOffset_ = Vector3(0.0f, 0.0f, -15.0f);

}

void CameraController::Update() {
	if (!target_) {
		return;
	}

	// WorldTransformとVector3の型がPlayer.hで正しく定義されていることが前提
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットから目標座標を計算
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	// 追従対象が画面外に出ないように補正
	targetPosition_.x = (std::max)(targetPosition_.x, targetWorldTransform.translation_.x - margin_.left);
	targetPosition_.x = (std::min)(targetPosition_.x, targetWorldTransform.translation_.x + margin_.right);
	targetPosition_.y = (std::max)(targetPosition_.y, targetWorldTransform.translation_.y - margin_.bottom);
	targetPosition_.y = (std::min)(targetPosition_.y, targetWorldTransform.translation_.y + margin_.top);

	// 座標補間によりゆったり追従
	// ★FIX: Vector3用のLerp関数が見つからないため、float用のLerpで各成分を個別に補間します。
	camera_.translation_.x = KamataEngine::MathUtility::Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
	camera_.translation_.y = KamataEngine::MathUtility::Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);
	camera_.translation_.z = KamataEngine::MathUtility::Lerp(camera_.translation_.z, targetPosition_.z, kInterpolationRate);

	// 移動範囲制限
	camera_.translation_.x = (std::max)(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = (std::min)(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = (std::max)(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = (std::min)(camera_.translation_.y, movableArea_.top);

	// 行列を更新する
	camera_.UpdateMatrix();

	// デバッグ出力
	std::cerr << "Camera Position: (" << camera_.translation_.x << ", " << camera_.translation_.y << ", " << camera_.translation_.z << ")" << std::endl;
}

void CameraController::Reset() {
	if (!target_) {
		return;
	}

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	camera_.UpdateMatrix();
}