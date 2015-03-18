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

// EXTERNAL INCLUDE
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/line-run.h>

namespace Dali
{

struct Vector2;

namespace Toolkit
{

namespace Text
{

struct LayoutParameters;

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

  enum Alignment
  {
    ALIGN_BEGIN,
    ALIGN_CENTER,
    ALIGN_END
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
   * @brief Query the required layout.
   *
   * @return The required layout.
   */
  unsigned int GetLayout() const;

  /**
   * @brief Choose the required line alignment.
   *
   * @param[in] alignment The required alignment.
   */
  void SetAlignment( Alignment alignment );

  /**
   * @brief Query the required line alignment.
   *
   * @return The required alignment.
   */
  Alignment GetAlignment() const;

  /**
   * @brief Store the visual position of glyphs in the VisualModel.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[out] glyphPositions The positions of all the glyphs.
   * @param[out] lines The laid-out lines.
   * @param[out] actualSize The size of the text after it has been laid-out.
   *
   * @return \e true if the text has been re-laid-out. \e false means the given width is too small to layout even a single character.
   */
  bool LayoutText( const LayoutParameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& actualSize );

  /**
   * @brief Re-lays out those lines with right to left characters.
   *
   * It doesn't change the phisical position of the glyphs in the model but sets their new position.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in,out] glyphPositions The positions of all the glyphs.
   */
  void ReLayoutRightToLeftLines( const LayoutParameters& layoutParameters,
                                 Vector<Vector2>& glyphPositions );

  /**
   * @brief Aligns the laid out lines.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] lines The laid-out lines.
   * @param[in,out] glyphPositions The positions of all the glyphs.
   */
  void Align( const LayoutParameters& layoutParameters,
              const Vector<LineRun>& lines,
              Vector<Vector2>& glyphPositions );

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
