#pragma once

// ★ main.cpp からここに引っ越しさせる
enum class Scene { Title, Tutorial, Game, GameOver };

class BaseScene {
public:
	virtual ~BaseScene() = default;

	// すべてのシーンで必ず実装する仮想関数
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// シーンを終了して次のシーンへ移りたいかを外側に伝える共通関数
	bool IsFinished() const { return isFinished_; }

	// ★ 追加した関数
	Scene GetNextScene() const { return nextScene_; }

protected:
	bool isFinished_ = false; // 各シーンで終了時に true にする

	// ★ 追加した変数
	Scene nextScene_ = Scene::Title;
};