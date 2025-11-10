#include "Particle.h"
#include "3d/PrimitiveDrawer.h"
#include "math/Vector4.h" // Vector4の定義が必要

void Particle::Initialize(const Vector3& position, const Vector3& velocity, const Vector4& color, int lifeTime) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.1f, 0.1f, 0.1f}; // 小さなサイズで初期化
	velocity_ = velocity;
	color_ = color;
	lifeTime_ = lifeTime;
	lifeTimer_ = lifeTime;
	worldTransform_.UpdateMatarix();
}

void Particle::Update() {
	if (lifeTimer_ > 0) {
		// 重力適用: 速度に重力を加える
		velocity_.y += kGravity;

		// 位置更新: 速度で位置を移動させる
		worldTransform_.translation_ += velocity_;

		// 描画をフェードアウトさせる（寿命が尽きるにつれて透明に）
		float t = (float)lifeTimer_ / lifeTime_; // 1.0から0.0へ
		color_.w = t;                            // アルファ値（透明度）に適用

		// スケールを徐々に小さくする
		worldTransform_.scale_ = {t * 0.1f, t * 0.1f, t * 0.1f};

		worldTransform_.UpdateMatarix();

		lifeTimer_--;
	}
}

void Particle::Draw() {
	if (lifeTimer_ > 0) {
		// 1. パーティクルの中心位置
		Vector3 p1 = worldTransform_.translation_;
		// 2. 線分の終点
		Vector3 p2 = p1;

		float size = worldTransform_.scale_.y;
		p2.y += size * 5.0f;

		KamataEngine::PrimitiveDrawer::GetInstance()->DrawLine3d(p1, p2, color_);
	}
}