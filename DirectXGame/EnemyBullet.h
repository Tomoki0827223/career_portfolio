#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"

using namespace KamataEngine;

class EnemyBullet {
public:
	// 弾の位置と速度（方向×速さ）を設定
	EnemyBullet(const Vector3& position, const Vector3& velocity);
	~EnemyBullet();

	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	// 衝突判定用ゲッター
	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void SetIsDead(bool dead) { isDead_ = dead; }
	// ★ 修正: ダメージを取得するGetterを追加 (currentDamage_を参照) ★
	int GetDamage() const { return currentDamage_; }
	// ★ 追記: ダメージを設定するSetterを追加 ★
	void SetDamage(int damage) { currentDamage_ = damage; }

private:
	const float radius_ = 0.5f; // 弾の半径
	// const int kDamage = 1;       // ★ 削除またはprivateのまま使用しない ★
	int currentDamage_ = 1;         // ★ 追記: 現在のダメージ (GameLogicから設定可能にする) ★
	const float kLifeTime = 180.0f; // 3秒で消滅 (60FPS想定)

	Vector3 velocity_; // 速度ベクトル
	Model* model_ = nullptr;
	WorldTransform worldTransform;
	bool isDead_ = false; // 削除フラグ
	int lifeTimer_ = 0;
};