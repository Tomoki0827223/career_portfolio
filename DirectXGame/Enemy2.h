#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"

using namespace KamataEngine;

class Enemy2 { // Enemy2に名前を変更
public:
	Enemy2(const Vector3& position);
	~Enemy2();

	void Initialize();
	// プレイヤーの位置を受け取り、追尾・更新を行う
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);


	// 衝突判定/被弾判定用
	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }

	// ダメージ処理
	void TakeDamage(int damage);

	// ★ 追記: 弾発射用 ★
	bool CanShoot() const { return shotTimer_ >= kShotInterval; }
	void ResetShotTimer() { shotTimer_ = 0; }
	Vector3 GetShotPosition() const { return worldTransform.translation_; }

private:
	// 敵のステータス
	const float kMoveSpeed = 0.1f;
	const float radius_ = 1.0f;
	const int kMaxHp = 20; // ★ HPを4に変更 (4回攻撃で倒せるように)
	int currentHp_ = kMaxHp;

	// ★ 追記: 弾発射用のタイマーと定数 (連射型のため短く設定) ★
	int shotTimer_ = 0;
	const int kShotInterval = 45; // 0.75秒に1回発射 (60FPS想定で45フレーム)

	// モデルとワールド変換
	Model* model_ = nullptr;
	WorldTransform worldTransform;

	bool isDead_ = false; // HPが0になったらtrue

	// ★追加: 回転の滑らかさ (0.0f〜1.0f)
	const float kRotationLerpRate = 0.1f;
};