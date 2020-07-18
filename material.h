#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "hittable.h"

class material {
   public:
    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
   public:
    lambertian(const color& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color& attenuation, ray& scattered) const {
        vec3 scatter_direction = rec.normal + vec3::random_unit_vector();
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

   private:
    color albedo;  // albedo: ratio of light reflection
};

class metal : public material {
   public:
    metal(const color& a, double f = 0) : albedo(a), fuzz(f) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * vec3::random_in_unit_sphere());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

   private:
    color albedo;
    double fuzz;
};

#endif