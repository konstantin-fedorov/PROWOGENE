#ifndef PROWOGENE_CORE_TYPES_H_
#define PROWOGENE_CORE_TYPES_H_

namespace prowogene {

/** Epsilon for comparision floating point numbers. */
const float kEps = 0.000001f;

/** @brief Value with information about it's 2D coordinates. */
template <typename T>
struct Point {
    /** Value of point. */
    T   value;
    /** X coordinate. */
    int x;
    /** Y coordinate. */
    int y;
};


/** @brief Type of curvature. */
typedef enum class _Gradient : unsigned char {
    // **
    // ****
    // ******
    // ********
    // **********
    // ************
    // **************
    // ****************
    /** Straight line. */
    Linear,
    // **
    // *****
    // *******
    // ********
    // ********
    // *********
    // ***********
    // ****************
    /** Sinusoidal curvature. */
    Sinusoidal,
    // *
    // *
    // **
    // **
    // ***
    // ****
    // *******
    // ****************
    /** Quadric curvature. */
    Quadric
} Gradient;


/** @brief Type of key point position. */
typedef enum class _Align : unsigned char {
    /** No align. */
    Default,
    /** Align to center. */
    Center,
    /** Align to left top corner. */
    Corner,
    /** Align random position. */
    Random
} Align;


/** @brief Type of key point. */
typedef enum class _KeyPoint : unsigned char {
    /** Not specified. */
    Default,
    /** Some point with minimal value. */
    Min,
    /** Some point with maximal value. */
    Max
} KeyPoint;


/** @brief Type of operation that will be applied between relevant
array's elements. */
typedef enum class _Operation : unsigned char {
    /** Add. */
    Add,
    /** Subtract. */
    Substract,
    /** Multiply. */
    Multiply,
    /** Divide. */
    Divide,
    /** Choose the greater one. */
    Max,
    /** Choose the less one. */
    Min
} Operation;


/** @brief Type of surface filling algoritm. */
typedef enum class _Surface : unsigned char {
    /** Independent random values. */
    WhiteNoise,
    /** Diamond-Square based gradient noise. */
    DiamondSquare,
    /** Circle with top point at center and values decreasing according to
    distance to center point. */
    RadialGradient,
    /** Same value everywhere. */
    Flat
} Surface;


/** @brief Type of distortion. */
typedef enum class _Distortion : unsigned char {
    /** No distortion. */
    None,
    /** Multiply every element on themself. */
    Quadric
} Distortion;


/** @brief Type of biome. Determines basic values, without any details.
Note: Not same as Location. */
typedef enum class _Biome : unsigned char {
    /** Basis. */
    Basis,
    /** Mountain. */
    Mountain,
    /** Sea. */
    Sea,
    /** River. */
    River,
    /** Beach. */
    Beach,
    /** Count. */
    Count,
    /** Temporary value, not valid. */
    Temp
} Biome;


/** @brief Type of location. Determines detailed type of surface.
Note: Not same as Biome. */
typedef enum class _Location : unsigned char {
    /** None. */
    None,
    /** Forest. */
    Forest,
    /** Glade. */
    Glade,
    /** Mountain. */
    Mountain,
    /** River. */
    River,
    /** Beach. */
    Beach,
    /** Sea. */
    Sea
} Location;

} // namespace prowogene

#endif // PROWOGENE_CORE_TYPES_H_
