#include "model3d.h"

namespace prowogene {
namespace utils {

Vector3D Vector3D::Multiply(const Vector3D& vector) const {
    Vector3D out;
    out.x = y * vector.z - z * vector.y;
    out.y = z * vector.x - x * vector.z;
    out.z = x * vector.y - y * vector.x;
    return out;
}

} // namespace utils
} // namespace prowogene
