#pragma once
#ifndef _Sampler_H_
#include "Vector.hpp"
#include "global.hpp"
class Sampler {
public:
	Sampler(int samplesPerPixel) :nSamplesPerPixel(samplesPerPixel) {}
	virtual float Get1D() = 0;
	virtual Vector2f Get2D() = 0;
	virtual void StartPixel() = 0;
	virtual std::unique_ptr<Sampler> Clone() = 0;
protected:
	int nSamplesPerPixel;
};

class RandomSampler : public Sampler
{
public:
	RandomSampler(int samplerPerPixel) :Sampler(samplerPerPixel) {}
	void StartPixel(){}
	float Get1D() { return get_random_float(); }
	Vector2f Get2D() { return Vector2f(get_random_float(), get_random_float()); }
	std::unique_ptr<Sampler> Clone() { return std::unique_ptr<Sampler>(new RandomSampler(nSamplesPerPixel)); }
};

#endif // !_Sampler_H_
