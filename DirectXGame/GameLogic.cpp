#include "GameLogic.h"
#include <random>
#include <string>
#include <fstream> // 追加
#include <sstream> // 追加
#include <algorithm> // 追加

// GameScene.cppの匿名名前空間内の乱数生成器を移動
namespace {
std::random_device seed_gen;
std::mt19937 engine(seed_gen());
// マップの範囲を定義
const float MAP_HALF_RANGE = 50.0f;
std::uniform_real_distribution<float> dist(-MAP_HALF_RANGE, MAP_HALF_RANGE);

//const float kScreenDamageRadius = 100.0f; // プレイヤー中心から15.0f以上離れた敵にはダメージを与えない

} // namespace

// GameScene.cppから移動
const float PI = 3.14159265358979323846f;

GameLogic::GameLogic(Player* player, BIt_Map_Font* font, KamataEngine::Sprite* hpBar, KamataEngine::Sprite* hpBarBase, KamataEngine::Sprite* expBar, KamataEngine::Sprite* expBarBase)
    : player_(player), font_(font), hpBar_(hpBar), hpBarBase_(hpBarBase), expBar_(expBar), expBarBase_(expBarBase) {

	// Audioインスタンスの取得とサウンドのロードをGameLogicで行う
	audio_ = KamataEngine::Audio::GetInstance(); //
	// 敵死亡時の効果音 (既存)
	soundHandleEnemyDie_ = audio_->LoadWave("audio/se/enemyDie.wav"); //
	// ★追加: プレイヤーの攻撃音をロード ★
	soundHandlePlayerAttack_ = audio_->LoadWave("audio/se/panti.wav"); //
	 
	// ★追加: PlayerにAudioインスタンスと攻撃音ハンドルを渡す ★
	player_->SetAudio(audio_, soundHandlePlayerAttack_); //
}

GameLogic::~GameLogic() {
	// 1. リスト内の全オブジェクトを解放
	for (Book* obj : books_) {
		delete obj;
	}
	books_.clear();
	for (Bullet* obj : bullets_) {
		delete obj;
	}
	bullets_.clear();
	for (Boomerang* obj : boomerangs_) {
		delete obj;
	}
	boomerangs_.clear();
	for (Minion* obj : minions_) {
		delete obj;
	}
	minions_.clear();
	for (Missile* obj : missiles_) {
		delete obj;
	}
	missiles_.clear();
	for (Wine* obj : wines_) {
		delete obj;
	}
	wines_.clear();
	for (Enemy* obj : enemies_) {
		delete obj;
	}
	enemies_.clear();
	for (EnemyBullet* obj : enemyBullets_) {
		delete obj;
	}
	enemyBullets_.clear();
	for (Experience* obj : experiences_) {
		delete obj;
	}
	experiences_.clear();

	//// 2. GameLogicが所有するモデルの解放 (Model::CreateFromOBJ()がnewで生成する場合)
	//delete modelBook_;
	//delete modelBullet_;
	//delete modelBoomerang_;
	//delete modelMinion_;
	//delete modelMissile_;
	//delete modelExperience_;
	//delete modelWine_;

	// 3. GameLogicが所有するUI要素の解放
	for (int i = 0; i < 3; ++i) {
		delete skillIconSprites_[i];
	}
}

void GameLogic::Initialize() {
	// GameScene::Initializeから移動: リストのクリアとタイマー、レベルアップ関連の初期化
	for (Experience* exp : experiences_) {
		delete exp;
	}
	experiences_.clear();
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	for (Enemy2* enemy2 : enemies2_) {
		delete enemy2;
	}
	enemies2_.clear();
	for (Enemy3* enemy2 : enemies3_) {
		delete enemy2;
	}
	enemies3_.clear();
	for (Enemy4* enemy4 : enemies4_) { // ★ 追記 ★
		delete enemy4;
	}
	enemies4_.clear(); // ★ 追記 ★
	for (Enemy5* enemy5 : enemies5_) {
		delete enemy5;
	}
	enemies5_.clear();
	for (EnemyBullet* enemyBullet : enemyBullets_) { // ★ 追記: 敵弾の解放 ★
		delete enemyBullet;
	}
	enemyBullets_.clear(); // ★ 追記: 敵弾の解放 ★
	for (Bullet* bullet : bullets_) {
		delete bullet;
	}
	bullets_.clear();
	for (Book* book : books_) {
		delete book;
	}
	books_.clear();
	for (Wine* wine : wines_) {
		delete wine;
	}
	wines_.clear();
	for (Boomerang* boomerang : boomerangs_) {
		delete boomerang;
	}
	boomerangs_.clear();
	for (Minion* minion : minions_) {
		delete minion;
	}
	minions_.clear();
	for (Missile* missile : missiles_) {
		delete missile;
	}
	missiles_.clear();

	bulletSpawnTimer_ = 0;
	wineSpawnTimer_ = 0;
	boomerangSpawnTimer_ = 0;
	missileSpawnTimer_ = 0;
	enemySpawnTimer_ = 0; // 追記

	level_ = 1;
	currentExp_ = 0;
	score_ = 0;
	requiredExp_ = kExpBase;
	isLevelUpPending_ = false;
	selectedSkillIndex_ = 0;

#ifdef _DEBUG
	// ユーザーが指定した初期サイズをここで設定します
	iconSize_ = {256.0f, 222.0f};
#endif

	// ★★★ Audioの初期化とサウンドロード ★★★
	audio_ = KamataEngine::Audio::GetInstance(); // Audioシングルトンインスタンスの取得

	// 既存のWAVファイルを割り当て（ファイルがResourcesフォルダにある前提）
	soundHandleLevelUp_ = audio_->LoadWave("audio/se/Levelup.wav");      // レベルアップファンファーレ
	soundHandleSelectCursor_ = audio_->LoadWave("audio/se/mokugyo.wav"); // スキル選択カーソル移動音

	// 他の効果音は、一旦既存の音源を仮で割り当てます。
	// 実際に使用する際は、適切なWAVファイル名に置き換えてください。
	// 例: soundHandleEnemyDie_ = audio_->LoadWave("enemy_die.wav");
	soundHandleEnemyDie_ = audio_->LoadWave("audio/se/enemyDie.wav");
	soundHandleSkillDecide_ = audio_->LoadWave("audio/se/enter.wav");
	soundHandleBulletShot_ = audio_->LoadWave("audio/se/shot.wav");
	soundHandleDamage_ = audio_->LoadWave("audio/se/damege.wav");
	soundHandleHeal_ = audio_->LoadWave("audio/se/heal01.wav");
	soundHandleBookSpawn_ = audio_->LoadWave("audio/se/Book01.wav");
	soundHandleBoomerangShot_ = audio_->LoadWave("audio/se/bumeran.wav");
	soundHandleMissileShot_ = audio_->LoadWave("audio/se/hanabi.wav");
	soundHandleMinionShot_ = audio_->LoadWave("audio/se/Inspiration.wav");

	// 既存スキルのテクスチャロード (アップロードされたファイル名と対応)
	skillTextureHandles_[static_cast<int>(SkillType::kBook)] = KamataEngine::TextureManager::Load("Sukill/book.png");  //
	skillTextureHandles_[static_cast<int>(SkillType::kBullet)] = KamataEngine::TextureManager::Load("Sukill/Gun.png"); //
	skillTextureHandles_[static_cast<int>(SkillType::kHeart)] = KamataEngine::TextureManager::Load("Sukill/Hart.png"); //
	skillTextureHandles_[static_cast<int>(SkillType::kWine)] = KamataEngine::TextureManager::Load("Sukill/Wine.png");  //

	// 新規スキルのテクスチャ (一時的に既存のテクスチャを割り当て。必要に応じて変更してください)
	skillTextureHandles_[static_cast<int>(SkillType::kBoomerang)] = KamataEngine::TextureManager::Load("Sukill/axe.png");   // Resources/axe/axe.png を想定
	skillTextureHandles_[static_cast<int>(SkillType::kMinion)] = KamataEngine::TextureManager::Load("Sukill/Minion.png");   // Resources/player/player.png を想定
	skillTextureHandles_[static_cast<int>(SkillType::kMissile)] = KamataEngine::TextureManager::Load("Sukill/Missile.png"); // Resources/Bullet/Bullet.png を想定

	// ★★★ 修正: アイコン描画用スプライトの初期化 (iconSize_ を使用) ★★★
	uint32_t initialTextureHandle = KamataEngine::TextureManager::Load("sample.png");
	for (int i = 0; i < 3; ++i) {
		// 初期化時に正しいサイズを設定
		skillIconSprites_[i] = KamataEngine::Sprite::Create(initialTextureHandle, {0, 0});

#ifdef _DEBUG
		skillIconSprites_[i]->SetSize(iconSize_);
#else
		// Releaseビルド時は固定サイズを設定（または初期値のまま）
		skillIconSprites_[i]->SetSize({400.0f, 108.0f});
#endif
	}

	LoadEnemyPopData();
}

void GameLogic::LoadEnemyPopData() {
	enemySpawnList_.clear();

	// ファイルを開く (パスは環境に合わせて調整してください)
	std::ifstream file("Resources/enemyPop.csv");
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		// コメント行(//)や空行をスキップ
		if (line.empty() || line.find("//") == 0) {
			continue;
		}

		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(ss, segment, ',')) {
			seglist.push_back(segment);
		}

		if (seglist.size() >= 4) {
			EnemySpawnData data;
			data.enemyType = std::stoi(seglist[0]);
			data.interval = std::stoi(seglist[1]);
			data.minScore = std::stoi(seglist[2]);
			data.maxScore = std::stoi(seglist[3]);
			data.timer = 0; // タイマー初期化
			enemySpawnList_.push_back(data);
		}
	}
	file.close();
}

// ----------------------------------------------------
// GameLogic::Update (メイン更新処理)
// ----------------------------------------------------
void GameLogic::Update() {
	// --- 背景モード（自動操作）時の処理 ---
	if (player_->IsAutoMode()) {
		static int autoFireTimer = 0;
		autoFireTimer++;

		// 1. 最も近い敵を探す
		Enemy* targetEnemy = nullptr;
		float minDistance = 1000.0f;
		Vector3 playerPos = player_->GetPosition();

		for (Enemy* enemy : enemies_) {
			Vector3 enemyPos = enemy->GetPosition();
			float distance = MathUtility::Length(enemyPos - playerPos); // distをdistanceに変更して衝突回避
			if (distance < minDistance) {
				minDistance = distance;
				targetEnemy = enemy;
			}
		}

		// 2. 敵がいれば、その方向（左右）への移動ベクトルをプレイヤーに伝える
		if (targetEnemy) {
			Vector3 enemyPos = targetEnemy->GetPosition();
			Vector3 toEnemy = enemyPos - playerPos;

			// 弾が敵に向かう方向を計算
			Vector3 fireDirection = toEnemy;
			MathUtility::Normalize(fireDirection);

			// 20フレームに1回、自動で弾を発射
			if (autoFireTimer % 5 == 0) {
				// Bullet(位置, 速度)
				Bullet* newBullet = new Bullet(playerPos, fireDirection * 0.8f);
				newBullet->Initialize();
				bullets_.push_back(newBullet);
			}
		}
	}


	// 既存の更新処理を呼ぶ (エラーになっていた未定義の関数は消す)
	CheckAllCollisions(); // CheckCollisions ではなく CheckAllCollisions
	// UpdateEnemies という関数はないので、既存の敵更新ループをそのまま使う

	Vector3 playerPos = player_->GetPosition();

	// 全ての敵に対して、現在のプレイヤー座標を渡して更新する
	for (Enemy* enemy : enemies_) {
		enemy->Update(playerPos);
	}

	// HPバーの更新 (GameScene::Updateから移動)
	int currentHp = player_->GetCurrentHp();
	int maxHp = player_->GetMaxHp();

	float hpRatio = (float)currentHp / maxHp;
	if (hpRatio < 0.0f) {
		hpRatio = 0.0f;
	}
	float newWidth = hpBarBase_->GetSize().x * hpRatio;
	Vector2 currentSize = hpBar_->GetSize();
	hpBar_->SetSize({newWidth, currentSize.y});

	// ★★★ 追記: EXPバーの更新 ★★★
	float expRatio = (float)currentExp_ / requiredExp_;
	if (expRatio > 1.0f) { // 経験値が要求経験値を超えた場合（レベルアップが未処理の場合）
		expRatio = 1.0f;
	}
	if (expRatio < 0.0f) {
		expRatio = 0.0f;
	}
	float newExpWidth = expBarBase_->GetSize().x * expRatio;
	Vector2 currentExpSize = expBar_->GetSize();
	expBar_->SetSize({newExpWidth, currentExpSize.y});
	// ---------------------------------

	if (isLevelUpPending_) {
		return;
	}

	// スコア表示の更新
	font_->Set(score_);

	// --- 修正後（CSVデータを使う形にする） ---
	for (auto& spawnData : enemySpawnList_) {
		// 現在のスコアが範囲内かチェック
		if (score_ >= spawnData.minScore && score_ <= spawnData.maxScore) {
			spawnData.timer++;
			if (spawnData.timer >= spawnData.interval) {
				// CSVで指定されたタイプの敵を出現させる
				SpawnEnemy(spawnData.enemyType);
				spawnData.timer = 0;
			}
		}
	}

	// Wineの生成
	wineSpawnTimer_++;
	const int kMinWineInterval = 100;
	int currentWineInterval = kWineSpawnInterval / (std::max)(1, player_->GetWineLevel());
	currentWineInterval = (std::max)(currentWineInterval, kMinWineInterval);

	if (player_->GetWineLevel() >= 1 && wines_.empty() && wineSpawnTimer_ >= currentWineInterval) {
		SpawnWine();
		wineSpawnTimer_ = 0;
	}

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update(playerPos);
	}
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Update(playerPos);

		// ★★★ 追記: Enemy2の発射ロジック (Enemy3/4と同様) ★★★
		if (enemy2->CanShoot() && enemyBullets_.size() < kMaxEnemyBullets) {
			enemy2->ResetShotTimer();

			// プレイヤーへの方向ベクトルを計算
			Vector3 diff = playerPos - enemy2->GetShotPosition();
			Vector3 direction = Math::Normalize(diff);

			// 速度を設定
			// Enemy3やEnemy4と差別化するため、ここでは弾速を0.8fに設定します
			const float kEnemyBulletSpeed = 1.2f;
			Vector3 velocity = direction * kEnemyBulletSpeed;

			// 弾を生成
			EnemyBullet* newBullet = new EnemyBullet(enemy2->GetShotPosition(), velocity);
			// 弾のダメージは Enemy3(10) や Enemy4(70) より低めの5に設定します
			newBullet->SetDamage(30);
			newBullet->Initialize();
			enemyBullets_.push_back(newBullet);
		}
		// ★★★ 追記ここまで ★★★
	}
	for (Enemy3* enemy3 : enemies3_) {
		enemy3->Update(playerPos);

		// ★ 追記: Enemy3の発射ロジック (Enemy4と同一) ★
		if (enemy3->CanShoot() && enemyBullets_.size() < kMaxEnemyBullets) {
			enemy3->ResetShotTimer();

			// プレイヤーへの方向ベクトルを計算
			Vector3 diff = playerPos - enemy3->GetShotPosition();
			Vector3 direction = Math::Normalize(diff);

			// 速度を設定
			// kEnemyBulletSpeedは下記で定義されているため、ここでは流用
			const float kEnemyBulletSpeed = 1.0f;
			Vector3 velocity = direction * kEnemyBulletSpeed;

			// 弾を生成
			EnemyBullet* newBullet = new EnemyBullet(enemy3->GetShotPosition(), velocity);
			newBullet->SetDamage(10); // ★ Enemy3の弾はダメージ1に設定 (Enemy4の弾と差別化) ★
			newBullet->Initialize();
			enemyBullets_.push_back(newBullet);
		}
	}
	for (Enemy4* enemy4 : enemies4_) {
		enemy4->Update(playerPos);

		// ★ 追記: Enemy4の発射ロジック ★
		if (enemy4->CanShoot() && enemyBullets_.size() < kMaxEnemyBullets) {
			enemy4->ResetShotTimer();

			// プレイヤーへの方向ベクトルを計算
			Vector3 diff = playerPos - enemy4->GetShotPosition();
			Vector3 direction = Math::Normalize(diff);

			// 速度を設定
			const float kEnemyBulletSpeed = 0.6f;
			Vector3 velocity = direction * kEnemyBulletSpeed;

			// 弾を生成
			EnemyBullet* newBullet = new EnemyBullet(enemy4->GetShotPosition(), velocity);
			newBullet->SetDamage(70); // ★ Enemy4の弾はダメージ3に設定 ★
			newBullet->Initialize();
			enemyBullets_.push_back(newBullet);
		}
	}
	for (Enemy5* enemy5 : enemies5_) {
		enemy5->Update(playerPos);
	}
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}

	// Book (周回攻撃) の更新
	for (Book* book : books_) {
		book->Update(playerPos);
	}

	// Bulletの自動生成と更新
	if (player_->GetBulletLevel() >= 1) {
		bulletSpawnTimer_++;
		const int kMinBulletInterval = 10;
		int currentBulletInterval = kBulletSpawnInterval / (std::max)(1, player_->GetBulletLevel());
		currentBulletInterval = (std::max)(currentBulletInterval, kMinBulletInterval);

		if (bulletSpawnTimer_ >= currentBulletInterval) {
			auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
				float minDistanceSq = 1e10f;
				Vector3 targetPos = playerPos;
				auto checkEnemy = [&](auto& enemies_list) {
					for (auto enemy : enemies_list) {
						if (enemy->IsDead())
							continue;
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
				checkEnemy(enemies3_);
				checkEnemy(enemies4_);
				return {targetPos, minDistanceSq};
			};

			auto [targetPos, minDistanceSq] = findNearestEnemy();

			if (minDistanceSq < 1e9f) {
				Vector3 velocity = targetPos - playerPos;
				Bullet* newBullet = new Bullet(playerPos, velocity);
				newBullet->Initialize();
				bullets_.push_back(newBullet);

				audio_->PlayWave(soundHandleBulletShot_);
			}
			bulletSpawnTimer_ = 0;
		}
	}
	for (Bullet* bullet : bullets_) {
		bullet->Update();
	}

	// Boomerang の自動生成と更新
	if (player_->GetBoomerangLevel() >= 1) {
		boomerangSpawnTimer_++;
		const int kMinBoomerangInterval = 30;
		int currentBoomerangInterval = kBoomerangSpawnInterval / (std::max)(1, player_->GetBoomerangLevel());
		currentBoomerangInterval = (std::max)(currentBoomerangInterval, kMinBoomerangInterval);

		if (boomerangSpawnTimer_ >= currentBoomerangInterval) {
			Vector3 velocity;
			auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
				float minDistanceSq = 1e10f;
				Vector3 targetPos = playerPos;
				auto checkEnemy = [&](auto& enemies_list) {
					for (auto enemy : enemies_list) {
						if (enemy->IsDead())
							continue;
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
				checkEnemy(enemies3_);
				checkEnemy(enemies4_);
				return {targetPos, minDistanceSq};
			};

			auto [targetPos, minDistanceSq] = findNearestEnemy();

			if (minDistanceSq < 1e9f) {
				velocity = targetPos - playerPos;
			} else {
				std::uniform_real_distribution<float> angleDist(0.0f, PI * 2.0f);
				float randomAngle = angleDist(engine);
				velocity.x = std::cos(randomAngle);
				velocity.y = std::sin(randomAngle);
			}

			Boomerang* newBoomerang = new Boomerang(playerPos, velocity);
			newBoomerang->Initialize();
			boomerangs_.push_back(newBoomerang);

			audio_->PlayWave(soundHandleBoomerangShot_);
			boomerangSpawnTimer_ = 0;
		}
	}
	for (Boomerang* boomerang : boomerangs_) {
		boomerang->Update(playerPos);
	}

	// Minion の更新と攻撃
	for (Minion* minion : minions_) {
		minion->Update(playerPos);
	}
	if (player_->GetMinionLevel() >= 1) {
		for (Minion* minion : minions_) {
			if (minion->CanAttack()) {
				auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
					float minDistanceSq = 1e10f;
					Vector3 targetPos = playerPos;
					auto checkEnemy = [&](auto& enemies_list) {
						for (auto enemy : enemies_list) {
							if (enemy->IsDead())
								continue;
							float distance = Math::Length(enemy->GetPosition() - minion->GetPosition());
							float distanceSq = distance * distance;
							if (distanceSq < minDistanceSq) {
								minDistanceSq = distanceSq;
								targetPos = enemy->GetPosition();
							}
						}
					};
					checkEnemy(enemies_);
					checkEnemy(enemies2_);
					checkEnemy(enemies3_);
					checkEnemy(enemies4_);
					return {targetPos, minDistanceSq};
				};

				auto [targetPos, minDistanceSq] = findNearestEnemy();

				if (minDistanceSq < 1e9f) {
					Vector3 velocity = targetPos - minion->GetPosition();
					Bullet* newBullet = new Bullet(minion->GetPosition(), velocity);
					newBullet->SetDamage(minion->GetDamage() + player_->GetMinionLevel() / 2);
					newBullet->Initialize();
					bullets_.push_back(newBullet);

					audio_->PlayWave(soundHandleMinionShot_);
				}
				minion->ResetAttackTimer();
			}
		}
	}

	// Missile の自動生成と更新
	if (player_->GetMissileLevel() >= 1) {
		missileSpawnTimer_++;
		const int kMinMissileInterval = 20;
		int currentMissileInterval = kMissileSpawnInterval / (std::max)(1, player_->GetMissileLevel());
		currentMissileInterval = (std::max)(currentMissileInterval, kMinMissileInterval);

		if (missileSpawnTimer_ >= currentMissileInterval) {
			auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
				float minDistanceSq = 1e10f;
				Vector3 targetPos = playerPos;
				auto checkEnemy = [&](auto& enemies_list) {
					for (auto enemy : enemies_list) {
						if (enemy->IsDead())
							continue;
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
				checkEnemy(enemies3_);
				checkEnemy(enemies4_);
				return {targetPos, minDistanceSq};
			};

			auto [targetPos, minDistanceSq] = findNearestEnemy();

			if (minDistanceSq < 1e9f) {
				Missile* newMissile = new Missile(playerPos, targetPos);
				newMissile->Initialize();
				missiles_.push_back(newMissile);

				audio_->PlayWave(soundHandleMissileShot_);
			}
			missileSpawnTimer_ = 0;
		}
	}

	auto findNearestEnemyForMissileHoming = [&]() -> Vector3 {
		float minDistanceSq = 1e10f;
		Vector3 targetPos = playerPos;
		auto checkEnemy = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
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
		checkEnemy(enemies3_);
		checkEnemy(enemies4_);
		return targetPos;
	};
	Vector3 nearestEnemyPos = findNearestEnemyForMissileHoming();

	for (Missile* missile : missiles_) {
		missile->Update(nearestEnemyPos);
	}

	// 衝突判定の実行
	CheckAllCollisions();

	// 経験値アイテムの更新・削除
	for (Experience* exp : experiences_) {
		exp->Update(playerPos);
	}
	for (Wine* wine : wines_) {
		wine->Update(playerPos);
	}

	// GameLogic.cpp の Update() 内 - 敵弾の削除処理
	// ★ 追記: 敵弾の削除処理 ★
	for (auto it = enemyBullets_.rbegin(); it != enemyBullets_.rend();) {
		EnemyBullet* enemyBullet = *it;
		if (enemyBullet->IsDead()) {
			delete enemyBullet;
			// 正しいイテレータの再設定:
			// eraseには「削除したい要素を指す」順方向イテレータが必要。
			// reverse_iteratorの `it` が指す要素を削除するには、
			// `std::next(it).base()` または `it.base()` の調整が必要。

			// C++の標準的な書き方 (std::next(it).base()を使用):
			// (std::next(it) は「削除したい要素の次」を指す逆イテレータ)
			// (std::next(it).base() は「削除したい要素」を指す順方向イテレータ)
			auto forward_it = std::next(it).base();
			forward_it = enemyBullets_.erase(forward_it);

			// eraseの戻り値 (forward_it) は「削除された要素の次」を指す。
			// これを逆イテレータに変換すると、「削除された要素の直前」を指す逆イテレータになる。
			it = std::vector<EnemyBullet*>::reverse_iterator(forward_it);
		} else {
			++it;
		}
	}

	// アイテムの削除処理 (取得/死亡判定) とレベルアップ判定
	for (auto it = experiences_.rbegin(); it != experiences_.rend();) {
		Experience* exp = *it;
		if (exp->IsDead()) {
			int expValue = 1;
			currentExp_ += expValue;
			score_ += expValue;
			font_->Set(score_);

			if (currentExp_ >= requiredExp_) {
				StartLevelUp();
			}

			delete exp;
			it = std::vector<Experience*>::reverse_iterator(experiences_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	// 敵の削除処理 (死亡判定)
	std::uniform_int_distribution<int> distCount1(10, 15);
	const int kEnemy2DropCount = 30;

	for (auto it = enemies_.rbegin(); it != enemies_.rend();) {
		Enemy* enemy = *it;
		if (enemy->IsDead()) {

			audio_->PlayWave(soundHandleEnemyDie_);

			deadEnemyPositions_.push_back(enemy->GetPosition());

			Vector3 dropPosition = enemy->GetPosition();
			int dropCount = distCount1(engine);
			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			delete enemy;
			it = std::vector<Enemy*>::reverse_iterator(enemies_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	for (auto it = enemies2_.rbegin(); it != enemies2_.rend();) {
		Enemy2* enemy2 = *it;
		if (enemy2->IsDead()) {

			audio_->PlayWave(soundHandleEnemyDie_);

			deadEnemyPositions_.push_back(enemy2->GetPosition());

			Vector3 dropPosition = enemy2->GetPosition();
			int dropCount = kEnemy2DropCount;
			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			delete enemy2;
			it = std::vector<Enemy2*>::reverse_iterator(enemies2_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}
	for (auto it = enemies3_.rbegin(); it != enemies3_.rend();) {
		Enemy3* enemy3 = *it; // 変数名を enemy3 に修正 (元のファイルでは enemy2 になっていた)
		if (enemy3->IsDead()) {

			audio_->PlayWave(soundHandleEnemyDie_);

			deadEnemyPositions_.push_back(enemy3->GetPosition());

			Vector3 dropPosition = enemy3->GetPosition();
			int dropCount = kEnemy2DropCount;
			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			delete enemy3;
			it = std::vector<Enemy3*>::reverse_iterator(enemies3_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}
	for (auto it = enemies4_.rbegin(); it != enemies4_.rend();) { // ★ 追記 ★
		Enemy4* enemy4 = *it;
		if (enemy4->IsDead()) {

			deadEnemyPositions_.push_back(enemy4->GetPosition());

			audio_->PlayWave(soundHandleEnemyDie_);

			Vector3 dropPosition = enemy4->GetPosition();
			int dropCount = kEnemy2DropCount;
			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			delete enemy4;
			it = std::vector<Enemy4*>::reverse_iterator(enemies4_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}
	for (auto it = enemies5_.rbegin(); it != enemies5_.rend();) {
		Enemy5* enemy5 = *it;
		if (enemy5->IsDead()) {

			deadEnemyPositions_.push_back(enemy5->GetPosition());

			audio_->PlayWave(soundHandleEnemyDie_);

			Vector3 dropPosition = enemy5->GetPosition();
			int dropCount = kEnemy2DropCount; // 仮でEnemy2と同じドロップ数
			for (int i = 0; i < dropCount; ++i) {
				Experience* newExp = new Experience(dropPosition);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}
			delete enemy5;
			it = std::vector<Enemy5*>::reverse_iterator(enemies5_.erase(std::next(it).base()));
		} else {
			++it;
		}
	}

	// 弾/スキルの削除処理
	for (auto itB = bullets_.rbegin(); itB != bullets_.rend();) {
		Bullet* bullet = *itB;
		if (bullet->IsDead()) {
			delete bullet;
			itB = std::vector<Bullet*>::reverse_iterator(bullets_.erase(std::next(itB).base()));
		} else {
			++itB;
		}
	}
	for (auto itB = boomerangs_.rbegin(); itB != boomerangs_.rend();) {
		Boomerang* boomerang = *itB;
		if (boomerang->IsDead()) {
			delete boomerang;
			itB = std::vector<Boomerang*>::reverse_iterator(boomerangs_.erase(std::next(itB).base()));
		} else {
			++itB;
		}
	}
	for (auto itM = missiles_.rbegin(); itM != missiles_.rend();) {
		Missile* missile = *itM;
		if (missile->IsDead()) {
			delete missile;
			itM = std::vector<Missile*>::reverse_iterator(missiles_.erase(std::next(itM).base()));
		} else {
			++itM;
		}
	}
}

void GameLogic::SpawnEnemy(int enemyType) {
	// 敵の上限数チェック
	int currentTotalEnemies = static_cast<int>(enemies_.size() + enemies2_.size() + enemies3_.size() + enemies4_.size() + enemies5_.size());
	int maxTotalEnemies = kMaxEnemies + kMaxEnemies2 + kMaxEnemies3 + kMaxEnemies4 + kMaxEnemies5;
	if (currentTotalEnemies >= maxTotalEnemies)
		return;

	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	// 座標計算
	std::uniform_real_distribution<float> angleDist(0.0f, 6.283f);
	float angle = angleDist(engine);
	float spawnDist = 50.0f;
	randomPos.x = playerPos.x + std::cos(angle) * spawnDist;
	randomPos.y = playerPos.y + std::sin(angle) * spawnDist;
	randomPos.z = 0.0f;

	// ★★★ ここで switch文 を使って生成していますか？ ★★★
	switch (enemyType) {
	case 0:
		if (enemies_.size() < kMaxEnemies) {
			Enemy* newEnemy = new Enemy(randomPos);
			newEnemy->Initialize();
			enemies_.push_back(newEnemy);
		}
		break;
	case 1:
		if (enemies2_.size() < kMaxEnemies2) {
			Enemy2* newEnemy2 = new Enemy2(randomPos);
			newEnemy2->Initialize();
			enemies2_.push_back(newEnemy2);
		}
		break;
	case 2:
		if (enemies3_.size() < kMaxEnemies3) {
			Enemy3* newEnemy3 = new Enemy3(randomPos);
			newEnemy3->Initialize();
			enemies3_.push_back(newEnemy3);
		}
		break;
	case 3:
		if (enemies4_.size() < kMaxEnemies4) {
			Enemy4* newEnemy4 = new Enemy4(randomPos);
			newEnemy4->Initialize();
			enemies4_.push_back(newEnemy4);
		}
		break;
	case 4:
		if (enemies5_.size() < kMaxEnemies5) {
			Enemy5* newEnemy5 = new Enemy5(randomPos);
			newEnemy5->Initialize();
			enemies5_.push_back(newEnemy5);
		}
		break;
	default:
		break;
	}
}

// ----------------------------------------------------
// GameLogic::CheckAllCollisions (衝突判定)
// ----------------------------------------------------


void GameLogic::CheckAllCollisions() {
	// 最新のプレイヤー座標を再取得（ワールド座標）
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = 0.5f;

	// --- 1. プレイヤーの攻撃 vs 敵 ---
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();
		auto checkPlayerAttackCollision = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 diff = enemy->GetPosition() - playerPos;
				// 純粋な距離判定のみにする（画面外チェックは削除）
				if (Math::Length(diff) <= (attackRadius + enemy->GetRadius())) {
					enemy->TakeDamage(1);
				}
			}
		};
		checkPlayerAttackCollision(enemies_);
		checkPlayerAttackCollision(enemies2_);
		checkPlayerAttackCollision(enemies3_);
		checkPlayerAttackCollision(enemies4_);
		checkPlayerAttackCollision(enemies5_);
	}

	// --- 2. 敵 vs プレイヤー（接触） ---
	auto checkEnemyPlayerCollision = [&](auto& enemies_list) {
		for (auto enemy : enemies_list) {
			if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
				continue;

			Vector3 diff = enemy->GetPosition() - playerPos;
			if (Math::Length(diff) <= (playerBodyRadius + enemy->GetRadius())) {
				int damageToTake = (std::max)(1, 1 - player_->GetDefense());
				player_->TakeDamage(damageToTake);
				audio_->PlayWave(soundHandleDamage_);
			}
		}
	};
	checkEnemyPlayerCollision(enemies_);
	checkEnemyPlayerCollision(enemies2_);
	checkEnemyPlayerCollision(enemies3_);
	checkEnemyPlayerCollision(enemies4_);
	checkEnemyPlayerCollision(enemies5_);

	// --- 2. Book (周回攻撃) vs 敵 ---
	for (Book* book : books_) {
		Vector3 bPos = book->GetPosition();
		auto checkBook = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				if (Math::Length(enemy->GetPosition() - bPos) <= (book->GetRadius() + enemy->GetRadius())) {
					enemy->TakeDamage(book->GetDamage());
				}
			}
		};
		checkBook(enemies_);
		checkBook(enemies2_);
		checkBook(enemies3_);
		checkBook(enemies4_);
		checkBook(enemies5_);
	}

	// --- 3. Bullet (オート攻撃) vs 敵 ---
	for (auto itB = bullets_.begin(); itB != bullets_.end();) {
		Bullet* bullet = *itB;
		if (bullet->IsDead()) {
			delete bullet;
			itB = bullets_.erase(itB);
			continue;
		}
		Vector3 bulletPos = bullet->GetPosition();
		float bulletRadius = bullet->GetRadius();
		int bulletDamage = bullet->GetDamage();
		bool hit = false;

		auto checkBulletCollision = [&](auto& enemies_list) -> bool {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				Vector3 enemyPos = enemy->GetPosition();

				// ★ 修正: 画面外チェックを削除 ★
				if (Math::Length(enemyPos - bulletPos) <= (bulletRadius + enemy->GetRadius())) {
					enemy->TakeDamage(bulletDamage);
					bullet->Die();
					return true;
				}
			}
			return false;
		};

		if (checkBulletCollision(enemies_) || checkBulletCollision(enemies2_) || checkBulletCollision(enemies3_) || checkBulletCollision(enemies4_) || checkBulletCollision(enemies5_)) {
			hit = true;
		}

		if (hit) {
			delete bullet;
			itB = bullets_.erase(itB);
		} else {
			++itB;
		}
	}

	// --- 4. Wine (回復アイテム) vs プレイヤー ---
	for (auto itW = wines_.begin(); itW != wines_.end();) {
		Wine* wine = *itW;
		Vector3 winePos = wine->GetPosition();
		if (Math::Length(winePos - playerPos) <= (playerBodyRadius + wine->GetRadius())) {
			player_->Heal(wine->GetHealAmount());
			audio_->PlayWave(soundHandleHeal_);
			wine->Die();
			delete wine;
			itW = wines_.erase(itW);
		} else {
			++itW;
		}
	}

	// --- 5. Boomerang / Missile (同様に画面外チェックを削除) ---
	auto checkWeaponCollision = [&](auto& weapons_list, auto handleHit) {
		for (auto it = weapons_list.begin(); it != weapons_list.end();) {
			auto weapon = *it;
			if (weapon->IsDead()) {
				delete weapon;
				it = weapons_list.erase(it);
				continue;
			}
			bool hit = false;
			Vector3 wPos = weapon->GetPosition();
			float wRad = weapon->GetRadius();
			int wDmg = weapon->GetDamage();

			auto checkAgainstEnemies = [&](auto& enemies_list) -> bool {
				for (auto enemy : enemies_list) {
					if (enemy->IsDead())
						continue;
					if (Math::Length(enemy->GetPosition() - wPos) <= (wRad + enemy->GetRadius())) {
						enemy->TakeDamage(wDmg);
						handleHit(weapon);
						return true;
					}
				}
				return false;
			};

			if (checkAgainstEnemies(enemies_) || checkAgainstEnemies(enemies2_) || checkAgainstEnemies(enemies3_) || checkAgainstEnemies(enemies4_) || checkAgainstEnemies(enemies5_)) {
				hit = true;
			}

			if (hit && weapon->IsDead()) { // 消滅するタイプの武器なら削除
				delete weapon;
				it = weapons_list.erase(it);
			} else {
				++it;
			}
		}
	};

	checkWeaponCollision(boomerangs_, [](auto b) { b->Hit(); });
	checkWeaponCollision(missiles_, [](auto m) { m->Die(); });

	// --- 6. 敵弾 vs プレイヤー ---
	for (auto itEB = enemyBullets_.begin(); itEB != enemyBullets_.end();) {
		EnemyBullet* eb = *itEB;
		if (eb->IsDead() || player_->GetCurrentHp() <= 0) {
			++itEB;
			continue;
		}
		if (Math::Length(eb->GetPosition() - playerPos) <= (playerBodyRadius + eb->GetRadius())) {
			int damage = (std::max)(1, eb->GetDamage() - player_->GetDefense());
			player_->TakeDamage(damage);
			audio_->PlayWave(soundHandleDamage_);
			eb->SetIsDead(true);
			itEB = enemyBullets_.erase(itEB);
		} else {
			++itEB;
		}
	}
}

// ----------------------------------------------------
// GameLogic::SpawnWine (Wineのランダム生成関数)
// ----------------------------------------------------
void GameLogic::SpawnWine() {
	if (player_->GetWineLevel() == 0) {
		return;
	}

	const float kMinSpawnDistance = 10.0f;
	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	float distance = 0.0f;

	do {
		randomPos = {dist(engine), dist(engine), 0.0f};
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	if (wines_.empty()) {
		Wine* newWine = new Wine(randomPos);
		newWine->Initialize();
		wines_.push_back(newWine);
	}
}

// ----------------------------------------------------
// GameLogic::StartLevelUp (レベルアップ開始処理)
// ----------------------------------------------------
void GameLogic::StartLevelUp() {
	level_++;
	currentExp_ -= requiredExp_;
	requiredExp_ = static_cast<int>(kExpBase * std::pow(kExpScale, level_ - 1));

	isLevelUpPending_ = true;
	selectedSkillIndex_ = 0; // 選択インデックスをリセット

	// スキル選択肢をランダムに3つ生成
	std::uniform_int_distribution<int> distType(0, static_cast<int>(SkillType::kSkillCount) - 1);

	currentSkillOptions_.clear();

	while (static_cast<int>(currentSkillOptions_.size()) < 3) {
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

// ----------------------------------------------------
// GameLogic::UpdateSkillSelection (スキル選択更新)
// ----------------------------------------------------
void GameLogic::UpdateSkillSelection() {
	Input* input = KamataEngine::Input::GetInstance();

	int optionCount = static_cast<int>(currentSkillOptions_.size());

	// 上キー/下キーで選択肢を移動
	if (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP)) {
		selectedSkillIndex_ = (selectedSkillIndex_ - 1 + optionCount) % optionCount;

		audio_->PlayWave(soundHandleSelectCursor_);
	}
	if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
		selectedSkillIndex_ = (selectedSkillIndex_ + 1) % optionCount;

		audio_->PlayWave(soundHandleSelectCursor_);
	}

	// 決定キー (スペースキーやエンターキー) でスキルを適用し、ゲームを再開
	if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {

		audio_->PlayWave(soundHandleSkillDecide_);

		ApplySkill(currentSkillOptions_[selectedSkillIndex_]);
		isLevelUpPending_ = false;

		// ゲーム再開
		currentSkillOptions_.clear(); // 選択肢をクリア
	}
}

// ----------------------------------------------------
// GameLogic::ApplySkill (スキル適用処理)
// ----------------------------------------------------
void GameLogic::ApplySkill(SkillType skill) {

	audio_->PlayWave(soundHandleLevelUp_);

	switch (skill) {
	case SkillType::kBook: {
		const int kBookMaxLevel = 5;
		int currentLevel = player_->GetBookLevel();

		if (currentLevel >= kBookMaxLevel) {
			// 上限に達した場合: Bookのダメージを上げる
			for (Book* book : books_) {
				book->SetDamage(book->GetDamage() + 1);
			}
		} else {
			int newLevel = currentLevel + 1;
			player_->SetBookLevel(newLevel);

			// ★★★ 修正箇所: Bookレベルに応じて防御力を設定 ★★★
			// レベルNで防御力 N * 20 に設定
			player_->SetDefense(newLevel * 20);

			// Bookオブジェクトの再生成
			for (Book* book : books_) {
				delete book;
			}
			books_.clear();
			for (int i = 0; i < newLevel; ++i) {
				Book* newBook = new Book();
				newBook->Initialize();
				// Bookのダメージをレベルに応じて設定 (レベルNでNダメージ)
				newBook->SetDamage(newLevel);
				books_.push_back(newBook);

				audio_->PlayWave(soundHandleBookSpawn_);
			}
		}
	} break;
	case SkillType::kBullet: {
		int newLevel = player_->GetBulletLevel() + 1;
		player_->SetBulletLevel(newLevel);
	} break;
	case SkillType::kHeart:
		player_->Heal(100);

		audio_->PlayWave(soundHandleHeal_);
		break;
	case SkillType::kWine: {
		int newLevel = player_->GetWineLevel() + 1;
		player_->SetWineLevel(newLevel);
	} break;
	case SkillType::kBoomerang: {
		int newLevel = player_->GetBoomerangLevel() + 1;
		player_->SetBoomerangLevel(newLevel);
	} break;
	case SkillType::kMinion: {
		int newLevel = player_->GetMinionLevel() + 1;
		player_->SetMinionLevel(newLevel);
		for (Minion* minion : minions_) {
			delete minion;
		}
		minions_.clear();
		for (int i = 0; i < newLevel; ++i) {
			Minion* newMinion = new Minion(i, newLevel);
			newMinion->Initialize();
			minions_.push_back(newMinion);
		}
	} break;
	case SkillType::kMissile: {
		int newLevel = player_->GetMissileLevel() + 1;
		player_->SetMissileLevel(newLevel);
	} break;
	default:
		break;
	}
}

// ----------------------------------------------------
// GameLogic::DrawObjects (3Dオブジェクト描画)
// ----------------------------------------------------
void GameLogic::DrawObjects(const Camera& camera) {
	// 経験値アイテムの描画
	for (Experience* exp : experiences_) {
		exp->Draw(camera);
	}

	// Wineアイテムの描画
	for (Wine* wine : wines_) {
		wine->Draw(camera);
	}

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw(camera);
	}
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Draw(camera);
	}
	for (Enemy3* enemy3 : enemies3_) { // 変数名を enemy3 に修正
		enemy3->Draw(camera);
	}
	for (Enemy4* enemy4 : enemies4_) { // ★ 追記 ★
		enemy4->Draw(camera);
	}
	for (Enemy5* enemy5 : enemies5_) {
		enemy5->Draw(camera);
	}
	// ★ 追記: 敵弾の描画 ★
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Draw(camera);
	}

	// Bulletの描画
	for (Bullet* bullet : bullets_) {
		bullet->Draw(camera);
	}

	// Book (周回攻撃) の描画
	for (Book* book : books_) {
		book->Draw(camera);
	}

	// Boomerangの描画
	for (Boomerang* boomerang : boomerangs_) {
		boomerang->Draw(camera);
	}

	// Minionの描画
	for (Minion* minion : minions_) {
		minion->Draw(camera);
	}

	// Missileの描画
	for (Missile* missile : missiles_) {
		missile->Draw(camera);
	}
}

#ifdef _DEBUG
void GameLogic::DrawImGui() {

	// デバッグウィンドウの開始
	if (ImGui::Begin("GameLogic Debug", nullptr, ImGuiWindowFlags_MenuBar)) {

		ImGui::Text("Skill Icon Settings");
		ImGui::Separator();

		// ImGui::SliderFloat2 で iconSize_ を操作
		ImGui::SliderFloat2("Icon Size", &iconSize_.x, 10.0f, 400.0f, "Width: %.1f, Height: %.1f");

		ImGui::Separator();
		ImGui::Text("Level Up");
		ImGui::Text("Player Level: %d", level_);
		ImGui::Text("Current Exp: %d / %d", currentExp_, requiredExp_);
		if (ImGui::Button("FORCE LEVEL UP")) {
			StartLevelUp();
		}

		ImGui::End();
	}
}
#endif // _DEBUG

// ----------------------------------------------------
// GameLogic::DrawSkillSelectionUI (スキル選択UI描画)
// ----------------------------------------------------
void GameLogic::DrawSkillSelectionUI(KamataEngine::Sprite* skillCursorSprite, KamataEngine::Sprite* skillOptionSprites[], KamataEngine::Sprite* skillScreenBackground) {

	// 1. 半透明の背景を描画
	skillScreenBackground->Draw();

	// 2. 選択中のスキルにカーソルを描画 (先に描画することで、オプションの下に表示される)
	KamataEngine::Sprite* selectedOption = skillOptionSprites[selectedSkillIndex_];

	// カーソルの位置を選択肢の中心に合わせる
	KamataEngine::Vector2 cursorPosition = selectedOption->GetPosition();
	KamataEngine::Vector2 cursorSize = skillCursorSprite->GetSize();

	// 選択肢の左上の座標からカーソルの左上の座標を計算
	KamataEngine::Vector2 optionSize = selectedOption->GetSize();
	KamataEngine::Vector2 cursorDrawPos = {cursorPosition.x - (cursorSize.x - optionSize.x) / 2.0f, cursorPosition.y - (cursorSize.y - optionSize.y) / 2.0f};

	skillCursorSprite->SetPosition(cursorDrawPos);
	skillCursorSprite->Draw();

	for (int i = 0; i < 3; ++i) {
		KamataEngine::Sprite* optionSprite = skillOptionSprites[i];
		optionSprite->Draw();

		SkillType type = currentSkillOptions_[i];

		uint32_t iconHandle = skillTextureHandles_[static_cast<int>(type)];

		KamataEngine::Sprite* iconSprite = skillIconSprites_[i];

		iconSprite->SetTextureHandle(iconHandle);

		// ★★★ 修正: iconSize_ の利用を_DEBUGで保護 ★★★
		KamataEngine::Vector2 currentIconSize;
#ifdef _DEBUG
		// デバッグ時はスライダーで設定されたサイズを適用
		iconSprite->SetSize(iconSize_);
		currentIconSize = iconSize_;
#else
		// Release時は固定サイズを使用
		iconSprite->SetSize({400.0f, 100.0f});
		currentIconSize = {64.0f, 64.0f};
#endif

		KamataEngine::Vector2 optionPos = optionSprite->GetPosition();
		KamataEngine::Vector2 iconPos = {
		    optionPos.x + 0.0f,                                                   // オプションの左端から少し右
		    optionPos.y + (optionSprite->GetSize().y - currentIconSize.y) / 15.0f // 中央揃え
		};

		iconSprite->SetPosition(iconPos);
		iconSprite->Draw();
	}
}