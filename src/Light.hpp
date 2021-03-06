//
// Created by Göksu Güvendiren on 2019-05-14.
//

#pragma once

#include "Vector.hpp"
#include "Object.hpp"
#include "Intersection.hpp"

class Light
{
public:
	virtual inline Vector3f Sample_Li(const Vector3f& p, Vector3f& wi, float& dist, float& pdf)const = 0;
	virtual inline float Pdf(const Vector3f& p, const Vector3f& wi) const = 0;
	virtual bool IsDelta() const { return false; }
	virtual inline Vector3f GetLe()const { return Vector3f(0); }
	virtual inline Vector3f GetLi(const Intersection& intersect, const Vector3f& wi)const { return Vector3f(0); }
};

class DiffuseLight :public Light
{


public:
	virtual inline Vector3f Sample_Li(const Vector3f& p, Vector3f& wi, float& dist, float& pdf)const;

	virtual inline float Pdf(const Vector3f& p, const Vector3f& wi) const;
	virtual inline Vector3f GetLe()const { return color; }
	virtual inline Vector3f GetLi(const Intersection& intersect, const Vector3f& wi)const { 
		if (dotProduct(intersect.normal, -wi) > 0)
			return color;
		return Vector3f(0); 
	}

public:

	Vector3f color;
	Object* obj;
};

Vector3f DiffuseLight::Sample_Li(const Vector3f& p, Vector3f& wi, float& dist, float& pdf) const {
	Intersection intersection;
	float pd;
	obj->Sample(intersection, pd);
	if (pd <= 0)
	{
		pdf = 0;
		return Vector3f(0);
	}


	Vector3f dir = (intersection.coords - p);
	dist = dir.norm();
	float dist2 = dist * dist;
	wi = normalize(dir);
	float cosTheta = dotProduct(intersection.normal, -wi);
	if (cosTheta <= 0)
	{
		pdf = 0;
		return Vector3f(0);
	}

	pdf = dist2 / (obj->getArea() * cosTheta);
	return color;
}

float DiffuseLight::Pdf(const Vector3f& p, const Vector3f& wi) const {
	Ray r(p, wi);
	Intersection inter = obj->getIntersection(r);
	if (!inter.happened)
	{
		return 0;
	}
	const auto w = normalize(p - inter.coords);
	float dist2 = inter.distance*inter.distance;
	float cosTheta = dotProduct(w, inter.normal);
	if (cosTheta <= 0)
		return 0;
	return dist2 / (obj->getArea() * cosTheta);
}


