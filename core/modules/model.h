#ifndef PROWOGENE_CORE_MODULES_MODEL_H_
#define PROWOGENE_CORE_MODULES_MODEL_H_

#include "general_settings.h"
#include "names_settings.h"
#include "system_settings.h"
#include "modules/texture.h"
#include "utils/model_io.h"
#include "utils/range.h"

namespace prowogene {
namespace modules {

/** Model settings config name. */
static const std::string kConfigModel = "model";

/** @brief Model module settings. */
struct ModelSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    void Check() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Enable chunk 3D model saving. */
    bool  chunks_enabled = false;
    /** Enable complex map 3D model saving. */
    bool  complex_enabled = false;
    /** Add materials for all saved models. */
    bool  materials_enabled = false;
    /** Add UV coordinates for all saved models. */
    bool  uv_enabled = false;
    /** Add normal vectors for verticies for all saved models. */
    bool  normals_enabled = false;
    /** Distance between 2 nearest points on heightmap in 3D units. */
    float edge_size = 1.0f;
    /** Distance between lowest and highest points of map. */
    float map_height = 20.0f;
    /** Coordinates order in model. Can store negative values. */
    std::string coord_format = "x z y";
};


/** @brief Module for 3D model creation.

Creates models for heightmap and links textures to them (if any). */
class ModelModule : public IModule {
 public:
    /** @copydoc IModule::Process */
    void Process() override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Create 3D model for part of height map. Only square areas supported.
    @param [in] x    - X coordinate of top left corner of the area.
    @param [in] y    - Y coordinate of top left corner of the area.
    @param [in] side - side size of area in points.
    @return 3D model for selected part of height map. */
    virtual utils::Model3d CreateArea(int x, int y, int side) const;

    /** Fill 3D model's vertexes according to height map.
    @param [in, out] model - Model for filling.
    @param [in] r          - Area ranges. */
    virtual void FillAreaVertexes(utils::Model3d& model,
                                  const utils::Range& r) const;

    /** Fill 3D model's UV.
    @param [in, out] model - Model for filling.
    @param [in] r          - Area ranges. */
    virtual void FillAreaUV(utils::Model3d& model,
                            const utils::Range& r) const;

    /** Fill 3D model's normal vectors according to height map.
    @param [in, out] model - Model for filling.
    @param [in] r          - Area ranges. */
    virtual void FillAreaNormals(utils::Model3d& model,
                                 const utils::Range& r) const;

    /** Fill 3D model's faces according to height map.
    @param [in, out] model - Model for filling.
    @param [in] r          - Area ranges. */
    virtual void FillAreaFaces(utils::Model3d& model,
                               const utils::Range& r) const;

    /** Fill 3D model's vertexes according to height map.
    @param [in] face   - Indexes of vertexes.
    @param [out] group - Tris to fill. */
    static void FillTris(const std::vector<int>& face,
                         utils::VertexGroup (&group)[3]);

 public:
    /** Height map from data storage. */
    utils::Array2D<float>* height_map_ = nullptr;
    /** 3D model input/output worker from data storage. */
    utils::ModelIO*        model_io_ = nullptr;

 protected:
    /** Settings for module. */
    struct {
        /** General settings. */
        GeneralSettings general;
        /** Model settings. */
        ModelSettings   model;
        /** Names settings. */
        NamesSettings   names;
        /** System settings. */
        SystemSettings  system;
        /** Texture settings. */
        TextureSettings texture;
    } settings_;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_MODEL_H_
