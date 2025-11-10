// Particle.h の修正
#pragma once
#include "3d/Camera.h" // ★追加
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

using namespace KamataEngine;

class Particle {
public:
	Particle() = default;
	~Particle() = default;

	void Initialize(const Vector3& position, const Vector3& velocity, const Vector4& color, int lifeTime);

	void Update();

	/// <summary>
	/// 描画処理：PrimitiveDrawerで小さなキューブとして描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了したかを取得
	/// </summary>
	bool IsDead() const { return lifeTimer_ <= 0; }

private:
	WorldTransform worldTransform_{};
	Vector3 velocity_ = {0.0f, 0.0f, 0.0f};
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	int lifeTime_ = 0;
	int lifeTimer_ = 0;
	// 重力加速度 (Y軸方向)
	const float kGravity = -0.05f;
};