#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

/// <summary>
/// 設定シーン
/// </summary>
class SettingScene {
public:
	~SettingScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// タイトルに戻るかを取得
	/// </summary>
	bool IsBackToTitle() const { return isBackToTitle_; }

private:
	bool isBackToTitle_ = false;
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	// 例: 設定画面の背景スプライト
	Sprite* backgroundSprite_ = nullptr;
	uint32_t backgroundTextureHandle_ = 0;
};