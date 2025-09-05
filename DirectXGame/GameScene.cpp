#include "GameScene.h"
#include "ExperienceOrb.h"
#include <random>
#include <cmath>


GameScene::~GameScene() {
	
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;

	for (auto& orb : experienceOrbs_) {
		delete orb;
	}
	experienceOrbs_.clear();
}

void GameScene::Initialize() {

	graph_ = new Graph();
	graph_->Initialize();

	stage_ = new Stage();
	stage_->Initialize();

	stage_->Update();

	// カメラの初期化
	camera_.Initialize();

	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();

	font_->Initialize();
}

void GameScene::Update() {

	stage_->Update();
	player_->Update();
	graph_->Update();

	score_++;
	font_->Set(score_);

	expSpawnTimer_ += 1.0f;     // 毎フレームタイマーを増やす

	if (expSpawnTimer_ >= 60) { // 60フレーム(1秒)ごとにスポーン
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		std::uniform_real_distribution<> dist(-10.0, 10.0); // ここを double に変更

		ExperienceOrb* newOrb = new ExperienceOrb();

		// dist(engine) を float に明示的にキャスト
		newOrb->Initialize({static_cast<float>(dist(engine)), static_cast<float>(dist(engine)), static_cast<float>(dist(engine))});

		experienceOrbs_.push_back(newOrb);
		expSpawnTimer_ = 0.0f;
	}

	// 経験値アイテムの更新と衝突判定
	auto it = experienceOrbs_.begin();
	while (it != experienceOrbs_.end()) {
		ExperienceOrb* orb = *it;
		orb->Update();

		// プレイヤーと経験値アイテムの距離を計算
		float distance = std::sqrt(
		    std::powf(player_->GetPosition().x - orb->GetPosition().x, 2.0f) + std::powf(player_->GetPosition().y - orb->GetPosition().y, 2.0f) +
		    std::powf(player_->GetPosition().z - orb->GetPosition().z, 2.0f));

		const float collisionDistance = 1.0f; // 衝突判定距離
		if (distance < collisionDistance) {
			player_->AddExperience(10); // 経験値を増やす
			orb->SetIsDead();           // 経験値アイテムを消滅させる
		}

		if (!orb->IsAlive()) {
			delete orb;
			it = experienceOrbs_.erase(it);
		} else {
			++it;
		}
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw();

	stage_->Draw();
	player_->Draw();
	graph_->Draw();
	font_->Draw();

	// 経験値アイテムを描画
	for (auto& orb : experienceOrbs_) {
		orb->Draw(camera_);
	}

	Model::PostDraw();
	Sprite::PostDraw();
}