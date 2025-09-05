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

	void AddExperience(int exp); // 経験値を増やす関数
	int GetLevel() const;        // レベルを取得する関数
	int GetExperience() const;   // 経験値を取得する関数

	// プレイヤーの座標取得
	KamataEngine::Vector3 GetPosition() const;

private:
	Model* modelPlayer_ = nullptr;
	Input* input_ = nullptr;
	Camera camera_;
	WorldTransform worldTransform;
	int level_ = 1;
	int experience_ = 0;
	int experienceToNextLevel_ = 100; // 次のレベルまでに必要な経験値
};