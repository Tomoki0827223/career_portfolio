#pragma once
#include "KamataEngine.h"
#include "algorithm"

using namespace KamataEngine;

class Particle {
public:
	// 初期化
	// ★修正: ダメージと Wine Particle モデルを追加
	void Initialize(Model* model, Vector3 position, Vector3 velocity, int damage, bool isWineParticle = false);
	// 更新
	void Update();
	// 描画
	void Draw(Camera* camera);

	bool isFinished_ = false;     // 終了フラグ
	float counter_ = 0.0f;        // カウンター
	const float kDuration = 1.0f; // 寿命

	// デスフラグのゲッター
	bool IsFinished() const { return isFinished_; }

	// ★追記: Wine Particle 用のゲッター
	bool IsWineParticle() const { return isWineParticle_; }
	int GetDamage() const { return damage_; }
	float GetRadius() const { return worldTransform_.scale_.x; } // スケールを半径として利用

	// ★★★ 追記: 位置を取得するためのゲッター ★★★
	Vector3 GetPosition() const { return worldTransform_.translation_; }

private:
	WorldTransform worldTransform_; // ワールド変形
	Model* model_ = nullptr;        // モデル

	ObjectColor objectcolor_;
	Vector4 color_;
	Vector3 velocity_; // 速度

	// ★追記: ダメージと Wine Particle フラグ
	int damage_ = 0;
	bool isWineParticle_ = false;
};