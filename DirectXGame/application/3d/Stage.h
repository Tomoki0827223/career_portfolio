#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Stage {
public:
	~Stage();
	void Initialize();
	// プレイヤーの位置を受け取って、床をずらす
	void Update(const Vector3& playerPos);
	void Draw(const Camera& camera);

private:
	Model* model_ = nullptr;

	// 上下左右に無限スクロールするため、3x3 = 9枚の床を用意
	static const int kFloorCountX = 3;
	static const int kFloorCountY = 3;
	WorldTransform worldTransforms_[kFloorCountX][kFloorCountY];

	// 床1枚のサイズ（60m x 60m と仮定）
	const float kFloorSize = 60.0f;
};