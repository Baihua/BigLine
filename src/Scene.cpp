//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <thread>
#include "Bxdf.hpp"

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


Vector3f Scene::castRay(const Ray& ray, int depth, bool isPerfectSpecular) const
{
	Intersection hitObjInter = this->intersect(ray);
	if (!hitObjInter.happened)
		return Vector3f();


	BxDF* bxdf = hitObjInter.m->bxdf;
	if (bxdf == NULL) return Vector3f();

	Vector3f L;
	if (hitObjInter.obj->IsLight())
	{
		if (depth == 0 || isPerfectSpecular)
			return hitObjInter.obj->light->GetLe();
		else
			return Vector3f(0);
	}

	int numLight = lights.size();
	Vector3f wo = -ray.direction;
	Vector3f p = hitObjInter.coords;
	Vector3f n = hitObjInter.normal;
	float dis, pdf;

	//进行光源采样
	int randOneLight = std::rand() % numLight;
	Light* selectLight = lights[randOneLight];
	Vector3f wi;
	Vector3f l = selectLight->Sample_Li(p, wi, dis, pdf);
	if (l.hasNegative())
		printf("\n-----sample_Li-----negative\n");
	pdf *= 1.0f / numLight;
	if (l.isAllZero() || pdf <= 0) { L = Vector3f(0); }
	else {
		//可见性判断
		Vector3f o = p + n * 0.001;
		Ray hitTest(o, wi);
		Intersection hit = intersect(hitTest);
		if (abs(hit.distance - dis) < EPSILON) {
			Vector3f f = bxdf->F(wi, wo, n);
			if (f.isAllZero())
				L = Vector3f(0);
			else
			{
				float scatteringpdf = bxdf->pdf(wi, wo, n);
				float weight = PowerHeuistic(1, pdf, 1, scatteringpdf);
				L = l * f * std::clamp(dotProduct(wi, n), 0.f, 1.f) * weight / pdf;
				printf("\nL:(%f,%f,%f), w:%f, pdf:%f, lpdf:%f\n", l.x, l.y, l.z, weight, pdf, scatteringpdf);
			}
		}
		else
		{
			L = Vector3f(0);
		}
	}

	//bxdf 采样
	Vector3f S;
	if (!bxdf->IsDelat())
	{
		float weight = 1;
		Vector3f wi;
		Vector3f f = bxdf->Sample_f(wo, wi, n, pdf);
		if (f.isAllZero() || pdf <= 0) {
			S = Vector3f(0);
		}
		else {
			Vector3f o = dotProduct(wi, n) > 0 ? p + n * 0.001f : p - n * 0.001f;
			Ray r(o, wi);
			Intersection intersect = this->intersect(r);
			if (intersect.happened && intersect.obj->light == selectLight)
			{
				S = selectLight->GetLi(intersect, wi);
			}
			else {
				S = Vector3f(0);
			}

			if (!S.isAllZero() && !bxdf->IsDelat()) {
				float scale = 1.0f / numLight;
				float lightPdf = 0;
				for (Light* l : lights) {
					if (!l->IsDelta())
						lightPdf += l->Pdf(o, wi) * scale;
				}
				weight = PowerHeuistic(1, pdf, 1, lightPdf);
				S = S * f * weight * std::clamp(dotProduct(wi, n), 0.f, 1.f) / pdf;
				printf("\nS:(%f,%f,%f), w:%f, pdf:%f, lpdf:%f\n", S.x, S.y, S.z, weight, pdf, lightPdf);
				L += S;
			}
		}
	}

	Vector3f P;
	if (bxdf->IsDelat() || depth < 3 || get_random_float() < RussianRoulette)//test rrp 生成新的path
	{
		float weight = 1, rr = RussianRoulette;
		Vector3f wi;
		Vector3f f = bxdf->Sample_f(wo, wi, n, pdf);
		if (f.isAllZero() || pdf <= 0) {
			P = Vector3f(0);
		}
		else {
			Vector3f o = dotProduct(wi, n) > 0 ? p + n * 0.001f : p - n * 0.001f;
			Ray r(o, wi);
			if (!bxdf->IsDelat() || depth >= 3) {
				rr = RussianRoulette;
			}
			else {
				rr = 1;
			}
			Vector3f  matF = castRay(r, depth + 1, bxdf->IsDelat()) * f * std::fabs(dotProduct(wi, n)) / pdf / rr;
			P = matF;
		}
	}
	return L + P;
}

// 直接光照与间接光照分开
Vector3f Scene::castRayDir_InDir(const Ray& ray, int depth, bool isPerfectSpecular) const
{

	float lightPdf = 0;
	Intersection hitObjInter = this->intersect(ray);
	if (!hitObjInter.happened)
		return Vector3f();

	BxDF* bxdf = hitObjInter.m->bxdf;
	if (bxdf == NULL) return Vector3f();


	if (bxdf->hasEmission() && (depth == 0 || isPerfectSpecular))
	{
		return bxdf->getEmission();
	}

	Vector3f wo = -ray.direction;
	Vector3f p = hitObjInter.coords;
	Vector3f n = hitObjInter.normal;

	Intersection lightInter;
	this->sampleLight(lightInter, lightPdf);
	Vector3f x = lightInter.coords;
	Vector3f ws = normalize(x - p);
	Vector3f emit = lightInter.emit;
	Vector3f nn = lightInter.normal;

	float distance = (p - x).norm();
	Vector3f L_dir;
	Vector3f o = p + n * 0.001;
	Ray hitTest(p, ws);
	Intersection hit = intersect(hitTest);
	if (!hit.happened || fabs(hit.distance) < EPSILON || hit.distance > distance - EPSILON) {
		float d1 = std::max(0.0f, dotProduct(n, ws));
		float d2 = std::max(0.0f, dotProduct(nn, -ws));
		L_dir = emit * bxdf->F(ws, wo, n) * d1 * d2 / (distance * distance) / lightPdf;
	}

	Vector3f L_indir;
	float bxdfPdf = 0;
	bool rrTest = false;
	if (get_random_float() < RussianRoulette)//test rrp
	{
		Vector3f wi;
		Vector3f value = bxdf->Sample_f(wo, wi, n, bxdfPdf);
		if (!value.isAllZero())
		{
			rrTest = true;
			Vector3f o = dotProduct(wi, n) > 0 ? p + n * 0.001f : p - n * 0.001f;
			Ray r(o, wi);
			Intersection intersect = this->intersect(r);
			if (intersect.happened)
			{
				if (!intersect.obj->hasEmit() || bxdf->IsDelat())
				{
					L_indir = castRayDir_InDir(r, depth + 1, bxdf->IsDelat()) * value * fabs(dotProduct(wi, n)) / bxdfPdf / RussianRoulette;
				}
			}
		}
	}

	return L_dir + L_indir;
}

// Implementation of Path Tracing
Vector3f Scene::castRay2(const Ray& ray, int depth, bool isPerfectSpecular) const
{
	Intersection hitObjInter = this->intersect(ray);
	if (!hitObjInter.happened)
		return Vector3f();

	BxDF* bxdf = hitObjInter.m->bxdf;
	if (bxdf == NULL) return Vector3f();
	if (bxdf->hasEmission())
	{
		return bxdf->getEmission();
	}
	Vector3f wo = -ray.direction;
	Vector3f p = hitObjInter.coords;
	Vector3f n = hitObjInter.normal;
	Vector3f L;
	float bxdfPdf = 0;

	if (get_random_float() < RussianRoulette)
	{
		Vector3f wi;
		Vector3f f = bxdf->Sample_f(wo, wi, n, bxdfPdf);
		if (bxdfPdf <= 0) { L = Vector3f(0); }
		else
		{
			Vector3f o = dotProduct(wi, n) > 0 ? p + n * 0.001f : p - n * 0.001f;
			Ray r(o, wi);
			Intersection intersect = this->intersect(r);
			L = castRay2(r, depth + 1, bxdf->IsDelat()) * f * fabs(dotProduct(wi, n)) / bxdfPdf / RussianRoulette;
		}
	}
	return  L;
}

//Vector3f Scene::shade(Vector3f pos, Vector3f wo) {
//
//}