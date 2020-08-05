#ifndef DALI_TOOLKIT_TEXT_UTILS_DEVEL_H
#define DALI_TOOLKIT_TEXT_UTILS_DEVEL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-array.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelText
{

/**
 * @brief Struct with the text and style parameters to be rendered into a pixel buffer.
 */
struct DALI_TOOLKIT_API RendererParameters
{
  RendererParameters()
  : text{},
    horizontalAlignment{ "begin" },
    verticalAlignment{ "top" },
    fontFamily{},
    fontWeight{},
    fontWidth{},
    fontSlant{},
    layout{ "singleLine" },
    circularAlignment{ "begin" },
    textColor{ Color::WHITE },
    fontSize{ 0.f },
    textWidth{ 0u },
    textHeight{ 0u },
    radius{ 0u },
    beginAngle{ 0.f },
    incrementAngle{ 0.f },
    ellipsisEnabled{ true },
    markupEnabled{ false },
    isTextColorSet{ false },
    minLineSize{ 0.f }
  {}

  std::string text;                ///< The text to be rendered encoded in utf8.

  std::string horizontalAlignment; ///< The horizontal alignment: one of {"begin", "center", "end"}.
  std::string verticalAlignment;   ///< The vertical alignment: one of {"top", "center", "bottom"}.

  std::string fontFamily;          ///< The font's family.
  std::string fontWeight;          ///< The font's weight: one of {"thin", "ultraLight", "extraLight", "light", "demiLight", "semiLight", "book", "normal", "regular", "medium", "demiBold", "semiBold", "bold", "ultraBold", "extraBold", "black", "heavy", "extraBlack"}.
  std::string fontWidth;           ///< The font's width: one of {"ultraCondensed", "extraCondensed", "condensed", "semiCondensed", "normal", "semiExpanded", "expanded", "extraExpanded", "ultraExpanded"}.
  std::string fontSlant;           ///< The font's slant. one of {"normal", "roman", "italic", "oblique"}
  std::string layout;              ///< The type of layout: one of {"singleLine", "multiLine", "circular"}
  std::string circularAlignment;   ///< The text alignment within the arc: one of {"begin", "center", "end"}. The @p horizontalAlignment and @p verticalAlignment can be used to align the text within the text area.

  Vector4 textColor;               ///< The default text's color. Default is white.

  float fontSize;           ///< The font's size (in points).

  unsigned int textWidth;          ///< The width in pixels of the boundaries where the text is going to be laid-out.
  unsigned int textHeight;         ///< The height in pixels of the boundaries where the text is going to be laid-out.

  unsigned int radius;             ///< The radius in pixels of the circular text.
  float beginAngle;                ///< The begin angle in degrees of the text area on the circle. The top of the circle is 0°, the right side 90°, the bottom 180° and the left 270°.
  float incrementAngle;            ///< The increment angle in degrees of the text area on the circle. The @p incrementAngle defines a direction. If positive, the text will be laid out clockwise.

  bool ellipsisEnabled:1;          ///< Whether the ellipsis layout option is enabled.
  bool markupEnabled:1;            ///< Whether the mark-up processor is enabled.
  bool isTextColorSet:1;           ///< Whether a default color has been set.

  float minLineSize;               ///< The line's minimum size (in points).
};

/**
 * @brief Struct with info of the embedded items layout.
 */
struct DALI_TOOLKIT_API EmbeddedItemInfo
{
  TextAbstraction::CharacterIndex characterIndex;       ///< Index to the character within the string.
  TextAbstraction::GlyphIndex glyphIndex;               ///< Index to the glyph
  Vector2 position;                                     ///< The layout position within the buffer (top, left corner).
  Size size;                                            ///< The size within the buffer of the embedded item.
  Size rotatedSize;                                     ///< The rotated size within the buffer of the embedded item.
  Degree angle;                                         ///< Rotation angle of the pixel buffer in degrees.
  TextAbstraction::ColorBlendingMode colorBlendingMode; ///< Whether the color of the image is multiplied by the color of the text.
};

/**
* @brief Struct with the parameters needed to build a shadow for the given pixel buffer.
*/
struct DALI_TOOLKIT_API ShadowParameters
{
  Devel::PixelBuffer input; ///< The input pixel buffer used to create the shadow.
  Vector4 textColor;        ///< The color of the text.
  Vector4 color;            ///< The color of the shadow.
  Vector2 offset;           ///< The offset of the shadow.
  bool blendShadow;         ///< Whether to blend the shadow.
};

/**
 * @brief Renders text into a pixel buffer.
 *
 * @note: Can process a mark-up string.
 * @note: It does the font selection, RTL reordering, shaping and layouting.
 * @note: The width of the pixel buffer may be different to the given @e textWidth
 *        due to some padding pixels added.
 *
 *  The text is laid-out for the given size @e (textWidth,textHeight).
 *  If the @e multiLineEnabled option is enabled, the text will wrap in lines.
 *  If the @e ellipsisEnabled option is enabled, the text will be ellided if
 *  there is no more space for new lines.
 *
 *  It won't be rendered the parts of the text exceeding the boundaries of
 *  the given width and height.
 *
 *  If the given @e textHeight is zero, a big enough pixel buffer will be created
 *  to render the full text.
 *
 *  If the given @e textWidth is zero, the 'natural size' of the text will be
 *  used to create the pixel buffer to render the full text.
 *
 *  If the radius is not zero, the text will be laid-out following a circular path.
 *  In that case the text is laid-out in a single line.
 *
 * If the mark-up string contains embedded items, the @p embeddedItemLayout vector
 * contains the layout info of each embedded item.
 *
 * @param[in] textParameters The text and style options.
 * @param[out] embeddedItemLayout The layout info of the embedded items.
 *
 * @return A pixel buffer with the text rendered on it.
 */
DALI_TOOLKIT_API Devel::PixelBuffer Render( const RendererParameters& textParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout );

/**
 * @brief Creates a shadow for the text given in the input pixel buffer.
 *
 * The function returns a RGBA8888 pixel buffer with the text and its shadow rendered on it.
 *
 * The pixel format of the @e input pixel buffer could be an A8 or an RGBA8888. If it's
 * an A8 pixel buffer, it uses the given @e textColor to give color to the text. Otherwise
 * it uses the color of the @e input pixel buffer.
 *
 * @param[in] shadowParameters The parameters needed to create the text's shadow.
 *
 * @return A pixel buffer with the text and the shadow rendered on it.
 */
DALI_TOOLKIT_API Devel::PixelBuffer CreateShadow(const ShadowParameters& shadowParameters);

/**
 * @brief Converts a @p pixelBuffer with pixel format A8 to RGBA8888 using the given @p color.
 *
 * @note Does nothing if the @p pixelBuffer is not A8.
 *
 * @param[in] pixelBuffer The pixel buffer with pixel format A8
 * @param[in] color The color used to convert to RGBA8888
 * @param[in] multiplyByAlpha Whether to multiply the @p color with the alpha value of the @p pixel @p buffer.
 *
 * @return The pixel buffer converted to RGBA8888.
 */
DALI_TOOLKIT_API Devel::PixelBuffer ConvertToRgba8888( Devel::PixelBuffer pixelBuffer, const Vector4& color, bool multiplyByAlpha );

/**
* @brief Updates the @p dst pixel buffer with the data from @p src pixel buffer.
*
* @note Both pixel buffers must have the same pixel format. Does nothing if both pixel format are different.
* @note The function does nothing if the @p src pixel buffer doesn't fit into the @p dst pixel buffer.
*
* The @p src pixel buffer could be blended with the @p dst pixel buffer if @p blend is set to @e true.
*
* @param[in] src The pixel buffer from where the data is read.
* @param[in] dst The pixel buffer where the data is written..
* @param[in] x The top left corner's X within the destination pixel buffer.
* @param[in] y The top left corner's y within the destination pixel buffer.
* @param[in] blend Whether to blend the source pixel buffer with the destination pixel buffer as background.
*/
DALI_TOOLKIT_API void UpdateBuffer( Devel::PixelBuffer src, Devel::PixelBuffer dst, unsigned int x, unsigned int y, bool blend);

/**
 * @brief Splits the text in pages of the size given in @p textParameters
 *
 * @note The returned indices are indices to utf32 characters. The input text is encoded in utf8.
 * @return An array with the indices of the last character of each page
 */
DALI_TOOLKIT_API Dali::Property::Array GetLastCharacterIndex( RendererParameters& textParameters );

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UTILS_DEVEL_H
