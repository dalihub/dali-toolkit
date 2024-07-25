#ifndef DALI_TOOLKIT_TEXT_TYPESETTER_H
#define DALI_TOOLKIT_TEXT_TYPESETTER_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/devel-api/text-abstraction/font-client.h>
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
    RENDER_NO_TEXT,         ///< Do not render the text itself but render the background styles such as outline and background.
    RENDER_NO_STYLES,       ///< Do not render any styles
    RENDER_MASK,            ///< Render an alpha mask (for color glyphs with no color animation, e.g. emoji)
    RENDER_OVERLAY_STYLE    ///< Do not render the text itself but render the style but overlay the style on the text (foreground styles such as strikethrough and underline)
  };

  /**
   * @brief Styles of the text.
   */
  enum Style
  {
    STYLE_NONE,         ///< No style
    STYLE_MASK,         ///< Alpha mask
    STYLE_SHADOW,       ///< Hard shadow
    STYLE_SOFT_SHADOW,  ///< Soft shadow
    STYLE_UNDERLINE,    ///< Underline
    STYLE_OUTLINE,      ///< Outline
    STYLE_BACKGROUND,   ///< Text background
    STYLE_STRIKETHROUGH ///< Strikethrough
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
   * @brief Set the font client.
   *
   * Set the font client used in the update/render process of the text model.
   *
   * @param[in] fontClient The font client used by the Typesetter.
   */
  void SetFontClient(TextAbstraction::FontClient& fontClient);

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

  /**
   * @brief After the Render, use the pixel information of the given cutoutBuffer to make the part where the pixel is drawn transparent.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] cutoutBuffer The buffer to use pixel information to cutout.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   * @param[in] originAlpha The original alpha of text.
   *
   * @return A pixel data with the text rendered.
   */
  PixelData RenderWithCutout(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, Devel::PixelBuffer cutoutBuffer, RenderBehaviour behaviour = RENDER_TEXT_AND_STYLES, bool ignoreHorizontalAlignment = false, Pixel::Format pixelFormat = Pixel::RGBA8888, float originAlpha = 1.f);

  /**
   * @brief Renders the text, return as Devel::PixelBuffer.
   *
   * This function is used to obtain the PixelBuffer required for cutout.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] cutoutBuffer The buffer to use pixel information to cutout.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   *
   * @return A pixel data with the text rendered.
   */
  Devel::PixelBuffer RenderWithPixelBuffer(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour = RENDER_TEXT_AND_STYLES, bool ignoreHorizontalAlignment = false, Pixel::Format pixelFormat = Pixel::RGBA8888);

  /**
   * @brief Create & draw the image buffer of single background color.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] backgroundColor The backgroundColor of image buffer.
   *
   * @return An image buffer with the text.
   */
  Devel::PixelBuffer CreateFullBackgroundBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Vector4& backgroundColor);

  /**
   * @brief Set Mask for two pixel buffer.
   *
   * The alpha value of bottomPixelBuffer is decreased as the alpha value of topPixelBuffer is higher.
   *
   * @param[in, out] topPixelBuffer The top layer buffer.
   * @param[in, out] bottomPixelBuffer The bottom layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] originAlpha The original alpha value of the text.
   */
  void SetMaskForImageBuffer(Devel::PixelBuffer& __restrict__ topPixelBuffer, Devel::PixelBuffer& __restrict__ bottomPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, float originAlpha);

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
   * @brief Create & draw the image buffer for the given range of the glyphs in the given style.
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
  Devel::PixelBuffer CreateImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Typesetter::Style style, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset, const TextAbstraction::GlyphIndex fromGlyphIndex, const TextAbstraction::GlyphIndex toGlyphIndex);

  /**
   * @brief Apply markup underline tags.
   *
   * The properties on TextLabel override the behavior of Markup.
   * Because the markup will be the bottom layer buffer
   *  - i.e: If you set property UNDERLINE to enabled and blue.
   *    And the TEXT is "<color value='green'>Hello</color> <u>World</u> <i>Hello</i> <b>World</b>".
   *    Then the output of the whole text is underlined by blue line.
   *
   * @param[in] topPixelBuffer The top layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   *
   * @return The image buffer with the markup.
   */
  Devel::PixelBuffer ApplyUnderlineMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset);

  /**
   * @brief Apply markup strikethrough tags.
   *
   * The properties on TextLabel override the behavior of Markup.
   * Because the markup will be the bottom layer buffer
   *  - i.e: If you set property STRIKETHROUGH to enabled and blue.
   *    And the TEXT is "<color value='green'>Hello</color> <s>World</s> <i>Hello</i> <b>World</b>".
   *    Then the whole text will have a blue line strikethrough.
   *
   * @param[in] topPixelBuffer The top layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   *
   * @return The image buffer with the markup.
   */
  Devel::PixelBuffer ApplyStrikethroughMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset);

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * Destroys the visual model.
   */
  virtual ~Typesetter();

private:
  ViewModel*                  mModel;
  TextAbstraction::FontClient mFontClient;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_TYPESETTER_H
