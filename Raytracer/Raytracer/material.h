#pragma once
#ifndef MATERIALH
#define MATERIALH

struct hit_record;
#include "ray.h"
#include "hitable.h"
#include "global_random.h"
#include "texture.h"
#include "onb.h"

float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
	if (discriminant > 0) {
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	else
		return false;
}


vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}





class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& albedo, ray& scattered, float& pdf) const = 0;
	virtual vec3 emitted(float u, float v, const vec3& p) {
		return vec3(0, 0, 0);
	}
	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, ray& scattered) const {
		return 0.0;
	}
};

class lambertian : public material {
public:
	lambertian(texture* a) : albedo(a) {}
	
	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, ray& scattered) const {
		float cosine = dot(rec.normal, unit_vector(scattered.direction()));
		if (cosine < 0) cosine = 0;
		return cosine / M_PI;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& alb, ray& scattered,float& pdf) const {
		onb uvw;
		uvw.build_from_w(rec.normal);
		vec3 direction = uvw.local(random_in_top_hemisphere_cosine());
		scattered = ray(rec.p, unit_vector(direction),r_in.time());
		alb = albedo->value(rec.u, rec.v, rec.p);
		pdf = dot(uvw.w(), scattered.direction()) / M_PI;
		return true;
	}

	texture* albedo;
};
class diffuse_light : public material {
public:
	texture*  light_texture;
	diffuse_light(texture* a) : light_texture(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& alb, ray& scattered, float& pdf) const {
		return false;
	}
	virtual vec3 emitted(float u, float v, const vec3& p) {
		return light_texture->value(u, v, p);
	}

};
/*
class metal : public material {
public:
	metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuzz;
};

class dielectric : public material {
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
		
			cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
			
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_idx);

		}
		else {
			reflect_prob = 1.0;
		}
		if (get_random() < reflect_prob) {
			scattered = ray(rec.p, reflected);
		}
		else {
			scattered = ray(rec.p, refracted);
			
			
		}
			
		return true;
	}

	float ref_idx;
};

class diffuse_light : public material {
public:
	texture*  light_texture;
	diffuse_light(texture* a): light_texture(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		return false;
	}
	virtual vec3 emitted(float u, float v, const vec3& p) {
		return light_texture->value(u, v, p);
	}

};

class isotropic : public material {
public:
	texture*  albedo;
	isotropic(texture* a) : albedo(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		scattered = ray(rec.p, random_in_unit_sphere());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};*/
#endif // !MATERIALH
