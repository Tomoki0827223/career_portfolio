#pragma once

#include "math/Vector2.h" // float(M_PI) などの定数や型の互換性のため
#include <cmath>          // cosf, sinf, powf, sqrtf のため

// M_PI が未定義の場合の対策 (多くのC++環境では <cmath> や <math.h> で提供されないため)
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace KamataEngine {
namespace MathEsing {

// 1. Sine (サイン曲線)
float easeInSine(float x);
float easeOutSine(float x);
float easeInOutSine(float x);

// 2. Quad (二次曲線)
float easeInQuad(float x);
float easeOutQuad(float x);
float easeInOutQuad(float x);

// 3. Cubic (三次曲線)
float easeInCubic(float x);
float easeOutCubic(float x);
float easeInOutCubic(float x);

// 4. Quart (四次曲線)
float easeInQuart(float x);
float easeOutQuart(float x);
float easeInOutQuart(float x);

// 5. Quint (五次曲線)
float easeInQuint(float x);
float easeOutQuint(float x);
float easeInOutQuint(float x);

// 6. Expo (指数曲線)
float easeInExpo(float x);
float easeOutExpo(float x);
float easeInOutExpo(float x);

// 7. Circ (円弧曲線)
float easeInCirc(float x);
float easeOutCirc(float x);
float easeInOutCirc(float x);

// 8. Back (後退/引き戻し)
float easeInBack(float x);
float easeOutBack(float x);
float easeInOutBack(float x);

// 9. Elastic (弾性/バネ)
float easeInElastic(float x);
float easeOutElastic(float x);
float easeInOutElastic(float x);

// 10. Bounce (バウンド/跳ね返り)
float easeInBounce(float x);
float easeOutBounce(float x);
float easeInOutBounce(float x);

} // namespace Math
} // namespace KamataEngine