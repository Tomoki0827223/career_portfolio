#pragma once
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "math/MathUtility.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Enemy {
public:
    Enemy(const Vector3& position); 
    virtual ~Enemy(); // デストラクタも virtual にする

	// 仮想関数の定義（基本種のパラメータを返す）
	virtual float GetBulletSpeed() const { return 1.0f; }
	virtual int GetBulletDamage() const { return 10; }

	virtual void Initialize();                          // virtual を追加
	virtual void Update(const Vector3& playerPosition); // virtual を追加

	void Draw(const Camera& camera);
    
    // ★追加: 敵の位置を取得するゲッター
	const Vector3& GetPosition() const { return worldTransform.translation_; }

    // 衝突判定/被弾判定用
    //Vector3 GetPosition() const { return worldTransform.translation_; }
    float GetRadius() const { return radius_; }
    bool IsDead() const { return isDead_; }

    // ダメージ処理
    void TakeDamage(int damage);

    // Enemy.h のクラス定義内に追加
	virtual bool CanShoot() { return false; } // 基本は撃たない
	virtual void ResetShotTimer() {}          // 派生クラスでオーバーライド
	virtual Vector3 GetShotPosition() { return worldTransform.translation_; }
	virtual int GetType() { return 0; } // 敵のタイプ判別用

protected: // private から protected に変更
	// 敵のステータス
	const float kMoveSpeed = 0.1f;
	const float radius_ = 1.0f;
	const int kMaxHp = 10;
	int currentHp_ = kMaxHp;

	// モデルとワールド変換
	Model* model_ = nullptr;
	WorldTransform worldTransform;

	bool isDead_ = false;
};