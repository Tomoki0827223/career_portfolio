#pragma once

#include "KamataEngine.h"
// 継承元となるCamera.hをインクルード
#include "3d/Camera.h"

// Playerクラスの定義がここでは不可能なため、前方宣言を使用します。
// 実際のファイルでは、Player.hをインクルードしてください。
class Player;

namespace KamataEngine {

	// ★追加: プレイヤーからのカメラのオフセットを定義 (X:0, Y:10, Z:-30) ★
const Vector3 kCameraOffset = {0.0f, 10.0f, -30.0f};

/// <summary>
/// プレイヤー追従カメラ
/// </summary>
class PlayerFollowCamera : public Camera {
public:
	PlayerFollowCamera() = default;
	~PlayerFollowCamera() = default;

	/// <summary>
	/// 初期化 (追従対象を設定)
	/// </summary>
	/// <param name="targetPlayer">追従するPlayerインスタンスへのポインタ</param>
	void Initialize(Player* targetPlayer);

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();

private:
	// 追従対象のプレイヤー
	Player* targetPlayer_ = nullptr;
};

} // namespace KamataEngine