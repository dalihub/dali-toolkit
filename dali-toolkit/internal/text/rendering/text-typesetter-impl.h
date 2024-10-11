#ifndef DALI_TOOLKIT_TEXT_TYPESETTER_IMPL_H
#define DALI_TOOLKIT_TEXT_TYPESETTER_IMPL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/ref-object.h>
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
class ModelInterface;
class ViewModel;

/**
 * @brief This class is seperated logics for TypeSetter.
 * It will reduce the complexicy of typesetter logic.
 */
struct Typesetter::Impl
{
public:
  /**
   * @brief Create an initialized image buffer filled with transparent color.
   *
   * Creates the pixel data used to generate the final image with the given size.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
   *
   * @return An image buffer.
   */
  static Devel::PixelBuffer CreateTransparentImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Pixel::Format pixelFormat);

public: // Constructor & Destructor
  /**
   * @brief Creates a Typesetter impl instance.
   */
  Impl(const ModelInterface* const model);

  ~Impl();

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
   * @brief Get the font client.
   *
   * @return The font client used by the Typesetter.
   */
  TextAbstraction::FontClient& GetFontClient();

public: // Image buffer creation
  void DrawGlyphsBackground(Devel::PixelBuffer& buffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const int32_t horizontalOffset, const int32_t verticalOffset);

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

private:
  std::unique_ptr<ViewModel>  mModel;
  TextAbstraction::FontClient mFontClient;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_TYPESETTER_IMPL_H
