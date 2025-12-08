#include "CameraController.h"
#include "Player.h"
#include <algorithm>
#include <iostream>

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {
	if (!target_) {
		return;
	}

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
	camera_.translation_ = Lerp(camera_.translation_, targetPosition_, kInterpolationRate);

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
