#pragma once
#include <cmath> // Math::LengthやMath::Normalizeで使用される可能性があるため

namespace KamataEngine {

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	// --- 1. 複合代入演算子 (メンバ関数) ---
	// これらが定義されていれば、ほとんどの二項演算子を非メンバ関数として定義できます。

	// 加算代入: v1 += v2
	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	// 減算代入: v1 -= v2
	Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	// スカラー乗算代入: v1 *= scalar
	Vector3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// スカラー除算代入: v1 /= scalar
	Vector3& operator/=(float scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
};

// --- 2. 二項演算子 (非メンバ関数) ---
// 複合代入演算子を用いて定義するのが一般的です。

// 加算: v1 + v2
inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result += rhs;
	return result;
}

// 減算: v1 - v2
inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result -= rhs;
	return result;
}

// スカラー乗算 (ベクトル * スカラー): v * s
inline Vector3 operator*(const Vector3& v, float scalar) {
	Vector3 result = v;
	result *= scalar;
	return result;
}

// スカラー乗算 (スカラー * ベクトル): s * v
inline Vector3 operator*(float scalar, const Vector3& v) {
	// v * scalar を呼び出す
	return v * scalar;
}

// スカラー除算: v / s
inline Vector3 operator/(const Vector3& v, float scalar) {
	Vector3 result = v;
	result /= scalar;
	return result;
}

// --- 3. 単項演算子 (メンバ関数または非メンバ関数) ---

// 単項マイナス (符号反転): -v
inline Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }

// --- 4. Math関数 (ベクトル操作) の定義（仮の例） ---
// Math::Length と Math::Normalize が使われていたため、最低限の定義を追加します。
// 実際のプロジェクトでは、これらは別のMathユーティリティヘッダに定義されているはずです。

struct Math final {
	// ベクトルの長さ（Magnitude）を計算
	static float Length(const Vector3& v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

	// ベクトルを正規化（長さを1にする）
	static Vector3 Normalize(const Vector3& v) {
		float len = Length(v);
		if (len != 0.0f) {
			return v / len;
		}
		// 長さが0の場合はゼロベクトルを返す
		return {0.0f, 0.0f, 0.0f};
	}
};

} // namespace KamataEngine