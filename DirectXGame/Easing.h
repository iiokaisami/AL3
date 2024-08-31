#pragma once
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

template<class T> T EasingFunc(T start, T movement, std::function<T(T)> func, T t) { return start + func(t) * movement; }

// Sine
template<class T> T EaseInSine(T x) { return 1 - cosf((x * M_PI) / 2); }

template<class T> T EaseOutSine(T x) { return sinf((x * M_PI) / 2); }

template<class T> T EaseInOutSine(T x) { return -(cosf(M_PI * x) - 1) / 2; }



// Quint
template<class T> T EaseInQuint(T x) { return x * x * x * x * x; }

template<class T> T EaseOutQuint(T x) { return 1 - powf(1 - x, 5); }

template<class T> T EaseInOutQuint(T x) { return x < 0.5 ? 16 * x * x * x * x * x : 1 - powf(-2 * x + 2, 5) / 2; }



// Circ
template<class T> T EaseInCirc(T x) { return 1 - sqrtf(1 - powf(x, 2)); }

template<class T> T EaseOutCirc(T x) { return sqrtf(1 - powf(x - 1, 2)); }

template<class T> T EaseInOutCirc(T x) { return x < 0.5 ? (1 - sqrtf(1 - powf(2 * x, 2))) / 2 : (sqrtf(1 - powf(-2 * x + 2, 2)) + 1) / 2; }



// Elastic
template<class T> T EaseInElastic(T x) {
	const T c4 = (2 * M_PI) / 3;

	return x == 0 ? 0 : x == 1 ? 1 : -powf(2, 10 * x - 10) * sinf((x * 10 - 10.75) * c4);
}

template<class T> T EaseOutElastic(T x) {
	const T c4 = (2 * M_PI) / 3;

	return x == 0 ? 0 : x == 1 ? 1 : powf(2, -10 * x) * sinf((x * 10 - 0.75) * c4) + 1;
}

template<class T> T EaseInOutElastic(T x) {
	const T c5 = (2 * M_PI) / 4.5;

	return x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? -(powf(2, 20 * x - 10) * sinf((20 * x - 11.125) * c5)) / 2 : (powf(2, -20 * x + 10) * sinf((20 * x - 11.125) * c5)) / 2 + 1;
}



// Expo
template<class T> T EaseInExpo(T x) { return x == 0 ? 0 : powf(2, 10 * x - 10); }



// Quad
template<class T> T EaseOutQuad(T x) { return 1 - (1 - x) * (1 - x); }


// Quart
template<class T> T EaseInOutQuart(T x) { return x < 0.5 ? 8 * x * x * x * x : 1 - powf(-2 * x + 2, 4) / 2; }
