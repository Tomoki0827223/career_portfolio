#define NOMINMAX
#include "TitleScnce.h"
#include <algorithm> // std::min/max/absを使うために必要
#include <numbers>

TitleScnce::~TitleScnce() {
	delete titlemodel_;
	delete titlemodelFont_;
	delete TitleSkydome_;
	delete sprite_;
	delete sprite2_;
	delete sprite3_;
	delete sprite4_;

	// ★追加: スライドスプライトの解放
	for (int i = 0; i < kSlideCount; ++i) {
		delete slideSprites_[i];
	}
}

void TitleScnce::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	textureHandle_ = KamataEngine::TextureManager::Load("Title/Title1.png");
	sprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	textureHandle2_ = KamataEngine::TextureManager::Load("Title/ShotGame.png");
	sprite2_ = KamataEngine::Sprite::Create(textureHandle2_, {0, 0});

	textureHandle3_ = KamataEngine::TextureManager::Load("Title/HitEnter.png");
	sprite3_ = KamataEngine::Sprite::Create(textureHandle3_, {0, 0});

	titleskydome.Initialize();
	Camera_.Initialize();

	Timer_ = 0.0f;

	// ★修正: 状態とフラグのリセットを整理
	state_ = State::TitleScreen;
	isFinished_ = false; // ★重要: シーン移行フラグを必ずリセット
	isGameFinished_ = false;

	// タイトルを中央に寄せるために調整
	titleWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	titleWorldTransformFont_.translation_ = {0.0f, 0.0f, 0.0f};
	titleskydome.translation_ = {0.0f, 0.0f, 0.0f};

	// sprite2_の初期位置を画面外（上）に設定
	if (sprite2_) {
		sprite2_->SetPosition({0, -static_cast<float>(1280)}); // 画面上部外へ（高さは適宜調整）
	}

	// スプライトの初期化 (既存のタイトル画像用)
	InitializeSprites();

	// ★スライドアウト演出用スプライトの初期化
	int screenWidth = 1280;
	int screenHeight = 720;
	int slideWidth = screenWidth / kSlideCount;

	for (int i = 0; i < kSlideCount; ++i) {
		// ★修正: 5種類の異なるテクスチャを読み込むように変更
		std::string filename = "Transition/Slide" + std::to_string(i) + ".png";
		slideTextureHandle_[i] = KamataEngine::TextureManager::Load(filename.c_str());

		slideSprites_[i] = KamataEngine::Sprite::Create(
		    slideTextureHandle_[i],
		    // X座標: 画面外の左側 (-スプライト幅) に設定
		    {-static_cast<float>(slideWidth), 0.0f}, // Y座標は0.0fで修正
		    // スプライトサイズ: 画面幅の1/5, 画面高全体
		    {static_cast<float>(slideWidth), static_cast<float>(screenHeight)});

		slideSprites_[i]->SetAnchorPoint({0.0f, 0.0f});

		// ★修正: スプライトの色を白（テクスチャの色をそのまま反映）に変更
		// テクスチャを使いたいので、黒の強制設定を解除し、不透明な白（テクスチャの色そのまま）にします。
		slideSprites_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	}
}

void TitleScnce::InitializeSprites() {
	sprites.push_back(sprite_);
	sprites.push_back(sprite2_);
	sprites.push_back(sprite3_);
	sprites.push_back(sprite4_);
}

void TitleScnce::Update() {
	Timer_ += 1.0f; // フレームごとに加算

	switch (state_) {
	case State::TitleScreen:
		// Enterキーで演出開始 (Transition状態へ移行)
		if (input_->TriggerKey(DIK_RETURN)) {
			// audio_->PlayWave(TitleSEHandle3_, false); // スタートSE
			state_ = State::Transition;
			transitionTimer_ = 0.0f; // タイマーリセット
		}

		// 降下アニメーション (TitleScreen状態でのみ継続)
		if (sprite2_) {
			auto pos = sprite2_->GetPosition();
			float targetY = 0.0f;
			float speed = 20.0f;
			if (pos.y < targetY) {
				pos.y += speed;
				if (pos.y > targetY)
					pos.y = targetY;
				sprite2_->SetPosition(pos);
			}
		}
		break;

	case State::Transition: { // ★スコープ追加
		// 移行演出の更新
		transitionTimer_ += 1.0f;

		// 変数定義をcase直下に移動
		int screenWidth = 1500;
		int slideWidth = screenWidth / kSlideCount;
		float screenCenterX = static_cast<float>(screenWidth) / 2.0f;

		// 演出が完全に終わるまで継続
		if (transitionTimer_ < kTransitionDuration + kSlideDuration * kSlideCount) {

			// --- A. 5枚のスライド処理 ---
			for (int i = 0; i < kSlideCount; ++i) {
				// 各スライドの開始時間 (時間差)
				float startTime = kSlideDuration * i;

				// スライド単体の演出時間 (現在の経過時間 - 遅延時間)
				float slideElapsedTime = transitionTimer_ - startTime; // ★C2065エラー解消

				if (slideElapsedTime >= 0.0f) {
					// 進行度 (0.0fから1.0f)
					float t = (std::min)(slideElapsedTime / kSlideDuration, 1.0f);

					// t_eased の定義
					float t_eased = EaseOutQuint(t);

					// スライドの動作: 画面左端外 (-slideWidth) から、画面中央で停止するロジック
					float startX = -static_cast<float>(slideWidth);

					// 終了位置: 中央に5枚が隙間なく並んだ位置の、i番目のスライドの開始X座標
					float totalSlidesWidth = static_cast<float>(slideWidth * kSlideCount);
					float centerStartX = screenCenterX - (totalSlidesWidth / 2.0f);
					float endX_center = centerStartX + static_cast<float>(slideWidth * i);

					// 補間されたX座標 (画面中央で停止するアニメーション)
					float currentX = startX * (1.0f - t_eased) + endX_center * t_eased;

					// スライドパネルの位置を更新
					slideSprites_[i]->SetPosition({currentX, 0.0f});
				}
			}

			// --- B. タイトルロゴなどのフェードアウト処理 ---
			float t_total = transitionTimer_ / kTransitionDuration;

			if (sprite2_) {
				// sprite2はTransition開始時の位置から上にフェードアウト
				float startY = sprite2_->GetPosition().y;
				float endY = -static_cast<float>(720);
				float t_eased = EaseOutQuint(t_total); // イージング適用

				float currentY = startY * (1.0f - t_eased) + endY * t_eased;
				sprite2_->SetPosition({sprite2_->GetPosition().x, currentY});
			}

			if (sprite3_) {
				// sprite3は透明度を下げてフェードアウト
				sprite3_->SetColor({1.0f, 1.0f, 1.0f, (std::max)(0.0f, 1.0f - t_total)});
			}

		} else {
			// 演出完了
			state_ = State::Finished;
			isFinished_ = true; // 次のシーン (Tutorial) へ移行するためのフラグ
		}
	} // ★スコープ終了
	break;

	case State::Finished:
		break;
	}
}


void TitleScnce::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 3Dオブジェクト描画 (省略)

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	// 1. 固定要素を描画
	//sprite_->SetPosition({0, 0});
	//sprite_->Draw();

	// 2. タイトルロゴを描画
	if (sprite2_) {
		sprite2_->Draw();
	}

	// 3. 「Hit Enter」を描画
	if (state_ == State::TitleScreen) {
		// TitleScreen時は点滅制御
		if (sprite3_ && static_cast<int>(Timer_) % 60 < 30) {
			sprite3_->SetPosition({0, 0});
			sprite3_->Draw();
		}
	} else if (state_ == State::Transition) {
		// Transition時はUpdateで設定された透明度で常に描画 (フェードアウト演出)
		if (sprite3_) {
			sprite3_->SetPosition({0, 0});
			sprite3_->Draw();
		}
	}

	// ★最前面に描画: Transition状態でのみスライドを描画する
	if (state_ == State::Transition) {
		for (int i = 0; i < kSlideCount; ++i) {
			if (slideSprites_[i]) {
				slideSprites_[i]->Draw();
			}
		}
	}

	KamataEngine::Sprite::PostDraw();
}

void TitleScnce::DrawBackground() {
	// 2D背景スプライトの描画を一時的に無効化
	/* if (sprite_) {
	    sprite_->SetPosition({0, 0});
	    sprite_->Draw();
	}
	*/
}