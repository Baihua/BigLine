#pragma once
#include <iostream>
#include <cmath>
#include <random>
#include<mutex>
#include "Sobolmatrices.hpp"
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
	std::uniform_real_distribution<float> dist(0.f, 1.f);
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

inline Vector3f UniformSampleOnSphere(const Vector3f& n) {
	float x_1 = get_random_float(), x_2 = get_random_float();
	float z = 1.0f - 2.0f * x_1;
	float r = std::sqrt(1.0f - z * z);
	float phi = 2 * M_PI * x_2;
	Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
	return toWorld(localRay, n);
}

inline Vector3f UniformSampleOnHemisphere(const Vector3f& n) {
	float x_1 = get_random_float(), x_2 = get_random_float();
	float z = x_1;
	float r =  std::sqrt(std::max(0.0f, 1.0f - z * z));
	float phi = 2 * M_PI * x_2;
	Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
	return toWorld(localRay, n);
}

inline Vector2f UniformSsampleDisk() {
	float r = std::sqrt(get_random_float());
	float theta = 2 * M_PI * get_random_float();
	return Vector2f(r * std::cos(theta), r * std::sin(theta));
}

inline Vector2f ConcentricSampleDisk() {
	Vector2f u(get_random_float(), get_random_float());
	Vector2f offset = 2*u - Vector2f(1, 1);
	if (offset.x == 0 && offset.y == 0)
		return Vector2f(0, 0);
	float theta, r;
	if (std::abs(offset.x) > std::abs(offset.y)) {
		r = offset.x;
		theta = M_PI/4.0f * (offset.y / offset.x);
	}
	else {
		r = offset.y;
		theta = M_PI/2.0 - M_PI/4.0f * (offset.x / offset.y);
	}
	return r * Vector2f(std::cos(theta), std::sin(theta));
}

inline Vector3f CosineSampleHemisphere(const Vector3f& n) {
	Vector2f d = ConcentricSampleDisk();
	float z = std::sqrt(std::max((float)0.f, 1.0f - d.x * d.x - d.y * d.y));
	return toWorld(Vector3f(d.x, d.y, z), n);
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

inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi) {
	return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}


inline float CosTheta(const Vector3f& w, const Vector3f& n) { return dotProduct(w, n); }

inline float Cos2Theta(const Vector3f& w, const Vector3f& n) { return dotProduct(w, n) * dotProduct(w, n); }

inline float AbsCosTheta(const Vector3f& w, const Vector3f& n) { return std::abs(dotProduct(w, n)); }

inline float Sin2Theta(const Vector3f& w, const Vector3f& n) { return 1.f - Cos2Theta(w, n); }
inline float SinTheta(const Vector3f& w, const Vector3f& n) { return std::sqrt(Sin2Theta(w, n)); }
inline float TanTheta(const Vector3f& w, const Vector3f& n) { return SinTheta(w, n) / CosTheta(w, n); }
inline float Tan2Theta(const Vector3f& w, const Vector3f& n) { return 1 / Cos2Theta(w, n) - 1; }

inline float SobolSample(int a, int dimension) {
	uint32_t v = 0;
	for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
		if (a & 1) v ^= SobolMatrices32[i];
	return v * 2.3283064365386963e-10f;/* 1/2^32 */
}

inline Vector3f ACESToneMapping(Vector3f color)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

inline float luminance(Vector3f v)
{
	return v.x * 0.2126f + v.y * 0.7152f + v.z * 0.0722f;
}