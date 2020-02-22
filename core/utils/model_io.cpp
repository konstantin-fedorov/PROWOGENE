#include "model_io.h"

#include "utils/obj.h"

namespace prowogene {
namespace utils {

using std::string;

void ModelIO::Save(const Model3d& model, const ModelIOParams& params) const {
    if (params.format == "obj") {
        Obj::Save(model, params);
    }
}

} // namespace utils
} // namespace prowogene
