#pragma once
#ifndef  _MICROFACETDISTRIBUTION_H__
#define _MICROFACETDISTRIBUTION_H__

#include "Vector.hpp"
#include "global.hpp"
class MicrofacetDistribution {
public:
	// 法线分布函数
	virtual float D(const Vector3f& wh, const Vector3f& n) const = 0;

	virtual const Vector3f Sample_wh(const Vector3f& n) const = 0;

	virtual float G1(const Vector3f& w, const Vector3f& n) const {
		return 1.f / (1.f + Lambda(w, n));
	}

	// 几何函数
	virtual float G(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const Vector3f& wh) const {
		if (dotProduct(wo, n) < 0 || dotProduct(wi, n) < 0)
			return 0.f;

		return 1.f / (1.f + Lambda(wo, n) + Lambda(wi, n));
	}

	// 几何函数 Smith
	virtual float G_Smith(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const Vector3f& wh) const {
		if (dotProduct(wo, n) < 0 || dotProduct(wi, n) < 0)
			return 0.f;

		return G1(wo, n) * G1(wi, n);
	}

	float PDF(const Vector3f& wh, const Vector3f& N) const {
		return D(wh, N) * abs(dotProduct(wh, N));
	}

protected:
	virtual float Lambda(const Vector3f& w, const Vector3f& n) const = 0;
};

class Beckmann : public MicrofacetDistribution {
public:
	Beckmann() { SetAlpha(0.5f); }

public:
	// roughness : 0 - 1
	void SetAlpha(float roughness) {
		roughness = std::max(roughness, (float)1e-3);
		alpha = roughness * roughness;
	}

public:
	// 法线分布函数
	virtual float D(const Vector3f& wh, const Vector3f& n) const
	{
		if (dotProduct(wh, n) < 0)
			return 0.f;

		const auto tan2Theta = Tan2Theta(wh, n);
		if (std::isnan(tan2Theta))
			return 0;

		const auto cos4Theta = Cos2Theta(wh, n) * Cos2Theta(wh, n);
		const auto alpha2 = alpha * alpha;
		return exp(-tan2Theta / alpha2) / (M_PI * alpha2 * cos4Theta);
	}

	virtual float Lambda(const Vector3f& w, const Vector3f& n) const {
		auto absTanTheta = std::abs(TanTheta(w, n));
		if (std::isnan(absTanTheta))
			return 98e8f;

		auto a = 1 / (alpha * absTanTheta);
		if (a >= 1.6f)
			return 0;

		return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
	}

	virtual const Vector3f Sample_wh(const Vector3f& n) const {
		// sample
		const float Xi1 = get_random_float();
		const float Xi2 = get_random_float();

		// theta
		const auto alpha2 = alpha * alpha;
		const auto tan2Theta = -alpha2 * log(Xi1);

		const auto cosTheta = 1.f / sqrt(1 + tan2Theta);
		const auto sinTheta = std::max(0.f, sqrt(1.f - cosTheta * cosTheta));

		// phi
		const auto phi = 2 * M_PI * Xi2;

		Vector3f dir = SphericalDirection(sinTheta, cosTheta, phi);
		return toWorld(dir, n);
	}


private:
	float alpha;
};
#endif // ! _MICROFACETDISTRIBUTION_H__
