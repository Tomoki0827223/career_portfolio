#include "GameScene.h"
#include <random>

GameScene::~GameScene() {
	delete gameLogic_; // ★★★ GameLogicの解放 ★★★
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete font_;
	delete hpBarBase_;
	delete hpBar_;

	// GameLogicへ移動したリストの解放処理は削除


	// スキル選択画面用スプライトの解放 (GameSceneに残すUI要素)
	delete skillScreenBackground_;
	delete skillCursorSprite_;
	for (int i = 0; i < 3; ++i) {
		delete skillOptionSprites_[i];
	}
}

void GameScene::Initialize() {
	// ... (既存の初期化処理はGameSceneに残るUI要素やSceneの初期化のみ)

	stage_ = new Stage();
	stage_->Initialize();
	stage_->Update();

	camera_.Initialize();
	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();
	font_->Initialize();

	// HPバーの初期化 (GameSceneに残すUI要素)
	hpBarBaseTexture_ = KamataEngine::TextureManager::Load("HP.png");
	hpBarTexture_ = KamataEngine::TextureManager::Load("HPR.png");
	const Vector2 kHpBarPos = {30.0f, 30.0f};
	const Vector2 kHpBarSize = {200.0f, 20.0f};

	hpBarBase_ = KamataEngine::Sprite::Create(hpBarBaseTexture_, kHpBarPos);
	hpBarBase_->SetSize(kHpBarSize);
	hpBar_ = KamataEngine::Sprite::Create(hpBarTexture_, kHpBarPos);
	hpBar_->SetSize(kHpBarSize);

	// スキル選択画面用スプライトの初期化 (GameSceneに残すUI要素)
	whiteTextureHandle_ = KamataEngine::TextureManager::Load("sample.png");
	skillScreenBackground_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	skillScreenBackground_->SetSize({1280.0f, 720.0f});
	skillScreenBackground_->SetColor({0.0f, 0.0f, 0.0f, 0.8f});

	const KamataEngine::Vector2 kOptionSize = {400.0f, 100.0f};
	const KamataEngine::Vector2 kBasePos = {440.0f, 180.0f};

	for (int i = 0; i < 3; ++i) {
		skillOptionSprites_[i] = KamataEngine::Sprite::Create(whiteTextureHandle_, {kBasePos.x, kBasePos.y + i * 120.0f});
		skillOptionSprites_[i]->SetSize(kOptionSize);
		skillOptionSprites_[i]->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	skillCursorSprite_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	skillCursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
	skillCursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f});

	// ★★★ GameLogicの生成と初期化 ★★★
	// GameLogicに依存オブジェクト (Player, Font, HPBar) を渡す
	gameLogic_ = new GameLogic(player_, font_, hpBar_, hpBarBase_);
	gameLogic_->Initialize();
}

void GameScene::Update() {

	// プレイヤーの更新 (GameSceneに残す)
	player_->Update();

	// HP/ゲームオーバー判定と処理 (GameSceneに残す)
	int currentHp = player_->GetCurrentHp();

	if (currentHp <= 0 && !isGameOver_) {
		isGameOver_ = true;
		player_->Die();
	}

	// ゲームオーバー中の処理 (GameSceneに残す)
	if (isGameOver_) {
		if (player_->IsDead() && player_->GetDeadTimer() > player_->GetMaxDeadTime()) {
			// シーン遷移ロジック
		}
		return;
	}

	// ------------------------------------
	// GameLogicの更新処理
	// ------------------------------------
	if (gameLogic_->IsLevelUpPending()) {
		gameLogic_->UpdateSkillSelection();
		// スキル選択中はGameLogicのUpdate()をスキップするが、UI描画のためにスコアの更新は行う
		// font_->Set(font_->Get()); // ★ 削除: Get()のビルドエラー回避とロジックの合理化 ★
		return;
	}

	stage_->Update();
	// ★★★ メインのゲームロジックと衝突判定を呼び出す ★★★
	gameLogic_->Update();
}

/**
 * @brief HPバーの描画処理 (GameSceneに残す)
 */
void GameScene::DrawHPBar() {
	hpBarBase_->Draw(); // ベース (枠)
	hpBar_->Draw();     // 現在HP (バー本体)
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 1. 3D描画のセットアップ
	Model::PreDraw();

	// 2. 3Dオブジェクトの描画
	stage_->Draw();
	player_->Draw();

	// ★★★ GameLogicが管理するオブジェクトの描画 ★★★
	gameLogic_->DrawObjects(camera_);

	// 3. 3D描画の終了
	Model::PostDraw();

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar(); // HPバーの描画

	// スキル選択画面の描画
	if (gameLogic_->IsLevelUpPending()) {
		// ★★★ GameLogicにUI要素を渡して描画させる ★★★
		gameLogic_->DrawSkillSelectionUI(skillCursorSprite_, skillOptionSprites_, skillScreenBackground_);
	}

	font_->Draw();

	// 6. 2D描画の終了
	Sprite::PostDraw();
}