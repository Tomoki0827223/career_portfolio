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

	// void ParticleBorn(Vector3 position);

	// プレイヤーの位置を取得するゲッターを追加
	const KamataEngine::Vector3& GetPosition() const { return worldTransform.translation_; }

	// 衝突判定に使うための半径を取得するゲッターを追加
	float GetRadius() const { return radius_; }

private:
	// パーティクル3Dモデルデータ
	Model* modelPlayer_ = nullptr;

	Input* input_ = nullptr;

	Camera camera_;

	WorldTransform worldTransform;
	float radius_ = 1.0f; // プレイヤーの半径 (適当な値)
};