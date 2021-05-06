#include "model.h"

namespace prowogene {
namespace modules {

using std::list;
using std::string;
using std::vector;
using utils::Array2D;
using utils::Model3d;
using utils::ModelIO;
using utils::ModelIOParams;
using utils::Vector3D;
using utils::Range;
using utils::VertexGroup;

bool ModelModule::Process() {
    const bool chunks_enabled = settings_.model.chunks_enabled;
    const bool complex_enabled = settings_.model.complex_enabled;
    if (!chunks_enabled && !complex_enabled) {
        return true;
    }

    const int size = settings_.general.size;
    const string& img_ext = settings_.system.extensions.image;
    const bool materials_enabled = settings_.model.materials_enabled;

    ModelIOParams params;
    params.add_normals =  settings_.model.normals_enabled;
    params.add_uv =       settings_.model.uv_enabled;
    params.coord_format = settings_.model.coord_format;
    params.format =       settings_.system.extensions.model;

    if (chunks_enabled) {
        const int chunk_size = settings_.general.chunk_size;
        const int chunk_count = size / chunk_size;

        for (int x = 0; x < chunk_count; ++x) {
            for (int y = 0; y < chunk_count; ++y) {
                const int x_beg = x * chunk_size;
                const int y_beg = y * chunk_size;
                const Model3d chunk = CreateArea(x_beg, y_beg, chunk_size);

                const auto& chunk_templ = settings_.names.chunk;
                const string mesh_name = chunk_templ.Apply(x, y);

                params.filename = mesh_name;
                if (!settings_.texture.chunks_enabled || !materials_enabled) {
                    model_io_->Save(chunk, params);
                    params.filename = "";
                    continue;
                }

                const auto& texture_name = settings_.names.texture;
                const string texture =     texture_name.Apply(x, y, img_ext);
                const auto& normal_name = settings_.names.texture;
                const string normal =     normal_name.Apply(x, y, img_ext);
                params.texture =    texture;
                params.normal_map = normal;
                model_io_->Save(chunk, params);
                params.texture =    "";
                params.normal_map = "";
                params.filename =   "";
            }
        }
    }

    if (complex_enabled) {
        const auto& minimap_name = settings_.names.minimap;
        const Model3d complex = CreateArea(0, 0, size);
        params.filename = minimap_name.model;
        if (settings_.texture.minimap.enabled && materials_enabled) {
            params.texture =    minimap_name.texture + "." + img_ext;
            params.normal_map = minimap_name.normal  + "." + img_ext;
            model_io_->Save(complex, params);
        } else {
            model_io_->Save(complex, params);
        }
    }
    return true;
}

list<string> ModelModule::GetNeededSettings() const {
    return {
        settings_.general.GetName(),
        settings_.model.GetName(),
        settings_.names.GetName(),
        settings_.system.GetName(),
        settings_.texture.GetName()
    };
}

void ModelModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.model);
    CopySettings(settings, settings_.names);
    CopySettings(settings, settings_.system);
    CopySettings(settings, settings_.texture);
}

std::string ModelModule::GetName() const {
    return "Model";
}

Model3d ModelModule::CreateArea(int x_beg, int y_beg, int side) const {
    Model3d model;
    const Range range(x_beg, x_beg + side, y_beg, y_beg + side);
    FillAreaVertexes(model, range);
    if (settings_.model.uv_enabled) {
        FillAreaUV(model, range);
    }
    if (settings_.model.normals_enabled) {
        FillAreaNormals(model, range);
    }
    FillAreaFaces(model, range);
    return model;
}

void ModelModule::FillAreaVertexes(Model3d& model, const Range& range) const {
    const int   size = settings_.general.size;
    const float edge = settings_.model.edge_size;
    const float height = settings_.model.map_height;
    const int   side = range.right - range.left;
    const int   vertex_count = (side + 1) * (side + 1);
    const float real_half_size = size / 2.0f * edge;

    int vertex_idx = 0;
    model.vertexes.resize(vertex_count);
    for (int x = range.left; x <= range.right; ++x) {
        for (int y = range.top; y <= range.bottom; ++y) {
            auto& vertex = model.vertexes[vertex_idx];
            vertex.x = x * edge - real_half_size;
            vertex.y = y * edge - real_half_size;
            vertex.z = (*height_map_)(x % size, y % size) * height;
            ++vertex_idx;
        }
    }
}

void ModelModule::FillAreaUV(Model3d& model, const Range& range) const {
    const int side = range.right - range.left;
    const int vertex_count = (side + 1) * (side + 1);

    int vertex_idx = 0;
    model.uv.resize(vertex_count);
    for (int x = range.left; x <= range.right; ++x) {
        for (int y = range.top; y <= range.bottom; ++y) {
            auto& tex = model.uv[vertex_idx];
            tex.u = static_cast<float>(x) / side;
            tex.v = static_cast<float>(side - y) / side;
            ++vertex_idx;
        }
    }
}

void ModelModule::FillAreaNormals(Model3d& model, const Range& range) const {
    const int   size = settings_.general.size;
    const float edge = settings_.model.edge_size;
    const int   side = range.right - range.left;
    const int   vertex_count = (side + 1) * (side + 1);
    int vertex_idx = 0;
    model.normals.resize(vertex_count);

    Vector3D right_minus_left;
    right_minus_left.x = 0;
    right_minus_left.y = -2 * edge;
    Vector3D top_minus_bottom;
    top_minus_bottom.x = 2 * edge;
    top_minus_bottom.y = 0;

    for (int x = range.left; x <= range.right; ++x) {
        for (int y = range.top; y <= range.bottom; ++y) {
            const float h_l = (*height_map_)((x - 1 + size) % size, y);
            const float h_r = (*height_map_)((x + 1) % size, y);
            const float h_t = (*height_map_)(x, (y - 1 + size) % size);
            const float h_b = (*height_map_)(x, (y + 1) % size);

            right_minus_left.z = h_r - h_l;
            right_minus_left.z *= settings_.model.map_height;
            top_minus_bottom.z = h_b - h_t;
            top_minus_bottom.z *= settings_.model.map_height;

            auto& vertex = model.normals[vertex_idx];
            vertex = right_minus_left.Multiply(top_minus_bottom);
            ++vertex_idx;
        }
    }
}

void ModelModule::FillAreaFaces(Model3d& model, const Range& range) const {
    const int side = range.right - range.left;
    const int faces_count = side * side * 2;

    model.faces.resize(faces_count);
    int face_idx = 0;
    const int rect_side = side + 1;
    for (int yi = range.top; yi < range.bottom; ++yi) {
        for (int xi = range.left; xi < range.right; ++xi) {
            const int y = yi - range.top;
            const int x = xi - range.left;

            const int tl = y * rect_side + x;
            const int tr = tl + 1;
            const int bl = (y + 1) * rect_side + x;
            const int br = bl + 1;

            const vector<vector<int> > faces = {
                { tl, tr, bl },
                { bl, tr, br }
            };
            for (const auto& face : faces) {
                FillTris(face, model.faces[face_idx].groups);
                ++face_idx;
            }
        }
    }
}

inline void ModelModule::FillTris(const vector<int>& face,
        VertexGroup (&group)[3]) {
    for (int i = 0; i < 3; ++i) {
        const int vertex_idx = face[i];
        auto& vertex = group[i];
        vertex.v_idx =  vertex_idx;
        vertex.vt_idx = vertex_idx;
        vertex.vn_idx = vertex_idx;
    }
}

} // namespace modules
} // namespace prowogene
