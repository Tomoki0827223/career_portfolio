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
    ~Enemy();

    void Initialize();
    // プレイヤーの位置を受け取り、追尾・更新を行う
    void Update(const Vector3& playerPosition); 
    void Draw(const Camera& camera);
    
    // ★追加: 敵の位置を取得するゲッター
	const Vector3& GetPosition() const { return worldTransform.translation_; }

    // 衝突判定/被弾判定用
    //Vector3 GetPosition() const { return worldTransform.translation_; }
    float GetRadius() const { return radius_; }
    bool IsDead() const { return isDead_; }

    // ダメージ処理
    void TakeDamage(int damage);


private:
    // 敵のステータス
    const float kMoveSpeed = 0.1f;
    const float radius_ = 1.0f;
    const int kMaxHp = 10;
    int currentHp_ = kMaxHp;

    // モデルとワールド変換
    Model* model_ = nullptr;
    WorldTransform worldTransform;

    // 回転制御用の変数
	float currentRotationY_ = 0.0f;       // 現在のY軸角度
	const float kRotationLerpRate = 0.1f; // 回転の滑らかさ（0.0～1.0 小さいほどゆっくり）

    bool isDead_ = false; // HPが0になったらtrue
};