#include "SkillSelect.h"
#include "application/ui/BIt_Map_Font.h" // BIt_Map_Fontも使用するため

// 乱数生成器 (GameSceneから移管)
namespace {
std::random_device seed_gen;
std::mt19937 engine(seed_gen());
} // namespace

// シングルトン実装
SkillSelect* SkillSelect::GetInstance() {
	static SkillSelect instance;
	return &instance;
}

SkillSelect::~SkillSelect() {
	// UIスプライトの解放
	delete skillScreenBackground_;
	delete skillCursorSprite_;
	for (int i = 0; i < 3; ++i) {
		delete skillOptionSprites_[i];
		// ★ 追加: アイコン用スプライトの解放 ★
		delete skillIconSprites_[i];
	}
}


void SkillSelect::Initialize(Player* player) {
	player_ = player;

	// --- テクスチャロード ---
	// 既存の white1x1.png をテクスチャとしてロード (GameSceneから移管)
	whiteTextureHandle_ = KamataEngine::TextureManager::Load("sample.png");

	// スキルアイコンのテクスチャロード (GameSceneから移管)
	skillTextureHandles_[static_cast<int>(SkillType::kBook)] = KamataEngine::TextureManager::Load("Sukill/book.png");

	skillTextureHandles_[static_cast<int>(SkillType::kBullet)] = KamataEngine::TextureManager::Load("Sukill/Gan.png"); // Gan.png を Bullet に対応
	skillTextureHandles_[static_cast<int>(SkillType::kHeart)] = KamataEngine::TextureManager::Load("Sukill/Hart.png"); // Hart.png を Heart に対応
	skillTextureHandles_[static_cast<int>(SkillType::kWine)] = KamataEngine::TextureManager::Load("Sukill/Wine.png");

	// --- UIスプライトの生成 (GameSceneから移管) ---
	// 1. 全画面背景スプライトの生成
	skillScreenBackground_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	skillScreenBackground_->SetSize({1280.0f, 720.0f});
	skillScreenBackground_->SetColor({0.0f, 0.0f, 0.0f, 0.8f}); // 黒で半透明 (80%透明)

	// 2. スキル選択肢スプライトの生成
	const KamataEngine::Vector2 kOptionSize = {400.0f, 100.0f}; // 選択肢のサイズ
	const KamataEngine::Vector2 kBasePos = {440.0f, 180.0f};    // 画面中央付近

	for (int i = 0; i < 3; ++i) {
		skillOptionSprites_[i] = KamataEngine::Sprite::Create(whiteTextureHandle_, {kBasePos.x, kBasePos.y + i * 120.0f});
		skillOptionSprites_[i]->SetSize(kOptionSize);
		skillOptionSprites_[i]->SetColor({0.2f, 0.2f, 0.2f, 1.0f}); // 濃い灰色
	}

	// 3. 選択カーソル/ハイライトスプライトの生成
	skillCursorSprite_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	skillCursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
	skillCursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f});
	
	// ★ 追加: アイコン用スプライトの生成 ★
	for (int i = 0; i < 3; ++i) {
		// Createを呼び出すことで、内部でInitializeが呼ばれvertBuff_が確保される
		skillIconSprites_[i] = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	}
}

void SkillSelect::StartSelection() {
	isSelecting_ = true;
	selectedSkillIndex_ = 0;

	// --- スキル選択肢をランダムに3つ生成 (GameSceneから移管) ---
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

void SkillSelect::Update() {
	if (!isSelecting_)
		return;

	// 入力インスタンスを取得
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
		isSelecting_ = false; // 選択終了
	}
}

void SkillSelect::Draw() {
	if (!isSelecting_)
		return;

	// 1. 半透明の背景を描画
	skillScreenBackground_->Draw();

	// 2. 選択中のスキルにカーソルを描画
	KamataEngine::Sprite* selectedOption = skillOptionSprites_[selectedSkillIndex_];
	KamataEngine::Vector2 cursorPosition = selectedOption->GetPosition();
	KamataEngine::Vector2 cursorSize = skillCursorSprite_->GetSize();

	KamataEngine::Vector2 optionSize = selectedOption->GetSize();
	// カーソルの位置を選択肢の中心に合わせる
	KamataEngine::Vector2 cursorDrawPos = {cursorPosition.x - (cursorSize.x - optionSize.x) / 2.0f, cursorPosition.y - (cursorSize.y - optionSize.y) / 2.0f};

	skillCursorSprite_->SetPosition(cursorDrawPos);
	skillCursorSprite_->Draw();

	// 3. 3つの選択肢の背景、画像、テキストを描画
	const float kIconSize = 80.0f;
	const float kIconOffsetX = -150.0f;

	for (int i = 0; i < 3; ++i) {
		// 選択肢の背景を描画
		skillOptionSprites_[i]->Draw();

		SkillType currentType = currentSkillOptions_[i];

		// --- スキルアイコンの描画 ---
		uint32_t iconHandle = GetTextureHandle(currentType);
		KamataEngine::Vector2 optionCenter = skillOptionSprites_[i]->GetPosition();

		// アイコンの左上座標を計算
		KamataEngine::Vector2 iconDrawPos = {
		    optionCenter.x + kIconOffsetX, // 修正: アイコンは中心描画ではないため、サイズ調整を削除
		    optionCenter.y - kIconSize / 2.0f};

		// ★★★ 修正: 既存のSpriteのプロパティを更新してからDraw()を呼び出す ★★★
		Sprite* iconSprite = skillIconSprites_[i]; // メンバ変数からインスタンスを取得

		iconSprite->SetTextureHandle(iconHandle);    // テクスチャを設定
		iconSprite->SetPosition(iconDrawPos);        // 位置を設定
		iconSprite->SetSize({kIconSize, kIconSize}); // サイズを設定
		iconSprite->SetAnchorPoint({0.0f, 0.0f});    // アンカーポイントを左上 (0, 0) に設定
		iconSprite->Draw();                          // 引数なしのメンバ関数 Draw() を呼び出す
		                                             // -----------------------------------------------------------------
		                                             // 選択されたスキルに効果を適用するロジック (GameSceneから移管)
	}
}
	void SkillSelect::ApplySkill(SkillType skill) {
	if (!player_)
		return;

	// Player.h に記述されている Setter/Getter を利用します
	switch (skill) {
	case SkillType::kBook: {
		int newLevel = player_->GetBookLevel() + 1;
		player_->SetBookLevel(newLevel);
		// GameScene::Update 内で books_ の再生成ロジックが実行されます
	} break;
	case SkillType::kBullet: {
		int newLevel = player_->GetBulletLevel() + 1;
		player_->SetBulletLevel(newLevel);
	} break;
	case SkillType::kHeart:
		player_->Heal(30); // HPを回復する
		break;
	case SkillType::kWine: {
		int newLevel = player_->GetWineLevel() + 1;
		player_->SetWineLevel(newLevel);
	} break;
	default:
		break;
	}
}


// スキルIDに対応するテクスチャハンドルを取得
uint32_t SkillSelect::GetTextureHandle(SkillType id) const { return skillTextureHandles_[static_cast<int>(id)]; }

// スキルIDに対応する情報 (名前・説明) を取得
std::pair<std::string, std::string> SkillSelect::GetSkillInfo(SkillType type) const {
	switch (type) {
	case SkillType::kBook:
		return {"魔法陣 (Book)", "プレイヤーの周りを回転する攻撃"};
	case SkillType::kBullet:
		return {"連射弾 (Gun)", "自動で敵に撃つ弾のレベルアップ"};
	case SkillType::kHeart:
		return {"HP回復 (Heart)", "HPを回復する"};
	case SkillType::kWine:
		return {"アイテム率 (Wine)", "ドロップアイテムの出現率アップ"};
	default:
		return {"不明", "不明なスキル"};
	}
}