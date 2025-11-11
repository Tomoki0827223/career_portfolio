#include "GameLogic.h"
#include <random>
#include <string> // std::to_string のために必要

// GameScene.cppの匿名名前空間内の乱数生成器を移動
namespace {
std::random_device seed_gen;
std::mt19937 engine(seed_gen());
// マップの範囲を定義
const float MAP_HALF_RANGE = 50.0f;
std::uniform_real_distribution<float> dist(-MAP_HALF_RANGE, MAP_HALF_RANGE);
} // namespace

// GameScene.cppから移動
const float PI = 3.14159265358979323846f;

GameLogic::GameLogic(Player* player, BIt_Map_Font* font, KamataEngine::Sprite* hpBar, KamataEngine::Sprite* hpBarBase) : player_(player), font_(font), hpBar_(hpBar), hpBarBase_(hpBarBase) {}

GameLogic::~GameLogic() {
	// 全ての動的オブジェクトの解放
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


	// 既存スキルのテクスチャロード (アップロードされたファイル名と対応)
	skillTextureHandles_[static_cast<int>(SkillType::kBook)] = KamataEngine::TextureManager::Load("Sukill/book.png");  //
	skillTextureHandles_[static_cast<int>(SkillType::kBullet)] = KamataEngine::TextureManager::Load("Sukill/Gun.png"); //
	skillTextureHandles_[static_cast<int>(SkillType::kHeart)] = KamataEngine::TextureManager::Load("Sukill/Hart.png"); //
	skillTextureHandles_[static_cast<int>(SkillType::kWine)] = KamataEngine::TextureManager::Load("Sukill/Wine.png");  //

	// 新規スキルのテクスチャ (一時的に既存のテクスチャを割り当て。必要に応じて変更してください)
	skillTextureHandles_[static_cast<int>(SkillType::kBoomerang)] = KamataEngine::TextureManager::Load("Sukill/axe.png");  // Resources/axe/axe.png を想定
	skillTextureHandles_[static_cast<int>(SkillType::kMinion)] = KamataEngine::TextureManager::Load("Sukill/player.png");  // Resources/player/player.png を想定
	skillTextureHandles_[static_cast<int>(SkillType::kMissile)] = KamataEngine::TextureManager::Load("Sukill/Bullet.png"); // Resources/Bullet/Bullet.png を想定

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
}

// ----------------------------------------------------
// GameLogic::Update (メイン更新処理)
// ----------------------------------------------------
void GameLogic::Update() {
	Vector3 playerPos = player_->GetPosition();

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

	if (isLevelUpPending_) {
		return;
	}

	// スコア表示の更新
	font_->Set(score_);

	// 敵の生成
	enemySpawnTimer_++;
	if (enemies_.size() + enemies2_.size() < kMaxEnemies + kMaxEnemies2 && enemySpawnTimer_ >= kEnemySpawnInterval) {
		SpawnEnemy();
		enemySpawnTimer_ = 0;
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
				return {targetPos, minDistanceSq};
			};

			auto [targetPos, minDistanceSq] = findNearestEnemy();

			if (minDistanceSq < 1e9f) {
				Vector3 velocity = targetPos - playerPos;
				Bullet* newBullet = new Bullet(playerPos, velocity);
				newBullet->Initialize();
				bullets_.push_back(newBullet);
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
					return {targetPos, minDistanceSq};
				};

				auto [targetPos, minDistanceSq] = findNearestEnemy();

				if (minDistanceSq < 1e9f) {
					Vector3 velocity = targetPos - minion->GetPosition();
					Bullet* newBullet = new Bullet(minion->GetPosition(), velocity);
					newBullet->SetDamage(minion->GetDamage() + player_->GetMinionLevel() / 2);
					newBullet->Initialize();
					bullets_.push_back(newBullet);
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
				return {targetPos, minDistanceSq};
			};

			auto [targetPos, minDistanceSq] = findNearestEnemy();

			if (minDistanceSq < 1e9f) {
				Missile* newMissile = new Missile(playerPos, targetPos);
				newMissile->Initialize();
				missiles_.push_back(newMissile);
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

// ----------------------------------------------------
// GameLogic::CheckAllCollisions (衝突判定)
// ----------------------------------------------------
void GameLogic::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = 0.5f;

	// 1. プレイヤーの攻撃 vs 敵 (近接攻撃判定)
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();
		auto checkPlayerAttackCollision = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();
				Vector3 diff = enemyPos - playerPos;
				float distance = Math::Length(diff);
				if (distance <= attackRadius + enemyRadius) {
					enemy->TakeDamage(1);
				}
			}
		};
		checkPlayerAttackCollision(enemies_);
		checkPlayerAttackCollision(enemies2_);
	}

	// 2. 敵 vs プレイヤー (敵からの接触ダメージ)
	for (Enemy* enemy : enemies_) {
		if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
			continue;
		Vector3 enemyPos = enemy->GetPosition();
		float enemyRadius = enemy->GetRadius();
		Vector3 diff = enemyPos - playerPos;
		float distance = Math::Length(diff);
		if (distance <= playerBodyRadius + enemyRadius) {
			player_->TakeDamage(1);
		}
	}

	// 3. Book (周回攻撃) vs 敵 の衝突判定
	for (Book* book : books_) {
		Vector3 bookPos = book->GetPosition();
		float bookRadius = book->GetRadius();
		int bookDamage = book->GetDamage();
		auto checkEnemyCollision = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();
				if (Math::Length(enemyPos - bookPos) <= bookRadius + enemyRadius) {
					enemy->TakeDamage(bookDamage);
				}
			}
		};
		checkEnemyCollision(enemies_);
		checkEnemyCollision(enemies2_);
	}

	// 4. Bullet (オート攻撃) vs 敵 の衝突判定
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
				float enemyRadius = enemy->GetRadius();
				if (Math::Length(enemyPos - bulletPos) <= bulletRadius + enemyRadius) {
					enemy->TakeDamage(bulletDamage);
					bullet->Die();
					return true;
				}
			}
			return false;
		};
		if (checkBulletCollision(enemies_) || checkBulletCollision(enemies2_)) {
			hit = true;
		}
		if (hit) {
			delete bullet;
			itB = bullets_.erase(itB);
		} else {
			++itB;
		}
	}

	// 5. Wine (回復アイテム) vs プレイヤー の衝突判定
	for (auto itW = wines_.begin(); itW != wines_.end();) {
		Wine* wine = *itW;
		Vector3 winePos = wine->GetPosition();
		float wineRadius = wine->GetRadius();
		if (Math::Length(winePos - playerPos) <= playerBodyRadius + wineRadius) {
			player_->Heal(wine->GetHealAmount());
			wine->Die();
			delete wine;
			itW = wines_.erase(itW);
		} else {
			++itW;
		}
	}

	// 6. Boomerang vs 敵 の衝突判定
	for (auto itB = boomerangs_.begin(); itB != boomerangs_.end();) {
		Boomerang* boomerang = *itB;
		if (boomerang->IsDead()) {
			delete boomerang;
			itB = boomerangs_.erase(itB);
			continue;
		}
		Vector3 boomerangPos = boomerang->GetPosition();
		float boomerangRadius = boomerang->GetRadius();
		int boomerangDamage = boomerang->GetDamage();
		bool hit = false;
		auto checkBoomerangCollision = [&](auto& enemies_list) -> bool {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();
				if (Math::Length(enemyPos - boomerangPos) <= boomerangRadius + enemyRadius) {
					enemy->TakeDamage(boomerangDamage);
					boomerang->Hit();
					return true;
				}
			}
			return false;
		};
		if (checkBoomerangCollision(enemies_) || checkBoomerangCollision(enemies2_)) {
			hit = true;
		}
		if (hit) {
			delete boomerang;
			itB = boomerangs_.erase(itB);
		} else {
			++itB;
		}
	}

	// 7. Missile vs 敵 の衝突判定
	for (auto itM = missiles_.begin(); itM != missiles_.end();) {
		Missile* missile = *itM;
		if (missile->IsDead()) {
			delete missile;
			itM = missiles_.erase(itM);
			continue;
		}
		Vector3 missilePos = missile->GetPosition();
		float missileRadius = missile->GetRadius();
		int missileDamage = missile->GetDamage();
		bool hit = false;
		auto checkMissileCollision = [&](auto& enemies_list) -> bool {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;
				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();
				if (Math::Length(enemyPos - missilePos) <= missileRadius + enemyRadius) {
					enemy->TakeDamage(missileDamage);
					missile->Die();
					return true;
				}
			}
			return false;
		};
		if (checkMissileCollision(enemies_) || checkMissileCollision(enemies2_)) {
			hit = true;
		}
		if (hit) {
			delete missile;
			itM = missiles_.erase(itM);
		} else {
			++itM;
		}
	}
}

// ----------------------------------------------------
// GameLogic::SpawnEnemy (敵のランダム生成関数)
// ----------------------------------------------------
void GameLogic::SpawnEnemy() {

	if (enemies_.size() + enemies2_.size() >= kMaxEnemies + kMaxEnemies2) {
		return;
	}

	const float kMinSpawnDistance = 20.0f;
	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	float distance = 0.0f;

	do {
		randomPos = {dist(engine), dist(engine), 0.0f};
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	std::uniform_int_distribution<int> distType(0, 3); // 0: Enemy2, 1-3: Enemy1
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
	}
	if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
		selectedSkillIndex_ = (selectedSkillIndex_ + 1) % optionCount;
	}

	// 決定キー (スペースキーやエンターキー) でスキルを適用し、ゲームを再開
	if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
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
	switch (skill) {
	case SkillType::kBook: {
		int newLevel = player_->GetBookLevel() + 1;
		player_->SetBookLevel(newLevel);
		for (Book* book : books_) {
			delete book;
		}
		books_.clear();
		for (int i = 0; i < newLevel; ++i) {
			Book* newBook = new Book();
			newBook->Initialize();
			books_.push_back(newBook);
		}
	} break;
	case SkillType::kBullet: {
		int newLevel = player_->GetBulletLevel() + 1;
		player_->SetBulletLevel(newLevel);
	} break;
	case SkillType::kHeart:
		player_->Heal(30);
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
		    optionPos.x + 0.0f,                                                 // オプションの左端から少し右
		    optionPos.y + (optionSprite->GetSize().y - currentIconSize.y) / 15.0f // 中央揃え
		};

		iconSprite->SetPosition(iconPos);
		iconSprite->Draw();
	}
}