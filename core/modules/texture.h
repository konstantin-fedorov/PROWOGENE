#ifndef PROGOGENE_CORE_TEXTURE_H_
#define PROGOGENE_CORE_TEXTURE_H_

#include "names_settings.h"
#include "system_settings.h"
#include "modules/basis.h"
#include "utils/image_io.h"

namespace prowogene {
namespace modules {

/** Texture settings config name. */
static const std::string kConfigTexture = "texture";

/** List of gradient key points with RGBA colors and height where that
color will be set. */
using GradientPoints = std::vector<std::pair<utils::RgbaPixel, float> >;

/** @brief Texture module settings. */
struct TextureSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Fill gradient points with values from JSON array */
    static GradientPoints GetGradientPoints(const utils::JsonArray& arr);
    /** Fill JSON array with gradient points. */
    static utils::JsonArray SetGradientPoints(const GradientPoints& arr);
    /** Fill decals filenames with values from JSON array */
    static std::vector<std::string> GetDecals(const utils::JsonArray& arr);
    /** Fill JSON array with decals filenames. */
    static utils::JsonArray SetDecals(const std::vector<std::string>& arr);

    /** Save height map. */
    bool height_map_enabled = false;
    /** Save images for all chunks. */
    bool chunks_enabled = false;
    /** Minimap settings. */
    struct {
        /** Save minimap or not. */
        bool enabled = false;
        /** Size of each chunk in pixels in minimap. */
        int  tile_size = 4;
        /** Image scaling with clear edges. When @c true scaling is fast, but
        result will have sharp pixel borders. When @c false result will be
        smooth, but that requiers more time to process. */
        bool clear_edges = true;
    } minimap;
    /** Normal map settings. */
    struct {
        /** Save normal maps for all textures or not. */
        bool  enabled = false;
        /** Normal map strength. [0.0, 1.0] */
        float strength = 0.5f;
        /** Invert vectors directions on normal maps. */
        bool  invert = false;
    } normals;
    /** Texture splatting settings. */
    struct {
        /** Depth of texture splatting. That specifies color transformations
        on texture overflow. Lighter colors have more influence to result
        color. [0.0, 1.0] */
        float depth = 0.2f;
        /** Width of band where colors will be splatted according to chunk
        texture size. [0.0, 1.0] */
        float bandwidth = 0.5f;
        /** Addition of random noise when splatting colors. [0.0, 1.0] */
        float randomness = 0.0f;
    } splatting;
    /** Gradient settings. */
    struct {
        /** Add gradient or not. */
        bool  enabled = false;
        /** Gradient's opacity [0.0, 1.0]. */
        float opacity = 0.0f;
        /** Add gradient only to minimap or to chunks too. */
        bool  only_minimap = true;
        /** List of gradient key points. */
        GradientPoints points = { };
    } gradient;
    /** Minimap shadow settings. */
    struct {
        /** Add shadow or not. */
        bool  enabled = false;
        /** Shadow strength [0.0, 1.0]. */
        float strength = 0.0f;
        /** Angle from that light point will shine. [0, 360]. */
        int   angle = 0;
    } shadow;
    /** Output images bitdepth. */
    int target_bitdepth = 24;
    /** Input images filenames. */
    struct {
        /** Base textures filenames. */
        struct {
            /** Mountain biome texture. */
            std::string mountain = "";
            /** Basis biome texture. */
            std::string basis = "";
            /** Sea biome texture. */
            std::string sea = "";
            /** River biome texture. */
            std::string river = "";
            /** Beach biome texture. */
            std::string beach = "";
        } bases;
        /** Decals that can be added to textures filenames. */
        struct {
            /** Add decals or not. */
            bool  enabled = false;
            /** Chance for decals addition. Selection of single decal from set
            is uniform. */
            float chance = 0.0f;
            /** Mountain biome decals. */
            std::vector<std::string> mountain = { };
            /** Basis biome decals. */
            std::vector<std::string> basis = { };
            /** Sea biome decals. */
            std::vector<std::string> sea = { };
            /** River biome decals. */
            std::vector<std::string> river = { };
            /** Beach biome decals. */
            std::vector<std::string> beach = { };
        } decals;
    } images;
};

/** @brief Module for images creation.

Creates minimap, chunk textures, normal maps and height map. */
class TextureModule : public IModule {
 public:
    /** @copydoc IModule::Deinit. */
    virtual void Deinit();
    /** @copydoc IModule::SetStorage */
    void SetStorage(Storage* storage) override;
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededData */
    std::list<std::string> GetNeededData() const override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Read input images according to selected filenames.
    @return @c true when all images are loaded, @c false otherwise. */
    virtual bool ReadReferenceTextures();

    /** Init bands for splatting textures according to biomes. */
    virtual void InitAlphaBands();

    /** Init gradient. */
    virtual void InitGradient();

    /** Get height of pixel from selected chunk.
    @param [in] ch_x - Chunk X coordinate.
    @param [in] ch_x - Chunk Y coordinate.
    @param [in] ch_x - Pixel position in selected chunk by X coordinate.
    @param [in] ch_x - Pixel position in selected chunk by Y coordinate.
    @return height of pixel in range [0.0 1.0] from heightmap. */
    virtual float GetPixelHeight(int ch_x, int ch_y, int u, int v) const;

    /** Get biome according to height value.
    @param [in] height - Height in range [0.0, 1.0] for finding biome.
    @return Biome that is placed on selected height. */
    virtual Biome DetectBiome(float height) const;

    /** Process chunks rows in one thread.
    @param [in] line_beg - First row Y coord that will be processed.
    @param [in] line_end - First row Y coord that willn't be processed. */
    virtual void ProcessTiles(int line_beg, int line_end);

    /** Blend image for selected chunk.
    @param [out] img - Output image.
    @param [in] ch_x - chunk X coordinate.
    @param [in] ch_y - chunk Y coordinate. */
    virtual void TextureSplatting(utils::Image& img, int ch_x, int ch_y);

    /** Optionally add decal to texture.
    @param [in] biome        - Selected chunk's biome.
    @param [in, out] texture - Texture to place decal.
    @param [in, out] rand    - Random number generator. */
    virtual void AddDecal(Biome biome,
                          utils::Image& texture,
                          utils::Random& rand);

    /** Add gradient to texture.
    @param [in, out] texture - Texture to add gradient.
    @param [in] x            - chunk X coordinate.
    @param [in] y            - chunk Y coordinate. */
    virtual void AddGradient(utils::Image& texture, int x, int y) const;

    /** Add gradient to texture.
    @param [in, out] texture - Texture to add shadow.
    @param [in] x            - chunk X coordinate.
    @param [in] y            - chunk Y coordinate. */
    virtual void AddShadow(utils::Image& texture, int x, int y);
    
    /** Draw chunk on minimap.
    @param [in] img   - Texture to draw.
    @param [in] scale - Scale down coeficient.
    @param [in] x     - chunk X coordinate.
    @param [in] y     - chunk Y coordinate. */
    virtual void DrawOnMinimap(utils::Image img, int scale, int x, int y);

    /** Prepare masks for blending.
    @param [in, out] height_sub_mask   - Height mask.
    @param [in, out] river_sub_mask    - River mask.
    @param [in, out] mountain_sub_mask - Mountain mask.
    @param [in] chunk_x                - chunk X coordinate.
    @param [in] chunk_y                - chunk Y coordinate.
    @param [in] resolution             - resolution of each map. */
    virtual void PrepareSplattingMasks(
            utils::Array2D<float>& height_sub_mask,
            utils::Array2D<float>& river_sub_mask,
            utils::Array2D<float>& mountain_sub_mask,
            int chunk_x,
            int chunk_y,
            int resolution) const;

    /** Mix pixel colors given from textures according to height.
    @param [in] heights    - Reference textures heights in current pixel.
    @param [in] depth      - Splatting depth.
    @param [in] max_height - Summary height of all alpha bands.
    @param [in] u          - Pixel position in selected chunk by U coord.
    @param [in] v          - Pixel position in selected chunk by V coord.
    @return Result pixel after mixing. */
    virtual utils::RgbaPixel MixPixelByAlphaBands(
            const std::vector<float>& heights,
            float depth,
            float max_height,
            int u,
            int v) const;

    /** Add pixel if it's biome is not sea.
    @param [in] pixel     - Pixel at bottom.
    @param [in] overlay   - Pixel added to top.
    @param [in] height    - Height of current pixel on height map.
    @param [in] coef      - Addition coeficient.
    @param [in, out] rand - Random number generator.
    @return Result pixel after mixing. */
    virtual utils::RgbaPixel OverlayExceptSea(const utils::RgbaPixel& pixel,
                                              const utils::RgbaPixel& overlay,
                                              float height,
                                              float coef,
                                              utils::Random& rand) const;

    /** Save height map to file. */
    virtual void SaveHeightMap();

    /** Save chunk to file.
    @param [in] texture - Chunk texture to save.
    @param [in] x       - chunk X coordinate.
    @param [in] y       - chunk Y coordinate. */
    virtual void SaveChunk(const utils::Image& texture, int x, int y) const;

    /** Scale image up.
    @param [in] texture - Texture to scale.
    @param [in] n       - Scale up times. */
    static void ScaleUpImage(utils::Image& texture, int n);

    /** Scale image down.
    @param [in] texture - Texture to scale.
    @param [in] n       - Scale down times.
    @param [in] sharp   - Use shrp edles or smooth. */
    static void ScaleDownImage(utils::Image& texture, int n, bool sharp);

    /** Get pixel's self height according to it's luminance.
    @param [in] pixel - Pixel for height detection.
    @return Height in range [0, 255]. */
    static int GetHeight(const utils::RgbaPixel& pixel);

    /** Mix values at selected proportion.
    @param [in] v1     - Value 1.
    @param [in] v2     - Value 2.
    @param [in] dist_1 - Distance from current value to value 1.
    @param [in] dist_2 - Distance from current value to value 2.
    @return Mixed value. */
    static uint8_t MixValue(uint8_t v1, uint8_t v2, int dist_1, int dist_2);

    /** Scale image part to a single pixel.
    @param [in] texture - Texture to scale.
    @param [in] x       - Pixel X coordinate.
    @param [in] y       - Pixel Y coordinate.
    @param [in] n       - Scale down times. */
    static utils::RgbaPixel ScaleDownPixel(const utils::Image& texture,
                                           int x,
                                           int y,
                                           int n);

    /** Alpha blend images.
    @param [in] image_back  - Back image.
    @param [in] image_front - Front image.
    @param [out] image_out  - Output image. */
    static void AlphaBlendImage(const utils::Image& image_back,
                                const utils::Image& image_front,
                                utils::Image& image_out);

    /** Alpha blend pixels.
    @param [in] image_back  - Back pixel.
    @param [in] image_front - Front pixel.
    @return image_out       - Output pixel. */
    static utils::RgbaPixel AlphaBlendPixel(const utils::RgbaPixel& bot,
                                            const utils::RgbaPixel& top);

    /** Calculate color component for alpha blending.
    @param [in] bot_c - Bottom pixel component.
    @param [in] bot_a - Bottom pixel alpha.
    @param [in] top_c - Top pixel component.
    @param [in] top_a - Top pixel alpha.
    @param [in] res_a - Result pixel alpha.
    @return Blended component value. */
    static uint8_t CalcComponent(int bot_c,
                                 int bot_a,
                                 int top_c,
                                 int top_a,
                                 int res_a);

    /** Create incline for alpha band.
    @param [in] range      - Range of incline and it's biome.
    @param [in] border     - Incline border.
    @param [in] decreasing - Incline is decreasing or not.
    @param [in] bandwidth  - Splatting badwidth.
    @param [in] mid        - Middle of incline.
    @param [in, out] band  - Alpha band to draw. */
    static void AlphaIncline(const std::tuple<Biome, int, int>& range,
                             int border,
                             bool decreasing,
                             float bandwidth,
                             int mid,
                             std::vector<float>& band);

    /** Create normal map according to image.
    @param [in] img     - image for normal map creation.
    @param [in] invert  - Invert normals or not.
    @param [in] coef    - Normal map coeficient.
    @param [out] normal - result normal map. */
    static void CreateNormal(const utils::Image& img,
                             bool invert,
                             float coef,
                             utils::Image& normal);


    /** Gradient pixel table. */
    std::vector<utils::RgbaPixel>           grad_table_;
    /** Alpha bands. */
    std::vector<std::vector<float> >        alpha_bands_;
    /** Reference textures. */
    std::vector<utils::Image>               reference_textures_;
    /** Reference textures heights. */
    std::vector<utils::Array2D<float> >     reference_textures_heights_;
    /** Reference decals. */
    std::vector<std::vector<utils::Image> > reference_decals_;
    /** Minimap image. */
    utils::Image                            minimap_;

    /** Height map from data storage. */
    utils::Array2D<float>* height_map_ = nullptr;
    /** River mask from data storage. */
    utils::Array2D<float>* river_mask_ = nullptr;
    /** Sea level from data storage. */
    float*                 sea_level_ = nullptr;
    /** Beach level from data storage. */
    float*                 beach_level_ = nullptr;
    /** Mountain mask from data storage. */
    utils::Array2D<float>* mountain_mask_ = nullptr;
    /** Image input/output worker from data storage. */
    utils::ImageIO*        image_io_ = nullptr;

    /** Settings for module. */
    struct {
        /** Basis settings. */
        BasisSettings   basis;
        /** General settings. */
        GeneralSettings general;
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

#endif // PROGOGENE_CORE_TEXTURE_H_
