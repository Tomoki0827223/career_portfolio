#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Experience {
public:
	// 【修正】GameSceneから参照するための静的定数をpublic宣言
	static const float ATTRACTION_RADIUS;      // 吸引開始半径
	static const float ATTRACTION_SPEED;       // 吸引速度
	static const float INITIAL_VELOCITY_SCALE; // 初速の最大スケール

	Experience(const Vector3& initialPos);
	~Experience();

	void Initialize();
	// 【修正】Update関数にプレイヤーの位置を引数として追加
	void Update(const Vector3& playerPos);
	void Draw(const Camera& camera);

	// アイテムの位置を取得するgetter（衝突判定などに使う）
	Vector3 GetPosition() const { return worldTransform.translation_; }

	// プレイヤーの位置を目標地点として設定する
	void SetTarget(const Vector3& targetPos) {
		targetPosition_ = targetPos;
		isAttracted_ = true; // 吸引状態に切り替える
	}

	// 【追加】GameSceneから吸引状態に切り替えるためのセッター
	void SetIsAttracted(bool isAttracted) { isAttracted_ = isAttracted; }

	// 取得されたかどうかのフラグ
	bool IsDead() const { return isDead_; }

	// 吸引されているかどうかのフラグを取得するGetter
	bool IsAttracted() const { return isAttracted_; }

private:
	Model* model_ = nullptr;
	WorldTransform worldTransform;

	// 状態管理用
	Vector3 targetPosition_ = {}; // プレイヤーの位置
	bool isAttracted_ = false;    // 吸引されているか
	bool isDead_ = false;         // 取得されたか

	// 【追加】散らばり処理用メンバ
	Vector3 scatterVelocity_ = {};
	int scatterTimer_ = 0;
	const int SCATTER_TIME = 15; // 散らばりモーションの継続フレーム数
};