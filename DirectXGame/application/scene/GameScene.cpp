#define NOMINMAX          // min/maxマクロの衝突を防ぐ
#define _USE_MATH_DEFINES // M_PIを使用可能にするために追加
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
	// delete graph_;
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

	// ★追加: スキル関連オブジェクトの解放 ★
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
	// ------------------------------------
	// ★ スキル選択画面用スプライトの解放は削除済み ★
}

void GameScene::Initialize() {

	// ★追加: リトライに備え、敵と経験値のリストを確実にクリアする★
	// 敵の解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (Enemy2* enemy2 : enemies2_) {
		delete enemy2;
	}
	enemies2_.clear();

	// 経験値アイテムの解放
	for (Experience* exp : experiences_) {
		delete exp;
	}
	experiences_.clear();

	// ★追加: スキル関連オブジェクトのクリアと初期化 ★
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

	bulletSpawnTimer_ = 0;
	wineSpawnTimer_ = 0;
	// ------------------------------------

	// graph_ = new Graph();
	// graph_->Initialize();

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
	requiredExp_ = kExpBase; // 100
	                         // スキル選択関連の変数は削除済み
}

void GameScene::StartLevelUp() {
	level_++;
	// 次のレベルの経験値バーに超過分を回す
	currentExp_ -= requiredExp_;

	// 次の必要経験値を計算 (例: 10 * 1.2^(level-1))
	// 経験値はintで計算し、小数点以下は切り捨てる（または四捨五入する）
	requiredExp_ = static_cast<int>(kExpBase * std::pow(kExpScale, level_ - 1));

	// SkillType::kSkillCountはenumの要素数として使用
	std::uniform_int_distribution<int> distType(0, static_cast<int>(SkillType::kSkillCount) - 1);

	// ★ 修正: ランダムに選んだスキルを即座に適用するロジックに変更 ★
	SkillType selectedSkill = static_cast<SkillType>(distType(engine));
	ApplySkill(selectedSkill);
}

// UpdateSkillSelection() は削除済み

void GameScene::ApplySkill(SkillType skill) {
	// ここにPlayerクラスの機能拡張やGameScene全体のパラメータ変更処理を記述します
	switch (skill) {
	case SkillType::kBook: {
		// Bookスキルレベルを上げる
		int newLevel = player_->GetBookLevel() + 1;
		player_->SetBookLevel(newLevel);

		// 既に存在するBookをクリアしてから再生成
		for (Book* book : books_) {
			delete book;
		}
		books_.clear();

		// レベル数に応じてBookを生成
		// 最初のBookは初期位置をランダムにすることで、重なりを防ぐ
		for (int i = 0; i < newLevel; ++i) {
			Book* newBook = new Book();
			newBook->Initialize();
			books_.push_back(newBook);
		}
	} break;
	case SkillType::kBullet: {
		// Bulletスキルレベルを上げる
		int newLevel = player_->GetBulletLevel() + 1;
		player_->SetBulletLevel(newLevel);
	} break;
	case SkillType::kHeart:
		// HPを回復する
		player_->Heal(30);
		break;
	case SkillType::kWine: {
		// Wineの出現レベルを上げる (Updateの生成ロジックに使用)
		int newLevel = player_->GetWineLevel() + 1;
		player_->SetWineLevel(newLevel);
	} break;
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
		// ★修正: Vector3Length -> Math::Length
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	// ★ 敵の種類をランダムに決定 (Enemy2を低確率で出現させる) ★
	std::uniform_int_distribution<int> distType(0, 3); // 0: Enemy2, 1-3: Enemy1 (4分の1の確率でEnemy2)
	if (enemies2_.size() < kMaxEnemies2 && distType(engine) == 0) {
		// ★修正: コンストラクタに初期位置を渡す★
		Enemy2* newEnemy2 = new Enemy2(randomPos);
		newEnemy2->Initialize(); // Initializeは引数なしと想定
		enemies2_.push_back(newEnemy2);
	} else if (enemies_.size() < kMaxEnemies) {
		// ★修正: コンストラクタに初期位置を渡す★
		Enemy* newEnemy = new Enemy(randomPos);
		newEnemy->Initialize(); // Initializeは引数なしと想定
		enemies_.push_back(newEnemy);
	}
}

// ★追加: Wineのランダム生成関数 ★
void GameScene::SpawnWine() {
	// Wineレベルが0の場合は生成しない
	if (player_->GetWineLevel() == 0) {
		return;
	}

	// プレイヤーから離れた位置に生成する (最小距離 10.0f)
	const float kMinSpawnDistance = 10.0f;
	Vector3 playerPos = player_->GetPosition();
	Vector3 randomPos;
	float distance = 0.0f;

	do {
		// x, yをランダムに生成し、zは0.0f（固定）に設定
		randomPos = {dist(engine), dist(engine), 0.0f};
		// ★修正: Vector3Length -> Math::Length
		distance = Math::Length(randomPos - playerPos);
	} while (distance < kMinSpawnDistance);

	// Wineは画面に最大1個までにする (Wineレベルが上がると生成間隔が短くなる)
	if (wines_.empty()) {
		// ★修正: コンストラクタに初期位置を渡す★
		Wine* newWine = new Wine(randomPos);
		newWine->Initialize(); // Initializeは引数なしと想定
		wines_.push_back(newWine);
	}
}

void GameScene::CheckAllCollisions() {
	Vector3 playerPos = player_->GetPosition();
	float playerBodyRadius = 0.5f; // プレイヤー本体の半径 (仮)

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
			// ★修正: Vector3Length -> Math::Length
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
			// ★修正: Vector3Length -> Math::Length
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
	for (Enemy* enemy : enemies_) {
		// 死亡した敵や、すでにHPが0のプレイヤーにはダメージを与えない
		if (enemy->IsDead() || player_->GetCurrentHp() <= 0)
			continue;

		Vector3 enemyPos = enemy->GetPosition();
		float enemyRadius = enemy->GetRadius();

		Vector3 diff = enemyPos - playerPos;
		// ★修正: Vector3Length -> Math::Length
		float distance = Math::Length(diff);

		// 接触判定
		if (distance <= playerBodyRadius + enemyRadius) {
			// プレイヤーにダメージを与える (ここでは接触1回で1ダメージと仮定)
			player_->TakeDamage(1);
			// ダメージを一度与えたら、敵をプレイヤーから少し遠ざけるなどの処理を追加しても良い
		}
	}

	// ------------------------------------
	// ★追加: Book (周回攻撃) vs 敵 の衝突判定 ★
	// ------------------------------------
	for (Book* book : books_) {
		Vector3 bookPos = book->GetPosition();
		float bookRadius = book->GetRadius();
		int bookDamage = book->GetDamage();

		// Enemy1 & Enemy2
		auto checkEnemyCollision = [&](auto& enemies_list) {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();

				// Bookと敵の接触判定
				if (Math::Length(enemyPos - bookPos) <= bookRadius + enemyRadius) {
					enemy->TakeDamage(bookDamage);
				}
			}
		};
		checkEnemyCollision(enemies_);
		checkEnemyCollision(enemies2_);
	}
	// ------------------------------------

	// ------------------------------------
	// ★追加: Bullet (オート攻撃) vs 敵 の衝突判定 ★
	// ------------------------------------
	for (auto itB = bullets_.begin(); itB != bullets_.end();) {
		Bullet* bullet = *itB;
		if (bullet->IsDead()) { // Updateで画面外に出たBulletはisDead=trueになっている
			delete bullet;
			itB = bullets_.erase(itB);
			continue;
		}

		Vector3 bulletPos = bullet->GetPosition();
		float bulletRadius = bullet->GetRadius();
		// ★修正: hitをここで定義
		bool hit = false;

		// Enemy1 & Enemy2
		auto checkBulletCollision = [&](auto& enemies_list) -> bool {
			for (auto enemy : enemies_list) {
				if (enemy->IsDead())
					continue;

				Vector3 enemyPos = enemy->GetPosition();
				float enemyRadius = enemy->GetRadius();

				if (Math::Length(enemyPos - bulletPos) <= bulletRadius + enemyRadius) {
					// ★修正: BulletのダメージはGetDamage()で取得
					enemy->TakeDamage(bullet->GetDamage());
					// ★修正: OnCollisionではなくDie()を使用
					bullet->Die();
					return true;
				}
			}
			return false;
		};

		// いずれかの敵に当たったらhitをtrueにする
		if (checkBulletCollision(enemies_) || checkBulletCollision(enemies2_)) {
			hit = true;
		}

		if (hit) {
			// 敵に当たったBulletをリストから削除
			delete bullet;
			itB = bullets_.erase(itB);
		} else {
			++itB;
		}
	}
	// ------------------------------------

	// ------------------------------------
	// ★追加: Wine (回復アイテム) vs プレイヤー の衝突判定 ★
	// ------------------------------------
	for (auto itW = wines_.begin(); itW != wines_.end();) {
		Wine* wine = *itW;
		Vector3 winePos = wine->GetPosition();
		float wineRadius = wine->GetRadius();

		// プレイヤーとWineの接触判定
		if (Math::Length(winePos - playerPos) <= playerBodyRadius + wineRadius) {
			// プレイヤーを回復
			player_->Heal(wine->GetHealAmount());

			// Wine取得時の処理: 経験値アイテムをランダムにばら撒く (Wine::OnCollision()で処理されていた内容をここに移動)
			const int kExpCount = 50; // 生成する経験値の数
			const float kScatterRadius = 5.0f;
			std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * (float)M_PI);
			std::uniform_real_distribution<float> scatterDist(0.5f, kScatterRadius);

			for (int i = 0; i < kExpCount; ++i) {
				float angle = angleDist(engine);
				float d = scatterDist(engine);
				float xOffset = std::cos(angle) * d;
				float zOffset = std::sin(angle) * d;

				Vector3 expSpawnPos = playerPos + Vector3{xOffset, 0.0f, zOffset};

				// ★修正: コンストラクタに初期位置を渡す★
				Experience* newExp = new Experience(expSpawnPos);
				newExp->Initialize();
				experiences_.push_back(newExp);
			}

			// ★修正: OnCollisionではなくDie()を使用
			wine->Die();

			// Wineをリストから削除
			delete wine;
			itW = wines_.erase(itW);
		} else {
			++itW;
		}
	}
	// ------------------------------------
}

void GameScene::Update() {

	// ★【1. 最優先】プレイヤーの更新 (死亡モーションのタイマーを必ず進める) ★
	// isGameOver_ や isLevelUpPending_ の状態にかかわらず、毎フレーム実行されます。
	player_->Update();

	// ------------------------------------
	// ★【2. HP/ゲームオーバー判定と処理】★
	// ------------------------------------
	int currentHp = player_->GetCurrentHp();
	int maxHp = player_->GetMaxHp();

	// HPが0以下になったらゲームオーバーフラグを立てる (最初の1フレームのみ)
	if (currentHp <= 0 && !isGameOver_) {
		isGameOver_ = true;
		player_->Die(); // 死亡モーション開始フラグを設定
	}

	// HPバーのサイズを更新（HPが0以下の場合も比率が0になる）
	float hpRatio = (float)currentHp / maxHp;
	if (hpRatio < 0.0f) { // 念のため比率がマイナスにならないようにする
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
			//     ※SceneManagerの適切な呼び出し方に合わせて修正してください
		}

		// ★ モーションが終了するまで、他のゲーム処理はスキップしてアニメーションを継続させます ★
		return;
	}

	// ------------------------------------
	// ★【3. 通常時のみ】レベルアップ待ちの処理は削除済み ★
	// ------------------------------------

	stage_->Update();
	// graph_->Update();

	// スコア表示の更新
	// **【修正】レベルと経験値を表示するように変更**
	// std::string levelString = "Lv:" + std::to_string(level_);
	font_->Set(score_);
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
	// ★追加: Wineの生成 ★
	// ------------------------------------
	wineSpawnTimer_++;
	// Wineレベルが1以上で、かつ画面にWineがない場合に生成
	// レベルが上がるほど、生成間隔が短くなるようにする (例: 間隔 = kInterval / level)
	const int kMinWineInterval = 100; // 最小間隔を設定 (極端に短くならないように)
	// ★修正: (std::max) で囲む
	int currentWineInterval = kWineSpawnInterval / (std::max)(1, player_->GetWineLevel());
	currentWineInterval = (std::max)(currentWineInterval, kMinWineInterval);

	if (player_->GetWineLevel() >= 1 && wines_.empty() && wineSpawnTimer_ >= currentWineInterval) {
		SpawnWine();
		wineSpawnTimer_ = 0; // タイマーリセット
	}
	// ------------------------------------

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
	// ★追加: Book (周回攻撃) の更新 ★
	// ------------------------------------
	for (Book* book : books_) {
		// プレイヤーの位置を渡して周回させる
		book->Update(playerPos);
	}
	// ------------------------------------

	// ------------------------------------
	// ★追加: Bulletの自動生成と更新 ★
	// ------------------------------------
	if (player_->GetBulletLevel() >= 1) {
		bulletSpawnTimer_++;
		// レベルが上がるほど、発射間隔が短くなるようにする (例: 間隔 = kInterval / level)
		const int kMinBulletInterval = 10; // 最小間隔を設定
		// ★修正: (std::max) で囲む
		int currentBulletInterval = kBulletSpawnInterval / (std::max)(1, player_->GetBulletLevel());
		currentBulletInterval = (std::max)(currentBulletInterval, kMinBulletInterval);

		if (bulletSpawnTimer_ >= currentBulletInterval) {
			// 最も近い敵を検索 (Enemy1/Enemy2両方から)
			auto findNearestEnemy = [&]() -> std::pair<Vector3, float> {
				float minDistanceSq = 1e10f;
				Vector3 targetPos = playerPos;

				auto checkEnemy = [&](auto& enemies_list) {
					for (auto enemy : enemies_list) {
						if (enemy->IsDead())
							continue;

						// ★修正: Math::Lengthを二乗して比較する★
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
				// ★修正: コンストラクタに引数を渡し、Initializeは引数なしに統一★
				Bullet* newBullet = new Bullet(playerPos, targetPos);
				newBullet->Initialize();
				bullets_.push_back(newBullet);
			}

			bulletSpawnTimer_ = 0;
		}
	}

	// Bulletの更新 (Bullet::Update内で画面外判定が行われ、isDead_がtrueになる)
	for (Bullet* bullet : bullets_) {
		bullet->Update();
	}
	// ------------------------------------

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

	// Wineの更新 (何もしないが関数を呼ぶ)
	for (Wine* wine : wines_) {
		wine->Update(playerPos);
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
				// ★修正: コンストラクタに初期位置を渡す★
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
				// ★修正: コンストラクタに初期位置を渡す★
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
	// Bulletの削除処理 (Bullet::UpdateとCheckAllCollisionsでisDead=trueになったものの削除)
	// ------------------------------------
	for (auto itB = bullets_.rbegin(); itB != bullets_.rend();) {
		Bullet* bullet = *itB;
		if (bullet->IsDead()) {
			delete bullet;
			itB = std::vector<Bullet*>::reverse_iterator(bullets_.erase(std::next(itB).base()));
		} else {
			++itB;
		}
	}
}


void GameScene::DrawHPBar() {
	// HPバーを描画 (左上配置)
	hpBarBase_->Draw(); // ベース (枠)
	hpBar_->Draw();     // 現在HP (バー本体)
}

void GameScene::Draw() {
	// ... (描画ロジックは変更なし)

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

	// ★追加: Wineアイテムの描画 ★
	for (Wine* wine : wines_) {
		wine->Draw(camera_);
	}

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw(camera_);
	}
	// ★ Enemy2の描画を追加 ★
	for (Enemy2* enemy2 : enemies2_) {
		enemy2->Draw(camera_);
	}

	// ★追加: Bulletの描画 ★
	for (Bullet* bullet : bullets_) {
		bullet->Draw(camera_);
	}

	// ★追加: Book (周回攻撃) の描画 ★
	for (Book* book : books_) {
		book->Draw(camera_);
	}

	// 3. 3D描画の終了
	Model::PostDraw(); // ★ Sprite描画の前にModelの描画を一旦区切る ★

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	DrawHPBar(); // HPバーの描画

	// font_->Draw();

	// ★ スキル選択画面の描画は削除済み ★

	font_->Draw(); // BIt_Map_Font が単独で描画を完結させているため、ここで呼ぶ

	// 6. 2D描画の終了
	Sprite::PostDraw();
}