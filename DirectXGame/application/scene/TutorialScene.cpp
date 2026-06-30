#include "TutorialScene.h"
#include <algorithm> // std::min
#include <cmath>     // ★追加: ロード画面の点滅演出に std::sin を使用するため

TutorialScene::~TutorialScene() {
	delete tutorialSprite_;
	delete backgroundSprite_; // ★追加: 背景スプライトの解放
	delete fadeOutSprite_;    // ★追加: 暗転用スプライトの解放
	delete loadingSprite_;    // ★追加: ロードスプライトの解放
}

void TutorialScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// ★背景スプライトの初期化
	// 例: "Tutorial/Background.png" を読み込む (画像ファイル名は適宜変更してください)
	backgroundTextureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Background.png");
	// 画面いっぱいに表示するために、座標 {0, 0} でスプライトを作成
	// ※ 画面サイズに合わせてサイズや座標を調整してください
	backgroundSprite_ = KamataEngine::Sprite::Create(backgroundTextureHandle_, {0, 0});

	// ゲーム説明画像 (例: Tutorial.png) を読み込む
	textureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Tutorial.png");
	tutorialSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	// ★追加: フェードアウト用暗転スプライトの初期化
	// 1x1の白テクスチャを読み込み、サイズを画面全体(1280x720)に広げる
	fadeTextureHandle_ = KamataEngine::TextureManager::Load("white1x1.png"); // ★1x1の白テクスチャを想定
	fadeOutSprite_ = KamataEngine::Sprite::Create(
	    fadeTextureHandle_, {0.0f, 0.0f}, // 座標
	    {1280.0f, 720.0f}                 // サイズ (画面全体)
	);
	// 色を黒(R=0, G=0, B=0)に設定し、透明度(A=0.0f)で初期化
	fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, 0.0f});

	// ★追加: ロード画面用スプライトの初期化
	// 画像ファイル名は適宜変更してください (例: Transition/Loading.png)
	loadingTextureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Loading.png");
	// 画面中央({640, 360})に表示し、アンカーポイントを中央({0.5, 0.5})に設定
	loadingSprite_ = KamataEngine::Sprite::Create(
	    loadingTextureHandle_, {640.0f, 360.0f}, // 座標を画面中央に設定 (1280x720の半分)
	    {0.5f, 0.5f}                             // アンカーポイントを中央に設定
	);
	// 初期状態は透明にしておく (描画しない)
	loadingSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});

	timer_ = 0.0f;

	// 初期化の時点では、シーンはまだ終了していないので false にする
	isFinished_ = false;
	isBackToTitle_ = false;
	nextScene_ = Scene::Game; // デフォルトの移行先をゲームにしておく

	// 初期状態設定
	state_ = State::FadeIn;
	fadeInTimer_ = 0.0f;
	fadeOutTimer_ = 0.0f;
	loadingTimer_ = 0.0f;
}

void TutorialScene::Update() {
	// --- コントローラー情報の取得 ---
	XINPUT_STATE joyState;
	bool hasJoy = input_->GetJoystickState(0, joyState);

	// ★ 修正: タイトルに戻る操作がされた時
	if (input_->TriggerKey(DIK_Q) || (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		isFinished_ = true;        // mainにシーン終了を伝える
		nextScene_ = Scene::Title; // 移行先を「タイトル」に指定
		return;
	}

	if (isFinished_) {
		return;
	}

	timer_ += 1.0f;

	switch (state_) {
	case State::FadeIn:
		fadeInTimer_ += 1.0f;
		if (fadeInTimer_ >= kFadeInDuration) {
			state_ = State::Active;
		}
		break;

	case State::Active: {
		// --- 決定操作 (Enterキー または Aボタン) ---
		bool enterTrigger = input_->TriggerKey(DIK_RETURN);
		if (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			enterTrigger = true;
		}

		if (enterTrigger) {
			state_ = State::Transition;
			fadeOutTimer_ = 0.0f;
			loadingTimer_ = 0.0f;
		}

		// スペースキーでもタイトルに戻れるようにする場合
		if (input_->TriggerKey(DIK_SPACE)) {
			isFinished_ = true;
			nextScene_ = Scene::Title;
		}
	} break;

	case State::Transition:
		if (fadeOutTimer_ < kFadeOutDuration) {
			fadeOutTimer_ += 1.0f;
		} else {
			loadingTimer_ += 1.0f;
			if (loadingTimer_ >= kLoadingHoldDuration) {
				state_ = State::Finished;

				// ★ 修正: 暗転とロード演出が完全に終わったら、満を持してゲームシーンへ移行する
				isFinished_ = true;
				nextScene_ = Scene::Game; // 移行先を「ゲーム」に指定
			}
		}
		break;

	case State::Finished:
		break;
	}
}


void TutorialScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	float alpha = 1.0f; // デフォルトは不透明

	if (state_ == State::FadeIn) {
		// 進行度 (0.0fから1.0f)
		float t = fadeInTimer_ / kFadeInDuration;
		// イージングを適用
		// ※ EaseOutQuintはユーザーが別途定義している関数と仮定します
		float t_eased = EaseOutQuint(t);

		// 透明度を 0.0 (透明) から 1.0 (不透明) へ変化させる
		alpha = t_eased;
	}

	// --- 背景スプライトの描画 ---
	if (backgroundSprite_) {
		backgroundSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		backgroundSprite_->Draw();
	}

	// --- メインスプライトの描画 ---
	if (tutorialSprite_) {
		tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		tutorialSprite_->Draw();
	}

	// ★最前面に描画: フェードアウト用暗転スプライトの描画
	if (state_ == State::Transition) {
		if (fadeOutSprite_) {
			// 進行度 (0.0fから1.0f)
			float t_fade = (std::min)(fadeOutTimer_ / kFadeOutDuration, 1.0f);

			// イージングを適用 (ここでは既存のEaseOutQuintを使用)
			float t_eased_fade = EaseOutQuint(t_fade);

			// 暗転スプライトの透明度を 0.0 (透明) から 1.0 (不透明) へ変化させる
			// 色はInitializeで黒({0,0,0})に設定済み
			fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, t_eased_fade});
			fadeOutSprite_->Draw();

			// ★追加: ロード画面の描画 (画面が完全に黒になった後)
			if (t_fade >= 1.0f && loadingSprite_) {
				// ロード演出の進捗 (0.0fから1.0f)
				// float t_load = (std::min)(loadingTimer_ / kLoadingHoldDuration, 1.0f);

				// タイマーが進んでいれば描画する
				if (loadingTimer_ > 0.0f) {
					// 例: loadingTimer_による点滅演出 (0.5 ~ 1.0 の間で点滅)
					// 点滅の周期を調整したい場合は '0.1f' の値を変更してください
					float blink = std::abs(std::sin(loadingTimer_ * 0.1f)) * 0.5f + 0.5f;
					loadingSprite_->SetColor({1.0f, 1.0f, 1.0f, blink});
					loadingSprite_->Draw();
				}
			}
		}
	} else if (state_ == State::Finished) {
		// 演出完了後は画面を完全に黒で覆ったままにする
		if (fadeOutSprite_) {
			fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
			fadeOutSprite_->Draw();
		}
	}

	KamataEngine::Sprite::PostDraw();
}