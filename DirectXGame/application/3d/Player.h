#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	~Player();

	void Initialize();

	void Update();

	void Draw();

	// プレイヤーの位置を取得するためのgetter
	Vector3 GetPosition() const { return worldTransform.translation_; }

	// HP関連 (追加)
	int GetCurrentHp() const { return currentHp_; }       // 現在HPを取得
	int GetMaxHp() const { return kMaxHp_; }              // 最大HPを取得
	void TakeDamage(int damage) { currentHp_ -= damage; } // ダメージを受ける
	void Heal(int amount);                                // ★修正: HP回復メソッドを追加 ★

	// 攻撃関連 (追加)
	bool IsAttacking() const { return isAttacking_; }        // 攻撃中か
	float GetAttackRadius() const { return kAttackRadius_; } // 近接攻撃の判定半径

	// ★追加: スキルレベルのGetter/Setter ★
	int GetBookLevel() const { return bookLevel_; }
	void SetBookLevel(int level) { bookLevel_ = level; }
	int GetBulletLevel() const { return bulletLevel_; }
	void SetBulletLevel(int level) { bulletLevel_ = level; }
	int GetWineLevel() const { return wineLevel_; }
	void SetWineLevel(int level) { wineLevel_ = level; }
	// ------------------------------------

	// ★ 死亡関連 (修正) ★
	void Die() { isDead_ = true; }          // 死亡フラグを立てる
	bool IsDead() const { return isDead_; } // 死亡したかを取得
	// ★ 【追加】死亡タイマーと最大時間のgetter ★
	int GetDeadTimer() const { return deadTimer_; }
	int GetMaxDeadTime() const { return kMaxDeadTime_; }

private:
	// パーティクル3Dモデルデータ
	Model* modelPlayer_ = nullptr;

	Input* input_ = nullptr;

	Camera camera_;

	WorldTransform worldTransform;

	// HP (追加)
	const int kMaxHp_ = 100; // 最大HPを10に設定
	int currentHp_ = kMaxHp_;

	// 攻撃関連 (追加)
	bool isAttacking_ = false;
	int attackTimer_ = 0;              // 攻撃の持続フレーム
	const int kMaxAttackTime_ = 10;    // 攻撃が持続するフレーム数
	const float kAttackRadius_ = 1.5f; // 近接攻撃の判定半径

	// ★ 死亡関連 (追加) ★
	bool isDead_ = false;         // 死亡フラグ
	int deadTimer_ = 0;           // 死亡モーションのタイマー
	const int kMaxDeadTime_ = 60; // 死亡モーションの総フレーム数 (1秒間)

	// ★追加: 新しいスキルレベル ★
	int bookLevel_ = 0;
	int bulletLevel_ = 0;
	int wineLevel_ = 0;
	// ------------------------------------
};