#include "GameScene.h"
#include <random>
#include <list>

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator()); // メルセンヌツイスタの初期化
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

GameScene::~GameScene() {
	delete gameLogic_; // ★★★ GameLogicの解放 ★★★
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete font_;
	delete hpBarBase_;
	delete hpBar_;
	delete expBarBase_;
	delete expBar_;
	// GameLogicへ移動したリストの解放処理は削除


	// スキル選択画面用スプライトの解放 (GameSceneに残すUI要素)
	delete skillScreenBackground_;
	delete skillCursorSprite_;
	for (int i = 0; i < 3; ++i) {
		delete skillOptionSprites_[i];
	}
	
	// ★★★ 追記: 残っているパーティクルの解放 ★★★
	for (Particle* particle : particles_) {
		delete particle;
	}
	particles_.clear(); // リスト自体をクリア
}

void GameScene::Initialize() {
	// ... (既存の初期化処理はGameSceneに残るUI要素やSceneの初期化のみ)

	stage_ = new Stage();
	stage_->Initialize();
	stage_->Update();

	// プレイヤーとカメラコントローラーのインスタンスが存在し、CameraControllerが利用可能であると仮定
	CameraController* cameraController = new CameraController();
	Vector3 playerInitialPos = player_->GetWorldTransform().translation_;
	cameraController->Initialize();
	cameraController->setTarget(player_);

	// マップの境界を (-100, -100) から (100, 100) と仮定し、カメラのZ深度を考慮して調整
	// カメラのZオフセットが -15.0f のため、この設定はX, Y軸のマップ境界を直接設定しています。
	// Z軸の範囲も設定が必要ですが、今回はX, Yのマップ移動に焦点を当てます。

	CameraController::Rect mapLimit = {
	    -100.0f, // left
	    100.0f,  // right
	    -100.0f, // bottom
	    100.0f   // top
	};
	cameraController->SetMovableArea(mapLimit);

	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");

	modelParticle_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();
	font_->Initialize();

	// HPバーの初期化 (GameSceneに残すUI要素)
	hpBarBaseTexture_ = KamataEngine::TextureManager::Load("HP.png");
	hpBarTexture_ = KamataEngine::TextureManager::Load("HPR.png");
	const Vector2 kHpBarPos = {30.0f, 60.0f};  // ★ 修正: HPバーを少し下に移動 ★
	const Vector2 kExpBarPos = {30.0f, 30.0f}; // ★ 追記: EXPバーの位置 (上側) ★
	const Vector2 kHpBarSize = {200.0f, 20.0f};

	sousaTextureHandle_ = KamataEngine::TextureManager::Load("sousa.png");
	sousaTextureHandle2_ = KamataEngine::TextureManager::Load("sousa2.png");
	sousaSprite_ = KamataEngine::Sprite::Create(sousaTextureHandle_, {1040.0f, 400.0f});
	sousaSprite2_ = KamataEngine::Sprite::Create(sousaTextureHandle2_, {1040.0f, 500.0f});

	hpBarBase_ = KamataEngine::Sprite::Create(hpBarBaseTexture_, kHpBarPos);
	hpBarBase_->SetSize(kHpBarSize);
	hpBar_ = KamataEngine::Sprite::Create(hpBarTexture_, kHpBarPos);
	hpBar_->SetSize(kHpBarSize);

	// ★★★ 追記: EXPバーの初期化 (HPバーと同じテクスチャを使用) ★★★
	expBarBaseTexture_ = KamataEngine::TextureManager::Load("exp.png");
	expBarTexture_ = KamataEngine::TextureManager::Load("expR.png");
	expBarBase_ = KamataEngine::Sprite::Create(expBarBaseTexture_, kExpBarPos);
	expBarBase_->SetSize(kHpBarSize); // サイズはHPバーと同じ
	expBar_ = KamataEngine::Sprite::Create(expBarTexture_, kExpBarPos);
	expBar_->SetSize(kHpBarSize); // サイズはHPバーと同じ

	// スキル選択画面用スプライトの初期化 (GameSceneに残すUI要素)
	whiteTextureHandle_ = KamataEngine::TextureManager::Load("white1x1.png");
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
	// GameLogicに依存オブジェクト (Player, Font, HPBar, EXPBar) を渡す
	gameLogic_ = new GameLogic(player_, font_, hpBar_, hpBarBase_, expBar_, expBarBase_); // ★ 修正: EXPバーを渡す ★
	gameLogic_->Initialize();
}

void GameScene::Update() {

	// プレイヤーの更新 (GameSceneに残す)
	player_->Update();
	cameraController_->Update();

	// プレイヤーのワールド座標に境界線 (3860x3860) の制限を適用
	const float kBoundaryMin = 0.0f;
	const float kBoundaryMax = 3860.0f;

	// X座標の制限
	worldTransform_.translation_.x = (std::max)(worldTransform_.translation_.x, kBoundaryMin);
	worldTransform_.translation_.x = (std::min)(worldTransform_.translation_.x, kBoundaryMax);

	// Y座標の制限
	worldTransform_.translation_.y = (std::max)(worldTransform_.translation_.y, kBoundaryMin);
	worldTransform_.translation_.y = (std::min)(worldTransform_.translation_.y, kBoundaryMax);

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

	player_->SetIsSkillSelecting(gameLogic_->IsLevelUpPending()); //


	// ★★★ 追記: 敵の死亡時パーティクル生成処理 ★★★
	// GameLogicから倒された敵の位置リストを取得し、パーティクルを生成する
	std::list<Vector3> deadPositions = gameLogic_->GetDeadEnemyPositions();
	for (const Vector3& position : deadPositions) {
		ParticleBorn(position);
	}
	// パーティクル生成後、リストをクリア
	gameLogic_->ClearDeadEnemyPositions();

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

	// ★★★ 追記: パーティクルの更新処理 ★★★
	particles_.remove_if([](Particle* particle) {
		particle->Update();
		if (particle->IsFinished()) {
			delete particle;
			return true;
		}
		return false;
	});
}

/**
 * @brief HPバーの描画処理 (GameSceneに残す)
 */
void GameScene::DrawHPBar() {
	hpBarBase_->Draw(); // ベース (枠)
	hpBar_->Draw();     // 現在HP (バー本体)
}

void GameScene::DrawEXPBar() {
	expBarBase_->Draw(); // ベース (枠)
	expBar_->Draw();     // 現在EXP (バー本体)
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 2. 3Dオブジェクトの描画
	Model::PreDraw();

	// CameraControllerから最新のカメラを取得
	const KamataEngine::Camera& currentCamera = cameraController_->GetViewProjection();

	// プレイヤーの描画
	playerModel_->Draw(player_->GetWorldTransform(), currentCamera);

	// StageのDrawは2Dなので、この3D描画のブロックから削除し、2D描画に移動していると仮定

	// GameLogic内のオブジェクトの描画
	// 224行目付近のエラー C2039 を修正: Draw -> DrawObjects
	gameLogic_->DrawObjects(currentCamera);

	// 3. 3D描画の終了
	Model::PostDraw();

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar(); // HPバーの描画
	DrawEXPBar();

	// スキル選択画面の描画
	if (gameLogic_->IsLevelUpPending()) {
		// ★★★ GameLogicにUI要素を渡して描画させる ★★★
		gameLogic_->DrawSkillSelectionUI(skillCursorSprite_, skillOptionSprites_, skillScreenBackground_);
	}

	sousaSprite_->Draw();
	sousaSprite2_->Draw();

	font_->Draw();

	// 6. 2D描画の終了
	Sprite::PostDraw();

	// ★★★ 修正: ImGui描画処理の統合を_DEBUGで囲む ★★★
#ifdef _DEBUG
	KamataEngine::ImGuiManager* imGuiManager = KamataEngine::ImGuiManager::GetInstance();

	// 1. ImGui受付開始
	imGuiManager->Begin();

	// 2. ImGuiウィンドウの定義
	gameLogic_->DrawImGui();

	// 3. ImGui受付終了
	imGuiManager->End();

	// 4. 描画コマンドの実行
	imGuiManager->Draw();
#endif // _DEBUG
}

void GameScene::ParticleBorn(Vector3 position) {

	for (int i = 0; i < 50; i++) {
		Particle* particle = new Particle();
		Vector3 velocity = {distribution(randomEngine), distribution(randomEngine), 0}; // ランダムな速度を生成
		// Normalize(velocity); // 古い記述
		MathUtility::Normalize(velocity); // ★修正: スコープを明示

		// あいまいだった演算子の行は、using namespaceを削除したことで解消されます。
		velocity = velocity * distribution(randomEngine); // ランダムな速度を生成
		velocity = velocity * 0.2f;                       // スピードを調整

		particle->Initialize(modelParticle_, position, velocity);
		particles_.push_back(particle);
	}
}