#pragma once
#ifndef _BxDF_H__
#define _BxDF_H__
#include "Vector.hpp"

class BxDF {
public:
	inline Vector3f getEmission() { return Vector3f(0); };
	inline bool hasEmission() { return false; }
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) = 0;
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) = 0;
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) = 0;
	virtual inline bool IsDelat() const { return false; }
};
#endif // !_BxDF_H__