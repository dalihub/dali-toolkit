#ifndef DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_H
#define DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/atlas/atlas-manager.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class AtlasGlyphManager;
}

class AtlasGlyphManager : public BaseHandle
{
public:
  /**
   * Description of GlyphManager state
   */
  struct Metrics
  {
    Metrics()
    : mGlyphCount(0u)
    {
    }

    ~Metrics()
    {
    }

    uint32_t              mGlyphCount;         ///< number of glyphs being managed
    std::string           mVerboseGlyphCounts; ///< a verbose list of the glyphs + ref counts
    AtlasManager::Metrics mAtlasMetrics;       ///< metrics from the Atlas Manager
  };

  struct GlyphStyle
  {
    GlyphStyle()
    : outline{0u},
      isItalic{false},
      isBold{false}
    {
    }

    uint16_t outline;      ///< The outline width of this glyph
    bool     isItalic : 1; ///< Whether the glyph is italic.
    bool     isBold : 1;   ///< Whether the glyph is bold.
  };

  /**
   * @brief Create a AtlasGlyphManager handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  AtlasGlyphManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AtlasGlyphManager();

  /**
   * @brief Create or retrieve AtlasGlyphManager singleton.
   *
   * @return A handle to the AtlasGlyphManager control.
   */
  static AtlasGlyphManager Get();

  /**
   * @brief Ask Atlas Manager to add a glyph
   *
   * @param[in] glyph glyph to add to an atlas
   * @param[in] style The style of this glyph
   * @param[in] bitmap bitmap to use for glyph addition
   * @param[out] slot information returned by atlas manager for addition
   */
  void Add(const Text::GlyphInfo&   glyph,
           const GlyphStyle&        style,
           const PixelData&         bitmap,
           AtlasManager::AtlasSlot& slot);

  /**
   * @brief Generate mesh data for an image contained in an atlas
   *
   * @param[in] imageId ID of image to generate geometry for
   * @param[in] position top left of image
   * @param[out] meshData generated MeshData
   */
  void GenerateMeshData(uint32_t                       imageId,
                        const Vector2&                 position,
                        Toolkit::AtlasManager::Mesh2D& mesh);

  /**
   * @brief Check to see if a glyph is being cached
   *
   * @param[in] fontId The font that this glyph comes from
   * @param[in] index The GlyphIndex of this glyph
   * @param[in] style The style of this glyph
   * @param[out] slot container holding information about the glyph( mImage = 0 indicates not being cached )
   *
   * @return Whether glyph is cached or not ?
   */
  bool IsCached(Text::FontId             fontId,
                Text::GlyphIndex         index,
                const GlyphStyle&        style,
                AtlasManager::AtlasSlot& slot);

  /**
   * @brief Retrieve the size of an atlas
   *
   * @param[in] atlasId Id of the atlas to interrogate
   *
   * @return The pixel size of the atlas
   */
  Vector2 GetAtlasSize(uint32_t atlasId);

  /**
    * @brief Set the atlas size and block size for subsequent Atlas generation
    *
    * @param[in] width width of atlas in pixels
    * @param[in] height height of atlas in pixels
    * @param[in] blockWidth width of a block in pixels
    * @param[in] blockHeight height of a block in pixels
    */
  void SetNewAtlasSize(uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight);

  /**
   * @brief Get the Pixel Format used by an atlas
   *
   * @param[in] atlasId Id of atlas to check
   *
   * @return The pixel format of the atlas
   */
  Pixel::Format GetPixelFormat(uint32_t atlasId);

  /**
   * @brief Get the texture set used by an atlas
   *
   * @param[in] atlasId Id of an atlas
   *
   * @return The texture set used by the atlas
   */
  TextureSet GetTextures(uint32_t atlasId) const;

  /**
   * @brief Get Glyph Manager metrics
   *
   * @return const reference to glyph manager metrics
   */
  const Metrics& GetMetrics();

  /**
   * @brief Adjust the reference count for glyph
   *
   * @param[in] fontId The font this image came from
   * @param[in] index The index of the glyph
   * @param[in] style The style of this glyph
   * @param[in] delta The adjustment to make to the reference count
   */
  void AdjustReferenceCount(Text::FontId fontId, Text::GlyphIndex index, const GlyphStyle& style, int32_t delta);

public:
  // Default copy and move operator
  AtlasGlyphManager(const AtlasGlyphManager& rhs) = default;
  AtlasGlyphManager(AtlasGlyphManager&& rhs)      = default;
  AtlasGlyphManager& operator=(const AtlasGlyphManager& rhs) = default;
  AtlasGlyphManager& operator=(AtlasGlyphManager&& rhs) = default;

private:
  explicit DALI_INTERNAL AtlasGlyphManager(Internal::AtlasGlyphManager* impl);
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_H
