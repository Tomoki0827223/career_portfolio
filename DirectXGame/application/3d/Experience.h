#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Experience {
public:
	// 【修正なし】GameSceneから参照するための静的定数をpublic宣言
	static const float ATTRACTION_RADIUS;      // 吸引開始半径
	static const float ATTRACTION_SPEED;       // 吸引速度
	static const float INITIAL_VELOCITY_SCALE; // 初速の最大スケール

	Experience(const Vector3& initialPos);
	~Experience();

	void Initialize();
	// Update関数にプレイヤーの位置を引数として追加
	void Update(const Vector3& playerPos);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }

	// プレイヤーの位置を目標地点として設定する (吸引フラグは設定しない)
	void SetTarget(const Vector3& targetPos) {
		targetPosition_ = targetPos;
		// isAttracted_ = true; // 【削除】この行を削除
	}

	// 【削除】SetIsAttracted関数を削除

	bool IsDead() const { return isDead_; }
	bool IsAttracted() const { return isAttracted_; }
	void StartAttraction() { isAttracted_ = true; } // 吸い込み開始フラグ

private:

	Model* model_ = nullptr;
	WorldTransform worldTransform;

	Vector3 targetPosition_ = {};
	bool isAttracted_ = false; // ★ これ1行だけにする！
	bool isDead_ = false;

	// 散らばり処理用メンバ
	Vector3 scatterVelocity_ = {};
	int scatterTimer_ = 0;
	const int SCATTER_TIME = 15; // 散らばりモーションの継続フレーム数 (例: 15フレーム)

	float attractionSpeed_ = 0.0f; // 徐々に速くするための変数
};