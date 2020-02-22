#ifndef PROWOGENE_CORE_UTILS_RANGE_H_
#define PROWOGENE_CORE_UTILS_RANGE_H_

namespace prowogene {
namespace utils {

/** @brief Borders of some area. */
struct Range {
    /** Constructor. */
    Range() { }

    /** Constructor.
    @param [in] l - Left border.
    @param [in] r - Right border.
    @param [in] t - Top border.
    @param [in] b - Bottom border. */
    Range(int l, int r, int t, int b) {
        left = l;
        right = r;
        top = t;
        bottom = b;
    }


    /** Left border. */
    int left = 0;
    /** Right border. */
    int right = 0;
    /** Top border. */
    int top = 0;
    /** Bottom border. */
    int bottom = 0;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_RANGES_H_
