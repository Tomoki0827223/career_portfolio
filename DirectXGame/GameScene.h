#pragma once
#include "BIt_Map_Font.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"

#include "Experience.h"
#include <random>
#include <vector>

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

	Model* expModel_ = nullptr; // 追加: 経験値用のモデル

	Graph* graph_ = nullptr;

	BIt_Map_Font* font_ = nullptr;
	int score_ = 0;

	Sprite* exp_ = nullptr;
	uint32_t expTextureHandle_ = 0;

	// プレイヤー位置を引数に追加
	void UpdateExperiences(const Vector3& playerPos);
	void DrawExperiences3D();
	void DrawExperiences2D();
	void SpawnExperiences(int count, int textureHandle, Model* model);
};

// Extern宣言は削除、カメラはDraw関数に引数で渡すように変更