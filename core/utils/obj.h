#ifndef PROWOGENE_CORE_UTILS_OBJ_H_
#define PROWOGENE_CORE_UTILS_OBJ_H_

#include "utils/model_io.h"

namespace prowogene {
namespace utils {

/** @brief Save class for Wavefront OBJ 3D model files. */
class Obj {
 public:
    /** Save 3D model to file.
    @param [in] model  - 3D model to save.
    @param [in] params - Saving params. */
    static void Save(const Model3d& model, const ModelIOParams& params);

 protected:
    /** Dectect coord order and their axis directions from string.
    @param [in] format       - String with coords order and their signs.
    @param [out] is_positive - X, Y and Z axis signs.
    @param [out] indexes     - X, Y and Z position indexes. */
    static void DetectCoordFormat(std::string format,
                                  bool (&is_positive)[3],
                                  int  (&indexes)[3]);

    /** Reorder point coords according user format.
    @param [in] coords      - input point coordinates.
    @param [in] is_positive - X, Y and Z axis signs.
    @param [in] indexes     - X, Y and Z position indexes.
    @param [out] out_values - output point coordinates. */
    static void ReorderCoords(const float (&coords)[3],
                              const bool(&is_positive)[3],
                              const int(&indexes)[3],
                              float (&out_values)[3]);

    /** Serialize vertex group to a string.
    @param [in] group       - Vertex group.
    @param [in] add_uv      - Add uv.
    @param [in] add_normals - Add normals.
    @return String representation of given vertex group. */
    static std::string SerializeGroup(const VertexGroup& group,
                                      bool add_uv,
                                      bool add_normals);
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_OBJ_H_
