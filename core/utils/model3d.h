#ifndef PROWOGENE_CORE_UTILS_MODEL3D_H_
#define PROWOGENE_CORE_UTILS_MODEL3D_H_

#include <vector>

namespace prowogene {
namespace utils {

/** @brief 3D object vertex. */
struct Vertex {
    /** X coordinate of vertex. */
    float x = 0.0f;
    /** Y coordinate of vertex. */
    float y = 0.0f;
    /** Z coordinate of vertex. */
    float z = 0.0f;
};


/** @brief UV vertex. */
struct TextureCoord {
    /** U coordinate of vertex. */
    float u = 0.0f;
    /** V coordinate of vertex. */
    float v = 0.0f;
};


/** @brief 3D vector. */
struct Vector3D {
    /** X length of vector. */
    float x = 0.0f;
    /** Y length of vector. */
    float y = 0.0f;
    /** Z length of vector. */
    float z = 0.0f;

    /** Scalar multiply this vector to another.
    @param [in] vector - Another vector to multiply.
    @return Result multiplication vector. */
    Vector3D Multiply(const Vector3D& vector) const;
};


/** @brief Group of corelated verticies indexes. */
struct VertexGroup {
    /** Vertex index. */
    int v_idx = 0;
    /** UV vertex index. */
    int vt_idx = -1;
    /** Normal vector vertext index. */
    int vn_idx = -1;
};


/** @brief Triangle face of verticies. */
struct ObjectFace {
    /** Group of pixels. */
    VertexGroup groups[3];
};


/** @brief 3D model info storage. */
struct Model3d {
    /** Verticies. */
    std::vector<Vertex>       vertexes;
    /** UV verticies. */
    std::vector<TextureCoord> uv;
    /** Normal vectors. */
    std::vector<Vector3D>       normals;
    /** 3D model faces. */
    std::vector<ObjectFace>   faces;
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_MODEL3D_H_
