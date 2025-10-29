#pragma once

#include <algorithm>
#include <array>
#include <random>
#include <vector>

// 依存すると思われるDirectXGameのユーティリティをインクルード
// 実際のパスに合わせて修正してください
#include "2d/Sprite.h"
#include "application/scene/GameScene.h" // Playerへのアクセスに必要
#include "base/TextureManager.h"
#include "input/Input.h"
#include "math/Vector2.h"

class SkillSelect {
public:
	// 4つのスキルを識別するための列挙型
	enum SkillID {
		kSkillWine,  // Wine.png: 攻撃力アップ
		kSkillBook,  // book.png: 経験値倍率アップ
		kSkillGun,   // Gan.png: 弾速アップ
		kSkillHeart, // Hart.png: HP回復と最大HPアップ
		kSkillCount  // スキルの総数 (4)
	};

	// 選択画面の状態
	enum State {
		kStateSelect, // スキルを選択中
		kStateDecided // スキルを決定済み
	};

	// スキル情報の構造体
	struct SkillData {
		SkillID id;
		uint32_t textureHandle; // 画像のテクスチャハンドル
		std::string name;
		std::string description;
	};

public:
	// シングルトンパターン（DirectXGameの構成に合わせる）
	static SkillSelect* GetInstance();

	// コンストラクタ、デストラクタ
	SkillSelect();
	~SkillSelect() = default;

	// 初期化処理（画像のロードとランダム選択）
	void Initialize();
	// 更新処理（入力、カーソル移動）
	void Update();
	// 描画処理
	void Draw();

	// スキル選択中かどうかを返す
	bool IsSelecting() const { return state_ == kStateSelect; }

private:
	// 選択されたスキルに効果を適用する（最も重要なロジック）
	void ApplySkill(SkillID id);
	// スキルIDに対応する画像をロードし、ハンドルを返す（初期化時に一度だけ実行）
	uint32_t LoadTexture(SkillID id);

private:
	// 選択肢の数
	static const int kNumChoices = 3;

	// 全スキルの情報
	std::array<SkillData, kSkillCount> allSkills_;

	// ランダムに選ばれた3つのスキル
	std::array<SkillID, kNumChoices> selectedSkills_;

	// 現在選択中のスキルインデックス (0, 1, 2)
	int currentSelectIndex_ = 0;

	// カーソル画像（選択枠）のハンドル
	uint32_t cursorHandle_ = 0;

	// スキル選択画面の状態
	State state_ = kStateSelect;

	// 画面のフェードアウト/イン用の不透明度
	float opacity_ = 1.0f;

	// 乱数生成器
	std::mt19937 engine_;
};