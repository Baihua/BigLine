//
// Created by Göksu Güvendiren on 2019-05-14.
//

#pragma once

#include "Vector.hpp"

class Light
{
public:
	virtual Vector3f Sample_Li(const Vector3f& p, Vector3f& wi, float& dist, float& pdf)const = 0;
	virtual float Pdf(const Vector3f& p, Vector3f& wi, Vector3f& n) const = 0;
	virtual bool IsDelta() const { return false; };
};

class AreaLight :public Light
{
public:
	virtual Vector3f Sample_Li(const Vector3f& p, Vector3f& wi, float& dist, float& pdf)const {
	

	}

	virtual float Pdf(const Vector3f& p, Vector3f& wi, Vector3f& n) const {

		
	}
public:
	std::shared_ptr<Object*> obj;
};
