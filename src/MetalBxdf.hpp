#pragma once
#ifndef _Metal_H__
#define _Metal_H__
#include "Vector.hpp"
#include "Bxdf.hpp"
#include "global.hpp"
#include "MicrofacetDistribution.hpp"

class Microfacet :public BxDF {
public:
	virtual inline Vector3f getEmission() { return Vector3f(0); };
	virtual inline bool hasEmission() { return false; }
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
		if (dotProduct(wo, N) < 0.0f)
			return 0;
		else
		{
			Vector3f h = normalize(wi + wo);
			return md->PDF(h,N) / (4 * dotProduct(wo, h));
		}
	}
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
		float cosalpha = dotProduct(N, wo);
		if (cosalpha > 0.0f) {

			float fre = 0.5;
			this->fresnel(wo, N, ior, fre);
			Vector3f H = normalize(wi + wo);
			float D  = md->D(H, N);
			float G = md->G(wo, wi, N, H);
			float f = D * fre * G / (4 * dotProduct(N, wi) * dotProduct(N, wo));
			return  reflectance* f;
		}
		else
			return Vector3f(0.0f);

	}
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) {
		Vector3f h = md->Sample_wh(N);
		wi = reflect(-wo, h);
		pdf = this->pdf(wo,wi,N);
		return this->F(wi, wo, N);
	}
	virtual inline bool IsDelat() const { return false; }

	void SetRoughness(float r) {
		this->roughness = r;
		md->SetAlpha(roughness);
	}
public:
	void fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const
	{
		float cosi = clamp(-1, 1, dotProduct(I, N));
		float etai = 1, etat = ior;
		if (cosi > 0) { std::swap(etai, etat); }
		// Compute sini using Snell's law
		float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
		// Total internal reflection
		if (sint >= 1) {
			kr = 1;
		}
		else {
			float cost = sqrtf(std::max(0.f, 1 - sint * sint));
			cosi = fabsf(cosi);
			float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
			float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
			kr = (Rs * Rs + Rp * Rp) / 2;
		}
		// As a consequence of the conservation of energy, transmittance is given by:
		// kt = 1 - kr;
	}
	

	Beckmann* md = new Beckmann();
	Vector3f reflectance;
	float ior = 1;

private:
	float roughness = 0.4f;

};
#endif // !_Metal_H__