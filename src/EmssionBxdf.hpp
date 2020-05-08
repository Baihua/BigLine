#pragma once
#ifndef _EmissionBxdf
#define _EmissionBxdf
#include "Bxdf.hpp"
#include "Vector.hpp"
//暂时当做光源的来用
class EmissionBxdf :public BxDF
{
public:
	virtual inline Vector3f getEmission() { return emission; };
	virtual inline bool hasEmission() { return true; }
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { return 0; };
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { return Vector3f(0); };
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) { return Vector3f(0); };

	Vector3f emission;
};
#endif // !_EmissionBxdf
