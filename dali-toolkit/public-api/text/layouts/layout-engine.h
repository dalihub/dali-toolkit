#ifndef __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__
#define __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDE
#include <dali-toolkit/public-api/text/text-definitions.h>

// EXTERNAL INCLUDE
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

struct Vector2;

namespace Toolkit
{

namespace Text
{

class VisualModel;

/**
 * @brief LayoutEngine is responsible for calculating the visual position of glyphs in layout.
 */
class LayoutEngine
{
public:

  enum Layout
  {
    SINGLE_LINE_BOX,
    MULTI_LINE_BOX
  };

  /**
   * @brief Create a new instance of a LayoutEngine.
   */
  LayoutEngine();

  /**
   * @brief Virtual destructor.
   */
  ~LayoutEngine();

  /**
   * @brief Choose the required layout.
   *
   * @param[in] layout The required layout.
   */
  void SetLayout( Layout layout );

  /**
   * @brief Store the visual position of glyphs in the VisualModel.
   *
   * @param[in] boundingBox The size of the box containing the text.
   * @param[in] glyphs A vector with glyphs.
   * @param[in] characterIndices Vector with indices pointing the first character of each glyph.
   * @param[in] charactersPerGlyph Vector with the number of characters that forms each glyph.
   * @param[in] visualModel The visual model to update.
   */
  void UpdateVisualModel( const Vector2& boundingBox,
                          const Vector<GlyphInfo>& glyphs,
                          const Vector<CharacterIndex>& characterIndices,
                          const Vector<Length>& charactersPerGlyph,
                          VisualModel& visualModel );

private:

  // Undefined
  LayoutEngine( const LayoutEngine& handle );

  // Undefined
  LayoutEngine& operator=( const LayoutEngine& handle );

private:

  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__
