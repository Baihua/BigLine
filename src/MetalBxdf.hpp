#pragma once
#ifndef _Metal_H__
#define _Metal_H__
#include "Vector.hpp"

class BxDF {
public:
	virtual inline Vector3f getEmission() { return Vector3f(0); };
	virtual inline bool hasEmission() { return false; }
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) = 0;
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) = 0;
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) {
	
	}
	virtual inline bool IsDelat() const { return false; }
	float roughness = 0.4f;
};
#endif // !_Metal_H__