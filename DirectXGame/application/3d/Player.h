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
	const WorldTransform& GetWorldTransform() const { return worldTransform_; } // メンバ変数名に合わせて適宜修正
	const Vector3& GetVelocity() const { return velocity_; }                    // メンバ変数名に合わせて適宜修正

	// HP関連 (追加)
	int GetCurrentHp() const { return currentHp_; }       // 現在HPを取得
	int GetMaxHp() const { return kMaxHp_; }              // 最大HPを取得
	void TakeDamage(int damage) { currentHp_ -= damage; } // ダメージを受ける
	void Heal(int amount);                                // ★修正: HP回復メソッドを追加 ★

	// 攻撃関連 (追加)
	bool IsAttacking() const { return isAttacking_; }        // 攻撃中か
	float GetAttackRadius() const { return kAttackRadius_; } // 近接攻撃の判定半径
	int GetDefense() const { return defense_; }
	void SetDefense(int defense) { defense_ = defense; }

	// ★追加: スキルレベルのGetter/Setter ★
	int GetBookLevel() const { return bookLevel_; }
	void SetBookLevel(int level) { bookLevel_ = level; }
	int GetBulletLevel() const { return bulletLevel_; }
	void SetBulletLevel(int level) { bulletLevel_ = level; }
	int GetWineLevel() const { return wineLevel_; }
	void SetWineLevel(int level) { wineLevel_ = level; }
	int GetBoomerangLevel() const { return boomerangLevel_; }
	void SetBoomerangLevel(int level) { boomerangLevel_ = level; }
	int GetMinionLevel() const { return minionLevel_; }
	void SetMinionLevel(int level) { minionLevel_ = level; }
	int GetMissileLevel() const { return missileLevel_; }
	void SetMissileLevel(int level) { missileLevel_ = level; }
	// ------------------------------------

	// ★ 死亡関連 (修正) ★
	void Die() { isDead_ = true; }          // 死亡フラグを立てる
	bool IsDead() const { return isDead_; } // 死亡したかを取得
	// ★ 【追加】死亡タイマーと最大時間のgetter ★
	int GetDeadTimer() const { return deadTimer_; }
	int GetMaxDeadTime() const { return kMaxDeadTime_; }

	// ★追加: スキル選択中かどうかを設定するsetter ★
	void SetIsSkillSelecting(bool isSelecting) { isSkillSelecting_ = isSelecting; }

	// ★追加: Audioインスタンスと攻撃SEハンドルを設定するメソッド ★
	void SetAudio(Audio* audio, uint32_t seHandle) {
		audio_ = audio;
		attackSeHandle_ = seHandle;
	}

private:

	// ★追加: Audio関連 ★
	Audio* audio_ = nullptr;
	uint32_t attackSeHandle_ = 0;

	// パーティクル3Dモデルデータ
	Model* modelPlayer_ = nullptr;

	Input* input_ = nullptr;

	Camera camera_;

	WorldTransform worldTransform;

	// HP (追加)
	const int kMaxHp_ = 350; // 最大HPを10に設定
	int currentHp_ = kMaxHp_;

	int defense_ = 0;

	// 攻撃関連 (追加)
	bool isAttacking_ = false;
	int attackTimer_ = 0;              // 攻撃の持続フレーム
	const int kMaxAttackTime_ = 10;    // 攻撃が持続するフレーム数
	const float kAttackRadius_ = 1.5f; // 近接攻撃の判定半径

	// ★ 死亡関連 (追加) ★
	bool isDead_ = false;         // 死亡フラグ
	int deadTimer_ = 0;           // 死亡モーションのタイマー
	const int kMaxDeadTime_ = 60; // 死亡モーションの総フレーム数 (1秒間)

	bool isSkillSelecting_ = false;

	// ★追加: 新しいスキルレベル ★
	int bookLevel_ = 0;
	int bulletLevel_ = 0;
	int wineLevel_ = 0;
	// ------------------------------------

	// ★★★ 新規追加するスキルレベルのメンバ変数 ★★★
	int boomerangLevel_ = 0;
	int minionLevel_ = 0;
	int missileLevel_ = 0;

	WorldTransform worldTransform_; // これらのメンバ変数がprivateにあると仮定
	Vector3 velocity_;              // これらのメンバ変数がprivateにあると仮定
};