#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class ExperienceItem {
public:
	ExperienceItem() = default;
	~ExperienceItem() = default;

	// アイテムの初期化に「サイズ係数」を追加
	void Initialize(const Vector3& position, float sizeFactor);

	// 更新処理
	void Update(const Vector3& playerPosition);

	// 描画処理
	void Draw(const Camera& camera);

	// ワールド座標を取得
	const Vector3& GetPosition() const { return worldTransform.translation_; }

	// 衝突判定に使うための半径
	float GetRadius() const { return radius_; }

	// 回収されたかどうかの状態
	bool IsCollected() const { return isCollected_; }
	void Collect() { isCollected_ = true; } // 回収処理

private:
	WorldTransform worldTransform;
	Model* model_ = nullptr;
	// ★ 以下の変数は固定値ではなく、初期化時に設定されるようにする
	float radius_ = 0.0f;          // 衝突判定用の半径
	float attractionRange_ = 0.0f; // プレイヤーが吸い寄せを開始する距離

	bool isCollected_ = false;

	// 吸い寄せ時の移動速度は固定とする
	const float attractionSpeed_ = 0.3f;
};