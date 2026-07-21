#ifndef DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_IMPL_H
#define DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_IMPL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <cstdint>
#include <string>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/slot-delegate.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>

namespace Dali
{
namespace Toolkit
{
class AtlasGlyphManager;

} // namespace Toolkit

namespace Toolkit
{
namespace Internal
{
class AtlasGlyphManager;
typedef IntrusivePtr<AtlasGlyphManager> AtlasGlyphManagerPtr;

class AtlasGlyphManager : public Dali::BaseObject
{
public:
  struct GlyphRecordEntry
  {
    Text::GlyphIndex mIndex;
    uint32_t         mImageId;
    int32_t          mCount;
    uint16_t         mOutlineWidth;
    bool             isItalic : 1;
    bool             isBold : 1;
  };

  struct FontGlyphRecord
  {
    Text::FontId             mFontId{0u};
    uint64_t                 mGeneration{0u};
    Vector<GlyphRecordEntry> mGlyphRecords;
  };

  /**
   * @brief Constructor
   */
  AtlasGlyphManager();

  /**
   * @copydoc Toolkit::AtlasGlyphManager::Add
   */
  void Add(const Text::GlyphInfo&                        glyph,
           const Toolkit::AtlasGlyphManager::GlyphStyle& style,
           const PixelData&                              bitmap,
           Dali::Toolkit::AtlasManager::AtlasSlot&       slot);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::GenerateMeshData
   */
  void GenerateMeshData(uint32_t                       imageId,
                        const Vector2&                 position,
                        Toolkit::AtlasManager::Mesh2D& mesh);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::IsCached
   */
  bool IsCached(Text::FontId                                  fontId,
                Text::GlyphIndex                              index,
                const Toolkit::AtlasGlyphManager::GlyphStyle& style,
                Dali::Toolkit::AtlasManager::AtlasSlot&       slot);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::GetAtlasSize
   */
  Vector2 GetAtlasSize(uint32_t atlasId);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::SetNewAtlasSize
   */
  void SetNewAtlasSize(uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::GetPixelFormat
   */
  Pixel::Format GetPixelFormat(uint32_t atlasId);

  /**
   * @brief Adjusts the reference count of an atlas glyph image.
   *
   * The font ID limits the lookup to matching font records, while the image ID
   * identifies the exact atlas image across cache generations. When the
   * reference count reaches zero, the image and glyph record are removed.
   *
   * @param[in] fontId The font identifier used to narrow the lookup.
   * @param[in] imageId The exact atlas image identifier.
   * @param[in] delta The value added to the current reference count.
   */
  void AdjustReferenceCount(Text::FontId fontId, uint32_t imageId, int32_t delta);

  /**
   * @copydoc Toolkit::AtlasGlyphManager::GetTextures
   */
  TextureSet GetTextures(uint32_t atlasId) const;

  /**
   * @copydoc Toolkit::AtlasGlyphManager::GetMetrics
   */
  const Toolkit::AtlasGlyphManager::Metrics& GetMetrics();

  /**
   * @brief Invalidates cached glyph lookup identities.
   *
   * Existing atlas images remain alive while referenced by active renderers.
   * Subsequent glyph lookups use a new cache generation and therefore do not
   * reuse records created before this call.
   */
  void InvalidateGlyphCache();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~AtlasGlyphManager();

public:
  /**
   * @brief Ensures locale changed signal is connected.
   *
   * Attempts to connect to Adaptor's LocaleChangedSignal if not already connected
   * and if Adaptor is available.
   */
  void EnsureLocaleChangedConnection();

private:
  /**
   * @brief Callback for locale change event.
   *
   * @param[in] locale The new locale string (unused, only generation is invalidated).
   */
  void OnLocaleChanged(std::string locale);

private:
  Dali::Toolkit::AtlasManager         mAtlasManager;
  std::vector<FontGlyphRecord>        mFontGlyphRecords;
  Toolkit::AtlasGlyphManager::Metrics mMetrics;
  Sampler                             mSampler;
  uint64_t                            mCacheGeneration{1u};
  SlotDelegate<AtlasGlyphManager>     mSlotDelegate;
  bool                                mLocaleChangedConnected{false};
};

} // namespace Internal

inline const Internal::AtlasGlyphManager& GetImplementation(const Toolkit::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  const BaseObject& handle = manager.GetBaseObject();

  return static_cast<const Internal::AtlasGlyphManager&>(handle);
}

inline Internal::AtlasGlyphManager& GetImplementation(Toolkit::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  BaseObject& handle = manager.GetBaseObject();

  return static_cast<Internal::AtlasGlyphManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ATLAS_GLYPH_MANAGER_IMPL_H
