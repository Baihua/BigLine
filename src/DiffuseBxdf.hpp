#pragma once
#ifndef _DIFFUSEBXDF_H__
#define _DIFFUSEBXDF_H__
#include "Bxdf.hpp"
#include "global.hpp"
class DiffuseBxdf :public BxDF {
public:
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) ;
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf);
	Vector3f Kd;
};

float DiffuseBxdf::pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	if (dotProduct(wo, N) > 0.0f)
		return 0.5f / M_PI;
	else
		return 0.0f;
}
Vector3f DiffuseBxdf::F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	float cosalpha = dotProduct(N, wo);
	if (cosalpha > 0.0f) {
		return Kd / M_PI;
	}
	else
		return Vector3f(0.0f);
}

Vector3f DiffuseBxdf::Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf)
{
	float cosalpha = dotProduct(N, wo);
	if (cosalpha < 0.0f) {
		pdf = 0;
		wi = Vector3f(0);
		return Vector3f(0);
	}
	wi = SampleOnHemisphere(N);
	pdf = 0.5f / M_PI;
	return Kd / M_PI;
}
#endif // !_DIFFUSEBXDF_H__
