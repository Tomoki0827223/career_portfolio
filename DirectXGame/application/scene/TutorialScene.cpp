#include "TutorialScene.h"
#include <algorithm> // std::min
#include <cmath>     // ★追加: ロード画面の点滅演出に std::sin を使用するため

TutorialScene::~TutorialScene() {
	delete tutorialSprite_;
	delete backgroundSprite_;
	delete fadeOutSprite_;
	delete loadingSprite_; // ロード画面スプライト
	delete slideSprite_;   // ★追加: スライド用スプライトの解放
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

	// ★追加: ロード画面用の暗転/スライド用スプライトの初期化
	// 画面全体を覆う黒いスプライトを作成します。
	slideTextureHandle_ = KamataEngine::TextureManager::Load("white1x1.png"); // 白い1x1テクスチャを使用
	                                                                          // 画面外の上側（Y=-720）に配置。アンカーポイントはデフォルトの左上({0, 0})を使用
	slideSprite_ = KamataEngine::Sprite::Create(
	    slideTextureHandle_, {0.0f, -720.0f} // ★修正: 初期位置を画面外の上側（画面の高さ分マイナス）に設定
	);
	slideSprite_->SetSize({1280.0f, 720.0f});         // 画面全体を覆うサイズ
	slideSprite_->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 色は完全に不透明な黒

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


	isFinished_ = false;
	isBackToTitle_ = false; // ★重要: 戻るフラグを確実にリセット
	timer_ = 0.0f;

	// 初期状態設定
	state_ = State::FadeIn;
	fadeInTimer_ = 0.0f;
	// ★追加: ロードタイマーのリセット
	fadeOutTimer_ = 0.0f;
	loadingTimer_ = 0.0f;
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
			loadingTimer_ = 0.0f;       // ★追加: ロードタイマーリセット
		}
		// TODO: 必要に応じて、説明スプライトのアニメーションや点滅などをここに追加
		break;


	case State::Transition:
		// 移行演出の更新 (フェードアウト + ロード演出)
		if (fadeOutTimer_ < kFadeOutDuration) {
			// フェーズ1: フェードアウト中 (暗転スプライトで画面全体を徐々に暗くする)
			fadeOutTimer_ += 1.0f;
		} else {
			// フェーズ2＆3: ロード画面の演出 (画面が完全に黒になった後)
			loadingTimer_ += 1.0f;

			if (slideSprite_) {
				float slideStartPosY = -720.0f; // 画面外上側
				float slideCenterPosY = 0.0f;   // 画面上端

				// フェーズ2: スライドイン (0 -> kSlideDuration)
				if (loadingTimer_ < kSlideDuration) {
					float t = loadingTimer_ / kSlideDuration;
					float easedT = easeOutBounce(t);

					float currentY = slideStartPosY + (slideCenterPosY - slideStartPosY) * easedT;
					// ★修正: SetPositionを使用
					slideSprite_->SetPosition({0.0f, currentY});

				}
				// フェーズ2: 中央で待機 (kSlideDuration -> kLoadingHoldDuration)
				else if (loadingTimer_ < kLoadingHoldDuration) {
					// SetPositionで位置を維持
					slideSprite_->SetPosition({0.0f, slideCenterPosY});
				}
				// フェーズ3: スライドアウト (kLoadingHoldDuration -> kLoadingHoldDuration + kSlideDuration)
				else if (loadingTimer_ < kLoadingHoldDuration + kSlideDuration) {
					float t = (loadingTimer_ - kLoadingHoldDuration) / kSlideDuration;
					// easeOutBounceを逆再生することで跳ねるように下へ退場させる
					float easedT = easeOutBounce(t);

					float slideEndPosY = 720.0f; // 画面下端 (画面外へ完全に出る手前)
					float currentY = slideCenterPosY + (slideEndPosY - slideCenterPosY) * easedT;
					// ★修正: SetPositionを使用
					slideSprite_->SetPosition({0.0f, currentY});

				}
				// フェーズ3: 完了 -> 次のシーンへ
				else {
					state_ = State::Finished;
					isFinished_ = true;
				}

				//slideSprite_->UpdateMatrix(); // スプライトの行列更新
			}
		}
		break;

	case State::Finished:
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
		// ※ EaseOutQuintはユーザーが別途定義している関数と仮定します
		float t_eased = EaseOutQuint(t);

		// 透明度を 0.0 (透明) から 1.0 (不透明) へ変化させる
		alpha = t_eased;
	}

// --- 背景・チュートリアルスプライトの描画 ---
	if (backgroundSprite_) {
		backgroundSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		backgroundSprite_->Draw();
	}
	if (tutorialSprite_) {
		tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		tutorialSprite_->Draw();
	}

	// ★フェードアウト/ロード画面の描画
	if (state_ == State::Transition) {
		float t_fade = (std::min)(fadeOutTimer_ / kFadeOutDuration, 1.0f);
		float t_eased_fade = EaseOutQuint(t_fade);

		// フェーズ1: 暗転用スプライトの描画 (フェードアウト)
		if (fadeOutSprite_) {
			fadeOutSprite_->SetColor({0.0f, 0.0f, 0.0f, t_eased_fade});
			fadeOutSprite_->Draw();
		}


		// フェーズ2/3: ロード画面（スライドスプライト）の描画
		if (t_fade >= 1.0f && slideSprite_) {
			// slideSpriteはUpdateで座標が更新されているのでDrawするだけ
			slideSprite_->Draw();

			// Now Loading画像/テキストの描画 (slideSpriteが画面内にある時のみ)
			if (loadingSprite_ && loadingTimer_ > 0.0f && loadingTimer_ < kLoadingHoldDuration + kSlideDuration) {

				// ★修正: loadingSpriteの位置を、slideSpriteの位置 + 画面中央オフセット に設定
				// slideSpriteの現在位置を取得し、中央揃えの位置に配置
				KamataEngine::Vector2 slidePos = slideSprite_->GetPosition(); // GetPositionが存在すると仮定
				loadingSprite_->SetPosition({slidePos.x + 640.0f, slidePos.y + 360.0f});

				// 点滅演出
				float currentT = (std::min)(loadingTimer_ / kLoadingHoldDuration, 1.0f); // 0.0 -> 1.0
				if (currentT < 1.0f) {
					float blink = std::abs(std::sin(loadingTimer_ * 0.1f)) * 0.5f + 0.5f;
					loadingSprite_->SetColor({1.0f, 1.0f, 1.0f, blink});
				} else {
					loadingSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
				}
				// ★削除: UpdateMatrixはSpriteクラスには通常存在しないため削除
				loadingSprite_->Draw();
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