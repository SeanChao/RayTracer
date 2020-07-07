#include <iostream>

#include "color.h"
#include "ray.h"
#include "vec3.h"

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);  // map y to [0, 1]
    // blend blue and white linearly
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 384;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    // P3: colors in ASCII, width * height, 255 for max color
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);

    auto lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ';
        for (int i = 0; i < image_width; i++) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin,
                  lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel(ray_color(r));
            write_color(std::cout, pixel);
        }
    }
    std::cerr << "\nDone.\n";
    return 0;
}