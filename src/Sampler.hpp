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
	virtual void NextSample() {};
	virtual std::unique_ptr<Sampler> Clone() = 0;
protected:
	int nSamplesPerPixel;
	int currentDimension;
	int currentIndex;
};

class RandomSampler : public Sampler
{
public:
	RandomSampler(int samplesPerPixel) :Sampler(samplesPerPixel) {}
	void StartPixel() {}

	float Get1D() { return get_random_float(); }
	Vector2f Get2D() { return Vector2f(get_random_float(), get_random_float()); }
	std::unique_ptr<Sampler> Clone() { return std::unique_ptr<Sampler>(new RandomSampler(nSamplesPerPixel)); }
};


class SobolSampler : public Sampler
{
public:
	SobolSampler(int samplesPerPixel) :Sampler(samplesPerPixel) {}
	void StartPixel() {
		currentDimension = 0;
		currentIndex = 0;
	}
	std::unique_ptr<Sampler> Clone() { return std::unique_ptr<Sampler>(new SobolSampler(nSamplesPerPixel)); }
	void NextSample() { currentIndex++; }
	float Get1D() {

		float v = SobolSample(currentDimension, currentIndex);
		currentDimension++;
		return v;
	}
	Vector2f Get2D() {
		float v1 = SobolSample(currentDimension, currentIndex);
		currentDimension++;
		float v2 = SobolSample(currentDimension, currentIndex);
		currentDimension++;
		return (v1,v2);
	}

};

#endif // !_Sampler_H_
