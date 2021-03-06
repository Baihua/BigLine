//
// Created by LEI XU on 5/13/19.
//
#pragma once
#ifndef RAYTRACING_OBJECT_H
#define RAYTRACING_OBJECT_H

#include "Vector.hpp"
#include "global.hpp"
#include "Bounds3.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
//#include"Light.hpp"
//class Material;
class Light;

class Object
{
public:
	Object() {}
	virtual ~Object() {}
	virtual bool intersect(const Ray& ray) = 0;
	virtual bool intersect(const Ray& ray, float&, uint32_t&) const = 0;
	virtual Intersection getIntersection(Ray _ray) = 0;
	virtual void getSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t&, const Vector2f&, Vector3f&, Vector2f&) const = 0;
	virtual Vector3f evalDiffuseColor(const Vector2f&) const = 0;
	virtual Bounds3 getBounds() = 0;
	virtual float getArea() = 0;
	virtual void Sample(Intersection& pos, float& pdf) = 0;
	virtual bool hasEmit() = 0;
	virtual void SetMaterial(Material* mat) = 0;
	virtual void SetLight(Light* light) { this->light = light; };
	virtual bool IsLight() { return  light != NULL; }
	virtual std::vector<Light*> GetLight() {
		if (IsLight()) return { light };
		else return {};
	}
	std::string name;
	Light* light = NULL;
};



#endif //RAYTRACING_OBJECT_H
