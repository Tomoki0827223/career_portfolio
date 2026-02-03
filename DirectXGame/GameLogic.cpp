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

GameLogic::GameLogic(
    Player* player, BIt_Map_Font* font, KamataEngine::Sprite* hpBar, KamataEngine::Sprite* hpBarBase, KamataEngine::Sprite* expBar, KamataEngine::Sprite* expBarBase, KamataEngine::Sprite* spBar,
    KamataEngine::Sprite* spBarBase)
    : player_(player), font_(font), hpBar_(hpBar), hpBarBase_(hpBarBase), expBar_(expBar), expBarBase_(expBarBase), spBar_(spBar), spBarBase_(spBarBase) { // 初期化リストにも追加


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

	// 古い enemies_ ～ enemies5_ のループをこれ1つに置き換え
	for (Enemy* obj : allEnemies_) {
		delete obj;
	}
	allEnemies_.clear();

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

	// 既存のリストを全て掃除
	for (auto* e : allEnemies_)
		delete e;
	allEnemies_.clear();

	// 他のリスト（bullets_, experiences_等）も同様に掃除
	CleanupDeadObjects(bullets_); // テンプレート関数を活用

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

	std::ifstream file("Resources/enemyPop.csv");
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		// 空行やコメント行を飛ばす
		if (line.empty() || line.find("//") == 0) {
			continue;
		}

		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> seglist;

		// カンマ区切りで seglist に入れる
		while (std::getline(ss, segment, ',')) {
			seglist.push_back(segment);
		}

		// ★ ここからが重要！文字列を数値に変換して変数に入れる ★
		if (seglist.size() >= 4) {
			int type = std::stoi(seglist[0]);     // 1列目: 敵タイプ
			int interval = std::stoi(seglist[1]); // 2列目: 出現間隔
			int minScore = std::stoi(seglist[2]); // 3列目: 開始スコア
			int maxScore = std::stoi(seglist[3]); // 4列目: 終了スコア

			// 囲い込み(5)も許可してリストに追加
			if (type >= 0 && type <= 5) {
				EnemySpawnData data;
				data.enemyType = type;
				data.interval = interval;
				data.minScore = minScore;
				data.maxScore = maxScore;
				data.timer = 0;
				enemySpawnList_.push_back(data);
			}
		}
	}
	file.close();
}

void GameLogic::Update() {
	Vector3 playerPos = player_->GetPosition();
	UpdateSpecialAndRapidFire(playerPos);

	if (isLevelUpPending_) {
		if (++skillSelectTimer_ < kSlowMotionLimit && skillSelectTimer_ % 4 != 0)
			return;
		if (skillSelectTimer_ >= kSlowMotionLimit)
			return;
	}

	// 1. 全ての敵の更新
	for (auto* enemy : allEnemies_) {
		enemy->Update(playerPos);
		if (enemy->CanShoot() && enemyBullets_.size() < kMaxEnemyBullets) {
			SpawnEnemyBullet(enemy, playerPos);
		}
	}

	// --- 修正: CSVのデータに基づいた出現管理 ---
	for (auto& spawnData : enemySpawnList_) {
		// 現在のスコアがCSVで指定された範囲内にあるかチェック
		if (score_ >= spawnData.minScore && score_ <= spawnData.maxScore) {
			spawnData.timer++; // 各出現パターンごとのタイマーを更新
			if (spawnData.timer >= spawnData.interval) {
				spawnData.timer = 0;
				SpawnEnemy(spawnData.enemyType); // 指定されたタイプを生成
			}
		}
	}

	// 3. 武器とUIの更新
	UpdateWeapons(playerPos);
	CheckAllCollisions();
	UpdateUI();

	// 4. アイテムの更新
	for (auto* exp : experiences_)
		exp->Update(playerPos);
	for (auto* wine : wines_)
		wine->Update(playerPos);

	// 5. 経験値の取得判定
	for (auto it = experiences_.begin(); it != experiences_.end();) {
		Experience* exp = *it;
		if (exp->IsDead()) {
			currentExp_ += 1;
			score_ += 1;

			if (!isSpecialActive_) {
				specialGauge_ += 0.5f;
				if (specialGauge_ > kMaxSpecialGauge_)
					specialGauge_ = kMaxSpecialGauge_;
			}

			while (currentExp_ >= requiredExp_)
				StartLevelUp();

			delete exp;                  // メモリを消す
			it = experiences_.erase(it); // ★ここ！コメントアウト(//)を外してリストからも消す
		} else {
			++it;
		}
	}

	// 6. 敵の死亡処理
	for (auto* enemy : allEnemies_) {
		if (enemy->IsDead()) {
			audio_->PlayWave(soundHandleEnemyDie_);
			deadEnemyPositions_.push_back(enemy->GetPosition());
			DropExperience(enemy->GetPosition(), enemy->GetType());
		}
	}

	// 7. 死んだオブジェクトの掃除
	CleanupDeadObjects(allEnemies_);
	CleanupDeadObjects(bullets_);
	CleanupDeadObjects(enemyBullets_);
	CleanupDeadObjects(boomerangs_);
	CleanupDeadObjects(missiles_);
	CleanupDeadObjects(wines_);
}

void GameLogic::SpawnEnemy(int enemyType) {
	// ★修正: enemies_ を allEnemies_ に統一
	const size_t kTotalEnemyMax = 100;
	if (allEnemies_.size() >= kTotalEnemyMax)
		return;

	Vector3 playerPos = player_->GetPosition();
	Vector3 spawnPos; // 1. ここで既に宣言されている

	// --- 出現位置の計算ロジックはOK！ ---
	if (enemyType == 5) {
		// --- 囲い込み出現: 一気に円形に配置する ---
		int numEnemies = 12;
		float spawnDist = 30.0f;
		for (int i = 0; i < numEnemies; ++i) {
			float angle = (2.0f * PI / numEnemies) * i;

			// ★修正箇所: ここでの「Vector3 spawnPos;」という再宣言を削除する
			spawnPos.x = playerPos.x + std::cos(angle) * spawnDist;
			spawnPos.y = playerPos.y + std::sin(angle) * spawnDist;
			spawnPos.z = 0.0f;

			Enemy* newEnemy = new Enemy(spawnPos);
			newEnemy->Initialize();
			allEnemies_.push_back(newEnemy);
		}

		// ★重要: タイプ5の時はループ内で全て生成済みなので、関数の残りの処理をスキップする
		return;

	} else {
		std::uniform_real_distribution<float> angleDist(0.0f, 6.283f);
		float angle = angleDist(engine);
		float spawnDist = 50.0f;
		spawnPos.x = playerPos.x + std::cos(angle) * spawnDist;
		spawnPos.y = playerPos.y + std::sin(angle) * spawnDist;
		spawnPos.z = 0.0f;
	}

	// 以下の処理は通常種の敵（0-4）の場合のみ実行される
	Enemy* newEnemy = nullptr;
	switch (enemyType) {
	case 0:
		newEnemy = new Enemy(spawnPos);
		break;
	case 1:
		newEnemy = new Enemy2(spawnPos);
		break;
	case 2:
		newEnemy = new Enemy3(spawnPos);
		break;
	case 3:
		newEnemy = new Enemy4(spawnPos);
		break;
	case 4:
		newEnemy = new Enemy5(spawnPos);
		break;
	default:
		newEnemy = new Enemy(spawnPos);
		break;
	}

	if (newEnemy) {
		newEnemy->Initialize();
		allEnemies_.push_back(newEnemy);
	}
}

// ----------------------------------------------------
// GameLogic::CheckAllCollisions (衝突判定)
// ----------------------------------------------------

void GameLogic::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = 0.5f;

	for (Enemy* enemy : allEnemies_) {
		enemy->Update(playerPos);

		// 敵の攻撃ロジック（タイプによって弾速などを変える場合は enemy->GetType() 等で判別）
		if (enemy->CanShoot() && enemyBullets_.size() < kMaxEnemyBullets) {
			enemy->ResetShotTimer();
			Vector3 direction = Math::Normalize(playerPos - enemy->GetShotPosition());
			Vector3 velocity = direction * 1.0f; // 共通の弾速（必要に応じて調整）

			EnemyBullet* newBullet = new EnemyBullet(enemy->GetShotPosition(), velocity);
			newBullet->SetDamage(10); // 共通ダメージ
			newBullet->Initialize();
			enemyBullets_.push_back(newBullet);
		}
	}

	// --- 1. プレイヤーの攻撃 vs 敵 ---
	if (player_->IsAttacking()) {
		float attackRadius = player_->GetAttackRadius();
		for (auto* enemy : allEnemies_) {
			if (enemy->IsDead())
				continue;
			if (Math::Length(enemy->GetPosition() - playerPos) <= (attackRadius + enemy->GetRadius())) {
				enemy->TakeDamage(1);
			}
		}
	}

	// --- 2. 敵 vs プレイヤー（接触ダメージ） ---
	for (auto* enemy : allEnemies_) {
		if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
			continue;
		if (Math::Length(enemy->GetPosition() - playerPos) <= (playerBodyRadius + enemy->GetRadius())) {
			int damageToTake = (std::max)(1, 1 - player_->GetDefense()); // 修正後の防御計算
			player_->TakeDamage(damageToTake);
			audio_->PlayWave(soundHandleDamage_);
		}
	}

	// --- 3. Book (周回攻撃) vs 敵 ---
	for (Book* book : books_) {
		Vector3 bPos = book->GetPosition();
		for (auto* enemy : allEnemies_) {
			if (enemy->IsDead())
				continue;
			if (Math::Length(enemy->GetPosition() - bPos) <= (book->GetRadius() + enemy->GetRadius())) {
				enemy->TakeDamage(book->GetDamage());
			}
		}
	}

	// --- 4. Bullet (オート弾) vs 敵 ---
	for (auto itB = bullets_.begin(); itB != bullets_.end();) {
		Bullet* bullet = *itB;
		bool hit = false;
		for (auto* enemy : allEnemies_) {
			if (enemy->IsDead())
				continue;
			if (Math::Length(enemy->GetPosition() - bullet->GetPosition()) <= (bullet->GetRadius() + enemy->GetRadius())) {
				enemy->TakeDamage(bullet->GetDamage());
				bullet->Die();
				hit = true;
				break;
			}
		}
		if (hit || bullet->IsDead()) {
			delete bullet;
			itB = bullets_.erase(itB);
		} else {
			++itB;
		}
	}

	// --- 5. Boomerang / Missile vs 敵 ---
	auto checkWeaponCollision = [&](auto& weapons_list, auto handleHit) {
		for (auto it = weapons_list.begin(); it != weapons_list.end();) {
			auto weapon = *it;
			bool hit = false;
			for (auto* enemy : allEnemies_) {
				if (enemy->IsDead())
					continue;
				if (Math::Length(enemy->GetPosition() - weapon->GetPosition()) <= (weapon->GetRadius() + enemy->GetRadius())) {
					enemy->TakeDamage(weapon->GetDamage());
					handleHit(weapon);
					hit = true;
					break;
				}
			}
			if (weapon->IsDead()) {
				delete weapon;
				it = weapons_list.erase(it);
			} else {
				++it;
			}
		}
	};
	checkWeaponCollision(boomerangs_, [](auto b) { b->Hit(); });
	checkWeaponCollision(missiles_, [](auto m) { m->Die(); });

	// --- 6. アイテム・敵弾 vs プレイヤー (既存のロジックでOK) ---
	// (WineやEnemyBulletsのループはそのまま維持)
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
	player_->SetIsInvincible(true); // 無敵開始
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
	Input* input = Input::GetInstance();
	// ★修正: [0].size() ではなく .size() に直す
	int optionCount = static_cast<int>(currentSkillOptions_.size());

	// コントローラー状態の取得
	XINPUT_STATE joyState, joyStatePrev;
	bool hasJoy = input->GetJoystickState(0, joyState);
	bool hasJoyPrev = input->GetJoystickStatePrevious(0, joyStatePrev);

	// --- 入力判定（キーボード ＋ 十字キー ＋ スティック） ---
	bool moveUp = input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP);
	bool moveDown = input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN);
	bool enter = input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN);

	if (hasJoy && hasJoyPrev) {
		// 上方向（十字キー or スティック）
		if (!(joyStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
			moveUp = true;
		if (joyStatePrev.Gamepad.sThumbLY <= 15000 && joyState.Gamepad.sThumbLY > 15000)
			moveUp = true;

		// 下方向
		if (!(joyStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
			moveDown = true;
		if (joyStatePrev.Gamepad.sThumbLY >= -15000 && joyState.Gamepad.sThumbLY < -15000)
			moveDown = true;

		// 決定（Aボタン）
		if (!(joyStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
			enter = true;
	}

	if (moveUp) {
		selectedSkillIndex_ = (selectedSkillIndex_ - 1 + optionCount) % optionCount;
		audio_->PlayWave(soundHandleSelectCursor_);
	}
	if (moveDown) {
		selectedSkillIndex_ = (selectedSkillIndex_ + 1) % optionCount;
		audio_->PlayWave(soundHandleSelectCursor_);
	}

	if (enter) {
		audio_->PlayWave(soundHandleSkillDecide_);
		ApplySkill(currentSkillOptions_[selectedSkillIndex_]);

		// 連続レベルアップ判定（whileでのStartLevelUpを反映）
		if (currentExp_ >= requiredExp_) {
			StartLevelUp();
		} else {
			isLevelUpPending_ = false;
			player_->SetIsInvincible(false);
		}
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

// --- UIの更新 (HP/EXPバー) ---
void GameLogic::UpdateUI() {
	float hpRatio = (std::clamp)((float)player_->GetCurrentHp() / player_->GetMaxHp(), 0.0f, 1.0f);
	hpBar_->SetSize({hpBarBase_->GetSize().x * hpRatio, hpBar_->GetSize().y});

	float expRatio = (std::clamp)((float)currentExp_ / requiredExp_, 0.0f, 1.0f);
	expBar_->SetSize({expBarBase_->GetSize().x * expRatio, expBar_->GetSize().y});

	// SPバーの更新
	float spRatio = (std::clamp)(specialGauge_ / kMaxSpecialGauge_, 0.0f, 1.0f);
	spBar_->SetSize({spBarBase_->GetSize().x * spRatio, spBar_->GetSize().y});

	font_->Set(score_);
}

void GameLogic::UpdateSpecialAndRapidFire(const Vector3& playerPos) {
	Input* input = Input::GetInstance();
	XINPUT_STATE joyState;
	bool hasJoy = input->GetJoystickState(0, joyState);

	// 入力判定
	bool triggerSpecial = input->TriggerKey(DIK_E);
	if (hasJoy && joyState.Gamepad.bRightTrigger > 128) {
		triggerSpecial = true;
	}

	// --- 発動判定 ---
	if (!isSpecialActive_ && specialGauge_ >= kMaxSpecialGauge_) {
		if (triggerSpecial) {
			isSpecialActive_ = true;
			specialTimer_ = kMaxSpecialTime_;

			// ★★★ 追加：古い弾をすべて削除して画面をスッキリさせる ★★★
			for (auto* b : bullets_) {
				delete b;
			}
			bullets_.clear();

			// 画面上の全経験値を吸い込み開始
			for (auto* exp : experiences_) {
				exp->StartAttraction();
			}
		}
	}

	// --- 必殺技発動中の処理 ---
	if (isSpecialActive_) {
		specialTimer_--;

		// ★★★ マシンガン（弾の発射）ロジックをここに統合 ★★★
		// 4フレームに1回、全方位に弾をバラまく
		if (specialTimer_ % 4 == 0) {
			Enemy* target = FindNearestEnemy(playerPos);

			// 1回の発射で3方向に弾を出す（角度をずらして全方位にする）
			for (int i = 0; i < 3; ++i) {
				Vector3 velocity = {std::cos(currentShotAngle_), std::sin(currentShotAngle_), 0.0f};
				Bullet* nb = new Bullet(playerPos, velocity);
				nb->Initialize();

				// 必殺技なのでダメージを高く設定
				nb->SetDamage(20);

				if (target) {
					nb->SetTargetPos(target->GetPosition());
				}
				bullets_.push_back(nb);

				// 次の弾の角度を少しずらす
				currentShotAngle_ += 0.5f;
			}
			// 発射音
			audio_->PlayWave(soundHandleBulletShot_);
		}

		// 終了判定
		if (specialTimer_ <= 0) {
			isSpecialActive_ = false;
			specialGauge_ = 0.0f; // ゲージリセット
		}
	}
}

// --- 各武器の自動生成と更新 ---
void GameLogic::UpdateWeapons(const Vector3& playerPos) {
	// Bullet (オート)
	if (player_->GetBulletLevel() >= 1) {
		if (++bulletSpawnTimer_ >= (std::max)(10, kBulletSpawnInterval / player_->GetBulletLevel())) {
			Enemy* target = FindNearestEnemy(playerPos);
			if (target) {
				Bullet* nb = new Bullet(playerPos, target->GetPosition() - playerPos);
				nb->Initialize();
				bullets_.push_back(nb);
				audio_->PlayWave(soundHandleBulletShot_);
			}
			bulletSpawnTimer_ = 0;
		}
	}
	for (auto* b : bullets_)
		b->Update();

	// Boomerang
	if (player_->GetBoomerangLevel() >= 1) {
		if (++boomerangSpawnTimer_ >= (std::max)(30, kBoomerangSpawnInterval / player_->GetBoomerangLevel())) {
			Enemy* target = FindNearestEnemy(playerPos);
			Vector3 vel = target ? target->GetPosition() - playerPos : Vector3(1, 0, 0);
			Boomerang* nbm = new Boomerang(playerPos, vel);
			nbm->Initialize();
			boomerangs_.push_back(nbm);
			audio_->PlayWave(soundHandleBoomerangShot_);
			boomerangSpawnTimer_ = 0;
		}
	}
	for (auto* b : boomerangs_)
		b->Update(playerPos);

	// Missile
	if (player_->GetMissileLevel() >= 1) {
		if (++missileSpawnTimer_ >= (std::max)(20, kMissileSpawnInterval / player_->GetMissileLevel())) {
			Enemy* target = FindNearestEnemy(playerPos);
			if (target) {
				Missile* nm = new Missile(playerPos, target->GetPosition());
				nm->Initialize();
				missiles_.push_back(nm);
				audio_->PlayWave(soundHandleMissileShot_);
			}
			missileSpawnTimer_ = 0;
		}
	}
	Enemy* homingTarget = FindNearestEnemy(playerPos);
	Vector3 targetPos = homingTarget ? homingTarget->GetPosition() : playerPos;
	for (auto* m : missiles_)
		m->Update(targetPos);

	// Book / Minion / EnemyBullet
	for (auto* b : books_)
		b->Update(playerPos);
	for (auto* m : minions_)
		m->Update(playerPos);
	for (auto* eb : enemyBullets_)
		eb->Update();
}

// --- 敵の弾発射ヘルパー ---
void GameLogic::SpawnEnemyBullet(Enemy* enemy, const Vector3& playerPos) {
	enemy->ResetShotTimer();
	Vector3 direction = Math::Normalize(playerPos - enemy->GetShotPosition());
	float speed = (enemy->GetType() == 1) ? 1.2f : (enemy->GetType() == 3) ? 0.6f : 1.0f;
	int damage = (enemy->GetType() == 1) ? 30 : (enemy->GetType() == 3) ? 70 : 10;

	EnemyBullet* nb = new EnemyBullet(enemy->GetShotPosition(), direction * speed);
	nb->SetDamage(damage);
	nb->Initialize();
	enemyBullets_.push_back(nb);
}

// ----------------------------------------------------
// GameLogic::DrawObjects (3Dオブジェクト描画)
// ----------------------------------------------------
void GameLogic::DrawObjects(const Camera& camera) {
	// 経験値・Wine
	for (Experience* exp : experiences_)
		exp->Draw(camera);
	for (Wine* wine : wines_)
		wine->Draw(camera);

	// ★ 敵の一括描画（これだけでOK）
	for (Enemy* enemy : allEnemies_) {
		enemy->Draw(camera);
	}

	// 弾・武器
	for (EnemyBullet* eb : enemyBullets_)
		eb->Draw(camera);
	for (Bullet* b : bullets_)
		b->Draw(camera);
	for (Book* bk : books_)
		bk->Draw(camera);
	for (Boomerang* bm : boomerangs_)
		bm->Draw(camera);
	for (Minion* m : minions_)
		m->Draw(camera);
	for (Missile* ms : missiles_)
		ms->Draw(camera);
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

Enemy* GameLogic::FindNearestEnemy(Vector3 basePos, float* outDistSq) {
	float minDistanceSq = 1e10f;
	Enemy* nearestEnemy = nullptr;

	for (auto* enemy : allEnemies_) {
		if (enemy->IsDead())
			continue;

		Vector3 diff = enemy->GetPosition() - basePos;
		// LengthSq の代わりに二乗和を手動計算
		float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		if (distSq < minDistanceSq) {
			minDistanceSq = distSq;
			nearestEnemy = enemy;
		}
	}

	if (outDistSq)
		*outDistSq = minDistanceSq;
	return nearestEnemy;
}

void GameLogic::DropExperience(Vector3 position, int type) {
	int dropCount = 0;

	if (type == 0) {
		// ★ dist を dropDist に変更して名前の衝突を避ける
		std::uniform_int_distribution<int> dropDist(10, 15);
		dropCount = dropDist(engine);
	} else {
		dropCount = 30;
	}

	for (int i = 0; i < dropCount; ++i) {
		Experience* newExp = new Experience(position);
		newExp->Initialize();
		experiences_.push_back(newExp);
	}
}

template<typename T> void GameLogic::CleanupDeadObjects(std::vector<T*>& list) {
	// 1. IsDead() が true の要素を後ろに集め、同時に delete する
	auto it = std::remove_if(list.begin(), list.end(), [](T* obj) {
		if (obj->IsDead()) {
			delete obj; // メモリ解放
			return true;
		}
		return false;
	});

	// 2. 不要になったポインタを vector から一括削除
	list.erase(it, list.end());
}

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