#ifndef __DALI_TOOLKIT_TEXT_VIEW_H__
#define __DALI_TOOLKIT_TEXT_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-view-interface.h>
#include <dali-toolkit/public-api/text/visual-model.h>

namespace Dali
{

struct Vector2;

namespace Toolkit
{

namespace Text
{

/**
 * @brief View provides an interface between the Text layout engine and rendering back-end.
 */
class View : public ViewInterface
{
public:

  /**
   * @brief Create a new instance of a View.
   */
  View();

  /**
   * @brief Virtual destructor.
   */
  virtual ~View();

  /**
   * @brief Set the visual model.
   *
   * @param[in] visualModel The visual model used by the View.
   */
  void SetVisualModel( VisualModelPtr visualModel );

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetNumberOfGlyphs()
   */
  virtual Length GetNumberOfGlyphs() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetGlyphs()
   */
  virtual void GetGlyphs( GlyphInfo* glyphs,
                          GlyphIndex glyphIndex,
                          Length numberOfGlyphs ) const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetGlyphPositions()
   */
  virtual void GetGlyphPositions( Vector2* glyphPositions,
                                  GlyphIndex glyphIndex,
                                  Length numberOfGlyphs ) const;

private:

  // Undefined
  View( const View& handle );

  // Undefined
  View& operator=( const View& handle );

private:

  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VIEW_H__
