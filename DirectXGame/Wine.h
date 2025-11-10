#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Wine {
public:
	// 【修正】コンストラクタはデフォルトのみに統一
	Wine() = default;
	// 【修正】デストラクタはインライン定義
	~Wine() { delete model_; }

	void Update(); // 引数なし
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform_.translation_; }
	float GetRadius() const { return radius_; }

	int GetHealAmount() const { return kHealAmount; }

	// 初期位置を渡すInitialize関数
	void Initialize(const Vector3& initialPos);

	bool IsActive() const { return isActive_; }
	void SetIsActive(bool isActive) { isActive_ = isActive; }

private:
	const float radius_ = 0.5f;
	const int kHealAmount = 50; // HP回復量

	Model* model_ = nullptr;
	WorldTransform worldTransform_{}; // ★worldTransform_の定義を追加/統一★

	// 【追加】落下速度
	float dropSpeed_ = 0.5f;
	// 【追加】アクティブフラグ
	bool isActive_ = false;
	// 【追加】地面のY座標 (ステージに合わせて調整してください)
	const float kGroundY = 0.0f;
};