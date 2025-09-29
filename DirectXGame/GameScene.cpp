#include "GameScene.h"
#include "ExperienceItem.h"
#include "affine.h" // Transform関数などを使用するために必要かもしれない
#include <random>

GameScene::~GameScene() {
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;

	// 経験値アイテムの削除処理を追加
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
	playerModel_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();

	font_->Initialize();

	// --- 経験値アイテムのランダム配置 ---
	const int kNumItems = 50;
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());

	// ステージ範囲の仮設定
	std::uniform_real_distribution<float> distPos(-20.0f, 20.0f);
	// ★ サイズ係数のランダム生成を追加
	//   例: 0.5f から 2.0f の間でランダムなサイズにする
	std::uniform_real_distribution<float> distSize(0.5f, 2.0f);

	float itemHeight = 0.5f;

	for (int i = 0; i < kNumItems; ++i) {
		ExperienceItem* item = new ExperienceItem();

		Vector3 randomPosition = {distPos(engine), itemHeight, distPos(engine)};

		// ★ ランダムなサイズ係数を決定
		float sizeFactor = distSize(engine);

		// ★ Initializeに位置とサイズ係数を渡す
		item->Initialize(randomPosition, sizeFactor);
		experienceItems_.push_back(item);
	}
	// --- 経験値アイテムのランダム配置 終了 ---
}

void GameScene::Update() {
	stage_->Update();

	player_->Update();

	// --- 修正箇所: プレイヤーの位置をここで一度だけ取得する ---
	const Vector3& playerPos = player_->GetPosition();
	// --------------------------------------------------------

	// アイテムの更新と衝突判定
	for (ExperienceItem* item : experienceItems_) {
		// ★ここを修正: プレイヤーの位置を渡してアイテムを更新する
		item->Update(playerPos);

		if (!item->IsCollected()) {
			// --- 衝突判定の処理 ---
			// 以下の行は上で宣言済みのため削除！ (二重宣言のエラー/警告を防ぐ)
			// const Vector3& playerPos = player_->GetPosition();

			const Vector3& itemPos = item->GetPosition();

			Vector3 diff = Subtract(playerPos, itemPos);
			float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

			float combinedRadius = player_->GetRadius() + item->GetRadius();
			float collisionRadiusSquared = combinedRadius * combinedRadius;

			if (distanceSquared <= collisionRadiusSquared) {
				item->Collect();
				score_ += 100;
			}

			// --- 衝突判定の処理 終了 ---
		}
	}

	// 回収されたアイテムをリストから削除 (メモリリーク防止)
	experienceItems_.remove_if([](ExperienceItem* item) {
		if (item->IsCollected()) {
			delete item;
			return true;
		}
		return false;
	});

	graph_->Update();

	// score_++; // アイテム回収でしかスコアが増えないようにする場合はコメントアウト
	font_->Set(score_);
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw();

	stage_->Draw();

	player_->Draw();

	// 経験値アイテムの描画
	for (ExperienceItem* item : experienceItems_) {
		item->Draw(camera_);
	}

	graph_->Draw();

	font_->Draw();

	Model::PostDraw();
	Sprite::PostDraw();
}