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
	//delete graph_;
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
	
	// ★ Enemy2の解放を追加 ★
	for (Enemy2* enemy2 : enemies2_) {
		delete enemy2;
	}
	enemies2_.clear();
}

void GameScene::Initialize() {

	//graph_ = new Graph();
	//graph_->Initialize();

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

	// ★ レベルアップシステム関連の初期化 (追加) ★
	level_ = 1;
	currentExp_ = 0;
	requiredExp_ = kExpBase; // 10
	isLevelUpPending_ = false;
	selectedSkillIndex_ = 0;
}

void GameScene::StartLevelUp() {
	level_++;
	// 次のレベルの経験値バーに超過分を回す
	currentExp_ -= requiredExp_;

	// 次の必要経験値を計算 (例: 10 * 1.2^(level-1))
	// 経験値はintで計算し、小数点以下は切り捨てる（または四捨五入する）
	requiredExp_ = static_cast<int>(kExpBase * std::pow(kExpScale, level_ - 1));

	isLevelUpPending_ = true;
	selectedSkillIndex_ = 0; // 選択インデックスをリセット

	// スキル選択肢をランダムに3つ生成
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	// SkillType::kSkillCountはenumの要素数として使用
	std::uniform_int_distribution<int> distType(0, static_cast<int>(SkillType::kSkillCount) - 1);

	currentSkillOptions_.clear();
	while (currentSkillOptions_.size() < 3) {
		SkillType newSkill = static_cast<SkillType>(distType(engine));

		// 重複チェック
		bool alreadyExists = false;
		for (SkillType skill : currentSkillOptions_) {
			if (skill == newSkill) {
				alreadyExists = true;
				break;
			}
		}
		if (!alreadyExists) {
			currentSkillOptions_.push_back(newSkill);
		}
	}
}

void GameScene::UpdateSkillSelection() {
	// 入力インスタンスを取得
	Input* input = KamataEngine::Input::GetInstance();

	// 上キー/下キーで選択肢を移動
	if (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP)) {
		selectedSkillIndex_ = (selectedSkillIndex_ - 1 + currentSkillOptions_.size()) % currentSkillOptions_.size();
	}
	if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
		selectedSkillIndex_ = (selectedSkillIndex_ + 1) % currentSkillOptions_.size();
	}

	// 決定キー (スペースキーやエンターキー) でスキルを適用し、ゲームを再開
	if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
		ApplySkill(currentSkillOptions_[selectedSkillIndex_]);
		isLevelUpPending_ = false;    // ゲーム再開
		currentSkillOptions_.clear(); // 選択肢をクリア
	}

	// **TODO: スキル選択画面のUI描画ロジックはDraw関数内に実装**
}

void GameScene::ApplySkill(SkillType skill) {
	// ここにPlayerクラスの機能拡張やGameScene全体のパラメータ変更処理を記述します
	switch (skill) {
	case SkillType::kAttackUp:
		// 例: プレイヤーの攻撃半径を増やす (Playerクラスにpublicなメソッドが必要)
		// player_->UpgradeAttackRadius(0.5f);
		break;
	case SkillType::kSpeedUp:
		// 例: プレイヤーの移動速度を増やす (Playerクラスにpublicなメソッドが必要)
		// player_->UpgradeMoveSpeed(0.1f);
		break;
	case SkillType::kHeal:
		// 例: プレイヤーのHPを回復する (Playerクラスにpublicなメソッドが必要)
		// player_->Heal(3);
		break;
	default:
		break;
	}
}

// 敵のランダム生成関数 (実装)
void GameScene::SpawnEnemy() {

	// ★ Enemy1とEnemy2の総数で制限 ★
	if (enemies_.size() + enemies2_.size() >= kMaxEnemies + kMaxEnemies2) {
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

	// ★ 敵の種類をランダムに決定 (Enemy2を低確率で出現させる) ★
	std::uniform_int_distribution<int> distType(0, 3); // 0: Enemy2, 1-3: Enemy1 (4分の1の確率でEnemy2)
	if (enemies2_.size() < kMaxEnemies2 && distType(engine) == 0) {
		Enemy2* newEnemy2 = new Enemy2(randomPos);
		newEnemy2->Initialize();
		enemies2_.push_back(newEnemy2);
	} else if (enemies_.size() < kMaxEnemies) {
		Enemy* newEnemy = new Enemy(randomPos);
		newEnemy->Initialize();
		enemies_.push_back(newEnemy);
	}
}

void GameScene::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	// float playerBodyRadius = 0.5f; // プレイヤー本体の半径 (仮)

	// ------------------------------------
	// 1. プレイヤーの攻撃 vs 敵 (近接攻撃判定)
	// ------------------------------------
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();

		// --- Enemy1への攻撃判定 ---
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

		// --- Enemy2への攻撃判定 (★追加★) ---
		for (Enemy2* enemy2 : enemies2_) {
			if (enemy2->IsDead())
				continue;

			Vector3 enemyPos = enemy2->GetPosition();
			float enemyRadius = enemy2->GetRadius();

			Vector3 diff = enemyPos - playerPos;
			float distance = Math::Length(diff);

			// 攻撃判定
			if (distance <= attackRadius + enemyRadius) {
				// 敵にダメージを与える
				enemy2->TakeDamage(1);
			}
		}
	}

	// ------------------------------------
	// 2. 敵 vs プレイヤー (敵からの接触ダメージ)
	// ------------------------------------
	// for (Enemy* enemy : enemies_) {
	//	// 死亡した敵や、すでにHPが0のプレイヤーにはダメージを与えない
	//	if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
	//		continue;

	//	Vector3 enemyPos = enemy->GetPosition();
	//	float enemyRadius = enemy->GetRadius();

	//	Vector3 diff = enemyPos - playerPos;
	//	float distance = Math::Length(diff);

	//	// 接触判定
	//	if (distance <= playerBodyRadius + enemyRadius) {
	//		// プレイヤーにダメージを与える (ここでは接触1回で1ダメージと仮定)
	//		player_->TakeDamage(1);
	//		// ダメージを一度与えたら、敵をプレイヤーから少し遠ざけるなどの処理を追加しても良い
	//	}
	//}
}

void GameScene::Update() {
	if (isGameOver_) {
		// ゲームオーバーシーンへの**遷移**ロジックをここに記述
		return;
	}

	// ★ レベルアップ待ち状態の場合はスキル選択画面の更新のみを行う ★
	if (isLevelUpPending_) {
		UpdateSkillSelection();
		return; // メインのゲーム更新はスキップ
	}
	// ------------------------------------

	stage_->Update();
	player_->Update();
	// graph_->Update();

	// スコア表示の更新
	// **【修正】レベルと経験値を表示するように変更**
	std::string levelString = "Lv:" + std::to_string(level_);
	font_->Set(levelString + " EXP:" + std::to_string(currentExp_) + "/" + std::to_string(requiredExp_));
	// ------------------------------------


	// ------------------------------------
	// 敵の生成
	// ------------------------------------
	enemySpawnTimer_++;
	// ★ 敵の総数で制限するように変更 ★
	if (enemies_.size() + enemies2_.size() < kMaxEnemies + kMaxEnemies2 && enemySpawnTimer_ >= kEnemySpawnInterval) {
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
	// ★ Enemy2の更新を追加 ★
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Update(playerPos);
	}

	// ------------------------------------
	// 衝突判定の実行
	// ------------------------------------
	CheckAllCollisions();

	// ------------------------------------
	// 経験値アイテムの更新・削除
	// ------------------------------------
	// Update時にプレイヤーの位置を渡す
	Vector3 playerPosForExp = player_->GetPosition();
	for (Experience* exp : experiences_) {
		exp->Update(playerPosForExp);
	}


	// --- アイテムの削除処理 (取得/死亡判定) ---
	for (auto it = experiences_.rbegin(); it != experiences_.rend();) {
		Experience* exp = *it;
		if (exp->IsDead()) {
			// **【修正】currentExp_を更新し、レベルアップ判定を行う**
			int expValue = 1; // Experience1つあたりの経験値を1とする
			currentExp_ += expValue;
			score_ += expValue; // スコアも引き続き加算

			// ★ レベルアップ判定 ★
			if (currentExp_ >= requiredExp_) {
				StartLevelUp(); // レベルアップ処理開始
			}
			// ----------------------------------------

			delete exp; // メモリを解放
			it = std::vector<Experience*>::reverse_iterator(experiences_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	// ------------------------------------
	// 敵の削除処理 (死亡判定)
	// ------------------------------------
	// 乱数生成をループの外に移動し、10〜15個ドロップさせる (Enemy1用)
	std::uniform_int_distribution<int> distCount1(10, 15);
	// ★ Enemy2用: 30個ドロップに固定 ★
	const int kEnemy2DropCount = 30;

	// --- Enemy1の削除 ---
	for (auto it = enemies_.rbegin(); it != enemies_.rend();) {
		Enemy* enemy = *it;
		if (enemy->IsDead()) {
			// **【修正】敵の死亡時に経験値を10-15個生成する**
			Vector3 dropPosition = enemy->GetPosition();
			int dropCount = distCount1(engine); // 10〜15個の乱数

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

	// --- Enemy2の削除 (★追加★) ---
	for (auto it = enemies2_.rbegin(); it != enemies2_.rend();) {
		Enemy2* enemy2 = *it;
		if (enemy2->IsDead()) {
			// **【Enemy2の経験値生成】敵の死亡時に経験値を30個生成する**
			Vector3 dropPosition = enemy2->GetPosition();
			int dropCount = kEnemy2DropCount; // 30個ドロップ

			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}

			delete enemy2; // メモリを解放
			it = std::vector<Enemy2*>::reverse_iterator(enemies2_.erase(std::next(it).base()));
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
	// ★ Enemy2の描画を追加 ★
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Draw(camera_);
	}

	// 3. 3D描画の終了
	Model::PostDraw(); // ★ Sprite描画の前にModelの描画を一旦区切る ★

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar(); // HPバーの描画

	// graph_->Draw(); // 既存のUI
	font_->Draw();

	// 6. 2D描画の終了
	Sprite::PostDraw();
}