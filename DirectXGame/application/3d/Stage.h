#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Stage {
public:
	~Stage();
	void Initialize();
	void Update();
	void Draw(const Camera& camera); // 引数に Camera を追加

private:
	// 3Dモデルとして地面を持つ
	Model* model_ = nullptr;
	WorldTransform worldTransform_;
};