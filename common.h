#ifndef COMMON_h
#define COMMON_h

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// Using

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) { return degrees * pi / 180; }

// Thread safe random number generator
int intRand(const int& min, const int& max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

// Returns a random real in [0,1).
inline double random_double() {
    return intRand(0, RAND_MAX) / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

// returns clamped value of x from min to max
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "camera.h"

#endif
