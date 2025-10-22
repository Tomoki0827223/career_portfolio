#pragma once
#include "KamataEngine.h"
#include "application/ui/BIt_Map_Font.h"
#include <cmath>
#include <random>
#include <vector>

using namespace KamataEngine;

// LevelUpManagerで使用するSkillTypeを定義（GameScene.hから移動）
enum class SkillType {
	kAttackUp,  // 攻撃力アップ
	kSpeedUp,   // 移動速度アップ
	kHeal,      // HP回復
	kSkillCount // スキル数のカウント用
};

class LevelUpManager {
public:
	LevelUpManager();
	~LevelUpManager();

	void Initialize();
	void Update();
	void Draw();

	// 経験値の加算とレベルアップ判定
	void AddExperience(int expValue);

	// 状態取得
	bool IsLevelUpPending() const { return isLevelUpPending_; }

private:
	// プレイヤーへの参照（スキル適用のため、後でPlayerクラスの機能拡張が必要）
	// Player* player_ = nullptr; // TODO: GameSceneから参照を渡す必要あり

	// レベルアップシステム関連
	int level_ = 1;               // 現在のレベル
	int currentExp_ = 0;          // 現在の経験値
	int requiredExp_ = 10;        // 次のレベルまでに必要な経験値
	const int kExpBase = 10;      // 最初の必要経験値
	const float kExpScale = 1.2f; // 必要経験値の増加率

	bool isLevelUpPending_ = false;              // レベルアップ待ち状態 (スキル選択画面表示中)
	int selectedSkillIndex_ = 0;                 // 選択中のスキルインデックス (0, 1, 2)
	std::vector<SkillType> currentSkillOptions_; // 現在のスキル選択肢 (3つ)

	// スプライト（GameSceneから移動）
	KamataEngine::Sprite* skillScreenBackground_ = nullptr; // 半透明の背景
	KamataEngine::Sprite* skillOptionSprites_[3] = {};      // 3つの選択肢の背景
	KamataEngine::Sprite* skillCursorSprite_ = nullptr;     // 選択カーソル

	uint32_t whiteTextureHandle_ = 0; // スプライトの色付けに使う1x1の白テクスチャ

	// 乱数生成器 (GameScene.cppから移動)
	std::mt19937 engine_;

	// プライベート関数（GameSceneから移動）
	void StartLevelUp();              // レベルアップ開始
	void UpdateSkillSelection();      // スキル選択画面の更新
	void ApplySkill(SkillType skill); // 選択したスキルを適用
};