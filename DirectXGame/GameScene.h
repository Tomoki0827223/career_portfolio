#pragma once
#include "BIt_Map_Font.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"
#include "ExperienceItem.h"

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


	WorldTransform worldTransform;

	Stage* stage_ = nullptr;

	Player* player_ = nullptr;

	Model* playerModel_ = nullptr;

	Graph* graph_ = nullptr;

	BIt_Map_Font* font_ = nullptr;
	int score_ = 0;

	// 経験値アイテムのリストを追加
	std::list<ExperienceItem*> experienceItems_;

	// ★ カメラの制御に必要な変数を追加 (既にあればスキップ)
	// 今回はカメラ位置を直接操作することで対応します。
	
	Camera camera_;

	// カメラのオフセットや注視点設定用の変数
	Vector3 cameraTarget_ = {0.0f, 0.0f, 0.0f};
	Vector3 cameraOffset_ = {0.0f, 5.0f, -15.0f}; // プレイヤーから見てZ軸後方、Y軸上空に配置
};