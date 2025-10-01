#include "GameScene.h"
#include "ExperienceItem.h"
#include "affine.h" // Transform関数などを使用するために必要かもしれない
#include <random>

GameScene::~GameScene() {

	delete stage_;
	delete player_;
	delete playerModel_;         // ★ 修正: GameSceneで解放する
	delete experienceItemModel_; // ★ 追加: 経験値アイテムモデルを解放する
	delete model_;

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
	// camera_.Initialize(); // ★ 修正: camera_ が実体になったためドット演算子を使用
	camera_.Initialize(); // C2228エラーの解消

	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();

	// ★ 修正: ここでモデルをロードし、Playerに設定する
	playerModel_ = Model::CreateFromOBJ("cube");
	player_->SetModel(playerModel_);

	// ★ 追加: 経験値アイテム用のモデルをロードする
	experienceItemModel_ = Model::CreateFromOBJ("cube"); // 同じモデルを使用

	font_ = new BIt_Map_Font();

	font_->Initialize();

	// --- 経験値アイテムのランダム配置 (X/Yランダム, Z固定) ---
	const int kNumItems = 200;
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());

	// ★ XとY座標の範囲を設定
	std::uniform_real_distribution<float> distPosX(-50.0f, 50.0f); // X軸の範囲
	std::uniform_real_distribution<float> distPosY(-30.0f, 30.0f); // Y軸の範囲

	// ★ Z座標を固定値に設定
	const float fixedZ = 0.0f;

	// ★ サイズ係数のランダム生成は不要なので削除

	for (int i = 0; i < kNumItems; ++i) {
		ExperienceItem* item = new ExperienceItem();

		// ★ Z軸を固定し、Y座標にはランダムな値を設定
		Vector3 randomPosition = {distPosX(engine), distPosY(engine), fixedZ};

		// ★ Initializeに位置のみを渡す
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
    // ★ カメラ追従ロジックの修正
    // ----------------------------------------------------
    
    // 1. カメラの注視点(ターゲット)をプレイヤーの位置に設定
	cameraTarget_ = playerPos; 

    // 2. カメラの位置を計算 (プレイヤー位置 + オフセット)
    //    *注: カメラを常にプレイヤーの後方上空に固定するシンプルな追従
	Vector3 newCameraPos = Subtract(playerPos, cameraOffset_);
    
    // 3. カメラオブジェクトの translation_ を更新し、行列を再計算
    //    - translation_ を直接更新します。
    //    - LookAt行列を再計算するために UpdateViewMatrix() を呼び出します。

    camera_.translation_ = newCameraPos;
    
    // カメラのtranslation_が更新されたので、ビュー行列を更新
    // Camera.hで確認されたメンバー関数を使用
    camera_.UpdateViewMatrix(); 
    camera_.UpdateMatrix(); // Matrix4x4を定数バッファに転送する前の最終更新 (Camera.hに存在)
    
    // ----------------------------------------------------


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
	Model::PreDraw(); // ★ モデル描画開始

	stage_->Draw();

	player_->Draw(camera_); // ★ プレイヤー描画

	// 経験値アイテムの描画
	for (ExperienceItem* item : experienceItems_) {
		item->Draw(camera_); // ★ 経験値アイテム描画
	}

	graph_->Draw();

	font_->Draw();

	Model::PostDraw();
	Sprite::PostDraw();
}