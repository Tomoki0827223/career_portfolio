#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"

using namespace KamataEngine;

class Enemy4 {
public:
	Enemy4(const Vector3& position);
	~Enemy4();

	void Initialize();
	// プレイヤーの位置を受け取り、追尾・更新を行う
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	// 衝突判定/被弾判定用
	Vector3 GetPosition() const { return worldTransform.translation_; } // ★ 修正: GetPositionを追加 ★
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }

	// ダメージ処理
	void TakeDamage(int damage);

	// 弾発射用
	bool CanShoot() const { return shotTimer_ >= kShotInterval; }           // ★ 修正: 弾発射判定を追加 ★
	void ResetShotTimer() { shotTimer_ = 0; }                               // ★ 修正: タイマーリセットを追加 ★
	Vector3 GetShotPosition() const { return worldTransform.translation_; } // ★ 修正: 発射位置を追加 ★

private:
	// 敵のステータス (Enemy3と同様のステータスを使用)
	const float kMoveSpeed = 0.1f;
	const float radius_ = 1.0f;
	const int kMaxHp = 10;
	int currentHp_ = kMaxHp;

	// ★ 追記: 弾発射用のタイマーと定数 ★
	int shotTimer_ = 0;
	const int kShotInterval = 120; // 2秒に1回発射 (60FPS想定)

	// モデルとワールド変換
	Model* model_ = nullptr;
	WorldTransform worldTransform;

	bool isDead_ = false; // HPが0になったらtrue

	// ★追加: 回転の滑らかさ (0.0f〜1.0f)
	const float kRotationLerpRate = 0.1f;
};