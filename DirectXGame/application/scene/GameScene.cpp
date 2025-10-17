#include "GameScene.h"

GameScene::~GameScene() {
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;

	// 経験値アイテムの解放 (追加)
	for (Experience* exp : experiences_) {
		delete exp;
	}
	experiences_.clear();
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

    // プレイヤーの初期化
	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();

	font_->Initialize();

	// 経験値アイテムのランダム配置 (追加)
	const int ITEM_COUNT = 200;
	const float MAP_HALF_RANGE = 50.0f; // 例: -50.0から+50.0の範囲に配置

	// C++11以降のランダム数生成器を使用 (より高品質な乱数)
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	// -MAP_HALF_RANGE から +MAP_HALF_RANGE の範囲で均等な乱数を生成する
	std::uniform_real_distribution<float> dist(-MAP_HALF_RANGE, MAP_HALF_RANGE);

	for (int i = 0; i < ITEM_COUNT; ++i) {
		// x, yをランダムに生成し、zは0.0f（固定）に設定
		Vector3 randomPos = {dist(engine), dist(engine), 0.0f};

		Experience* newExp = new Experience(randomPos);
		newExp->Initialize();
		experiences_.push_back(newExp); // 生成したアイテムをベクトルに追加
	}
}

void GameScene::Update() {
	stage_->Update();
	player_->Update();
	graph_->Update();
	// score_++; // スコア加算は経験値取得時のみに変更する
	font_->Set(score_);

	Vector3 playerPos = player_->GetPosition(); // プレイヤーの位置を取得

	// 経験値アイテムの更新と処理
	for (Experience* exp : experiences_) {
		// 吸引されていない場合のみ、距離チェックを行う
		if (!exp->IsAttracted()) {
			// プレイヤーとアイテムの距離を計算
			Vector3 diff = exp->GetPosition() - playerPos;
			float distance = Math::Length(diff);

			// 吸引範囲内の場合
			if (distance <= ATTRACTION_RADIUS) {
				exp->SetTarget(playerPos); // プレイヤーを目標地点に設定し、吸引状態にする
			}
		}

		// 各アイテムのUpdateを呼ぶ (吸引移動の処理)
		exp->Update();
	}

	// --- アイテムの削除処理 (取得/死亡判定) ---
	// 取得済み(isDead_ == true)のアイテムをリストから削除する
	experiences_.erase(
	    std::remove_if(
	        experiences_.begin(), experiences_.end(),
	        [this](Experience* exp) {
		        if (exp->IsDead()) {
			        // 取得されたらスコアを加算
			        score_ += 1; // 経験値アイテム1つでスコア1加算

			        delete exp;  // メモリを解放
			        return true; // 削除対象
		        }
		        return false; // 削除しない
	        }),
	    experiences_.end());

	// TODO: ここにプレイヤーと経験値アイテムの衝突判定とアイテム消滅のロジックを追加する
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw();

	stage_->Draw();

	player_->Draw();

	// 経験値アイテムの描画 (追加)
	for (Experience* exp : experiences_) {
		// GameSceneのカメラを渡す (この呼び出しがExperience.hの修正により正しくなる)
		exp->Draw(camera_);
	}

	graph_->Draw();

	font_->Draw();

	Model::PostDraw();
	Sprite::PostDraw();
}