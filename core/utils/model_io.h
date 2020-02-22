#ifndef PROWOGENE_CORE_UTILS_MODEL_IO_H_
#define PROWOGENE_CORE_UTILS_MODEL_IO_H_

#include <string>

#include "model3d.h"

namespace prowogene {
namespace utils {

/** @brief Model input/output params. */
struct ModelIOParams {
    /** File format. In most cases, it's also filename extension. */
    std::string format       = "obj";
    /** Filename without extension. */
    std::string filename     = "model";
    /** Coord order and axis directions. */
    std::string coord_format = "x y z";
    /** Add normal vectors to 3D model. */
    bool        add_normals  = false;
    /** Add UV coords. */
    bool        add_uv       = false;
    /** Texture filename. Make sense only when add_uv is @c true . */
    std::string texture      = "";
    /** Normal map filename. Make sense only when add_normals is @c true . */
    std::string normal_map   = "";
};

/** @brief Model input/output worker. */
class ModelIO {
 public:
    /** Save 3D model to file.
    @param [in] model - 3D model to save.
    @param [in] par   - Saving params. */
    virtual void Save(const Model3d& model, const ModelIOParams& par) const;
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_MODEL_IO_H_
