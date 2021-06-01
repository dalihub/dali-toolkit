#ifndef DALI_TOOLKIT_TEXT_TYPESETTER_H
#define DALI_TOOLKIT_TEXT_TYPESETTER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
class ModelInterface;
class ViewModel;
class Typesetter;

typedef IntrusivePtr<Typesetter> TypesetterPtr;

/**
 * @brief This class is responsible of controlling the data flow of the text's rendering process.
 */
class Typesetter : public RefObject
{
public:
  /**
   * @brief Behaviours of how to render the text.
   */
  enum RenderBehaviour
  {
    RENDER_TEXT_AND_STYLES, ///< Render both the text and its styles
    RENDER_NO_TEXT,         ///< Do not render the text itself
    RENDER_NO_STYLES,       ///< Do not render any styles
    RENDER_MASK             ///< Render an alpha mask (for color glyphs with no color animation, e.g. emoji)
  };

  /**
   * @brief Styles of the text.
   */
  enum Style
  {
    STYLE_NONE,        ///< No style
    STYLE_MASK,        ///< Alpha mask
    STYLE_SHADOW,      ///< Hard shadow
    STYLE_SOFT_SHADOW, ///< Soft shadow
    STYLE_UNDERLINE,   ///< Underline
    STYLE_OUTLINE,     ///< Outline
    STYLE_BACKGROUND   ///< Text background
  };

public: // Constructor.
  /**
   * @brief Creates a Typesetter instance.
   *
   * The typesetter composes the final text retrieving the glyphs and the
   * styles from the text's model.
   *
   * @param[in] model Pointer to the text's data model.
   */
  static TypesetterPtr New(const ModelInterface* const model);

public:
  /**
   * @brief Retrieves the pointer to the view model.
   *
   * @return A pointer to the view model.
   */
  ViewModel* GetViewModel();

  /**
   * @brief Renders the text.
   *
   * Does the following operations:
   * - Finds the visible pages needed to be rendered.
   * - Elide glyphs if needed.
   * - Creates image buffers for diffrent text styles with the given size.
   * - Combines different image buffers to create the pixel data used to generate the final image
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   *
   * @return A pixel data with the text rendered.
   */
  PixelData Render(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour = RENDER_TEXT_AND_STYLES, bool ignoreHorizontalAlignment = false, Pixel::Format pixelFormat = Pixel::RGBA8888);

private:
  /**
   * @brief Private constructor.
   *
   * @param[in] model Pointer to the text's data model.
   */
  Typesetter(const ModelInterface* const model);

  // Declared private and left undefined to avoid copies.
  Typesetter(const Typesetter& handle);

  // Declared private and left undefined to avoid copies.
  Typesetter& operator=(const Typesetter& handle);

  /**
   * @brief Create the image buffer for the given range of the glyphs in the given style.
   *
   * Does the following operations:
   * - Retrieves the data buffers from the text model.
   * - Creates the pixel data used to generate the final image with the given size.
   * - Traverse the visible glyphs, retrieve their bitmaps and compose the final pixel data.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] style The style of the text.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   * @param[in] fromGlyphIndex The index of the first glyph within the text to be drawn
   * @param[in] toGlyphIndex The index of the last glyph within the text to be drawn
   *
   * @return An image buffer with the text.
   */
  Devel::PixelBuffer CreateImageBuffer(const unsigned int bufferWidth, const unsigned int bufferHeight, Typesetter::Style style, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat, int horizontalOffset, int verticalOffset, TextAbstraction::GlyphIndex fromGlyphIndex, TextAbstraction::GlyphIndex toGlyphIndex);

  /**
   * @brief Combine the two RGBA image buffers together.
   *
   * The top layer buffer will blend over the bottom layer buffer:
   * - If the pixel is not fully opaque from either buffer, it will be blended with
   *   the pixel from the other buffer and copied to the combined buffer.
   * - If the pixels from both buffers are fully opaque, the pixels from the top layer
   *   buffer will be copied to the combined buffer.
   *
   * @param[in] topPixelBuffer The top layer buffer.
   * @param[in] bottomPixelBuffer The bottom layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   *
   * @return The combined image buffer with the text.
   *
   */
  Devel::PixelBuffer CombineImageBuffer(Devel::PixelBuffer topPixelBuffer, Devel::PixelBuffer bottomPixelBuffer, const unsigned int bufferWidth, const unsigned int bufferHeightbool);

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * Destroys the visual model.
   */
  virtual ~Typesetter();

private:
  ViewModel* mModel;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_TYPESETTER_H
