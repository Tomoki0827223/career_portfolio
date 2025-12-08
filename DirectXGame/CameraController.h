#pragma once

#include "math/MathUtility.h"
#include "math/Vector3.h"
#include "3D/Camera.h"

class Player;

class CameraController {
public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f; // 補間率を設定
	static inline const float kVelocityBias = 30.0f;     // 速度バイアス

	Rect movableArea_ = {0, 100, 0, 100};
	Rect margin_ = {0, 0, 0, 0}; // 各方向へのマージンを追加
	Player* target_ = nullptr;

	void Initialize();
	void Update();
	void Reset();

	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	void SetMargin(const Rect& margin) { margin_ = margin; } // マージン設定メソッドを追加
	void setTarget(Player* target) { target_ = target; }

	const Camera& GetViewProjection() const { return camera_; }

private:
	Camera camera_;
	Vector3 targetOffset_ = {0, 0, -15.0f};
	Vector3 targetPosition_; // 目標座標を追加
};
