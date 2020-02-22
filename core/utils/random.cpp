#include "random.h"

namespace prowogene {
namespace utils {

Random::Random(int seed) {
    gen_.seed(static_cast<unsigned int>(seed));
}

unsigned int Random::Next() {
    return gen_();
}

float Random::Next(float min, float max) {
    const int range = Next(0, INT32_MAX - 1);
    const float progress = static_cast<float>(range) / INT32_MAX;
    const float result = min + progress * (max - min);
    return result;
}

int Random::Next(int min, int max) {
    const int range = max - min + 1;
    const int result = (Next() % range) + min;
    return result;
}

} // namespace prowogene
} // namespace utils
