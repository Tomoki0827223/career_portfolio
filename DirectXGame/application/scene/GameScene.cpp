#include "GameScene.h"

// 乱数生成器のグローバル宣言 (Initializeで使用)
namespace {
std::random_device seed_gen;
std::mt19937 engine(seed_gen());
// マップの範囲を定義
const float MAP_HALF_RANGE = 50.0f;
std::uniform_real_distribution<float> dist(-MAP_HALF_RANGE, MAP_HALF_RANGE);
} // namespace

GameScene::~GameScene() {
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;
	delete hpBarBase_; // 追加
	delete hpBar_;     // 追加

	// 経験値アイテムの解放
	for (Experience* exp : experiences_) {
		delete exp;
	}
	experiences_.clear();

	// 敵の解放 (追加)
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
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

	// HPバーの初期化 (追加)
	// ユーザーがアップロードしたファイルを基にテクスチャをロード
	hpBarBaseTexture_ = KamataEngine::TextureManager::Load("HP.png");
	hpBarTexture_ = KamataEngine::TextureManager::Load("HPR.png");

	// 画面左上に配置
	const Vector2 kHpBarPos = {30.0f, 30.0f};
	const Vector2 kHpBarSize = {200.0f, 20.0f}; // HPバーのサイズ (幅200、高さ20)

	hpBarBase_ = KamataEngine::Sprite::Create(hpBarBaseTexture_, kHpBarPos);
	hpBarBase_->SetSize(kHpBarSize); // ベースは最大サイズ

	hpBar_ = KamataEngine::Sprite::Create(hpBarTexture_, kHpBarPos);
	hpBar_->SetSize(kHpBarSize); // HP減少でサイズを変更するため、初期は最大サイズ
}

// 敵のランダム生成関数 (実装)
void GameScene::SpawnEnemy() {
	if (enemies_.size() >= kMaxEnemies) {
		return;
	}

	// プレイヤーから離れた位置に生成する (最小距離 20.0f)
	const float kMinSpawnDistance = 20.0f;
	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	float distance = 0.0f;

	do {
		// x, yをランダムに生成し、zは0.0f（固定）に設定
		randomPos = {dist(engine), dist(engine), 0.0f};
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	Enemy* newEnemy = new Enemy(randomPos);
	newEnemy->Initialize();
	enemies_.push_back(newEnemy);
}

// 衝突判定関数 (実装)
void GameScene::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = 0.5f; // プレイヤー本体の半径 (仮)

	// ------------------------------------
	// 1. プレイヤーの攻撃 vs 敵 (近接攻撃判定)
	// ------------------------------------
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();

		for (Enemy* enemy : enemies_) {
			if (enemy->IsDead())
				continue;

			Vector3 enemyPos = enemy->GetPosition();
			float enemyRadius = enemy->GetRadius();

			Vector3 diff = enemyPos - playerPos;
			float distance = Math::Length(diff);

			// 攻撃判定: プレイヤー位置と敵が、攻撃半径+敵半径内にいれば命中
			if (distance <= attackRadius + enemyRadius) {
				// 敵にダメージを与える
				enemy->TakeDamage(1);
			}
		}
	}

	// ------------------------------------
	// 2. 敵 vs プレイヤー (敵からの接触ダメージ)
	// ------------------------------------
	for (Enemy* enemy : enemies_) {
		// 死亡した敵や、すでにHPが0のプレイヤーにはダメージを与えない
		if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
			continue;

		Vector3 enemyPos = enemy->GetPosition();
		float enemyRadius = enemy->GetRadius();

		Vector3 diff = enemyPos - playerPos;
		float distance = Math::Length(diff);

		// 接触判定
		if (distance <= playerBodyRadius + enemyRadius) {
			// プレイヤーにダメージを与える (ここでは接触1回で1ダメージと仮定)
			player_->TakeDamage(1);
			// ダメージを一度与えたら、敵をプレイヤーから少し遠ざけるなどの処理を追加しても良い
		}
	}

	// ------------------------------------
	// 3. プレイヤー vs 経験値アイテム (吸引開始の判定)
	// ------------------------------------
	// GameScene.hに定義されている吸引範囲 ATTRACTION_RADIUS (10.0f) を使用
	for (Experience* exp : experiences_) {
		if (exp->IsAttracted()) {
			// 吸引中であれば、常にプレイヤーの位置をターゲットとして更新する
			exp->SetTarget(playerPos);
			continue;
		}

		Vector3 expPos = exp->GetPosition();
		Vector3 diff = expPos - playerPos;
		float distance = Math::Length(diff);

		// プレイヤーの周囲の吸引範囲に入ったら、吸引を開始する
		// 【修正】Experience::ATTRACTION_RADIUSを使用するように変更
		if (distance <= Experience::ATTRACTION_RADIUS) {
			exp->SetTarget(playerPos); // SetTarget内部でisAttracted_がtrueになる
		}
	}
} // ★ここにあった余分な '}' を削除★

void GameScene::Update() {
	if (isGameOver_) {
		// ゲームオーバーシーンへの**遷移**ロジックをここに記述
		// 例: SceneManager::GetInstance()->ChangeScene(new GameOverScene());
		return;
	}

	stage_->Update();
	player_->Update();
	graph_->Update();

	// スコア表示の更新
	font_->Set(score_);

	// ------------------------------------
	// 敵の生成
	// ------------------------------------
	enemySpawnTimer_++;
	if (enemies_.size() < kMaxEnemies && enemySpawnTimer_ >= kEnemySpawnInterval) {
		SpawnEnemy();
		enemySpawnTimer_ = 0; // タイマーリセット
	}

	// ------------------------------------
	// 敵の更新 (追尾)
	// ------------------------------------
	Vector3 playerPos = player_->GetPosition();
	for (Enemy* enemy : enemies_) {
		enemy->Update(playerPos);
	}

	// ------------------------------------
	// 衝突判定の実行
	// ------------------------------------
	CheckAllCollisions();

	// ------------------------------------
	// 経験値アイテムの更新・削除
	// ------------------------------------
	// 【修正】Update時にプレイヤーの位置を渡す
	Vector3 playerPosForExp = player_->GetPosition();
	for (Experience* exp : experiences_) {
		exp->Update(playerPosForExp);
	}

	// --- アイテムの削除処理 (取得/死亡判定) ---
	// 後方からループで処理することで、イテレータの無効化を避ける
	for (auto it = experiences_.rbegin(); it != experiences_.rend();) {
		Experience* exp = *it;
		if (exp->IsDead()) {
			score_ += 1; // 取得されたらスコアを加算

			delete exp; // メモリを解放
			// rbegin() / rend() を使用しているため、削除には base() が必要
			it = std::vector<Experience*>::reverse_iterator(experiences_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	// ------------------------------------
	// 敵の削除処理 (死亡判定)
	// ------------------------------------
	// 乱数生成をループの外に移動し、10〜15個ドロップさせる
	std::uniform_int_distribution<int> distCount(10, 15);

	for (auto it = enemies_.rbegin(); it != enemies_.rend();) {
		Enemy* enemy = *it;
		if (enemy->IsDead()) {
			// **【修正】敵の死亡時に経験値を10-15個生成する**
			Vector3 dropPosition = enemy->GetPosition();
			int dropCount = distCount(engine); // 10〜15個の乱数

			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			// ----------------------------------------

			delete enemy; // メモリを解放
			// リバースイテレータを順方向イテレータに変換して削除
			it = std::vector<Enemy*>::reverse_iterator(enemies_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	// ------------------------------------
	// HPバーの更新とゲームオーバー判定
	// ------------------------------------
	int currentHp = player_->GetCurrentHp();
	int maxHp = player_->GetMaxHp();

	if (currentHp <= 0) {
		isGameOver_ = true; // ゲームオーバーフラグを立てる
		                    // **TODO**: ここでゲームオーバーシーンへの遷移処理を実装してください。
	}

	// HPバーのサイズを更新
	float hpRatio = (float)currentHp / maxHp;
	float newWidth = hpBarBase_->GetSize().x * hpRatio;
	Vector2 currentSize = hpBar_->GetSize();

	// HPバーの幅のみをHPの比率に合わせて変更する
	hpBar_->SetSize({newWidth, currentSize.y});
}

/**
 * @brief HPバーの描画処理
 */
void GameScene::DrawHPBar() {
	// HPバーを描画 (左上配置)
	hpBarBase_->Draw(); // ベース (枠)
	hpBar_->Draw();     // 現在HP (バー本体)
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 1. 3D描画のセットアップ
	Model::PreDraw(); // Sprite::PreDrawより先にModel::PreDrawを呼ぶ

	// 2. 3Dオブジェクトの描画
	stage_->Draw();
	player_->Draw();

	// 経験値アイテムの描画
	for (Experience* exp : experiences_) {
		exp->Draw(camera_);
	}

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw(camera_);
	}

	// 3. 3D描画の終了
	Model::PostDraw(); // ★ Sprite描画の前にModelの描画を一旦区切る ★

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar(); // HPバーの描画

	graph_->Draw(); // 既存のUI
	font_->Draw();

	// 6. 2D描画の終了
	Sprite::PostDraw();
}