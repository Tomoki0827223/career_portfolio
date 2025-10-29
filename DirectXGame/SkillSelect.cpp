#include "SkillSelect.h"

// 必要なインクルード（KamataEngineのパスを想定）
#include "2d/Sprite.h"
#include "application/3d/Player.h" // Playerクラスへの参照が必要
#include "base/TextureManager.h"
#include "input/Input.h"
#include <imgui.h> // デバッグ用にImguiを仮定

// シングルトンインスタンスの定義
SkillSelect* SkillSelect::GetInstance() {
	static SkillSelect instance;
	return &instance;
}

SkillSelect::SkillSelect() {
	// 乱数シードの初期化
	std::random_device seed_gen;
	engine_ = std::mt19937(seed_gen());
}

void SkillSelect::Initialize() {
	// スキル選択画面が表示される際の初期化
	currentSelectIndex_ = 0;
	state_ = kStateSelect;
	opacity_ = 1.0f;

	// --- 1. 全スキルのデータ初期化とテクスチャロード ---
	allSkills_[kSkillWine] = {kSkillWine, LoadTexture(kSkillWine), "ワインの力", "攻撃力が少し上がる"};
	allSkills_[kSkillBook] = {kSkillBook, LoadTexture(kSkillBook), "賢者の書", "経験値獲得量がアップ"};
	allSkills_[kSkillGun] = {kSkillGun, LoadTexture(kSkillGun), "銃の極意", "弾の速度と威力がアップ"};
	allSkills_[kSkillHeart] = {kSkillHeart, LoadTexture(kSkillHeart), "癒やしの心", "HPが全回復し、最大HPが少し増える"};

	// カーソル画像のロード (仮にResourcesフォルダ内にカーソル画像があるとする)
	cursorHandle_ = TextureManager::Load("Resources/SelectCursor.png");

	// --- 2. ランダムに3つのスキルを選択 ---
	std::vector<SkillID> allIDs;
	for (int i = 0; i < kSkillCount; ++i) {
		allIDs.push_back(static_cast<SkillID>(i));
	}

	// IDリストをシャッフル
	std::shuffle(allIDs.begin(), allIDs.end(), engine_);

	// シャッフルされたリストの先頭3つを選択肢とする
	for (int i = 0; i < kNumChoices; ++i) {
		selectedSkills_[i] = allIDs[i];
	}
}

// スキルIDに対応する画像をロード
uint32_t SkillSelect::LoadTexture(SkillID id) {
	std::string filePath;
	switch (id) {
	case kSkillWine:
		// ユーザーがアップロードしたファイル名
		filePath = "Resources/Sukill/Wine.png";
		break;
	case kSkillBook:
		filePath = "Resources/Sukill/book.png";
		break;
	case kSkillGun:
		filePath = "Resources/Sukill/Gan.png";
		break;
	case kSkillHeart:
		filePath = "Resources/Sukill/Hart.png";
		break;
	default:
		// エラー時のフォールバック画像
		filePath = "Resources/white1x1.png";
		break;
	}
	// TextureManagerを使って画像をロード
	return TextureManager::Load(filePath);
}

void SkillSelect::Update() {
	if (state_ == kStateDecided) {
		// スキル決定後のフェードアウト/シーン切り替え処理
		opacity_ -= 0.05f;
		if (opacity_ <= 0.0f) {
			// ゲームシーンに戻る処理などをGameSceneに実装
			// GameScene::GetInstance()->ChangeToPlayState();
		}
		return;
	}

	// --- 1. カーソル移動の入力処理 ---
	Input* input = Input::GetInstance();

	// 右キーまたはDキー
	if (input->IsKeyTrigger(DIK_RIGHT) || input->IsKeyTrigger(DIK_D)) {
		currentSelectIndex_ = (currentSelectIndex_ + 1) % kNumChoices;
	}
	// 左キーまたはAキー
	if (input->IsKeyTrigger(DIK_LEFT) || input->IsKeyTrigger(DIK_A)) {
		currentSelectIndex_ = (currentSelectIndex_ - 1 + kNumChoices) % kNumChoices;
	}

	// --- 2. 決定ボタンの入力処理 ---
	// EnterキーまたはSpaceキー
	if (input->IsKeyTrigger(DIK_RETURN) || input->IsKeyTrigger(DIK_SPACE)) {
		SkillID chosenSkill = selectedSkills_[currentSelectIndex_];
		ApplySkill(chosenSkill);
		state_ = kStateDecided;
	}

	// ImGuiデバッグ表示（開発時のみ）
	// ImGui::Begin("Skill Select");
	// ImGui::Text("Current Selection: %d", currentSelectIndex_);
	// for (int i = 0; i < kNumChoices; ++i) {
	//     ImGui::Text("Choice %d: %s", i, allSkills_[selectedSkills_[i]].name.c_str());
	// }
	// ImGui::End();
}

void SkillSelect::Draw() {
	// 画面全体を覆う半透明な背景を描画（スキル選択を強調）
	Sprite::Draw(Vector2{640.0f, 360.0f}, TextureManager::Load("Resources/white1x1.png"), {1280.0f, 720.0f}, Vector4{0.0f, 0.0f, 0.0f, 0.8f * opacity_});

	float startX = 200.0f;                // 描画開始X座標
	float spacing = 440.0f;               // 画像間の間隔
	Vector2 imageSize = {256.0f, 256.0f}; // 画像のサイズ
	Vector2 textPos = {640.0f, 100.0f};   // 説明文の表示位置

	for (int i = 0; i < kNumChoices; ++i) {
		SkillData& data = allSkills_[selectedSkills_[i]];
		Vector2 position = {startX + i * spacing, 360.0f};

		// --- スキル画像の描画 ---
		Sprite::Draw(position, data.textureHandle, imageSize, Vector4{1.0f, 1.0f, 1.0f, opacity_});

		// --- 選択カーソルの描画 ---
		if (i == currentSelectIndex_) {
			// 選択されている画像の上にカーソル（枠）を描画
			Sprite::Draw(position, cursorHandle_, imageSize + Vector2{20.0f, 20.0f}, Vector4{1.0f, 1.0f, 1.0f, opacity_});
		}
	}

	// --- スキル名と説明の描画 ---
	// 選択中のスキルの情報を取得
	SkillData& currentData = allSkills_[selectedSkills_[currentSelectIndex_]];

	// ここでビットマップフォントやDebugTextなどを使って文字を表示します
	// 例: DebugText::GetInstance()->Print(textPos, currentData.name + ": " + currentData.description, opacity_);

	// 決定後のフェードアウト
	if (state_ == kStateDecided) {
		Sprite::Draw(Vector2{640.0f, 360.0f}, TextureManager::Load("Resources/white1x1.png"), {1280.0f, 720.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f - opacity_});
	}
}

// 選択されたスキルに効果を適用するロジック
void SkillSelect::ApplySkill(SkillID id) {
	// GameSceneからPlayerインスタンスを取得
	// ※ 既存のPlayerクラスにこれらのメソッドを追加する必要があります
	Player* player = GameScene::GetInstance()->GetPlayer();

	if (!player)
		return;

	switch (id) {
	case kSkillWine:
		// 攻撃力アップ: Player::IncreaseAttackPower(1.2f)のようなメソッドを実装
		// player->IncreaseAttackPower(1.2f);
		// 仮の実装
		// player->SetAttack(player->GetAttack() * 1.2f);
		break;
	case kSkillBook:
		// 経験値獲得量アップ
		// player->IncreaseExpMultiplier(1.1f);
		break;
	case kSkillGun:
		// 弾速・威力アップ
		// player->IncreaseBulletStats(1.2f, 1.1f);
		break;
	case kSkillHeart:
		// HP全回復 & 最大HPアップ: Player::Heal()、Player::IncreaseMaxHP()のようなメソッドを実装
		// player->Heal(player->GetMaxHP());
		// player->IncreaseMaxHP(10);
		break;
	}
}