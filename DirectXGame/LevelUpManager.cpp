#include "LevelUpManager.h"
#include "math/MathUtility.h" // Math::Lengthなどが必要なら含める

LevelUpManager::LevelUpManager() : engine_(std::random_device()()) {}

LevelUpManager::~LevelUpManager() {
	delete skillScreenBackground_;
	delete skillCursorSprite_;
	for (int i = 0; i < 3; ++i) {
		delete skillOptionSprites_[i];
	}
}

void LevelUpManager::Initialize() {
	// レベルアップシステム関連の初期化
	level_ = 1;
	currentExp_ = 0;
	requiredExp_ = kExpBase; // 10
	isLevelUpPending_ = false;
	selectedSkillIndex_ = 0;

	// スキル選択画面用スプライトの初期化
	// white1x1.png は GameScene.cpp から読み込みを移動
	whiteTextureHandle_ = KamataEngine::TextureManager::Load("white1x1.png");

	// 1. 全画面背景スプライトの生成
	skillScreenBackground_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0});
	skillScreenBackground_->SetSize({1280.0f, 720.0f});         // 画面サイズに合わせる (仮定)
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
	skillCursorSprite_ = KamataEngine::Sprite::Create(whiteTextureHandle_, {0, 0}); // 位置はDrawで更新
	skillCursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});    // 選択肢より少し大きく
	skillCursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f});                         // 黄色で半透明
}

void LevelUpManager::AddExperience(int expValue) {
	if (isLevelUpPending_) {
		return;
	}

	currentExp_ += expValue;

	// レベルアップ判定
	if (currentExp_ >= requiredExp_) {
		StartLevelUp();
	}
}

void LevelUpManager::StartLevelUp() {
	level_++;
	// 次のレベルの経験値バーに超過分を回す
	currentExp_ -= requiredExp_;

	// 次の必要経験値を計算 (例: 10 * 1.2^(level-1))
	requiredExp_ = static_cast<int>(kExpBase * std::pow(kExpScale, level_ - 1));

	isLevelUpPending_ = true;
	selectedSkillIndex_ = 0; // 選択インデックスをリセット

	// スキル選択肢をランダムに3つ生成
	std::uniform_int_distribution<int> distType(0, static_cast<int>(SkillType::kSkillCount) - 1);

	currentSkillOptions_.clear();
	while (static_cast<int>(currentSkillOptions_.size()) < 3) {
		SkillType newSkill = static_cast<SkillType>(distType(engine_));

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

void LevelUpManager::UpdateSkillSelection() {
	// スキル選択画面中もキー入力を受け付けるために、Inputクラスの更新を必ず行う
	KamataEngine::Input::GetInstance()->Update();

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
		currentSkillOptions_.clear(); // 選択肢をクリア
	}
}

void LevelUpManager::ApplySkill(SkillType skill) {
	// TODO: Player クラスの機能拡張後、ここで player_->Upgrade... などと呼び出す
	switch (skill) {
	case SkillType::kAttackUp:
		// プレイヤーの攻撃力を上げる
		break;
	case SkillType::kSpeedUp:
		// プレイヤーの移動速度を上げる
		break;
	case SkillType::kHeal:
		// プレイヤーのHPを回復する
		break;
	default:
		break;
	}
}

void LevelUpManager::Update() {
	if (isLevelUpPending_) {
		UpdateSkillSelection();
	}
	// isLevelUpPending_がfalseの場合、何もせずリターン
}

void LevelUpManager::Draw() {
	if (!isLevelUpPending_) {
		return;
	}

	// 1. 半透明の背景を描画
	skillScreenBackground_->Draw();

	// 2. 選択中のスキルにカーソルを描画
	KamataEngine::Sprite* selectedOption = skillOptionSprites_[selectedSkillIndex_];

	KamataEngine::Vector2 cursorPosition = selectedOption->GetPosition();
	KamataEngine::Vector2 cursorSize = skillCursorSprite_->GetSize();
	KamataEngine::Vector2 optionSize = selectedOption->GetSize();

	KamataEngine::Vector2 cursorDrawPos = {cursorPosition.x - (cursorSize.x - optionSize.x) / 2.0f, cursorPosition.y - (cursorSize.y - optionSize.y) / 2.0f};

	skillCursorSprite_->SetPosition(cursorDrawPos);
	skillCursorSprite_->Draw();

	// 3. 3つの選択肢の背景を描画
	for (int i = 0; i < 3; ++i) {
		skillOptionSprites_[i]->Draw();

		// TODO: スキル名を BIt_Map_Font で表示するには、
		// BIt_Map_Font が文字列描画に対応するか、
		// 文字描画に DebugText (KamataEngineにある場合) を使う必要があります。
		// ここでは、一旦、スプライトの箱だけを表示します。
	}
}