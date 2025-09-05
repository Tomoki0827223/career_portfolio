#pragma once

#include <KamataEngine.h>

using namespace KamataEngine;

class Experience {
public:
	Experience(int textureHandle, Vector2 position, Model* model = nullptr);
	void Update();
	void Draw2D();                                                   // 2D描画用
	void Draw3D(const Camera& camera);                               // 3D描画用// 修正: カメラをconst参照で受け取るように変更
	Vector3 GetPosition() const;                                     // 修正: 戻り値をVector3に
	bool IsCollected(const Vector3& playerPos, float collectRadius); // 修正: 引数をVector3に

private:
	Sprite* sprite_;
	Vector2 position_;
	bool collected_;
	Model* model_ = nullptr;
};