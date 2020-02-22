#ifndef PROWOGENE_CORE_UTILS_ARRAY2D_TOOLS_H_
#define PROWOGENE_CORE_UTILS_ARRAY2D_TOOLS_H_

#include <utility>

#include "types.h"
#include "utils/array2d.h"

namespace prowogene {
namespace utils {

/** @brief Some common algorithms for work with Array2D. */
class Array2DTools {
 public:
    /** Generate gradient noise with "diamond-square algorithm".
    @param [out] arr      - Array to fill with noise.
    @param [in] size      - Output noise resolution. [1, ...], power of 2.
    @param [in] seed      - Random number generator seed.
    @param [in] octave    - Details size. Less values cause bigger details.
                            [1, size].
    @param [in] min_value - Noise minimal value.
    @param [in] max_value - Noise maximal value.
    @return @c true if generation succeeded, @c false otherwise. */
    static bool DiamondSquare(Array2D<float>& arr,
                              int size,
                              int seed,
                              int octave,
                              float min_value,
                              float max_value);

    /** Generate white noise.
    @param [out] arr - Array to fill with noise.
    @param [in] size - Output noise width and height. [1, ...].
    @param [in] seed - Random number generator seed. */
    static void WhiteNoise(Array2D<float>& arr, int size, int seed);

    /** Create spot at the center of the array with value 1.0 in center
    and 0.0 further than diameter.
    @param [out] arr     - Array to fill with noise.
    @param [in] diameter - Diameter of spot.
    @param [in] gradient - Type of spot values decreasing algoritm. */
    static void RadialGradient(Array2D<float>& arr,
                               int diameter,
                               Gradient gradient);

    /** Create spot at the center of the array with value 1.0 in center
    and 0.0 further than diameter.
    @param [out] arr     - Array to fill with noise.
    @param [in] diameter - Diameter of spot.
    @param [in] gradient - Type of spot values decreasing algoritm.
    @param [in] arr_size - Output array width and height. When it is greater
                           than diameter, other values will be set to 0.0.
                           [diameter, ...]. */
    static void RadialGradient(Array2D<float>& arr,
                               int diameter,
                               Gradient g,
                               int arr_size);

    /** Fill array with result of operation applyed to 2 arrays.
    @param [out] arr         - Output array.
    @param [in] overflow     - Operation type.
    @param [in] first        - First input array.
    @param [in] second       - Second input array.
    @param [in] thread_count - Maximal thread count for processing.
    @return @c true if operation applyed successfully, @c false otherwise.
            If that method returns @c false , check arrays resolutions. */
    static bool ApplyFilter(Array2D<float>& arr,
                          Operation overflow,
                          const Array2D<float>& first,
                          const Array2D<float>& second,
                          int thread_count = 1);

    /** Move all values in array with wrap on each side.
    @param [in, out] arr - Array to drag.
    @param [in] x        - Shift by X coordinate.
    @param [in] y        - Shift by Y coordinate. */
    static void Drag(Array2D<float>& arr, int x, int y);

    /** Move all values in array with wrap on each side according to key
    point position.
    @param [in, out] arr - Array to drag.
    @param [in] type     - Key point type.
    @param [in] align    - Key point result align.
    @param [in] seed     - Seed of random number generator (needed only when
                           align is set to @c Align::Random ). */
    static void SetAlign(Array2D<float>& arr,
                         KeyPoint type,
                         Align align,
                         int seed = 0);

    /** Find value for array, which is bigger than part of array's values.
    @param [out] level           - Output value.
    @param [in] arr              - Array for finding value.
    @param [in] dimension        - Part of all elements in array that must be
                                   less than specified level.
    @param [in] iterations_count - Binary search iterations count. */
    static void GetLevel(float& level,
                         const Array2D<float>& arr,
                         float dimension,
                         int iterations_count);

    /** Find minimal and maximal values in array.
    @param [in] arr  - Array for finding values.
    @param [out] min - Minimal value in array.
    @param [out] max - Maximal value in array. */
    static void GetMinMax(const Array2D<float>& arr, float& min, float& max);

    /** Set all values to specified range with saving their relative values to
    each other.
    @param [in, out] arr     - Array to modify values.
    @param [in] min          - Output array minimal value.
    @param [in] max          - Output array maximal value.
    @param [in] thread_count - Maximal thread count for processing. */
    static void ToRange(Array2D<float>& arr,
                        float min,
                        float max,
                        int thread_count = 1);

    /** Change array's resolution with saving values (where it's possible).
    @param [in, out] arr - Array to resize.
    @param [in] x        - Width of output array.
    @param [in] y        - Height of output array.
    @param [in] val      - Value for element which wasn't in input array. */
    static void ChangeRes(Array2D<float>& arr, int x, int y, float val);

    /** Find minimal and maximal values in part of array.
    @param [in] min    - Minimal value in array.
    @param [in] max    - Maximal value in array.
    @param [in] arr    - Array for finding values.
    @param [in] x      - X coordinate of araea's top left corner.
    @param [in] y      - Y coordinate of araea's top left corner.
    @param [in] width  - Width of area.
    @param [in] height - Height of area. */
    static void FindMinMaxInArea(Point<int>& min,
                                 Point<int>& max,
                                 const Array2D<int>& arr,
                                 int x,
                                 int y,
                                 int width,
                                 int height);

    /** Find minimal and maximal values in part of array.
    @param [in] min    - Minimal value in array.
    @param [in] max    - Maximal value in array.
    @param [in] arr    - Array for finding values.
    @param [in] x      - X coordinate of araea's top left corner.
    @param [in] y      - Y coordinate of araea's top left corner.
    @param [in] width  - Width of area.
    @param [in] height - Height of area. */
    static void FindMinMaxInArea(Point<float>& min,
                                 Point<float>& max,
                                 const Array2D<float>& arr,
                                 int x,
                                 int y,
                                 int width,
                                 int height);

    /** Smooth array's values.
    @param [in, out] arr     - Array to modify values.
    @param [in] radius       - Smooth radius.
    @param [in] thread_count - Maximal thread count for processing. */
    static void Smooth(Array2D<float>& arr, int radius, int thread_count = 1);

    /** Fill array with surface type.
    @param [out] arr        - Array to fill.
    @param [in] surface     - Type of surface.
    @param [in] seed        - Random nuber generator seed.
    @param [in] periodicity - Periodicity of gradient noise. */
    static void ApplySurface(Array2D<float>& arr,
                             Surface surface,
                             int seed,
                             int periodicity);

    /** Apply distortion to array.
    @param [in, out] arr - Array to modify values.
    @param [in] dist     - Type of distortion. */
    static void ApplyDistortion(Array2D<float>& arr, Distortion dist);

    /** Apply gradient to array.
    @param [in, out] arr - Array to modify values.
    @param [in] dist     - Type of gradient. */
    static void ApplyGradient(Array2D<float>& arr, Gradient grad);

    /** Scale up array's values.
    @param [in, out] arr - Array to scale up.
    @param [in] n        - Scale up times. */
    static void ScaleUp(Array2D<float>& arr, int n);

 protected:
    /** Split values count to about the same pieces.
    @param [in] count     - Count of pieces.
    @param [in] data_size - Data size to split.
    @return Array of first and last indicies in data. */
    static std::vector<std::pair<size_t, size_t> > SplitIndices(int count,
        size_t data_size);
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_ARRAY2D_TOOLS_H_
