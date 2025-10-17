#pragma once
#include "BIt_Map_Font.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"
#include "Experience.h" // 追加
#include <vector>       // 追加
#include <random>       // 追加
#include <algorithm>

using namespace KamataEngine;

class GameScene {
public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

private:
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	// 3Dモデルデータ
	Model* model_ = nullptr;

	Camera camera_;

	WorldTransform worldTransform;

	Stage* stage_ = nullptr;

	Player* player_ = nullptr;

	Model* playerModel_ = nullptr;

	Graph* graph_ = nullptr;

	BIt_Map_Font* font_ = nullptr;
	int score_ = 0;

	// 経験値アイテムを格納するベクトル (追加)
	std::vector<Experience*> experiences_;
	// 吸引範囲 (例: 10.0f)
	const float ATTRACTION_RADIUS = 10.0f;
};