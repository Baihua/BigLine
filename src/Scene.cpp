//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <thread>

void Scene::buildBVH() {
	printf(" - Generating BVH...\n\n");
	this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray& ray) const
{
	return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection& pos, float& pdf) const
{
	float emit_area_sum = 0;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		if (objects[k]->hasEmit()) {
			emit_area_sum += objects[k]->getArea();
		}
	}
	float p = get_random_float() * emit_area_sum;
	emit_area_sum = 0;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		if (objects[k]->hasEmit()) {
			emit_area_sum += objects[k]->getArea();
			if (p <= emit_area_sum) {
				objects[k]->Sample(pos, pdf);
				break;
			}
		}
	}
}

bool Scene::trace(
	const Ray& ray,
	const std::vector<Object*>& objects,
	float& tNear, uint32_t& index, Object** hitObject)
{
	*hitObject = nullptr;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		Vector2f uvK;
		if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
			*hitObject = objects[k];
			tNear = tNearK;
			index = indexK;
		}
	}


	return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray& ray, int depth) const
{

	// TO DO Implement Path Tracing Algorithm here
	Intersection hitObjInter = this->intersect(ray);
	if (!hitObjInter.happened)
		return Vector3f();
	
	if (hitObjInter.obj->hasEmit())
		return hitObjInter.m->getEmission();
	Vector3f wo = -ray.direction;
	Vector3f p = hitObjInter.coords;
	Vector3f n = hitObjInter.normal;

	float pdf = 1;
	Intersection lightInter;
	this->sampleLight(lightInter, pdf);
	Vector3f x = lightInter.coords;
	Vector3f ws = normalize(x - p);
	Vector3f emit = lightInter.emit;
	Vector3f nn = lightInter.normal;

	float distance = (p - x).norm();
	Vector3f L_dir;
	{
		Vector3f o = p + n* 0.001;
		Ray hitTest(p, ws);
		Intersection hit = intersect(hitTest);
		if (!hit.happened || fabs(hit.distance) < EPSILON || hit.distance > distance - EPSILON) {
			//printf("\n%f,distance: %f\n", hit.distance, distance);
			float d1 = std::max(0.0f, dotProduct(n, ws));
			float d2 = std::max(0.0f, dotProduct(nn, -ws));
			L_dir = emit * hitObjInter.m->F(ws, wo, n) * d1 * d2 / (distance * distance) / pdf;
		}
	}

	Vector3f L_indir;

	if (get_random_float() < RussianRoulette)//test rrp
	{
		Vector3f wi;
		float pdf = 1;
		Vector3f value = hitObjInter.m->Sample_f(wo, wi, n, pdf);
		if (!value.isAllZero())
		{
			Vector3f o = dotProduct(wi, n) > 0 ? p + n * 0.001f: p - n * 0.001f;
			Ray r(o, wi);
			Intersection intersect = this->intersect(r);
			if (intersect.happened /*&& !intersect.obj->hasEmit()*/)
			{
				//L_indir = castRay(r, 0) * value * fabs(dotProduct(wi, n)) / pdf / RussianRoulette;

				//if (hitObjInter.m->hasPerfectSpecula())
				//{
				////	L_indir = intersect.m->getEmission() * value * fabs(dotProduct(wi, n)) / pdf / RussianRoulette;
				//}
				if (!intersect.obj->hasEmit() || hitObjInter.m->hasPerfectSpecula()) {
				
					L_indir = castRay(r, 0) * value * fabs(dotProduct(wi, n)) / pdf / RussianRoulette;
				}
			}
		}
	}
	return L_dir + L_indir;
}

//Vector3f Scene::shade(Vector3f pos, Vector3f wo) {
//
//}