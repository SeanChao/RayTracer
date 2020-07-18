#include <iostream>

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

// Returns `t` of the hit point that we faces or -1.0
double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0) {
        // not hit
        return -1.0;
    } else {
        // returns `t` of the hit point that we faces
        return (-half_b - sqrt(discriminant)) / a;
    }
}

// Assign the given ray a color in the world.
// If the ray hits nothing, it's in blue-scale background color.
color ray_color(const ray& r, const hittable& world, int depth) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return color(0, 0, 0);
    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);  // map y to [0, 1]
    // blend blue and white linearly in background
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 320;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // P3: colors in ASCII, width * height, 255 for max color
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    hittable_list world;
    world.add(make_shared<sphere>(
        point3(0, 0, -1), 0.5, make_shared<lambertian>(color(0.7, 0.3, 0.3))));

    world.add(
        make_shared<sphere>(point3(0, -100.5, -1), 100,
                            make_shared<lambertian>(color(0.8, 0.8, 0.0))));

    world.add(make_shared<sphere>(point3(1, 0, -1), 0.5,
                                  make_shared<metal>(color(.8, .6, .2), 0.5)));
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5,
                                  make_shared<metal>(color(.8, .8, .8))));
    world.add(make_shared<sphere>(point3(0.5, 0.5, -1), 0.2,
                                  make_shared<metal>(color(.2, .4, .8))));

    camera cam;

    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\r>> " << j << ' ';
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);  // accumulator
            for (int s = 0; s < samples_per_pixel; s++) {
                // randomly pick surronding color to antialiasing
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\rDone.\n";
    return 0;
}
