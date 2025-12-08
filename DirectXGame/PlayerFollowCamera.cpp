// PlayerFollowCamera.cpp

#include "PlayerFollowCamera.h"
// ★修正: インクルードパスが前回修正した正しいものになっているか確認してください ★
#include "application/3d/Player.h"

using namespace KamataEngine;

void PlayerFollowCamera::Initialize(Player* targetPlayer) {
	// 追従対象のプレイヤーを保持
	targetPlayer_ = targetPlayer;

	// 継承元のCamera::Initialize()を呼び出し、行列や定数バッファを初期化
	Camera::Initialize();

	// ★修正: カメラの初期位置をプレイヤー位置 + オフセットに設定 ★
	if (targetPlayer_) {
		Vector3 playerPos = targetPlayer_->GetPosition();
		this->translation_ = playerPos + kCameraOffset;
	}

	Camera::UpdateMatrix();
}

void PlayerFollowCamera::Update() {
	// 追従対象が有効であるか確認
	if (targetPlayer_) {
		// プレイヤーのワールド座標を取得
		Vector3 playerPos = targetPlayer_->GetPosition();

		// ★★★ プレイヤー追従ロジックをオフセット計算に修正 ★★★
		// プレイヤーの位置に定数オフセットを加算し、カメラのワールド座標とする
		this->translation_ = playerPos + kCameraOffset;

		// 継承元のCamera::UpdateMatrix()を呼び出し、ビュー行列を再計算します。
		Camera::UpdateMatrix();
	}
}