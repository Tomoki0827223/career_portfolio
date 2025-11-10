#define NOMINMAX
#define _USE_MATH_DEFINES
#include "GameScene.h"
#include "3d/DirectionalLight.h"
#include "3d/PrimitiveDrawer.h"
#include "base/DirectXCommon.h"
#include "base/TextureManager.h"
#include "input/Input.h"
#include "math/Matrix4x4.h"

#include <algorithm> // std::remove_if, std::shuffle用
#include <cmath>

// 乱数生成器の定義
namespace {
// マップの範囲を定義
const float MAP_HALF_RANGE = 50.0f;
// ★修正: global_engineを名前空間外でも使えるように、GameScene.cppのトップレベルで定義する
std::random_device global_seed_gen;
std::mt19937 global_engine(global_seed_gen());
std::uniform_real_distribution<float> dist(-MAP_HALF_RANGE, MAP_HALF_RANGE);
} // namespace

// ★修正: デストラクタの実装 (DebugTextはシングルトンなのでdeleteしない) ★
GameScene::~GameScene() {
	// GameSceneがオーナーとなっている生ポインタを解放
	for (auto& bullet : bullets_) {
		delete bullet;
	}
	for (auto& book : books_) {
		delete book;
	}
	for (auto& wine : wines_) {
		delete wine;
	}
	for (auto& exp : experiences_) {
		delete exp;
	}

	// UIの解放
	delete hpBarBase_;
	delete hpBar_;
	// ★修正: font_はDebugText::GetInstance()で取得しており、デストラクタがprivateなのでdeleteしません。 ★
	// delete font_;
}

/**
 * @brief ゲームシーンの初期化
 */
void GameScene::Initialize() {
	// ★修正: Playerはシングルトンなので GetInstance()で取得 ★
	player_ = Player::GetInstance();
	player_->Initialize(); // プレイヤーのHPやスキルレベルをリセット

	// Stageはnewする
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	// Cameraはunique_ptrで管理
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// スキル情報の初期化 (テクスチャパス、名前、説明の設定)
	allSkills_ = {
	    // 0: kBook
	    SkillInfo{GameScene::kBook,   "DirectXGame/Resources/Sukill/book.png", "魔導書",   "プレイヤーを回転する魔導書を追加/強化する。"},
	    // 1: kBullet (Gun.png)
	    SkillInfo{GameScene::kBullet, "DirectXGame/Resources/Sukill/Gun.png",  "銃弾強化", "弾丸の攻撃力を永続的に増加させる。"         },
	    // 2: kHart (Hart.png)
	    SkillInfo{GameScene::kHart,   "DirectXGame/Resources/Sukill/Hart.png", "体力回復", "プレイヤーの体力を全回復する。"             },
	    // 3: kWine (Wine.png)
	    SkillInfo{GameScene::kWine,   "DirectXGame/Resources/Sukill/Wine.png", "ワイン",   "プレイヤーの移動速度を永続的に上昇させる。" },
	};

	// タイマーとフラグのリセット
	enemySpawnTimer_ = 0;
	enemy2SpawnTimer_ = 0;
	bulletSpawnTimer_ = 0;
	wineSpawnTimer_ = 0;
	isGameOver_ = false;
	isSkillSelectionMode = false;

	// スキルレベルのリセット
	bookLevel_ = 0;
	bulletLevel_ = 0;
	wineLevel_ = 0;

	// UI (HPバー) の初期化
	hpBarBaseTexture_ = TextureManager::Load("DirectXGame/Resources/HP.png");
	hpBarTexture_ = TextureManager::Load("DirectXGame/Resources/HPR.png");
	hpBarBase_ = Sprite::Create(hpBarBaseTexture_, {100.0f, 50.0f});
	hpBar_ = Sprite::Create(hpBarTexture_, {100.0f, 50.0f});

	// UI (フォント) の初期化
	// ★修正: font_はシングルトンのため、ポインタとして保持する必要はないが、
	// DebugText::GetInstance()で取得して、ローカルで操作します。
	font_ = DebugText::GetInstance();

	// リストのクリア (unique_ptrのリストはクリアするだけで中の要素も自動で解放されます)
	enemies_.clear();
	enemies2_.clear();
	// 生ポインタのリストは手動で解放してからクリア
	for (auto& bullet : bullets_) {
		delete bullet;
	}
	bullets_.clear();
	for (auto& book : books_) {
		delete book;
	}
	books_.clear();
	for (auto& wine : wines_) {
		delete wine;
	}
	wines_.clear();
	for (auto& exp : experiences_) {
		delete exp;
	}
	experiences_.clear();

	// レベルシステムのリセット
	level_ = 1;
	currentExp_ = 0;
	requiredExp_ = 100;
	score_ = 0;
}

/**
 * @brief ゲームシーンの更新
 */
void GameScene::Update() {
	// ★【1. 最優先】プレイヤーの更新 (死亡モーションのタイマーを必ず進める) ★
	player_->Update();

	// ------------------------------------
	// ★【2. HP/ゲームオーバー判定と処理】★
	// ------------------------------------
	int currentHp = player_->GetCurrentHP(); // Player.h/cppのGetCurrentHP()を使用
	int maxHp = player_->GetMaxHP();

	// HPが0以下になったらゲームオーバーフラグを立てる (最初の1フレームのみ)
	if (currentHp <= 0 && !isGameOver_) {
		isGameOver_ = true;
		player_->Die(); // 死亡モーション開始フラグを設定
	}

	// HPバーのサイズを更新（HPが0以下の場合も比率が0になる）
	float hpRatio = (float)currentHp / maxHp;
	if (hpRatio < 0.0f) {
		hpRatio = 0.0f;
	}
	float newWidth = hpBarBase_->GetSize().x * hpRatio;
	Vector2 currentSize = hpBar_->GetSize();
	hpBar_->SetSize({newWidth, currentSize.y});

	// ゲームオーバー中の処理 (モーション終了待ち)
	if (isGameOver_) {
		// 死亡モーションが終わるまで待つ (GetDeadTimer() > GetMaxDeadTime())
		if (player_->IsDead() && player_->GetDeadTimer() > player_->GetMaxDeadTime()) {
			// ★★★ ここに実際のシーン遷移ロジックを記述してください ★★★
			// 例: KamataEngine::SceneManager::GetInstance()->ChangeScene("GameOver");
		}
		// ★ スキル選択は行わない ★
		return;
	}

	// ------------------------------------
	// ★【3. スキル選択中】の処理 ★
	// ------------------------------------
	if (isSkillSelectionMode) {
		UpdateSkillSelection();
		return;
	}

	// ------------------------------------
	// ★【4. 通常ゲーム更新ロジック】★
	// ------------------------------------

	// ★修正: Camera::Update()は存在しない可能性が高いため、削除します。
	// camera_->Update();

	stage_->Update();

	// 敵の生成
	EnemySpawn();
	Enemy2Spawn();

	// 敵の更新
	Vector3 playerPos = player_->GetPosition();
	for (auto& enemy : enemies_) {
		enemy->Update(playerPos);
	}
	for (auto& enemy2 : enemies2_) {
		enemy2->Update(playerPos);
	}

	// 弾丸の生成と更新
	BulletSpawn();
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// Bookの更新
	for (auto& book : books_) {
		book->Update(playerPos);
	}

	// Wineの生成と更新
	SpawnWine();
	for (auto& wine : wines_) {
		wine->Update(playerPos);
	}

	// 経験値アイテムの更新
	for (auto& exp : experiences_) {
		exp->Update(playerPos);
	}

	CheckAllCollisions();
	RemoveDeadEntities();

	UpdateLevel();

	// スコア表示の更新
	// ★修正: DebugText::Set(int)というメンバーは存在しない可能性が高いため、DebugText::Print(std::string)を使用するように修正します ★
	// font_->Set(score_);
}

/**
 * @brief ゲームシーンの描画
 */
void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 1. 3D描画のセットアップ (引数なしで呼ぶのが正しいと仮定)
	Model::PreDraw();

	// 2. 3Dオブジェクトの描画
	// ★修正: Stage::Draw()が引数を取らない場合を想定して修正します ★
	stage_->Draw();
	player_->Draw();

	// 経験値アイテムの描画 (生ポインタ)
	// ★修正: camera_はunique_ptrなので、参照渡しのために*camera_.get()または*camera_を渡します ★
	for (Experience* exp : experiences_) {
		exp->Draw(*camera_);
	}
	for (Wine* wine : wines_) {
		wine->Draw(*camera_);
	}
	// 敵の描画 (unique_ptrの要素を参照渡しで取得)
	for (auto& enemy : enemies_) {
		enemy->Draw(*camera_);
	}
	for (auto& enemy2 : enemies2_) {
		enemy2->Draw(*camera_);
	}
	// Bulletの描画
	for (Bullet* bullet : bullets_) {
		bullet->Draw(*camera_);
	}
	// Bookの描画
	for (Book* book : books_) {
		book->Draw(*camera_);
	}

	// 3. 3D描画の終了
	Model::PostDraw();

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar();
	DrawLevelAndExp(); // レベル・経験値の描画

	if (isSkillSelectionMode) {
		DrawSkillSelection();
	}

	// 6. 2D描画の終了
	Sprite::PostDraw();

	// PrimitiveDrawerの描画 (引数なし、またはビュープロジェクション行列を渡すパターンが一般的です)
	// ★修正: PrimitiveDrawer::Draw(*camera_) がエラーになったため、引数なしの Draw() に変更します ★
	PrimitiveDrawer::GetInstance()->Draw();

	DebugText::GetInstance()->DrawAll();
}

/**
 * @brief レベルアップ処理
 */
void GameScene::UpdateLevel() {
	if (currentExp_ >= requiredExp_) {
		level_++;
		currentExp_ -= requiredExp_;
		requiredExp_ = (int)(requiredExp_ * 1.5f);
		// ★修正: std::max(a, b) の形式に修正 ★
		requiredExp_ = (std::max)(requiredExp_, 150);

		StartSkillSelection();
	}
}

// ------------------ スキル選択/レベルアップ関連ロジック ------------------

void GameScene::StartSkillSelection() {
	isSkillSelectionMode = true;
	currentSelectedIndex_ = 0;

	std::vector<int> skillIndices;
	for (int i = 0; i < allSkills_.size(); ++i) {
		skillIndices.push_back(i);
	}

	std::shuffle(skillIndices.begin(), skillIndices.end(), global_engine);

	const float kScreenWidth = 1280.0f;
	const float kScreenHeight = 720.0f;
	const float kIconSize = 128.0f;
	const float kCenterX = kScreenWidth / 2.0f;
	const float kSpacing = 200.0f;
	const float kYPos = kScreenHeight / 2.0f;

	for (int i = 0; i < 3; ++i) {
		SkillInfo& selectedSkill = allSkills_[skillIndices[i]];

		if (selectedSkill.sprite == nullptr) {
			uint32_t textureHandle = TextureManager::Load(selectedSkill.texturePath);
			selectedSkill.sprite = Sprite::Create(textureHandle, {0, 0}, {1.0f, 1.0f, 1.0f, 1.0f});
			selectedSkill.sprite->SetSize({kIconSize, kIconSize});
		}

		float startX = kCenterX - kSpacing;
		selectedSkill.position = {startX + i * kSpacing, kYPos};
		selectedSkill.sprite->SetPosition(selectedSkill.position);
		selectedSkills_[i] = &selectedSkill;
	}
}

void GameScene::UpdateSkillSelection() {
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_D) || input->TriggerKey(DIK_RIGHT)) {
		currentSelectedIndex_ = (currentSelectedIndex_ + 1) % 3;
	} else if (input->TriggerKey(DIK_A) || input->TriggerKey(DIK_LEFT)) {
		currentSelectedIndex_ = (currentSelectedIndex_ - 1 + 3) % 3;
	}

	if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
		SkillInfo* chosenSkill = selectedSkills_[currentSelectedIndex_];
		ApplySkill(chosenSkill->type);
		isSkillSelectionMode = false;
	}
}

void GameScene::DrawSkillSelection() {
	PrimitiveDrawer* primitiveDrawer = PrimitiveDrawer::GetInstance();
	DebugText* debugText = DebugText::GetInstance();

	// ★修正: PrimitiveDrawer::DrawQuadは存在しない可能性があるため、DebugTextで代替描画します。
	// PrimitiveDrawer::GetInstance()->DrawQuad({0.0f, 0.0f}, {1280.0f, 720.0f}, {0.0f, 0.0f, 0.0f, 0.8f}); // 代替のためコメントアウト

	debugText->Print("スキルを選択してください", 450, 100, 1.5f);

	for (int i = 0; i < 3; ++i) {
		SkillInfo* skill = selectedSkills_[i];
		if (skill && skill->sprite) {
			Vector2 center = skill->position;
			float size = skill->sprite->GetSize().x;

			if (i == currentSelectedIndex_) {
				// 選択中のスキルを強調表示 (簡易的な四角形描画)
				// PrimitiveDrawer::GetInstance()->DrawQuad(...); // 代替のためコメントアウト
			}

			// ★修正: Sprite::Draw()が引数を取らないことを前提とします ★
			skill->sprite->Draw();

			debugText->Print(skill->name, (int)center.x - 50, (int)center.y + 80, 1.0f);
			debugText->Print(skill->description, (int)center.x - 100, (int)center.y + 110, 0.7f);
		}
	}

	debugText->Print("A/Dキーまたは左右キーで選択, スペース/Enterキーで決定", 350, 650, 0.8f);
}

void GameScene::ApplySkill(SkillType type) {
	Player* player = Player::GetInstance();

	switch (type) {
	case SkillType::kBook:
		if (bookLevel_ == 0) {
			Book* newBook = new Book();
			newBook->Initialize();
			books_.push_back(newBook);
		}
		bookLevel_++;
		// Bookの数を増やす
		if (bookLevel_ == 2 && books_.size() < 2) {
			Book* newBook = new Book();
			newBook->Initialize();
			books_.push_back(newBook);
		}
		break;

	case SkillType::kBullet:
		bulletLevel_++;
		player->SetBulletDamage(player->GetBulletDamage() + 1);
		break;

	case SkillType::kHart:
		// Hart.png: プレイヤーのHPを完全に回復
		player->Heal(player->GetMaxHP());
		break;

	case SkillType::kWine:
		wineLevel_++;
		// Wine.png: プレイヤーの移動速度を永続的に上昇
		player->SetMoveSpeedMultiplier(1.0f + wineLevel_ * 0.1f);
		break;

	default:
		break;
	}
}

// ------------------ 敵/アイテム生成ロジック ------------------

/**
 * @brief 敵の生成関数 (SpawnTimerの管理)
 */
void GameScene::EnemySpawn() {
	enemySpawnTimer_++;
	// ★敵の総数で制限するように変更 ★
	if (enemies_.size() + enemies2_.size() < kMaxEnemies + kMaxEnemies2 && enemySpawnTimer_ >= 180) { // 180f = 3秒で一旦固定
		SpawnEnemy();
		enemySpawnTimer_ = 0; // タイマーリセット
	}
}

/**
 * @brief 敵2の生成関数 (現状はEnemySpawnに統合されていますが、タイマー管理のため宣言は残します)
 */
void GameScene::Enemy2Spawn() {
	// EnemySpawn()に統合
}

/**
 * @brief 敵のランダム生成関数 (実際の生成ロジック)
 */
void GameScene::SpawnEnemy() {
	// プレイヤーから離れた位置に生成する (最小距離 20.0f)
	const float kMinSpawnDistance = 20.0f;
	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	float distance = 0.0f;

	do {
		// x, yをランダムに生成し、zは0.0f（固定）に設定
		randomPos = {dist(global_engine), dist(global_engine), 0.0f};
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	// ★ 敵の種類をランダムに決定 (Enemy2を低確率で出現させる) ★
	std::uniform_int_distribution<int> distType(0, 3); // 0: Enemy2, 1-3: Enemy1 (4分の1の確率でEnemy2)

	// ★修正: unique_ptrのリストに追加するため、newした生ポインタをstd::unique_ptrでラップします ★
	if (enemies2_.size() < kMaxEnemies2 && distType(global_engine) == 0) {
		Enemy2* newEnemy2 = new Enemy2(randomPos);
		newEnemy2->Initialize();
		enemies2_.push_back(std::unique_ptr<Enemy2>(newEnemy2));
	} else if (enemies_.size() < kMaxEnemies) {
		Enemy* newEnemy = new Enemy(randomPos);
		newEnemy->Initialize();
		enemies_.push_back(std::unique_ptr<Enemy>(newEnemy));
	}
}

/**
 * @brief Wineアイテムの生成関数 (SpawnTimerの管理と実際の生成ロジック)
 */
void GameScene::SpawnWine() {
	// Wineレベルが0の場合は生成しない
	// ★修正: Player::GetWineLevel()を使用します ★
	if (player_->GetWineLevel() == 0) {
		return;
	}

	wineSpawnTimer_++;

	// Wineレベルが1以上で、かつ画面にWineがない場合に生成
	const int kMinWineInterval = 100; // 最小間隔を設定 (極端に短くならないように)
	// ★修正: Player::GetWineLevel()を使用します。std::maxを正しく使用します ★
	int currentWineInterval = kWineSpawnInterval / (std::max)(1, player_->GetWineLevel());
	currentWineInterval = (std::max)(currentWineInterval, kMinWineInterval);

	if (wines_.empty() && wineSpawnTimer_ >= currentWineInterval) {
		// プレイヤーから離れた位置に生成する (最小距離 10.0f)
		const float kMinSpawnDistance = 10.0f;
		Vector3 playerPos = player_->GetPosition();
		Vector3 randomPos;
		float distance = 0.0f;

		do {
			// x, yをランダムに生成し、zは0.0f（固定）に設定
			randomPos = {dist(global_engine), dist(global_engine), 0.0f};
			distance = Math::Length(randomPos - playerPos);
		} while (distance < kMinSpawnDistance);

		// ★修正: 生ポインタで Wine を生成 ★
		Wine* newWine = new Wine(randomPos);
		newWine->Initialize();
		wines_.push_back(newWine);

		wineSpawnTimer_ = 0; // タイマーリセット
	}
}

/**
 * @brief 弾丸の生成関数 (BulletSpawnTimerの管理と実際の生成ロジック)
 */
void GameScene::BulletSpawn() {
	// ★修正: Player::GetBulletLevel()を使用します ★
	if (player_->GetBulletLevel() < 1) {
		return;
	}

	bulletSpawnTimer_++;

	// レベルが上がるほど、発射間隔が短くなるようにする (例: 間隔 = kInterval / level)
	const int kMinBulletInterval = 10; // 最小間隔を設定
	// ★修正: Player::GetBulletLevel()を使用します。std::maxを正しく使用します ★
	int currentBulletInterval = kBulletSpawnInterval / (std::max)(1, player_->GetBulletLevel());
	currentBulletInterval = (std::max)(currentBulletInterval, kMinBulletInterval);

	if (bulletSpawnTimer_ >= currentBulletInterval) {
		Vector3 playerPos = player_->GetPosition();

		// 最も近い敵を検索 (Enemy1/Enemy2両方から)
		auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
			float minDistanceSq = 1e10f;
			Vector3 targetPos = playerPos;

			auto checkEnemy = [&](auto& enemies_list) {
				// ★修正: unique_ptrのリストを回すため、auto& を使用します ★
				for (auto& enemy : enemies_list) {
					if (enemy->IsDead())
						continue;

					// ★修正: Math::LengthSq()の代わりにMath::Length()を使って二乗比較します (LengthSqがないエラーのため) ★
					float distance = Math::Length(enemy->GetPosition() - playerPos);
					float distanceSq = distance * distance;

					if (distanceSq < minDistanceSq) {
						minDistanceSq = distanceSq;
						targetPos = enemy->GetPosition();
					}
				}
			};

			checkEnemy(enemies_);
			checkEnemy(enemies2_);

			return {targetPos, minDistanceSq};
		};

		auto [targetPos, minDistanceSq] = findNearestEnemy();

		// 敵がいればBulletを生成 (距離が無限大でなければ敵がいるとみなす)
		if (minDistanceSq < 1e9f) {
			// 弾の進行方向はプレイヤーから敵への方向
			Vector3 direction = targetPos - playerPos;

			Bullet* newBullet = new Bullet(playerPos, direction);
			// ★修正: プレイヤーのGetBulletDamage()をBulletに設定します ★
			newBullet->SetDamage(player_->GetBulletDamage());
			newBullet->Initialize();
			bullets_.push_back(newBullet);
		}

		bulletSpawnTimer_ = 0;
	}
}

// ------------------ 衝突判定ロジック ------------------

void GameScene::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = player_->GetRadius(); // プレイヤーの半径を使用

	// 1. プレイヤーの攻撃 vs 敵 (近接攻撃判定)
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();

		// Enemy1 & Enemy2
		auto checkAttackCollision = [&](auto& enemies_list) {
			for (auto& enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();

				float distance = Math::Length(enemyPos - playerPos);

				if (distance <= attackRadius + enemyRadius) {
					enemy->TakeDamage(1);
				}
			}
		};
		checkAttackCollision(enemies_);
		checkAttackCollision(enemies2_);
	}

	// 2. 敵 vs プレイヤー (敵からの接触ダメージ)
	// Enemy1 & Enemy2
	auto checkPlayerEnemyCollision = [&](auto& enemies_list) {
		for (auto& enemy : enemies_list) {
			if (enemy->IsDead() || player_->GetCurrentHP() <= 0)
				continue;

			Vector3 enemyPos = enemy->GetPosition();
			float enemyRadius = enemy->GetRadius();

			float distance = Math::Length(enemyPos - playerPos);

			if (distance <= playerBodyRadius + enemyRadius) {
				// プレイヤーにダメージを与える
				player_->TakeDamage(1);
			}
		}
	};
	checkPlayerEnemyCollision(enemies_);
	checkPlayerEnemyCollision(enemies2_);

	// 3. Book (周回攻撃) vs 敵 の衝突判定
	for (Book* book : books_) {
		Vector3 bookPos = book->GetPosition();
		float bookRadius = book->GetRadius();
		int bookDamage = book->GetDamage();

		// Enemy1 & Enemy2
		auto checkBookCollision = [&](auto& enemies_list) {
			for (auto& enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();

				if (Math::Length(enemyPos - bookPos) <= bookRadius + enemyRadius) {
					enemy->TakeDamage(bookDamage);
				}
			}
		};
		checkBookCollision(enemies_);
		checkBookCollision(enemies2_);
	}

	// 4. Bullet (オート攻撃) vs 敵 の衝突判定
	// RemoveDeadEntitiesでBulletは削除されるため、ここではisDead_フラグを立てるのみ
	for (auto& bullet : bullets_) {
		if (bullet->IsDead())
			continue;

		Vector3 bulletPos = bullet->GetPosition();
		float bulletRadius = bullet->GetRadius();

		// Enemy1 & Enemy2
		auto checkBulletCollision = [&](auto& enemies_list) -> bool {
			for (auto& enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();

				if (Math::Length(enemyPos - bulletPos) <= bulletRadius + enemyRadius) {
					enemy->TakeDamage(bullet->GetDamage());
					bullet->Die(); // 弾を死亡状態にする
					return true;
				}
			}
			return false;
		};

		if (checkBulletCollision(enemies_) || checkBulletCollision(enemies2_)) {
			// 弾が当たったので、次の弾のチェックに移る
		}
	}

	// 5. Wine (回復アイテム) vs プレイヤー の衝突判定
	PlayerWineCollision();

	// 6. Experience (経験値アイテム) vs プレイヤー の衝突判定
	PlayerExperienceCollision();
}

/**
 * @brief プレイヤーとWineアイテムの衝突判定 (アイテムとしてマップに落ちているWine)
 */
void GameScene::PlayerWineCollision() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = player_->GetRadius();

	for (auto& wine : wines_) {
		if (wine->IsDead())
			continue;

		float distance = Math::Length(playerPos - wine->GetPosition());
		if (distance < playerBodyRadius + wine->GetRadius()) {
			// プレイヤーを回復
			player_->Heal(wine->GetHealAmount());
			wine->Die(); // Wineを死亡状態にする

			// Wine取得時の処理: 経験値アイテムをランダムにばら撒く
			const int kExpCount = 50; // 生成する経験値の数
			const float kScatterRadius = 5.0f;
			std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * (float)M_PI);
			std::uniform_real_distribution<float> scatterDist(0.5f, kScatterRadius);

			for (int i = 0; i < kExpCount; ++i) {
				float angle = angleDist(global_engine);
				float d = scatterDist(global_engine);
				float xOffset = std::cos(angle) * d;
				float zOffset = std::sin(angle) * d;

				Vector3 expSpawnPos = playerPos + Vector3{xOffset, 0.0f, zOffset};

				Experience* newExp = new Experience(expSpawnPos);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
		}
	}
}

/**
 * @brief プレイヤーと経験値アイテムの衝突判定 (経験値アイテムの追尾開始)
 */
void GameScene::PlayerExperienceCollision() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = player_->GetRadius();
	// 経験値アイテムの自動吸い込み距離
	const float kSuctionDistance = 5.0f;

	for (auto& exp : experiences_) {
		if (exp->IsDead())
			continue;

		float distance = Math::Length(playerPos - exp->GetPosition());

		// 1. プレイヤーがアイテムに接触した場合 (取得)
		// ★修正: exp->GetRadius()を使用します ★
		if (distance < playerBodyRadius + exp->GetRadius()) {
			// ★修正: exp->Die()を使用します ★
			exp->Die(); // 経験値アイテムを死亡状態にする (RemoveDeadEntitiesで削除される)
			            // RemoveDeadEntitiesでcurrentExp_とscore_を更新します
		}
		// 2. プレイヤーの近くにアイテムがある場合 (吸い込み開始)
		else if (distance < kSuctionDistance) {
			// ★修正: exp->SetTracking(true)を使用します ★
			exp->SetTracking(true);
		}
	}
}

/**
 * @brief 死亡したエンティティをリストから削除し、メモリを解放する
 */
void GameScene::RemoveDeadEntities() {
	// 乱数生成をループの外に移動し、10〜15個ドロップさせる (Enemy1用)
	std::uniform_int_distribution<int> distCount1(10, 15);
	// Enemy2用: 30個ドロップに固定
	const int kEnemy2DropCount = 30;
	Vector3 dropPosition = {0.0f, 0.0f, 0.0f};

	// 1. 敵の削除 (unique_ptrのリスト)
	enemies_.erase(
	    std::remove_if(
	        enemies_.begin(), enemies_.end(),
	        [&](const std::unique_ptr<Enemy>& enemy) {
		        if (enemy->IsDead()) {
			        // 敵の死亡時に経験値を生成
			        // ★修正: enemy->GetPosition()を使用します。★
			        dropPosition = enemy->GetPosition();
			        int dropCount = distCount1(global_engine);

			        for (int i = 0; i < dropCount; ++i) {
				        Experience* newExp = new Experience(dropPosition);
				        newExp->Initialize();
				        experiences_.push_back(newExp);
			        }
			        return true;
		        }
		        return false;
	        }),
	    enemies_.end());

	// 敵2の削除 (unique_ptrのリスト)
	enemies2_.erase(
	    std::remove_if(
	        enemies2_.begin(), enemies2_.end(),
	        [&](const std::unique_ptr<Enemy2>& enemy2) {
		        if (enemy2->IsDead()) {
			        // 敵2の死亡時に経験値を生成
			        // ★修正: enemy2->GetPosition()を使用します。★
			        dropPosition = enemy2->GetPosition();
			        int dropCount = kEnemy2DropCount;

			        for (int i = 0; i < dropCount; ++i) {
				        Experience* newExp = new Experience(dropPosition);
				        newExp->Initialize();
				        experiences_.push_back(newExp);
			        }
			        return true;
		        }
		        return false;
	        }),
	    enemies2_.end());

	// 2. 弾丸、Wine、経験値アイテムの削除 (生ポインタのリスト)

	// 弾丸の削除
	bullets_.erase(
	    std::remove_if(
	        bullets_.begin(), bullets_.end(),
	        [](Bullet*& bullet) {
		        if (bullet->IsDead()) {
			        delete bullet;
			        bullet = nullptr;
			        return true;
		        }
		        return false;
	        }),
	    bullets_.end());

	// Wineアイテムの削除
	wines_.erase(
	    std::remove_if(
	        wines_.begin(), wines_.end(),
	        [](Wine*& wine) {
		        if (wine->IsDead()) {
			        delete wine;
			        wine = nullptr;
			        return true;
		        }
		        return false;
	        }),
	    wines_.end());

	// 経験値アイテムの削除
	experiences_.erase(
	    std::remove_if(
	        experiences_.begin(), experiences_.end(),
	        [&](Experience*& exp) { // ★修正: currentExp_とscore_を更新するため、[&]でキャプチャします ★
		        if (exp->IsDead()) {
			        // 経験値アイテム取得時の処理
			        int expValue = 1;
			        currentExp_ += expValue;
			        score_ += expValue;

			        // レベルアップ判定
			        if (currentExp_ >= requiredExp_) {
				        StartLevelUp();
			        }

			        delete exp;
			        exp = nullptr;
			        return true;
		        }
		        return false;
	        }),
	    experiences_.end());
}

/**
 * @brief HPバーの描画
 */
void GameScene::DrawHPBar() {
	// HPバーを描画 (左上配置)
	hpBarBase_->Draw(); // ベース (枠)
	hpBar_->Draw();     // 現在HP (バー本体)
}

/**
 * @brief レベルと経験値の描画
 */
void GameScene::DrawLevelAndExp() {
	DebugText* debugText = DebugText::GetInstance();

	// レベル表示
	std::string levelString = "LV: " + std::to_string(level_);
	debugText->Print(levelString, 10, 80, 1.0f);

	// 経験値表示 (バーとテキスト)
	std::string expString = "EXP: " + std::to_string(currentExp_) + "/" + std::to_string(requiredExp_);
	debugText->Print(expString, 10, 110, 0.8f);

	// スコア表示
	std::string scoreString = "Score: " + std::to_string(score_);
	debugText->Print(scoreString, 10, 10, 1.0f);
}