#include "utils/array2d_tools.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <thread>

#include "utils/random.h"

namespace prowogene {
namespace utils {

using std::thread;
using std::vector;
using Band = std::pair<size_t, size_t>;

bool Array2DTools::DiamondSquare(Array2D<float> &arr, int size, int seed,
        int octave, float min_value, float max_value) {
    if ((size & (size - 1)) || octave > size) {
        return false;
    }

    if (arr.Width() != size || arr.Height() != size)
        arr.Resize(size, size);

    Random rand = Random(seed);
    int depth = 0;
    int size_copy = size;
    while (size_copy) {
        size_copy >>= 1;
        ++depth;
    }
    --depth;

    const int real_octave = std::min(depth, std::max(1, octave));
    int max_dist = size;
    for (int i = octave - 1; i > 0; --i) {
        max_dist >>= 1;
    }

    int areas_count = 1;
    int area_size = size;
    arr(0, 0) = 0;
    for (int i = 0; i < depth; ++i) {
        const int real_distortion = std::min(area_size, max_dist);
        const float temp_dist = static_cast<float>(real_distortion);

        for (int m = 0; m < areas_count; ++m) {
            for (int n = 0; n < areas_count; ++n) {
                const int x1 = area_size * m;
                const int y1 = area_size * n;
                const int x2 = (area_size * (m + 1));
                const int y2 = (area_size * (n + 1));
                const int xc = (x1 + x2) >> 1;
                const int yc = (y1 + y2) >> 1;

                arr(xc, yc) = (arr(x1,        y1) +
                               arr(x1,        y2 % size) +
                               arr(x2 % size, y1) +
                               arr(x2 % size, y2 % size)
                               ) / 4 + rand.Next(-temp_dist, temp_dist);
            }
        }

        for (int m = 0; m < areas_count; ++m) {
            for (int n = 0; n < areas_count; ++n) {
                const int x1 = area_size * m;
                const int y1 = area_size * n;
                const int x2 = area_size * (m + 1);
                const int y2 = area_size * (n + 1);
                const int xc = (x1 + x2) >> 1;
                const int yc = (y1 + y2) >> 1;

                const int x2_real = x2 % size;
                const int y2_real = y2 % size;

                arr(xc, y1) = (arr(xc,      yc) +
                               arr(x1,      y1) +
                               arr(x2_real, y1) +
                               arr(xc,      (2 * y1 - yc + size) % size)
                               ) / 4 + rand.Next(-temp_dist, temp_dist);
                arr(x2_real, yc) = (arr(xc,                   yc) +
                                    arr(x2_real,              y1) +
                                    arr(x2_real,              y2_real) +
                                    arr((2 * x2 - xc) % size, yc)
                                    ) / 4 + rand.Next(-temp_dist, temp_dist);
                arr(xc, y2_real) = (arr(xc,      yc) +
                                    arr(x2_real, y2_real) +
                                    arr(x1,      y2_real) +
                                    arr(xc,      (2 * y2 - yc) % size))
                                    / 4 + rand.Next(-temp_dist, temp_dist);
                arr(x1, yc) = (arr(xc,                          yc) +
                               arr(x1,                          y2_real) +
                               arr(x1,                          y1) +
                               arr((2 * x1 - xc + size) % size, yc)
                               ) / 4 + rand.Next(-temp_dist, temp_dist);
            }
        }
        areas_count <<= 1;
        area_size >>= 1;
    }

    Array2DTools::ToRange(arr, min_value, max_value);
    return true;
}

void Array2DTools::WhiteNoise(Array2D<float>& arr, int size, int seed) {
    if (arr.Width() != size || arr.Height() != size) {
        arr.Resize(size, size);
    }

    Random rand = Random(seed);
    for (auto& elem : arr) {
        elem = rand.Next(0.0f, 1.0f);
    }
}

void Array2DTools::RadialGradient(Array2D<float>& arr,
        int diameter, Gradient type) {
    RadialGradient(arr, diameter, type, diameter);
}

void Array2DTools::RadialGradient(Array2D<float> &arr,
        int diameter, Gradient type, int arr_size) {
    if (arr.Width() != arr_size || arr.Height() != arr_size) {
        arr.Resize(arr_size, arr_size);
    }

    const int center = arr_size / 2;
    const int center_sqr = center * center;
    const int grad_radius = diameter / 2;
    const int grad_radius_sqr = grad_radius * grad_radius;

    int x = 0;
    int y = 0;
    while (y <= center) {
        while (x <= center) {
            const int dist_sqr = (center - x) * (center - x) +
                                 (center - y) * (center - y);
            float value = 0.0f;

            switch (type) {
            case Gradient::Linear:
                if (dist_sqr <= grad_radius_sqr) {
                    value = (float)(grad_radius_sqr - dist_sqr);
                }
                break;

            case Gradient::Sinusoidal:
                if (dist_sqr <= grad_radius_sqr) {
                    value = cos(sqrt(dist_sqr *
                            static_cast<float>(M_PI * M_PI) /
                                               grad_radius_sqr)) + 1;
                }
                break;

            case Gradient::Quadric:
                if (dist_sqr <= grad_radius_sqr) {
                    value = static_cast<float>(grad_radius_sqr - dist_sqr);
                    value *= value;
                }
                break;

            default:
                break;
            }

            arr(x,                y               ) = value;
            arr(y,                x               ) = value;
            arr(arr_size - 1 - x, y               ) = value;
            arr(y,                arr_size - 1 - x) = value;
            arr(arr_size - 1 - x, arr_size - 1 - y) = value;
            arr(arr_size - 1 - y, arr_size - 1 - x) = value;
            arr(x,                arr_size - 1 - y) = value;
            arr(arr_size - 1 - y, x               ) = value;

            ++x;
        }
        ++y;
        x = y;
    }
    Array2DTools::ToRange(arr, 0.0f, 1.0f);
}

static void __ApplyFilter__(const float* first, const float* second,
        Operation overflow, float* res, int beg, int end) {
    switch (overflow) {
    case Operation::Add:
        for (int i = beg; i < end; ++i) {
            res[i] = first[i] + second[i];
        }
        break;
    case Operation::Substract:
        for (int i = beg; i < end; ++i) {
            res[i] = first[i] - second[i];
        }
        break;
    case Operation::Multiply:
        for (int i = beg; i < end; ++i) {
            res[i] = first[i] * second[i];
        }
        break;
    case Operation::Divide:
        for (int i = beg; i < end; ++i) {
            res[i] = first[i] / second[i];
        }
        break;
    case Operation::Max:
        for (int i = beg; i < end; ++i) {
            res[i] = std::max(first[i], second[i]);
        }
        break;
    case Operation::Min:
        for (int i = beg; i < end; ++i) {
            res[i] = std::min(first[i], second[i]);
        }
        break;
    default:
        break;
    }
}

bool Array2DTools::ApplyFilter(Array2D<float> &arr, Operation overflow,
        const Array2D<float> &first, const Array2D<float> &second,
        int thread_count) {
    const int width = arr.Width();
    const int height = arr.Height();

    if (first.Width() != width || first.Height() != height ||
            second.Width() != width || second.Height() != height ||
            !thread_count) {
        return false;
    }

    const vector<Band> bands = SplitIndices(thread_count, first.Size());
    const int thread_size = static_cast<int>(bands.size());

    vector<thread> threads(thread_size);
    for (int i = 0; i < thread_size; ++i) {
        threads[i] = thread(__ApplyFilter__, first.Data(), second.Data(),
                            overflow, arr.Data(),
                            static_cast<int>(bands[i].first),
                            static_cast<int>(bands[i].second));
    }
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    return true;
}

void Array2DTools::Drag(Array2D<float> &arr, int x, int y) {
    const int width = arr.Width();
    const int height = arr.Height();
    Array2D<float> buffer = arr;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            buffer((i + x + width - 1) % width,
                   (j + y + height - 1) % height) = arr(i, j);
        }
    }
    arr = buffer;
}

void Array2DTools::SetAlign(Array2D<float>& arr, KeyPoint type, Align align,
         int seed) {
    if (type == KeyPoint::Default || align == Align::Default) {
        return;
    }

    const int width = arr.Width();
    const int height = arr.Height();

    float max = 0;
    float min = 0;
    float temp_point = arr(0, 0);
    int x_displace = 0;
    int y_displace = 0;
    bool condition = false;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (type == KeyPoint::Max) {
                condition = arr(x, y) > temp_point;
            }
            if (type == KeyPoint::Min) {
                condition = arr(x, y) < temp_point;
            }

            if (condition) {
                temp_point = arr(x, y);
                x_displace = x;
                y_displace = y;
            }
        }
    }

    switch (align) {
        case Align::Center:
            Array2DTools::Drag(arr, width / 2 -  x_displace,
                                    height / 2 - y_displace);
            break;
        case Align::Corner:
            Array2DTools::Drag(arr, -x_displace, -y_displace);
            break;
        case Align::Random: {
            Random rand = Random(seed);
            const int shift_x = rand.Next(0, width - 1);
            const int shift_y = rand.Next(0, height - 1);
            Array2DTools::Drag(arr, shift_x, shift_y);
            }
            break;
        default:
            break;
    }
}

void Array2DTools::GetLevel(float &level, const Array2D<float> &arr,
         float dimension, int iterations_count) {
    const int width = arr.Width();
    const int height = arr.Height();

    float min = arr(0, 0);
    float max = arr(0, 0);
    Array2DTools::GetMinMax(arr, min, max);

    float bound_min = min;
    float bound_max = max;
    float current_level = 0.0f;
    int count_less;

    level = min;
    for (int i = 0; i < iterations_count; ++i) {
        count_less = 0;
        current_level = (bound_min + bound_max) / 2;

        for (const auto& elem : arr) {
            if (elem < current_level) {
                count_less++;
            }
        }

        if (static_cast<float>(count_less) / (height * width) > dimension) {
            bound_max = current_level;
        } else {
            bound_min = current_level;
        }
    }
    level = current_level;
}

void Array2DTools::GetMinMax(const Array2D<float> &arr, float &min,
        float &max) {
    min = arr(0, 0);
    max = arr(0, 0);

    for (const auto& elem : arr) {
        if (elem < min) {
            min = elem;
        }
        if (elem > max) {
            max = elem;
        }
    }
}

static void __ToRange__(float* arr, float min_v, float max_v,
        float real_min, float real_max, int beg, int end) {
    const float target_range = max_v - min_v;
    for (int i = beg; i < end; ++i) {
        arr[i] = ((arr[i] - real_min) / real_max) * target_range + min_v;
    }
}

void Array2DTools::ToRange(Array2D<float> &arr, float min_v, float max_v,
        int thread_count) {
    float min = arr(0, 0);
    float max = arr(0, 0);
    Array2DTools::GetMinMax(arr, min, max);
    max -= min;

    const vector<Band> bands = SplitIndices(thread_count, arr.Size());
    const int thread_size = static_cast<int>(bands.size());

    vector<thread> threads(thread_size);
    for (int i = 0; i < thread_size; ++i) {
        threads[i] = thread(__ToRange__, arr.Data(), min_v, max_v, min, max,
                                         static_cast<int>(bands[i].first),
                                         static_cast<int>(bands[i].second));
    }
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void Array2DTools::ChangeRes(Array2D<float>& arr, int x, int y, float val) {
    const int prev_width = arr.Width();
    const int prev_height = arr.Height();

    if (prev_width == x && prev_height == y) {
        return;
    }

    Array2D<float> buffer = arr;
    arr.Resize(x, y);

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            if (i < prev_width && j < prev_height) {
                arr(i, j) = buffer(i, j);
            } else {
                arr(i, j) = val;
            }
        }
    }
}

template <typename T>
void __FindMinMaxInArea__(Point<T>& min, Point<T>& max, const Array2D<T>& arr,
        int x, int y, int width, int height) {
    min.value = arr(x, y);
    min.x = x;
    min.y = y;
    max.value = arr(x, y);
    max.x = x;
    max.y = y;
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            if (min.value > arr(i, j)) {
                min.value = arr(i, j);
                min.x = i;
                min.y = j;
            }
            if (max.value < arr(i, j)) {
                max.value = arr(i, j);
                max.x = i;
                max.y = j;
            }
        }
    }
}

void Array2DTools::FindMinMaxInArea(Point<int>& min, Point<int>& max,
        const Array2D<int>& arr, int x, int y, int width, int height) {
    __FindMinMaxInArea__(min, max, arr, x, y, width, height);
}
void Array2DTools::FindMinMaxInArea(Point<float>& min, Point<float>& max,
        const Array2D<float>& arr, int x, int y, int width, int height) {
    __FindMinMaxInArea__(min, max, arr, x, y, width, height);
}

static void __Smooth__(const Array2D<float>* arr, Array2D<float>* buffer,
        const Array2D<float>* coefs, float coef_sum, int rad,
        int x_beg, int x_end) noexcept {
    const int width = arr->Width();
    const int height = arr->Height();
    for (int x = x_beg; x < x_end; ++x) {
        for (int y = 0; y < height; ++y) {
            float new_val = 0.0f;
            for (int i = -rad; i <= rad + 1; ++i) {
                for (int j = -rad; j <= rad + 1; ++j) {
                    const int x_left = x - rad - i;
                    const int y_top =  y - rad - j;
                    const int real_i = std::min(width,  std::max(0, x_left));
                    const int real_j = std::min(height, std::max(0, y_top));
                    new_val += (*coefs)(rad + i, rad + j) * (*arr)(real_i, real_j);
                }
            }
            (*buffer)(x, y) = new_val / coef_sum;
        }
    }
}

void Array2DTools::Smooth(Array2D<float>& arr, int radius, int thread_count) {
    if (!radius) {
        return;
    }
    if (radius < 0) {
        radius = -radius;
    }

    const int coef_size = radius * 2 + 1;
    Array2D<float> coefs;
    Array2DTools::RadialGradient(coefs, coef_size, Gradient::Sinusoidal);

    float coef_sum = 0.0f;
    for (auto elem : coefs) {
        coef_sum += elem;
    }

    Array2D<float> buffer = arr;

    const vector<Band> bands = SplitIndices(thread_count, arr.Width());
    const int thread_size = static_cast<int>(bands.size());

    vector<thread> threads(thread_size);
    for (int i = 0; i < thread_size; ++i) {
        threads[i] = thread(__Smooth__, &arr, &buffer, &coefs, coef_sum, radius,
                                        static_cast<int>(bands[i].first),
                                        static_cast<int>(bands[i].second));
    }
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
    arr = buffer;
}

void Array2DTools::ApplySurface(Array2D<float> &arr, Surface surface,
        int seed, int periodicity) {
    const int size = arr.Width();
    switch (surface) {
        case Surface::DiamondSquare:
            DiamondSquare(arr, size, seed, periodicity, 0.0f, 1.0f);
            break;
        case Surface::WhiteNoise:
            WhiteNoise(arr, size, seed);
            break;
        case Surface::RadialGradient:
            RadialGradient(arr, size, Gradient::Linear);
            break;
        case Surface::Flat:
            for (auto& elem : arr) {
                elem = 0.0f;
            }
            break;
        default:
            break;
    }
}

void Array2DTools::ApplyDistortion(Array2D<float> &arr, Distortion dist) {
    if (dist == Distortion::Quadric) {
        ApplyFilter(arr, Operation::Multiply, arr, arr);
    }
}

void Array2DTools::ApplyGradient(Array2D<float> &arr, Gradient grad) {
    switch (grad) {
        case Gradient::Linear:
            break;
        case Gradient::Sinusoidal:
            for (auto& elem : arr) {
                elem = (1.0f - std::cos(elem * static_cast<float>(M_PI))) / 2;
            }
            ToRange(arr, 0.0f, 1.0f);
            break;
        case Gradient::Quadric:
            ApplyFilter(arr, Operation::Multiply, arr, arr);
            break;
        default:
            break;
    }
}

void Array2DTools::ScaleUp(Array2D<float>& arr, int n) {
    if (n < 2) {
        return;
    }
    const int width = arr.Width();
    const int height = arr.Height();

    Array2D<float> out(width * n, height * n);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float val = arr(x, y);
            const int beg_i = x * n;
            const int beg_j = y * n;
            for (int j = beg_j; j < beg_j + n; ++j) {
                for (int i = beg_i; i < beg_i + n; ++i) {
                    out(i, j) = val;
                }
            }
        }
    }
    arr = out;
}

vector<Band> Array2DTools::SplitIndices(int count, size_t data_size) {
    if (!data_size || !count) {
        return vector<Band>();
    }
    if (count > data_size) {
        return { {0, data_size - 1} };
    }
    vector<Band> bands;
    bands.reserve(count);
    const int common_piece = static_cast<int>(data_size / count);
    int start_idx = 0;
    for (int i = 0; i < count - 1; ++i) {
        bands.push_back({ start_idx, start_idx + common_piece });
        start_idx += common_piece;
    }
    bands.push_back({ start_idx, data_size });
    return bands;
}

} // namespace utils
} // namespace prowogene

