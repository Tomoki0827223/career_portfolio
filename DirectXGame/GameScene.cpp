#include "GameScene.h"
#include "ExperienceItem.h"
#include "affine.h"
#include <random>

GameScene::~GameScene() {

	delete stage_;
	delete player_;
	// モデルとフォント、グラフを解放
	delete playerModel_;
	delete experienceItemModel_;
	delete model_;
	delete graph_;
	delete font_;

	// 経験値アイテムの削除処理
	for (ExperienceItem* item : experienceItems_) {
		delete item;
	}
	experienceItems_.clear();
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

	// ★ モデルロードと設定 (ダミーモデル "cube" を使用してクラッシュを回避)
	// ※ もしここでクラッシュが止まれば、元の "block_4" のモデルファイルに問題があります。
	playerModel_ = Model::CreateFromOBJ("cube");
	player_->SetModel(playerModel_);

	// ★ 経験値アイテム用のモデルロードと設定
	experienceItemModel_ = Model::CreateFromOBJ("cube");

	font_ = new BIt_Map_Font();
	font_->Initialize();

	// --- 経験値アイテムのランダム配置 (X/Yランダム, Z固定) ---
	const int kNumItems = 200;
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());

	// XとY座標の範囲を設定
	std::uniform_real_distribution<float> distPosX(-50.0f, 50.0f);
	std::uniform_real_distribution<float> distPosY(-30.0f, 30.0f);
	const float fixedZ = 0.0f;

	for (int i = 0; i < kNumItems; ++i) {
		ExperienceItem* item = new ExperienceItem();

		Vector3 randomPosition = {distPosX(engine), distPosY(engine), fixedZ};

		// ★ モデルと位置を設定
		item->SetModel(experienceItemModel_);
		item->Initialize(randomPosition);
		experienceItems_.push_back(item);
	}
	// --- 経験値アイテムのランダム配置 終了 ---
}

void GameScene::Update() {
	stage_->Update();

	player_->Update();

	// プレイヤーの位置を取得
	const Vector3& playerPos = player_->GetPosition();

	// ----------------------------------------------------
	// ★ カメラの更新処理 (最重要)
	// ----------------------------------------------------

	// 1. カメラの位置を計算 (プレイヤー位置 + オフセット)
	Vector3 newCameraPos = Subtract(playerPos, cameraOffset_);

	// 2. カメラのtranslation_を更新
	camera_.translation_ = newCameraPos;

	// 3. カメラの行列を更新し、GPUに転送
	camera_.UpdateMatrix();   // ビュー行列と射影行列を計算
	camera_.TransferMatrix(); // 計算した行列をGPUに送る

	// ----------------------------------------------------

	// アイテムの更新と衝突判定
	for (ExperienceItem* item : experienceItems_) {
		item->Update(playerPos);

		if (!item->IsCollected()) {
			// --- 衝突判定の処理 ---
			const Vector3& itemPos = item->GetPosition();

			Vector3 diff = Subtract(playerPos, itemPos);
			float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

			float combinedRadius = player_->GetRadius() + item->GetRadius();
			float collisionRadiusSquared = combinedRadius * combinedRadius;

			if (distanceSquared <= collisionRadiusSquared) {
				item->Collect();
				// スコア増加
				score_ += 100;
			}
			// --- 衝突判定の処理 終了 ---
		}
	}

	// 回収されたアイテムをリストから削除
	experienceItems_.remove_if([](ExperienceItem* item) {
		if (item->IsCollected()) {
			delete item;
			return true;
		}
		return false;
	});

	graph_->Update();
	font_->Set(score_);
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw(); // ★ モデル描画開始

	stage_->Draw();

	player_->Draw(camera_); // ★ プレイヤー描画 (GameSceneのカメラを使用)

	// 経験値アイテムの描画
	for (ExperienceItem* item : experienceItems_) {
		item->Draw(camera_); // ★ 経験値アイテム描画 (GameSceneのカメラを使用)
	}

	Model::PostDraw(); // ★ モデル描画終了

	graph_->Draw();
	font_->Draw();

	Sprite::PostDraw();
}
