#pragma once
#include "BIt_Map_Font.h"
#include "GameLogic.h"     // GameLogicで敵などを管理しているのでこれがあればOK
#include "GameOverScene.h" // main.cppで使っているが、ここでのincludeは不要かも（後述）
#include "KamataEngine.h"
#include "Particle.h"
#include "Player.h"
#include "Stage.h"
#include "math/MathUtility.h"
#include <vector>

using namespace KamataEngine;

class GameScene {
public:
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsGameOver() const { return isGameOver_; }
	// ゲームオーバーフラグをリセットするためのpublicメソッド
	void ResetGameOverFlag() { isGameOver_ = false; }

	void ParticleBorn(Vector3 position);

	void SetIsBackground(bool isBackground) { isBackground_ = isBackground; }
	bool isBackground_ = false;

	Player* GetPlayer() const { return player_; } // ★追加

	int GetScore() const { return gameLogic_->GetScore(); }

private:

	// HPバー描画関数
	void DrawHPBar();
	void DrawEXPBar();

	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* sprite_ = nullptr;

	// カメラ・モデル・ステージ・プレイヤー
	Camera camera_;
	WorldTransform worldTransform; // worldTransform_ と重複している可能性あり（後述）

	Stage* stage_ = nullptr;
	Player* player_ = nullptr;
	Model* playerModel_ = nullptr;
	Model* model_ = nullptr;

	// フォント
	BIt_Map_Font* font_ = nullptr;

	// ★★★ GameLogic クラスのインスタンス ★★★
	GameLogic* gameLogic_ = nullptr;

	// UI関連
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;

	KamataEngine::Sprite* expBarBase_ = nullptr;
	KamataEngine::Sprite* expBar_ = nullptr;
	uint32_t expBarBaseTexture_ = 0;
	uint32_t expBarTexture_ = 0;

	uint32_t sousaTextureHandle_ = 0;
	uint32_t sousaTextureHandle2_ = 0;
	KamataEngine::Sprite* sousaSprite_ = nullptr;
	KamataEngine::Sprite* sousaSprite2_ = nullptr;

	// ゲームオーバーフラグ
	bool isGameOver_ = false;

	// スキル選択画面用UI
	KamataEngine::Sprite* skillScreenBackground_ = nullptr;
	KamataEngine::Sprite* skillOptionSprites_[3] = {};
	KamataEngine::Sprite* skillCursorSprite_ = nullptr;
	uint32_t whiteTextureHandle_ = 0;

	// パーティクル関連
	Model* modelParticle_ = nullptr;
	Particle* particle_ = nullptr; // 使っていないなら削除可

	// GameSceneのメンバ変数にもう一つ `WorldTransform worldTransform_;` がある可能性があります
	// 上記の `worldTransform` と重複していないか確認推奨

	std::list<Particle*> particles_;

};