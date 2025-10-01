#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Experience {
public:
	// コンストラクタで位置を設定できるようにする
	Experience(const Vector3& initialPos);
	~Experience();

	void Initialize();
	void Update();
	void Draw(const Camera& camera);
	
	// アイテムの位置を取得するgetter（衝突判定などに使う）
	Vector3 GetPosition() const { return worldTransform.translation_; }

	// プレイヤーの位置を目標地点として設定する
	void SetTarget(const Vector3& targetPos) {
		targetPosition_ = targetPos;
		isAttracted_ = true; // 吸引状態に切り替える
	}

	// 取得されたかどうかのフラグ
	bool IsDead() const { return isDead_; }

	// 吸引されているかどうかのフラグを取得するGetter (追加)
	bool IsAttracted() const { return isAttracted_; }

private:

	Model* model_ = nullptr;
	WorldTransform worldTransform;

	// 必要に応じて経験値の量などのメンバを追加

	// 状態管理用
	Vector3 targetPosition_ = {}; // プレイヤーの位置
	bool isAttracted_ = false;    // 吸引されているか
	bool isDead_ = false;         // 取得されたか

	const float ATTRACTION_SPEED = 0.5f; // 吸引速度
};