#include "EasingFunctions.h"

// EasingFunctions.h で <cmath> をインクルードしているため、ここでは不要です。
// また、M_PIもヘッダーで定義されています。

namespace KamataEngine {
namespace MathEsing {

// 1. Sine (サイン曲線)
float easeInSine(float x) { return 1.0f - cosf((x * float(M_PI)) / 2.0f); }

float easeOutSine(float x) { return sinf((x * float(M_PI)) / 2.0f); }

float easeInOutSine(float x) { return -(cosf(float(M_PI) * x) - 1.0f) / 2.0f; }

// 2. Quad (二次曲線)
float easeInQuad(float x) { return x * x; }

float easeOutQuad(float x) { return 1.0f - (1.0f - x) * (1.0f - x); }

float easeInOutQuad(float x) { return x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f; }

// 3. Cubic (三次曲線)
float easeInCubic(float x) { return x * x * x; }

float easeOutCubic(float x) { return 1.0f - powf(1.0f - x, 3.0f); }

float easeInOutCubic(float x) { return x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f; }

// 4. Quart (四次曲線)
float easeInQuart(float x) { return x * x * x * x; }

float easeOutQuart(float x) { return 1.0f - powf(1.0f - x, 4.0f); }

float easeInOutQuart(float x) { return x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f; }

// 5. Quint (五次曲線)
float easeInQuint(float x) { return x * x * x * x * x; }

float easeOutQuint(float x) { return 1.0f - powf(1.0f - x, 5.0f); }

float easeInOutQuint(float x) { return x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f; }

// 6. Expo (指数曲線)
float easeInExpo(float x) { return x == 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f); }

float easeOutExpo(float x) { return x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x); }

float easeInOutExpo(float x) { return x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ? powf(2.0f, 20.0f * x - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f; }

// 7. Circ (円弧曲線)
float easeInCirc(float x) { return 1.0f - sqrtf(1.0f - powf(x, 2.0f)); }

float easeOutCirc(float x) { return sqrtf(1.0f - powf(x - 1.0f, 2.0f)); }

float easeInOutCirc(float x) { return x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f : (sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f; }

// 8. Back (後退/引き戻し)
float easeInBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * x * x * x - c1 * x * x;
}

float easeOutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * powf(x - 1.0f, 2.0f);
}

float easeInOutBack(float x) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f ? (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f : (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

// 9. Elastic (弾性/バネ)
float easeInElastic(float x) {
	const float c4 = (2.0f * float(M_PI)) / 3.0f;

	return x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);
}

float easeOutElastic(float x) {
	const float c4 = (2.0f * float(M_PI)) / 3.0f;

	return x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
}

float easeInOutElastic(float x) {
	const float c5 = (2.0f * float(M_PI)) / 4.5f;

	return x == 0.0f   ? 0.0f
	       : x == 1.0f ? 1.0f
	       : x < 0.5f  ? -(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f
	                   : (powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
}

// 10. Bounce (バウンド/跳ね返り)
float easeOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	} else if (x < 2.0f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	} else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	} else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}

float easeInBounce(float x) { return 1.0f - easeOutBounce(1.0f - x); }

float easeInOutBounce(float x) { return x < 0.5f ? (1.0f - easeOutBounce(1.0f - 2.0f * x)) / 2.0f : (1.0f + easeOutBounce(2.0f * x - 1.0f)) / 2.0f; }

} // namespace Math
} // namespace KamataEngine