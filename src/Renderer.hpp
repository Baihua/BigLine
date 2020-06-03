//
// Created by goksu on 2/25/20.
//
#include "Scene.hpp"
#include "Sampler.hpp"
#include<mutex>
#pragma once
struct hit_payload
{
	float tNear;
	uint32_t index;
	Vector2f uv;
	Object* hit_obj;
};

class Renderer
{
public:
	void Render(const Scene& scene);
	void TheadRender(int beginRow, int endRow,std::unique_ptr<Sampler> thrSamper);
	void FinishedOneRowNum();
	void SetSomeSetting(int spp, int threadNum) {
		SPP = spp; this->threadNum = threadNum;
	}

	void ToneMapping();
private:
	std::mutex mutex;
	std::vector<Vector3f> framebuffer;
	int finishedRowNum;
	const Scene* pScene;
	std::unique_ptr<Sampler> sampler;
	int SPP = 16;
	int threadNum = 1;
};
