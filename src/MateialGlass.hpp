#pragma once

#include "Material.hpp"
#include "Vector.hpp"
#include "global.hpp"
extern const float  EPSILON;
class MateialGlass :public Material {
public:
	//MateialMirror()
	// sample a ray by Material properties
	virtual inline Vector3f sample(const Vector3f& wi, const Vector3f& N);
	// given a ray, calculate the PdF of this ray
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	// given a ray, calculate the contribution of this ray
	virtual inline Vector3f eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf);
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) { 
		return Vector3f(0);
	}

	Vector3f reflectance;
};

Vector3f MateialGlass::sample(const Vector3f& wi, const Vector3f& N) {
	return reflect(-wi, N);
}

float MateialGlass::pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	return 1;
}

Vector3f MateialGlass::eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	return Vector3f(0.0f);
}

Vector3f MateialGlass::Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf)
{
	float kr = 1;
	this->fresnel(wo, N, ior, kr);
	if (kr > 0.999) {
		wi = reflect(-wo, N);
		pdf = 1;
		return reflectance / fabs(dotProduct(N, wi));
	}
	else
	{
		//float f = get_random_float();
		//if (f < kr) {
		//	//选用反射；
		//	wi = reflect(-wo, N);
		//	pdf = kr;
		//	return kr * reflectance / fabs(dotProduct(N, wi));

		//}
		//else
		{	//选用折射
			pdf = 1 - kr;
			wi = refract(-wo, N, ior);
			bool enter = dotProduct(wo, N) > 0;
			float v = enter ? ior *ior:1 / (ior * ior)  ;
			return v * (1 - kr) * reflectance / fabs(dotProduct(N, wi));
		}

	}
}