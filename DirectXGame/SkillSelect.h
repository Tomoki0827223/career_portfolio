#pragma once

#include "2d/Sprite.h"
#include "KamataEngine.h"
#include "application/3d/Player.h" // スキル適用に必要
#include "base/TextureManager.h"
#include "input/Input.h"
#include "math/Vector2.h"
#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

using namespace KamataEngine;

// ★ GameSceneから移管: 4つのスキルを識別するための列挙型 ★
enum class SkillType {
	kBook,      // Book: プレイヤーの周りを回る攻撃 (book.png)
	kBullet,    // Bullet: 自動で敵に撃つ (Gan.png)
	kHeart,     // Heart: HP回復 (Hart.png)
	kWine,      // Wine: ランダムドロップアイテムの出現 (Wine.png)
	kSkillCount // スキル数のカウント用
};

class SkillSelect {
public:
	// シングルトンパターン (GameSceneが利用するため)
	static SkillSelect* GetInstance();

	SkillSelect() = default;
	~SkillSelect();

	// 初期化処理（UIスプライトの生成とテクスチャロード）
	void Initialize(Player* player);

	// スキル選択を開始する (ランダム選択と状態リセット)
	void StartSelection();

	// 更新処理（入力、カーソル移動）
	void Update();

	// 描画処理（背景、アイコン、カーソル）
	void Draw();

	// スキル選択中かどうかを返す
	bool IsSelecting() const { return isSelecting_; }

private:
	// 選択されたスキルに効果を適用する (GameSceneから移管)
	void ApplySkill(SkillType skill);

	// スキルIDに対応するテクスチャハンドルを取得
	uint32_t GetTextureHandle(SkillType id) const;

	// スキルIDに対応する情報 (名前・説明) を取得
	std::pair<std::string, std::string> GetSkillInfo(SkillType type) const;

private:
	// 依存オブジェクト
	Player* player_ = nullptr; // スキル適用対象

	// UIパーツ (GameSceneから移管)
	Sprite* skillScreenBackground_ = nullptr; // 半透明の背景
	Sprite* skillOptionSprites_[3] = {};      // 3つの選択肢の背景
	Sprite* skillCursorSprite_ = nullptr;     // 選択カーソル

	// ★ 追加: アイコン描画用のSpriteをメンバとして保持する ★
	Sprite* skillIconSprites_[3] = {}; // ← これを追加

	// テクスチャハンドル (GameSceneから移管)
	uint32_t whiteTextureHandle_ = 0;
	uint32_t skillTextureHandles_[static_cast<int>(SkillType::kSkillCount)] = {};

	// スキル選択ロジック (GameSceneから移管)
	bool isSelecting_ = false;                   // スキル選択中か
	int selectedSkillIndex_ = 0;                 // 選択中のスキルインデックス (0, 1, 2)
	std::vector<SkillType> currentSkillOptions_; // 現在のスキル選択肢 (3つ)

	// 乱数生成器
	std::mt19937 engine_;
};