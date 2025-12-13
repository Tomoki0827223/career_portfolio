#pragma once

#include "3d/Camera.h"
#include "math/MathUtility.h" // Lerpのために必要
#include "math/Vector3.h"

class Player;

using namespace KamataEngine;

class CameraController {
public:

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 30.0f;

	// インクラス初期化をfloat型で明示し、互換性を高める
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};
	Rect margin_ = {0.0f, 0.0f, 0.0f, 0.0f};
	Player* target_ = nullptr;

	void Initialize();
	void Update();
	void Reset();

	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	void SetMargin(const Rect& margin) { margin_ = margin; }
	void setTarget(Player* target) { target_ = target; }

	
	// ViewProjectionを返すメソッド
	const Camera& GetViewProjection() const { return camera_; }

private:
	
	Camera camera_;
	// ★修正点: targetOffset_の初期化を削除し、宣言のみにする
	Vector3 targetOffset_;
	Vector3 targetPosition_;
};