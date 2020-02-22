#ifndef PROWOGENE_CORE_UTILS_RANDOM_H_
#define PROWOGENE_CORE_UTILS_RANDOM_H_

#include <random>

namespace prowogene {
namespace utils {

/** @brief Cross-platform RNG class. */
class Random {
 public:
    /** Constructor.
    @param [in] seed - Seed of number generator. */
    Random(int seed);

    /** Get next random number.
    @return Next random number. */
    virtual unsigned int Next();

    /** Get next random number in specified range.
    @param [in] min - Minimal possible value.
    @param [in] max - Maximal possible value.
    @return Next random number. */
    virtual float Next(float min, float max);

    /** @copydoc Random::Next(float, float) */
    virtual int   Next(int min, int max);

 protected:
    std::mt19937 gen_;
};

} // namespace prowogene
} // namespace utils

#endif // PROWOGENE_CORE_UTILS_RANDOM_H_
