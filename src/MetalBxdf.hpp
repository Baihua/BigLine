#pragma once
#ifndef _Metal_H__
#define _Metal_H__
#include "Vector.hpp"
#include "Bxdf.hpp"
#include "global.hpp"

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
			return (DistributionGGX(N, h, roughness) * fabs(dotProduct(h, N))) / (4 * dotProduct(wo, wi));
		}
	}
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
		float cosalpha = dotProduct(N, wo);
		if (cosalpha > 0.0f) {

			float fre = 0.5;
			this->fresnel(wo, N, ior, fre);
			Vector3f H = normalize(wi + wo);

			float NDF = clamp(0, 1, this->DistributionGGX(N, H, roughness));
			float G = this->GeometrySmith(N, wo, wi, roughness);

			float f = NDF * fre * G / (4 * dotProduct(N, wi) * dotProduct(N, wo));
			return  reflectance * f;
		}
		else
			return Vector3f(0.0f);

	}
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf) {
		wi = SampleOnHemisphere(N);
		pdf = this->pdf(wo,wi,N);
		return this->F(wi, wo, N);
	}
	virtual inline bool IsDelat() const { return false; }


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

	float DistributionGGX(Vector3f N, Vector3f H, float roughness)
	{
		float a = roughness;//* roughness;
		float a2 = a * a;
		float NdotH = std::max(dotProduct(N, H), 0.0f);
		float NdotH2 = NdotH * NdotH;

		float nom = a2;
		float denom = (NdotH2 * (a2 - 1.0) + 1.0);
		denom = M_PI * denom * denom;

		return nom / denom;
	}

	float BeckmannNormalDistribution(float roughness, Vector3f N, Vector3f H)
	{
		float NdotH = std::max(dotProduct(N, H), 0.0f);
		float roughnessSqr = roughness * roughness;
		float NdotHSqr = NdotH * NdotH;
		return std::max(0.0, (1.0 / (M_PI * roughnessSqr * NdotHSqr * NdotHSqr)) * exp((NdotHSqr - 1) / (roughnessSqr * NdotHSqr)));
	}

	float GeometrySchlickGGX(float NdotV, float roughness)
	{
		float r = (roughness + 1.0);
		float k = (r * r) / 8.0;

		float nom = NdotV;
		float denom = NdotV * (1.0 - k) + k;

		return nom / denom;
	}

	float GeometrySmith(Vector3f N, Vector3f V, Vector3f L, float k)
	{
		float NdotV = std::max(dotProduct(N, V), 0.0f);
		float NdotL = std::max(dotProduct(N, L), 0.0f);
		float ggx1 = GeometrySchlickGGX(NdotV, k);
		float ggx2 = GeometrySchlickGGX(NdotL, k);

		return ggx1 * ggx2;
	}

	Vector3f reflectance;
	float roughness = 0.4f;
	float ior = 1;


};
#endif // !_Metal_H__