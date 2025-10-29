#include "TutorialScene.h"
#include <algorithm> // std::min
#include <cmath>     // std::sin, std::pow, std::sqrt を使用するため

// ★追加: イージング関数（EaseOutCubic）
float EaseOutCubic(float t) { return 1.0f - std::pow(1.0f - t, 3.0f); }

TutorialScene::~TutorialScene() {
	delete tutorialSprite_;
	delete tutorialSprite_1; // ★追加
	delete tutorialSprite_2; // ★追加
	delete backgroundSprite_;
	delete fadeOutSprite_;
	delete loadingSprite_;
	delete arrowSprite_; // ★追加
}

void TutorialScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// 初期化に備えてフラグをリセット
	isFinished_ = false;
	isBackToTitle_ = false;
	currentSelectIndex_ = 0; // ★追加: 選択インデックスのリセット

	// ★背景スプライトの初期化
	backgroundTextureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Background.png");
	backgroundSprite_ = KamataEngine::Sprite::Create(backgroundTextureHandle_, {0, 0});

	// ゲーム説明画像 (index 0)
	textureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Tutorial.png");
	tutorialSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	// ★追加: ゲーム説明画像 (index 1)
	textureHandle_1 = KamataEngine::TextureManager::Load("Tutorial/Tutorial2.png"); // ※パスは適宜変更してください
	tutorialSprite_1 = KamataEngine::Sprite::Create(textureHandle_1, {0, 0});

	// ★追加: ゲーム説明画像 (index 2)
	textureHandle_2 = KamataEngine::TextureManager::Load("Tutorial/Tutorial3.png"); // ※パスは適宜変更してください
	tutorialSprite_2 = KamataEngine::Sprite::Create(textureHandle_2, {0, 0});

	// ★追加: フェードアウト用暗転スプライトの初期化 (既存ロジック)
	fadeTextureHandle_ = KamataEngine::TextureManager::Load("white1x1.png");
	fadeOutSprite_ = KamataEngine::Sprite::Create(fadeTextureHandle_, {0.0f, 0.0f}, {1280.0f, 720.0f});
	fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, 0.0f});

	// ★追加: ロード画面用スプライトの初期化 (既存ロジック)
	loadingTextureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Loading.png");
	loadingSprite_ = KamataEngine::Sprite::Create(loadingTextureHandle_, {640.0f, 360.0f}, {0.5f, 0.5f});
	loadingSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});

	// ★追加: 矢印スプライトの作成 (SelectSceneから移植)
	arrowTextureHandle_ = KamataEngine::TextureManager::Load("Tutorial/Sentaku.png");
	arrowSprite_ = KamataEngine::Sprite::Create(arrowTextureHandle_, {0, 0});
	arrowSprite_->SetAnchorPoint({0.5f, 0.5f});

	// ★追加: 矢印の初期位置を設定 (SelectSceneから移植して調整)
	float screenWidth = 1280.0f;
	// チュートリアル画面で矢印を置くY座標を調整
	//float arrowY = 600.0f;
	float arrowY = 200.0f;
	// ページに応じて矢印の位置を変えることを想定し、一旦中央に初期化
	currentArrowX_ = screenWidth * 0.50f;
	currentArrowY_ = arrowY;
	targetArrowX_ = currentArrowX_;
	targetArrowY_ = currentArrowY_;
	arrowSprite_->SetPosition({currentArrowX_, currentArrowY_});

	// 初期状態設定
	state_ = State::FadeIn;
	fadeInTimer_ = 0.0f;
	fadeOutTimer_ = 0.0f;
	loadingTimer_ = 0.0f;
	// timer_ の処理はSelectSceneに無いので削除
}

void TutorialScene::Update() {

	// タイトルに戻るボタンの判定 (例としてDIK_Qを使用)
	// Qキーで強制的にタイトルに戻るロジックは、今回の要望に合わせるため削除または無視します。

	if (isBackToTitle_ || isFinished_ || isToSetting_) { // ★修正: isToSetting_ を追加
		return;
	}

	switch (state_) {
	case State::FadeIn:
		fadeInTimer_ += 1.0f;

		if (fadeInTimer_ >= kFadeInDuration) {
			state_ = State::Active;
		}
		break;

	case State::Active:

		// 左矢印キーが押されたら前の画面へ
		if (input_->TriggerKey(DIK_A)) {
			currentSelectIndex_--;
			if (currentSelectIndex_ < 0) {
				currentSelectIndex_ = 2; // 最後の画面にループ (3ページを想定)
			}
			StartArrowMovement();
		}

		// 右矢印キーが押されたら次の画面へ
		if (input_->TriggerKey(DIK_D)) {
			currentSelectIndex_++;
			if (currentSelectIndex_ > 2) {
				currentSelectIndex_ = 0; // 最初の画面にループ (3ページを想定)
			}
			StartArrowMovement();
		}

		// Enterキーが押されたら、現在の選択に応じて遷移
		if (input_->TriggerKey(DIK_SPACE)) {
			if (currentSelectIndex_ == 0) {
				// インデックス0: タイトルシーンへ戻る
				isBackToTitle_ = true;
			} else if (currentSelectIndex_ == 1) {
				// インデックス1: ゲームシーンへ移行
				state_ = State::Transition;
				fadeOutTimer_ = 0.0f;
				loadingTimer_ = 0.0f;
			} else if (currentSelectIndex_ == 2) {
				// インデックス2: 設定シーンへ移行
				isToSetting_ = true;
			}
		}

		// ★スペースキーでタイトルへ戻る (既存ロジックは削除または無視)

		// ★追加: 矢印のアニメーション更新 (SelectSceneから移植)
		UpdateArrowAnimation();

		break;

	case State::Transition:
		// 移行演出の更新 (フェードアウト + ロード演出)
		if (fadeOutTimer_ < kFadeOutDuration) {
			fadeOutTimer_ += 1.0f;
		} else {
			loadingTimer_ += 1.0f;

			if (loadingTimer_ >= kLoadingHoldDuration) {
				state_ = State::Finished;
				isFinished_ = true;
			}
		}
		break;

	case State::Finished:
		break;
	}
}

// ★追加: 矢印の移動を開始する関数 (SelectSceneから移植)
void TutorialScene::StartArrowMovement() {
	float screenWidth = 1280.0f;
	// チュートリアル画面で矢印を置くY座標
	float arrowY = 200.0f;

	// 現在の位置を目標位置に設定
	targetArrowX_ = currentArrowX_;
	targetArrowY_ = currentArrowY_;

	// 新しい目標位置を設定 (SelectSceneの座標を流用)
	switch (currentSelectIndex_) {
	case 0: // 最初のページ (例: 戻る/前へ の場所)
		targetArrowX_ = screenWidth * 0.23f;
		targetArrowY_ = arrowY;
		break;
	case 1: // 中央のページ
		targetArrowX_ = screenWidth * 0.50f;
		targetArrowY_ = arrowY;
		break;
	case 2: // 最後のページ (例: ゲーム開始/次へ の場所)
		targetArrowX_ = screenWidth * 0.77f;
		targetArrowY_ = arrowY;
		break;
	}

	// アニメーション開始
	isArrowMoving_ = true;
	// 矢印が上下に浮遊するアニメーションのタイマーをリセット
	animationTimer_ = 0.0f;
}

// ★追加: 矢印のアニメーション更新 (SelectSceneから移植)
void TutorialScene::UpdateArrowAnimation() {
	// 上下の浮遊アニメーション
	float floatSpeed = 0.05f;
	float floatAmplitude = 10.0f;
	animationTimer_ += floatSpeed;
	arrowVerticalOffset_ = floatAmplitude * std::sin(animationTimer_);

	// 位置移動のアニメーション
	if (isArrowMoving_) {
		float moveSpeed = 0.1f;
		float distance = static_cast<float>(std::sqrt(std::pow(targetArrowX_ - currentArrowX_, 2.0f) + std::pow(targetArrowY_ - currentArrowY_, 2.0f)));

		if (distance > 1.0f) {
			// イージングでスムーズに移動
			float moveT = moveSpeed;
			moveT = EaseOutCubic(moveT);

			currentArrowX_ += (targetArrowX_ - currentArrowX_) * moveT;
			currentArrowY_ += (targetArrowY_ - currentArrowY_) * moveT;
		} else {
			// 移動完了
			currentArrowX_ = targetArrowX_;
			currentArrowY_ = targetArrowY_;
			isArrowMoving_ = false;
		}
	}

	// 最終的な矢印の位置を設定
	if (arrowSprite_) {
		arrowSprite_->SetPosition({currentArrowX_, currentArrowY_ + arrowVerticalOffset_});
	}
}

void TutorialScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	float alpha = 1.0f;

	if (state_ == State::FadeIn) {
		float t = fadeInTimer_ / kFadeInDuration;
		// EaseOutQuintの代わりにSelectSceneにあるEaseOutCubicを使用
		float t_eased = EaseOutCubic(t);
		alpha = t_eased;
	}

	// --- 背景スプライトの描画 ---
	if (backgroundSprite_) {
		backgroundSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		backgroundSprite_->Draw();
	}

	// --- メインスプライトの描画 ---
	// ★変更: SelectSceneに合わせて選択インデックスに応じて描画するスプライトを切り替え
	switch (currentSelectIndex_) {
	case 0:
		if (tutorialSprite_) {
			tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
			tutorialSprite_->Draw();
		}
		break;
	case 1:
		if (tutorialSprite_1) {
			tutorialSprite_1->SetColor({1.0f, 1.0f, 1.0f, alpha});
			tutorialSprite_1->Draw();
		}
		break;
	case 2:
		if (tutorialSprite_2) {
			tutorialSprite_2->SetColor({1.0f, 1.0f, 1.0f, alpha});
			tutorialSprite_2->Draw();
		}
		break;
	}

	// ★追加: 矢印スプライトを描画
	// Active状態でのみ描画 (フェードイン・アウト中は非表示)
	if (arrowSprite_ && state_ == State::Active) {
		arrowSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha}); // フェードイン中は透明度を適用
		arrowSprite_->Draw();
	}

	// ★最前面に描画: フェードアウト用暗転スプライトの描画 (既存ロジックをEaseOutCubicに修正)
	if (state_ == State::Transition) {
		if (fadeOutSprite_) {
			float t_fade = (std::min)(fadeOutTimer_ / kFadeOutDuration, 1.0f);
			// EaseOutQuintの代わりにSelectSceneにあるEaseOutCubicを使用
			float t_eased_fade = EaseOutCubic(t_fade);

			fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, t_eased_fade});
			fadeOutSprite_->Draw();

			// ロード画面の描画
			if (t_fade >= 1.0f && loadingSprite_) {
				if (loadingTimer_ > 0.0f) {
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