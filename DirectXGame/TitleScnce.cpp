#include "TitleScnce.h"
#include <numbers>

TitleScnce::~TitleScnce() {
	delete titlemodel_;
	delete titlemodelFont_;
	delete TitleSkydome_;
	delete sprite_;
	delete sprite2_;
	delete sprite3_;
	delete sprite4_;
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

	// タイトルを中央に寄せるために調整
	titleWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f}; // x, y, zの値を調整

	titleWorldTransformFont_.translation_ = {0.0f, 0.0f, 0.0f}; // x, y, zの値を調整

	titleskydome.translation_ = {0.0f, 0.0f, 0.0f};

	// sprite2_の初期位置を画面外（上）に設定
	if (sprite2_) {
		sprite2_->SetPosition({0, -static_cast<float>(1280)}); // 画面上部外へ（高さは適宜調整）
	}

	// スプライトの初期化
	InitializeSprites();
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

	case State::Transition:
		// 移行演出の更新
		transitionTimer_ += 1.0f;

		if (transitionTimer_ < kTransitionDuration) {
			// 進行度 (0.0fから1.0f)
			float t = transitionTimer_ / kTransitionDuration;
			// ★ イージングを適用 (t_eased: 0.0 -> 1.0へ滑らかに変化)
			float t_eased = EaseOutQuint(t);

			// タイトルロゴ(sprite2)を上にスライドアウトさせる演出
			if (sprite2_) {
				float startY = 0.0f;
				float endY = -static_cast<float>(720); // 画面外のY座標
				// イージングでY座標を補間 (Lerp)
				float currentY = startY * (1.0f - t_eased) + endY * t_eased;
				sprite2_->SetPosition({sprite2_->GetPosition().x, currentY});
			}

			// "Hit Enter" (sprite3) をフェードアウト
			if (sprite3_) {
				sprite3_->SetColor({1.0f, 1.0f, 1.0f, 1.0f - t});
			}

		} else {
			// 演出完了
			state_ = State::Finished;
			isFinished_ = true; // 次のシーン (Tutorial) へ移行するためのフラグ
		}
		break;

	case State::Finished:
		break;
	}
}

void TitleScnce::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 3Dオブジェクト描画
	KamataEngine::Model::PreDraw();
	KamataEngine::Model::PostDraw();

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	// ★ここで背景スプライトを全画面に描画
	// 例: 1280x720の青色背景
	// KamataEngine::Sprite::DrawRect({0, 0}, {1280, 720}, {0.4f, 0.6f, 0.9f, 1.0f}); // RGBA

	sprite_->SetPosition({0, 0});
	sprite_->Draw();

	// タイトル画像
	if (sprite2_) {
		sprite2_->Draw();
	}

	// 「Hit Enter」
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

	KamataEngine::Sprite::PostDraw();
}