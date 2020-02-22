#include "obj.h"

#include <algorithm>
#include <fstream>

namespace prowogene {
namespace utils {

using std::ofstream;
using std::string;

static const size_t kBufferSize = 8192;
static const int    kIndexBase = 1;

void Obj::Save(const Model3d& model, const ModelIOParams& params) {
    const std::string full_filename = params.filename + ".obj";
    const bool add_uv = params.add_uv;
    const bool add_normals = params.add_normals;

    ofstream file;
    char buffer[kBufferSize];
    file.rdbuf()->pubsetbuf(buffer, kBufferSize);
    file.open(full_filename);
    if (!file.is_open()) {
        return;
    }

    bool coord_is_positive[3];
    int  coord_indexes[3];
    DetectCoordFormat(params.coord_format, coord_is_positive, coord_indexes);

    if (params.texture.size()) {
        file << "mtllib " << params.filename << ".mtl\n";
    }
    file << "o " << params.filename << "\n";

    const size_t vertexes_count = model.vertexes.size();
    for (size_t i = 0; i < vertexes_count; ++i) {
        float res[3];
        auto& vertex = model.vertexes[i];
        ReorderCoords({ vertex.x, vertex.y, vertex.z }, coord_is_positive,
                      coord_indexes, res);
        file << "v " << res[0] << " " << res[1] << " " << res[2] << "\n";
    }

    if (params.add_uv) {
        const size_t uv_count = model.uv.size();
        for (size_t i = 0; i < uv_count; ++i) {
            file << "vt " << model.uv[i].u << " " << model.uv[i].v << "\n";
        }
    }

    if (add_normals) {
        const size_t normals_count = model.vertexes.size();
        for (size_t i = 0; i < normals_count; ++i) {
            float res[3];
            auto& normal = model.normals[i];
            ReorderCoords({ normal.x, normal.y, normal.z }, coord_is_positive,
                          coord_indexes, res);
            file << "vn " << res[0] << " " << res[1] << " " << res[2] << "\n";
        }
    }

    if (params.texture.size()) {
        file << "usemtl material_" << params.filename << "\n";
    }

    for (size_t i = 0; i < model.faces.size(); ++i) {
        const auto& group = model.faces[i].groups;
        file << "f " <<
            SerializeGroup(group[0], add_uv, add_normals) << " " <<
            SerializeGroup(group[1], add_uv, add_normals) << " " <<
            SerializeGroup(group[2], add_uv, add_normals) << "\n";
    }

    file.close();

    if (params.texture.size()) {
        file.open(params.filename + ".mtl");
        file << "newmtl material_" << params.filename << "\n";
        file << "map_Kd " << params.texture << "\n";
        file << "Ks 0 0 0\n";
        file.close();
    }
}

void Obj::DetectCoordFormat(std::string format, bool(&is_positive)[3],
        int(&indexes)[3]) {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);

    is_positive[0] = format.find("-x") == string::npos;
    is_positive[1] = format.find("-y") == string::npos;
    is_positive[2] = format.find("-z") == string::npos;

    size_t x_index = format.find("x");
    size_t y_index = format.find("y");
    size_t z_index = format.find("z");
    if (x_index == string::npos ||
            y_index == string::npos ||
            z_index == string::npos) {
        indexes[0] = 0;
        indexes[1] = 1;
        indexes[2] = 2;
        return;
    }

    indexes[0] = static_cast<int>((x_index > y_index) + (x_index > z_index));
    indexes[1] = static_cast<int>((y_index > x_index) + (y_index > z_index));
    indexes[2] = static_cast<int>((z_index > x_index) + (z_index > y_index));
}

void Obj::ReorderCoords(const float(&coords)[3], const bool(&is_positive)[3],
        const int(&indexes)[3], float(&out_values)[3]) {
    float values[3];
    for (int i = 0; i < 3; ++i) {
        values[indexes[i]] = coords[i] * (is_positive[i] ? 1 : -1);
    }
    out_values[0] = values[0];
    out_values[1] = values[1];
    out_values[2] = values[2];
}

inline string Obj::SerializeGroup(const VertexGroup& group,
        bool add_uv, bool add_normals) {
    if (add_uv) {
        if (add_normals) {
            return std::to_string(group.v_idx +  kIndexBase) + "/" +
                   std::to_string(group.vt_idx + kIndexBase) +"/" +
                   std::to_string(group.vn_idx + kIndexBase);
        } else {
            return std::to_string(group.v_idx +  kIndexBase) + "/" +
                   std::to_string(group.vt_idx + kIndexBase);
        }
    } else {
        if (add_normals) {
            return std::to_string(group.v_idx +  kIndexBase) + "//" +
                   std::to_string(group.vn_idx + kIndexBase);
        } else {
            return std::to_string(group.v_idx + kIndexBase);
        }
    }
}

} // namespace utils
} // namespace prowogene
