#pragma once

#include "Material.hpp"
#include "Vector.hpp"

class MateialMirror :public Material {
public:
	//MateialMirror()
	// sample a ray by Material properties
	virtual inline Vector3f sample(const Vector3f& wi, const Vector3f& N);
	// given a ray, calculate the PdF of this ray
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	// given a ray, calculate the contribution of this ray
	virtual inline Vector3f eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf);
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { return Vector3f(0); }
	Vector3f reflectance;
};

Vector3f MateialMirror::sample(const Vector3f& wi, const Vector3f& N) {
	return reflect(-wi, N);
}

float MateialMirror::pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	return 0;
}

Vector3f MateialMirror::eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {

		return Vector3f(0.0f);
}
Vector3f MateialMirror::Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf)
{
	wi = reflect(-wo, N);
	pdf = 1;
	return reflectance / fabs(dotProduct(N, wi));
}