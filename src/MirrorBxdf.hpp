#pragma once
#ifndef _MIRRORBXDF_H_
#define _MIRRORBXDF_H_
#include "Bxdf.hpp"
#include "global.hpp"
class MirrorBxdf : public  BxDF {
public:
	virtual inline Vector3f getEmission() { return Vector3f(0); };

	virtual inline bool hasEmission() { return false; }

	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { return 0; }

	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { return Vector3f(0); }

	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) {
		wi = reflect(-wo, N);
		pdf = 1;
		return reflectance / fabs(dotProduct(N, wi));
	}
	virtual inline bool IsDelat() const { return true; }

	Vector3f reflectance;
};
#endif // !_MIRRORBXDF_H_
