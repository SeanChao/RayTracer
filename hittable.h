#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

struct hit_record {
    point3 p;     // hit point
    vec3 normal;  // normal vector
    double t;     // t value of the ray
};

class hittable {
   public:
    // t_min to t_max gives the range of a hit that 'counts'
    virtual bool hit(const ray& r, double t_min, double t_max,
                     hit_record& rec) const = 0;
};

#endif