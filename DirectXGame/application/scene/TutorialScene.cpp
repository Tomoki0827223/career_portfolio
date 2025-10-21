#include "TutorialScene.h"

TutorialScene::~TutorialScene() {
	delete tutorialSprite_;
	delete backgroundSprite_; // ★追加: 背景スプライトの解放
	delete fadeOutSprite_;    // ★追加: 暗転用スプライトの解放
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

	isFinished_ = false;
	isBackToTitle_ = false; // ★重要: 戻るフラグを確実にリセット
	timer_ = 0.0f;

	// 初期状態設定
	state_ = State::FadeIn;
	fadeInTimer_ = 0.0f;
}

void TutorialScene::Update() {
	timer_ += 1.0f;

	switch (state_) {
	case State::FadeIn:
		fadeInTimer_ += 1.0f;

		if (fadeInTimer_ >= kFadeInDuration) {
			// フェードイン完了 -> Active状態へ移行
			state_ = State::Active;
		}
		// フェードイン中は操作を受け付けない
		break;

	case State::Active:
		// ★スペースキーでタイトルへ戻る
		if (input_->TriggerKey(DIK_SPACE)) {
			isBackToTitle_ = true;
		}

		// ★ ゲームシーンへ移行する条件
		// Enterキーが押されたら Transition 状態へ移行
		if (input_->TriggerKey(DIK_RETURN)) {
			state_ = State::Transition; // ★修正: 移行演出へ
			fadeOutTimer_ = 0.0f;       // ★追加: タイマーリセット
		}
		// TODO: 必要に応じて、説明スプライトのアニメーションや点滅などをここに追加
		break;

	case State::Transition:
		// 移行演出の更新 (フェードアウト)
		fadeOutTimer_ += 1.0f;

		if (fadeOutTimer_ >= kFadeOutDuration) {
			// フェードアウト完了 -> Finished状態へ移行し、次のシーンへ
			state_ = State::Finished; // ★修正: Finished状態へ
			isFinished_ = true;       // ★修正: ここでフラグを立てる
		}
		break;

	case State::Finished: // ★追加: Finished状態での処理
		// isFinished_ が true の間、Draw で暗転が維持される
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
			float t = (std::min)(fadeOutTimer_ / kFadeOutDuration, 1.0f);

			// イージングを適用 (ここでは既存のEaseOutQuintを使用)
			float t_eased = EaseOutQuint(t);

			// 暗転スプライトの透明度を 0.0 (透明) から 1.0 (不透明) へ変化させる
			// 色はInitializeで黒({0,0,0})に設定済み
			fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, t_eased});
			fadeOutSprite_->Draw();
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