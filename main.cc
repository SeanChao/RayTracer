#include <iostream>

#include "ThreadPool.h"
#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2,
                          b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

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

std::mutex cnt_mutex;
int rendered_pixels = 0;

void render_pixel(int j, int i, const hittable_list& world, const camera& cam,
                  int w, int h, int samples_per_pixel, int max_depth,
                  std::vector<color>& result) {
    color pixel_color(0, 0, 0);  // accumulator
    for (int s = 0; s < samples_per_pixel; s++) {
        // randomly pick surronding color to antialiasing
        auto u = (i + random_double()) / (w - 1);
        auto v = (j + random_double()) / (h - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
    }
    cnt_mutex.lock();
    rendered_pixels++;
    std::cerr << "\r" << w * h - rendered_pixels << ' ' << std::flush;
    cnt_mutex.unlock();
    result[j * w + i] = pixel_color;
}

void concurrent_render(const int thread_cnt, const hittable_list& world,
                       const camera& cam, int image_width, int image_height,
                       int samples_per_pixel, int max_depth) {
    ThreadPool thread_pool(thread_cnt);
    std::vector<color> result(image_width * image_height);
    std::cerr << ">> Rendering" << std::endl;
    for (int j = image_height - 1; j >= 0; j--) {
        for (int i = 0; i < image_width; i++) {
            thread_pool.enqueue([j, i, &world, cam, image_width, image_height,
                                 samples_per_pixel, max_depth, &result]() {
                render_pixel(j, i, world, cam, image_width, image_height,
                             samples_per_pixel, max_depth, result);
            });
        }
    }
    thread_pool.wait_until_nothing_in_flight();
    std::cerr << "\r";
    std::cerr << ">> Writting to file" << std::endl;
    for (int j = image_height - 1; j >= 0; j--) {
        for (int i = 0; i < image_width; i++) {
            write_color(std::cout, result[j * image_width + i],
                        samples_per_pixel);
        }
    }
}

int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 3840;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // P3: colors in ASCII, width * height, 255 for max color
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    // World
    auto world = random_scene();

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
               dist_to_focus);
    const int thread_cnt = 4;
    concurrent_render(thread_cnt, world, cam, image_width, image_height,
                      samples_per_pixel, max_depth);
    std::cerr << "\rDone.\n";
    return 0;
}
