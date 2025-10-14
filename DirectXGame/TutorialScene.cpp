#include "TutorialScene.h"

TutorialScene::~TutorialScene() {
	delete tutorialSprite_;
	delete backgroundSprite_; // ★追加: 背景スプライトの解放
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

	isFinished_ = false;
	timer_ = 0.0f;

	// 初期状態設定
	state_ = State::FadeIn; // 明示的に初期状態を設定
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
		// ★ ゲームシーンへ移行する条件
		// Enterキーが押されたら終了フラグを立てる (Transition状態をスキップ)
		if (input_->TriggerKey(DIK_RETURN)) {
			isFinished_ = true;
		}
		// TODO: 必要に応じて、説明スプライトのアニメーションや点滅などをここに追加
		// 既存の「Press Enter to Start」点滅表示もActive状態でのみ動作させることが推奨されます。
		break;

	case State::Transition:
		// 今回はActiveから直接isFinished_ = true;としているため、ここは空のままでOK
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
	// ★メインスプライトより前に描画する
	if (backgroundSprite_) {
		// 背景もフェードイン演出の透明度を適用 (全体がフェードインするように)
		backgroundSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		backgroundSprite_->Draw();
	}

	// --- メインスプライトの描画 ---
	if (tutorialSprite_) {
		// スプライトの色に透明度を適用
		tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		tutorialSprite_->Draw();
	}

	// 例: 「Press Enter to Start」などの点滅表示はActive状態でのみ動作
	// ... (変更なし) ...

	KamataEngine::Sprite::PostDraw();
}