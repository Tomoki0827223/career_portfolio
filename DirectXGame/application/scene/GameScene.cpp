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
	// ... (ステージ、カメラ、プレイヤーの初期化はそのまま) ...
	stage_ = new Stage();
	stage_->Initialize();
	camera_.Initialize();
	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");
	modelParticle_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();
	font_->Initialize();
	font_->SetPosition({1100.0f, 10.0f});

	// --- 1. 各種バー（HP/EXP/SP）の生成 ---
	const Vector2 kBarSize = {200.0f, 20.0f};

	hpBarBaseTexture_ = TextureManager::Load("HP.png");
	hpBarTexture_ = TextureManager::Load("HPR.png");
	hpBarBase_ = Sprite::Create(hpBarBaseTexture_, {30.0f, 60.0f});
	hpBarBase_->SetSize(kBarSize);
	hpBar_ = Sprite::Create(hpBarTexture_, {30.0f, 60.0f});
	hpBar_->SetSize(kBarSize);

	expBarBaseTexture_ = TextureManager::Load("exp.png");
	expBarTexture_ = TextureManager::Load("expR.png");
	expBarBase_ = Sprite::Create(expBarBaseTexture_, {30.0f, 30.0f});
	expBarBase_->SetSize(kBarSize);
	expBar_ = Sprite::Create(expBarTexture_, {30.0f, 30.0f});
	expBar_->SetSize(kBarSize);

	spBarBase_ = Sprite::Create(expBarBaseTexture_, {30.0f, 90.0f});
	spBarBase_->SetSize(kBarSize);
	spBar_ = Sprite::Create(expBarTexture_, {30.0f, 90.0f});
	spBar_->SetSize(kBarSize);
	spBar_->SetColor({0.5f, 0.5f, 1.0f, 1.0f});

	// --- 2. スキル選択用UI（背景・枠・カーソル）の生成 ---
	// ★ ここが重要！gameLogicに渡す前にこれらを完成させる必要があります
	whiteTextureHandle_ = TextureManager::Load("white1x1.png");
	skillScreenBackground_ = Sprite::Create(whiteTextureHandle_, {0, 0});
	skillScreenBackground_->SetSize({1280.0f, 720.0f});
	skillScreenBackground_->SetColor({0.0f, 0.0f, 0.0f, 0.8f});

	const KamataEngine::Vector2 kOptionSize = {400.0f, 100.0f};
	const KamataEngine::Vector2 kBasePos = {440.0f, 180.0f};

	for (int i = 0; i < 3; ++i) {
		skillOptionSprites_[i] = Sprite::Create(whiteTextureHandle_, {kBasePos.x, kBasePos.y + i * 120.0f});
		skillOptionSprites_[i]->SetSize(kOptionSize);
		skillOptionSprites_[i]->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	// ★ これが nullptr だったので落ちていました。ここで生成を完了させます
	skillCursorSprite_ = Sprite::Create(whiteTextureHandle_, {0, 0});
	skillCursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
	skillCursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f});

	// --- 3. 全ての準備が整ってから GameLogic を生成 ---
	gameLogic_ = new GameLogic(player_, font_, hpBar_, hpBarBase_, expBar_, expBarBase_, spBar_, spBarBase_);
	gameLogic_->Initialize();
	gameLogic_->SetIsBackground(isBackground_);

	// 操作説明など
	sousaTextureHandle_ = TextureManager::Load("sousa.png");
	sousaTextureHandle2_ = TextureManager::Load("sousa2.png");
	sousaSprite_ = Sprite::Create(sousaTextureHandle_, {1040.0f, 400.0f});
	sousaSprite2_ = Sprite::Create(sousaTextureHandle2_, {1040.0f, 500.0f});
}

void GameScene::Update() {
	// 1. プレイヤーを更新（死亡演出もここに含まれる）
	player_->Update();

	// 2. カメラの更新（プレイヤーを追い続ける）
	Vector3 playerPos = player_->GetPosition();
	camera_.translation_.x = playerPos.x;
	camera_.translation_.y = playerPos.y;
	camera_.translation_.z = playerPos.z - 60.0f;
	camera_.UpdateMatrix();
	camera_.TransferMatrix();

	// 3. ゲームロジックの更新（敵などの更新）
	// ゲームオーバー時でも敵を動かし続けたい場合はそのまま、止めたい場合は if(!isGameOver_) で囲む
	// ★追加：ゲームオーバーの判定とシーン終了の通知
	if (isGameOver_) {
		// プレイヤーの死亡演出（タイマー）が終了したかチェック
		if (player_->GetDeadTimer() >= player_->GetMaxDeadTime()) {

			isFinished_ = true;           // mainへ終了を伝える
			nextScene_ = Scene::GameOver; // 次はゲームオーバーシーンへ行く指示

			// ※スコアの受け渡し（SetResultScore）は、後ほど共通データ（SharedData）を作った際に
			// ここで sharedData_->score = GetScore(); のように書くことになります。
		}
	}

	// --- ゲームオーバー判定 ---
	int currentHp = player_->GetCurrentHp();
	if (currentHp <= 0 && !isGameOver_) {
		isGameOver_ = true;
		player_->Die(); // プレイヤーの死亡フラグを立てる
	}

	// ★修正: ゲームオーバー演出中は、以下のゲーム進行処理をスキップする
	if (isGameOver_) {
		return;
	}


	player_->SetIsSkillSelecting(gameLogic_->IsLevelUpPending());

	// スキル選択中の処理
	if (gameLogic_->IsLevelUpPending()) {
		gameLogic_->UpdateSkillSelection();
		return;
	}

	// 敵の死亡パーティクル
	std::list<Vector3> deadPositions = gameLogic_->GetDeadEnemyPositions();
	for (const Vector3& position : deadPositions) {
		ParticleBorn(position);
	}
	gameLogic_->ClearDeadEnemyPositions();

	stage_->Update(player_->GetPosition());

	// パーティクルの更新
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

	// 1. 3Dモデル（キャラやステージ）の描画
	// 背景時でもゲームの様子を見せるために常に実行
	Model::PreDraw();
	stage_->Draw(camera_);
	player_->Draw(camera_);
	gameLogic_->DrawObjects(camera_);
	for (Particle* particle : particles_) { // パーティクルもあれば描画
		particle->Draw(&camera_);
	}
	Model::PostDraw();

	// --- ここから2D描画 ---

	// 背景モードでない場合のみUIを描画する
	if (!isBackground_) {
		// Sprite描画の開始
		Sprite::PreDraw();

		DrawHPBar();
		DrawEXPBar();

		spBarBase_->Draw();
		spBar_->Draw();

		// スキル選択画面の描画
		if (gameLogic_->IsLevelUpPending()) {
			gameLogic_->DrawSkillSelectionUI(skillCursorSprite_, skillOptionSprites_, skillScreenBackground_);
		}

		sousaSprite_->Draw();
		sousaSprite2_->Draw();
		font_->Draw();

		// Sprite描画の終了
		Sprite::PostDraw();
	}

	// ImGuiはデバッグ用なので、背景モードに関わらず必要なら表示
#ifdef _DEBUG
	if (!isBackground_) { // タイトル画面でデバッグUIが邪魔ならここでもチェック
		KamataEngine::ImGuiManager* imGuiManager = KamataEngine::ImGuiManager::GetInstance();
		imGuiManager->Begin();
		gameLogic_->DrawImGui();
		imGuiManager->End();
		imGuiManager->Draw();
	}
#endif
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