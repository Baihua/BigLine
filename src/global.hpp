#pragma once
#include <iostream>
#include <cmath>
#include <random>
#include<mutex>

#undef M_PI
#define M_PI 3.141592653589793f

extern const float  EPSILON;
const float kInfinity = std::numeric_limits<float>::max();
extern std::mutex m;
inline float clamp(const float& lo, const float& hi, const float& v)
{
	return std::max(lo, std::min(hi, v));
}

inline  bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

inline float get_random_float()
{

	m.lock();
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]
	m.unlock();

	return dist(rng);
}

inline void UpdateProgress(float progress)
{
	int barWidth = 70;

	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
};

inline Vector3f toWorld(const Vector3f& a, const Vector3f& N) {
	Vector3f B, C;
	if (std::fabs(N.x) > std::fabs(N.y)) {
		float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
		C = Vector3f(N.z * invLen, 0.0f, -N.x * invLen);
	}
	else {
		float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
		C = Vector3f(0.0f, N.z * invLen, -N.y * invLen);
	}
	B = crossProduct(C, N);
	return a.x * B + a.y * C + a.z * N;
}

inline Vector3f SampleOnHemisphere(const Vector3f& n) {
	float x_1 = get_random_float(), x_2 = get_random_float();
	float z = std::fabs(1.0f - 2.0f * x_1);
	float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
	Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
	return toWorld(localRay, n);
}
inline Vector3f reflect(const Vector3f& I, const Vector3f& N)
{
	return I - 2 * dotProduct(I, N) * N;
}