#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Player {
public:
	~Player();

	void Initialize();

	void Update();

	// 描画には外部からカメラを渡す
	void Draw(Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }

private:
	Model* modelPlayer_ = nullptr;
	Input* input_ = nullptr;

	// Camera camera_; // この行を削除

	WorldTransform worldTransform;
};