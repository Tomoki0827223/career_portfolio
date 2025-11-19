#pragma once

namespace KamataEngine {

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;
};

// --- Vector2 の演算子オーバーロード ---

// 加算 (Vector2 + Vector2)
inline Vector2 operator+(const Vector2& a, const Vector2& b) { return {a.x + b.x, a.y + b.y}; }

// 減算 (Vector2 - Vector2)
inline Vector2 operator-(const Vector2& a, const Vector2& b) { return {a.x - b.x, a.y - b.y}; }

// スカラー乗算 (Vector2 * float)
inline Vector2 operator*(const Vector2& v, float s) { return {v.x * s, v.y * s}; }

// スカラー乗算 (float * Vector2)
inline Vector2 operator*(float s, const Vector2& v) { return v * s; }

// スカラー除算 (Vector2 / float)
inline Vector2 operator/(const Vector2& v, float s) {
	// ゼロ除算のチェックはここでは省略しますが、実運用では必要です
	return {v.x / s, v.y / s};
}

} // namespace KamataEngine