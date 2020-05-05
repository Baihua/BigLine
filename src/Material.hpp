//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum MaterialType { DIFFUSE, MICROFACET };

class Material {
public:
	// Compute reflection direction
	Vector3f reflect(const Vector3f& I, const Vector3f& N) const
	{
		
		return I - 2 * dotProduct(I, N) * N;
	}

	// Compute refraction direction using Snell's law
	//
	// We need to handle with care the two possible situations:
	//
	//    - When the ray is inside the object
	//
	//    - When the ray is outside.
	//
	// If the ray is outside, you need to make cosi positive cosi = -N.I
	//
	// If the ray is inside, you need to invert the refractive indices and negate the normal N
	Vector3f refract(const Vector3f& I, const Vector3f& N, const float& ior) const
	{
		float cosi = clamp(-1, 1, dotProduct(I, N));
		float etai = 1, etat = ior;
		Vector3f n = N;
		if (cosi < 0) { cosi = -cosi; }
		else { std::swap(etai, etat); n = -N; }
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);

		if (k < 0)
			printf("dddd\n");
		/*else
			printf("xxx\n");*/

		return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
	}

	bool refract(const Vector3f& I, const Vector3f& N, Vector3f& R, const float& ior)
	{
		float cosi = clamp(-1, 1, dotProduct(I, N));
		float etai = 1, etat = ior;
		Vector3f n = N;
		if (cosi < 0) { cosi = -cosi; }
		else { std::swap(etai, etat); n = -N; }
		
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);
		if (k < 0)
		{
			R = Vector3f(0);
			return false;
		}
		R = eta * I + (eta * cosi - sqrtf(k)) * n;
		return true;
	}

	// Compute Fresnel equation
	//
	// \param I is the incident view direction
	//
	// \param N is the normal at the intersection point
	//
	// \param ior is the material refractive index
	//
	// \param[out] kr is the amount of light reflected
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

	Vector3f toWorld(const Vector3f& a, const Vector3f& N) {
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

	//Trowbridge-Reitz GGX
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

public:
	MaterialType m_type;
	//Vector3f m_color;
	Vector3f m_emission;
	float ior;
	Vector3f Kd, Ks;
	float specularExponent;
	//Texture tex;

	inline Material(MaterialType t = DIFFUSE, Vector3f e = Vector3f(0, 0, 0));
	inline MaterialType getType();
	//inline Vector3f getColor();
	inline Vector3f getColorAt(double u, double v);
	inline Vector3f getEmission();
	inline bool hasEmission();
	// sample a ray by Material properties
	virtual inline Vector3f sample(const Vector3f& wi, const Vector3f& N);
	// given a ray, calculate the PdF of this ray
	virtual inline float pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	// given a ray, calculate the contribution of this ray
	virtual inline Vector3f eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	virtual inline Vector3f F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	virtual inline Vector3f Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf);
	virtual inline bool hasPerfectSpecula() { return false; }
	
	float Roughness = 0.3f;
};

Material::Material(MaterialType t, Vector3f e) {
	m_type = t;
	//m_color = c;
	m_emission = e;
}

MaterialType Material::getType() { return m_type; }
///Vector3f Material::getColor(){return m_color;}
Vector3f Material::getEmission() { return m_emission; }
bool Material::hasEmission() {
	if (m_emission.norm() > EPSILON) return true;
	else return false;
}

Vector3f Material::getColorAt(double u, double v) {
	return Vector3f();
}


Vector3f Material::sample(const Vector3f& wi, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	case MICROFACET:
	{
		// uniform sample on the hemisphere
		float x_1 = get_random_float(), x_2 = get_random_float();
		float z = std::fabs(1.0f - 2.0f * x_1);
		float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
		Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
		return toWorld(localRay, N);

		break;
	}
	}
}

float Material::pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	case MICROFACET:
	{
		// uniform sample probability 1 / (2 * PI)
		if (dotProduct(wo, N) > 0.0f)
			return 0.5f / M_PI;
		else
			return 0.0f;
		break;
	}
	}
}

Vector3f Material::eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	{
		// calculate the contribution of diffuse   model
		float cosalpha = dotProduct(N, wo);
		if (cosalpha > 0.0f) {
			return Kd / M_PI;
		}
		else
			return Vector3f(0.0f);
		break;
	}
	case MICROFACET:
	{
		float cosalpha = dotProduct(N, wo);
		if (cosalpha > 0.0f) {

			float fre = 0.5;
			this->fresnel(wo, N, ior, fre);
			Vector3f H = normalize(wi + wo);

			float NDF = clamp(0, 1, this->DistributionGGX(N, H, Roughness));
			float G = this->GeometrySmith(N, wo, wi, Roughness);

			float f = NDF * fre * G / (4 * dotProduct(N, wi) * dotProduct(N, wo));
			return Vector3f(1.0, 1.0, 1.0) * f;
		}
		else
			return Vector3f(0.0f);
		break;
	}
	}
}

Vector3f Material::F(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)
{
	float cosalpha = dotProduct(N, wo);
	if (cosalpha > 0.0f) {
		return Kd / M_PI;
	}
	else
		return Vector3f(0.0f);
}

Vector3f Material::Sample_f(const Vector3f& wo, Vector3f& wi, const Vector3f& N, float& pdf)
{
	float cosalpha = dotProduct(N, wo);
	if (cosalpha < 0.0f) {
		pdf = 0;
		wi = Vector3f(0);
		return Vector3f(0);
	}

	float x_1 = get_random_float(), x_2 = get_random_float();
	float z = std::fabs(1.0f - 2.0f * x_1);
	float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
	Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
	wi = toWorld(localRay, N);
	
	
	pdf = 0.5f / M_PI;

	return Kd/M_PI;
}
#endif //RAYTRACING_MATERIAL_H

