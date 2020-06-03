//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"
#include <thread>
#include <time.h>
#include <string>

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.01;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{

	pScene = &scene;
	framebuffer.resize(scene.width * scene.height);
	finishedRowNum = 0;
	int numPerThre = scene.height / threadNum;
	std::vector<std::shared_ptr<std::thread>> threads;
	int startR = 0;
	int endR = 0;
	std::cout << "SPP: " << SPP << "\n";
	sampler = std::unique_ptr<Sampler>(new SobolSampler(SPP));
	for (int i = 0; i < threadNum; i++) {
		startR = i * numPerThre;

		if (i == threadNum - 1)
			endR = scene.height;
		else
			endR = (i + 1) * numPerThre;
		std::shared_ptr<std::thread>  t = std::make_shared<std::thread>(&Renderer::TheadRender, this, startR, endR, sampler->Clone()
			);
		threads.push_back(t);
	}
	for (std::shared_ptr<std::thread> t : threads) {
		t->join();
	}

	UpdateProgress(1.f);
	time_t t = time(0);
	char tmp[128] = { NULL }; //
	strftime(tmp, sizeof(tmp), "outputImages/%Y-%m-%d-%H-%M_spp", localtime(&t));
	std::string st(tmp);
	st += std::to_string(SPP) + "_.ppm";
	FILE* fp = fopen(st.c_str(), "wb");
	(void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);

	ToneMapping();
	 
	for (auto i = 0; i < scene.height * scene.width; ++i) {
		static unsigned char color[3];
		color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
		color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
		color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
		fwrite(color, 1, 3, fp);
	}
	fclose(fp);
}

void Renderer::ToneMapping()
{
	float avgl = 0;
	float inv = 1.0f / framebuffer.size();
	for (auto v : framebuffer) {
		float l = luminance(v);
		avgl += inv *std::exp(std::log10(0.00001 + l));	
	}
	printf("\n\navgl%f\n",avgl);

	for (auto& v : framebuffer) {
		v = ACESToneMapping(v, avgl);
	}
}

void Renderer::TheadRender(int beginRow, int endRow,std::unique_ptr<Sampler> spr) {

	float scale = tan(deg2rad(pScene->fov * 0.5));
	float imageAspectRatio = pScene->width / (float)pScene->height;
	Vector3f eye_pos =  pScene->eyePos;// (278, 273, -800);
	int m = beginRow * pScene->width;
	for (uint32_t j = beginRow; j < endRow; ++j) {
		for (uint32_t i = 0; i < pScene->width; ++i) {
			// generate primary ray direction

			spr->StartPixel();
			for (int k = 0; k < SPP; k++) {
				Vector2f d2 = spr->Get2D();
				float x = (2 * (i + d2.x) / (float)pScene->width - 1) *
					imageAspectRatio * scale;
				float y = (1 - 2 * (j + d2.y) / (float)pScene->height) * scale;
				Vector3f dir = normalize(Vector3f(x, y, 1));
				framebuffer[m] += pScene->castRay(Ray(eye_pos, dir), 0, false) / SPP;
				spr->NextSample();
			}
			m++;
		}

		FinishedOneRowNum();


		//UpdateProgress(j / (float)pScene->height);
	}
	//UpdateProgress(1.f);
}
void Renderer::FinishedOneRowNum() {
	mutex.lock();
	finishedRowNum++;
	UpdateProgress(finishedRowNum / (float)pScene->height);
	mutex.unlock();
}
