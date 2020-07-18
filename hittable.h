#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class material;

struct hit_record {
    point3 p;                      // hit point
    vec3 normal;                   // normal vector
    shared_ptr<material> mat_ptr;  // pointer to material class
    double t;                      // t value of the ray
    bool front_face;               // whether hit in the front face

    // set boolean variable front_face, set normal to point outward
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
   public:
    // t_min to t_max gives the range of a hit that 'counts'
    virtual bool hit(const ray& r, double t_min, double t_max,
                     hit_record& rec) const = 0;
};

#endif