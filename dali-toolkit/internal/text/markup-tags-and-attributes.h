#ifndef DALI_TOOLKIT_TEXT_MARKUPS_AND_ATTRIBUTES_H
#define DALI_TOOLKIT_TEXT_MARKUPS_AND_ATTRIBUTES_H

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
#include <string>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Use markup styling to style Text-Controller.
 * The markup tag is opend by <> and closed by </>
 *
 * @details You can use markup elements to change the style of the text.
 * Since the text controls do not process markup elements by default, you must first set the EnableMarkup property of the Text-Controller  to true:
 *
 * @note The markup processor does not check for markup validity, and styles are rendered in priority order.
 * Incorrect or incompatible elements can cause the text to be rendered incorrectly.
 */
namespace MARKUP
{
namespace TAG
{
// HTML-ISH tag and attribute constants.
// Note they must be lower case in order to make the comparison to work
// as the parser converts all the read tags to lower case.

/**
 * @brief Sets the color for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<color value='red'>Hello world</color>";
 *
 * @endcode
 *
 * @see COLOR_ATTRIBUTES
 *
 */
static const std::string COLOR("color");

/**
 * @brief Sets the font values for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<font family='DejaVuSerif' size='18'>Hello world</font>";
 *
 * @endcode
 *
 * @see FONT_ATTRIBUTES
 */
static const std::string FONT("font");

/**
 * @brief Sets Bold decoration for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<b>Hello world</b>";
 *
 * @endcode
 *
 * @see
 */
static const std::string BOLD("b");

/**
 * @brief Sets Italic decoration for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<i>Hello world</i>";
 *
 * @endcode
 *
 */
static const std::string ITALIC("i");

/**
 * @brief Sets the underlined values for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u>Hello world</u>";
 *
 * @endcode
 *
 * @see UNDERLINE_ATTRIBUTES
 */
static const std::string UNDERLINE("u");

/**
 * @todo Sets the shadow for the characters inside the element.
 *
 */
static const std::string SHADOW("shadow"); ///< This tag under construction.

/**
 * @todo Sets the glow for the characters inside the element.
 *
 */
static const std::string GLOW("glow"); ///< This tag under construction.

/**
 * @todo Sets the outline for the characters inside the element.
 *
 */
static const std::string OUTLINE("outline"); ///< This tag under construction.

/**
 * @brief Defines an embedded item within the text.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<item 'width'=26 'height'=26 'url'='path/image.png' 'color-blending'='multiply'/>";
 *
 * @endcode
 *
 * @see EMBEDDED_ITEM_ATTRIBUTES
 */
static const std::string EMBEDDED_ITEM("item");

/**
 * @brief Defines a hyperlink for the text inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<a href='https://www.tizen.org'>TIZEN</a>";
 *
 * @endcode
 *
 * @see ANCHOR_ATTRIBUTES
 */
static const std::string ANCHOR("a");

/**
 * @brief Sets the background color for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<background color='yellow'>Hello world</background>";
 *
 * @endcode
 *
 * @see BACKGROUND_ATTRIBUTES
 */
static const std::string BACKGROUND("background");

/**
 * @brief Use span tag to set many styles on character's level for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='green' char-space-value='10.0f'>Hello world</span>";
 *
 * @endcode
 *
 * @see SPAN_ATTRIBUTES
 */
static const std::string SPAN("span");

/**
 * @brief Sets the strikethrough values for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<s>Hello world</s>";
 *
 * @endcode
 *
 * @see STRIKETHROUGH_ATTRIBUTES
 */
static const std::string STRIKETHROUGH("s");

/**
 * @brief Use paragraph tag to set many styles on paragraph's level for the lines inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "test before paragraph tag <p>test paragraph tag </p>test after paragraph tag ";
 *
 * @endcode
 *
 * @see PARAGRAPH_ATTRIBUTES
 */
static const std::string PARAGRAPH("p");

/**
 * @brief Sets the character spacing values for the characters inside the element.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<char-spacing value='3.0f'>Hello world</char-spacing>";
 *
 * @endcode
 *
 * @see CHARACTER_SPACING_ATTRIBUTES
 */
static const std::string CHARACTER_SPACING("char-spacing");
} // namespace TAG

namespace COLOR_ATTRIBUTES
{
/**
 * @brief Use the value attribute to define the color.
 * The supported attribute values are red, green, blue, yellow, magenta, cyan, white, black, and transparent.
 * Web colors and colors represented in 32-bit hexadecimal 0xAARRGGBB format are also supported.
 *
 * The following examples show text in red color:
 * @code
 *
 * textController.Text = "<color value='red'>Hello world</color>"; /// Color coded with a text constant
 *
 * @endcode
 *
 * @code
 *
 * textController.Text = "<color value='0xFFFF0000'>Hello world</color>"); /// Color packed inside an ARGB hexadecimal value
 *
 * @endcode
 */
static const std::string VALUE("value");
} // namespace COLOR_ATTRIBUTES

namespace FONT_ATTRIBUTES
{
/**
 * @brief Use the family attribute to define the font name.
 *
 * Example:
 * @code
 *
 * textController.Text = "<font family='SamsungSans' >Hello world</font>";
 *
 * @endcode
 *
 */
static const std::string FAMILY("family");

/**
 * @brief Use the size attribute to define the font size in points.
 *
 * Example:
 * @code
 *
 * textController.Text = "<font size='50' >Hello world</font>";
 *
 * @endcode
 *
 */
static const std::string SIZE("size");

/**
 * @brief Use the weight attribute to define the font weight.
 *
 * Example:
 * @code
 *
 * textController.Text = "<font weight='bold' >Hello world</font>";
 *
 * @endcode
 *
 */
static const std::string WEIGHT("weight");

/**
 * @brief Use the width attribute to define the font width.
 *
 * Example:
 * @code
 *
 * textController.Text = "<font width='condensed' >Hello world</font>";
 *
 * @endcode
 *
 */
static const std::string WIDTH("width");

/**
 * @brief Use the slant attribute to define the font slant.
 *
 * Example:
 * @code
 *
 * textController.Text = "<font slant='italic' >Hello world</font>";
 *
 * @endcode
 *
 */
static const std::string SLANT("slant");
} // namespace FONT_ATTRIBUTES

namespace UNDERLINE_ATTRIBUTES
{
/**
 * @brief Use the color attribute to define the color of underline.
 * The supported attribute values are red, green, blue, yellow, magenta, cyan, white, black, and transparent.
 * Web colors and colors represented in 32-bit hexadecimal 0xAARRGGBB format are also supported.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u color='green'>Hello world</u>";
 *
 * @endcode
 */
static const std::string COLOR("color");

/**
 * @brief Use the height attribute to define the height of underline.
 * It is float value.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u height='2.0f'>Hello world</u>";
 *
 * @endcode
 */
static const std::string HEIGHT("height");

/**
 * @brief Use the type attribute to define the type of underline.
 * The supported attribute values are solid, dashed and double
 * The default value is solid
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u type='double'>Hello world</u>";
 *
 * @endcode
 */
static const std::string TYPE("type");

/**
 * @brief Use the dash-gap attribute to define the dash-gap of underline.
 * The gap in pixels between the dashes of the dashed underline. Only valid when "DASHED" underline type is used.
 *
 * It is float value.
 * @note If not provided then the default gap is used (1 pixel).
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u type='dashed' dash-gap='2.0f'>Hello world</u>";
 *
 * @endcode
 */
static const std::string DASH_GAP("dash-gap");

/**
 * @brief Use the dash-width attribute to define the dash-width of underline.
 * The width in pixels of the dashes of the dashed underline. Only valid when "DASHED" underline type is used.
 * It is float value.
 * @note If not provided then the default width is used (2 pixel).
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<u type='dashed' dash-width='3.0f'>Hello world</u>";
 *
 * @endcode
 */
static const std::string DASH_WIDTH("dash-width");

} // namespace UNDERLINE_ATTRIBUTES

namespace SPAN_ATTRIBUTES
{
//NOTE: the MAX_NUM_OF_ATTRIBUTES in "markup-processor.cpp" should be updated when add a new attribute for span tag.

/**
 * @brief The font family attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span font-family='SamsungSans' >Hello world</span>";
 * @endcode
 * @see FONT_ATTRIBUTES::FAMILY
 */
static const std::string FONT_FAMILY("font-family");

/**
 * @brief The font size attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span font-size='50' >Hello world</span>";
 * @endcode
 * @see FONT_ATTRIBUTES::SIZE
 */
static const std::string FONT_SIZE("font-size");

/**
 * @brief The font weight attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span font-weight='bold' >Hello world</span>";
 * @endcode
 * @see FONT_ATTRIBUTES::WEIGHT
 */
static const std::string FONT_WEIGHT("font-weight");

/**
 * @brief The font width attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span font-width='condensed' >Hello world</span>";
 * @endcode
 * @see FONT_ATTRIBUTES::WIDTH
 */
static const std::string FONT_WIDTH("font-width");

/**
 * @brief The font slant attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span font-slant='italic' >Hello world</span>";
 * @endcode
 * @see FONT_ATTRIBUTES::SLANT
 */
static const std::string FONT_SLANT("font-slant");

/**
 * @brief The color value attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span text-color='blue' >Hello world</span>";
 * @endcode
 * @see COLOR_ATTRIBUTES::VALUE
 */
static const std::string TEXT_COLOR("text-color");

/**
 * @brief The background color attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span background-color='yellow' >Hello world</span>";
 * @endcode
 * @see BACKGROUND_ATTRIBUTES::COLOR
 */
static const std::string BACKGROUND_COLOR("background-color");

/**
 * @brief The undeline color attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span u-color='green' >Hello world</span>";
 * @endcode
 * @see UNDERLINE_ATTRIBUTES::COLOR
 */
static const std::string UNDERLINE_COLOR("u-color");

/**
 * @brief The undeline height attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span u-height='3.0f' >Hello world</span>";
 * @endcode
 * @see UNDERLINE_ATTRIBUTES::HEIGHT
 */
static const std::string UNDERLINE_HEIGHT("u-height");

/**
 * @brief The undeline type attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span u-type='solid' >Hello world</span>";
 * @endcode
 * @see UNDERLINE_ATTRIBUTES::TYPE
 */
static const std::string UNDERLINE_TYPE("u-type");

/**
 * @brief The undeline dash-gap attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span u-type='dashed' u-dash-gap='2.0f' >Hello world</span>";
 * @endcode
 * @see UNDERLINE_ATTRIBUTES::DASH_GAP
 */
static const std::string UNDERLINE_DASH_GAP("u-dash-gap");

/**
 * @brief The undeline dash-width attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span u-type='dashed' u-dash-width='4.0f' >Hello world</span>";
 * @endcode
 * @see UNDERLINE_ATTRIBUTES::DASH_WIDTH
 */
static const std::string UNDERLINE_DASH_WIDTH("u-dash-width");

/**
 * @brief The strikethrough color attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span s-color='green' >Hello world</span>";
 * @endcode
 * @see STRIKETHROUGH_ATTRIBUTES::COLOR
 */
static const std::string STRIKETHROUGH_COLOR("s-color");

/**
 * @brief The strikethrough height attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span s-height='3.0f' >Hello world</span>";
 * @endcode
 * @see STRIKETHROUGH_ATTRIBUTES::HEIGHT
 */
static const std::string STRIKETHROUGH_HEIGHT("s-height");

/**
 * @brief The character-spacing value attribute.
 *
 * Example:
 * @code
 * textController.Text = "<span char-space-value='5.0f' >Hello world</span>";
 * @endcode
 * @see CHARACTER_SPACING_ATTRIBUTES::VALUE
 */
static const std::string CHARACTER_SPACING_VALUE("char-space-value");
} // namespace SPAN_ATTRIBUTES

namespace STRIKETHROUGH_ATTRIBUTES
{
/**
 * @brief Use the color attribute to define the color of strikethrough.
 * The supported attribute values are red, green, blue, yellow, magenta, cyan, white, black, and transparent.
 * Web colors and colors represented in 32-bit hexadecimal 0xAARRGGBB format are also supported.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<s color='green'>Hello world</s>";
 *
 * @endcode
 */
static const std::string COLOR("color");

/**
 * @brief Use the height attribute to define the height of strikethrough.
 * It is float value.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<s height='2.0f'>Hello world</s>";
 *
 * @endcode
 */
static const std::string HEIGHT("height");
} // namespace STRIKETHROUGH_ATTRIBUTES

namespace PARAGRAPH_ATTRIBUTES
{
/**
 * @brief Use the align attribute to define the horizontal alignment of paragraph.
 * The supported attribute values are begin, center and end .
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = ""text outside<p align='end'>Paragraph end</p>text outside<p align='center'>Paragraph center</p>text outside<p align='begin' >Paragraph begin</p>";
 *
 * @endcode
 */
static const std::string ALIGN("align");

/**
 * @brief Use the rrel-line-height attribute to define the relative height of the line (a factor that will be multiplied by text height).
 * It is float value.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<p>line 1</p><p rel-line-height=0.5>line\n2</p>line 3<p rel-line-height=3>line\n4</p>line 5";
 *
 * @endcode
 * @note If the value is less than 1, the lines could to be overlapped.
 */
static const std::string RELATIVE_LINE_HEIGHT("rel-line-height");

} // namespace PARAGRAPH_ATTRIBUTES

namespace CHARACTER_SPACING_ATTRIBUTES
{
/**
 * @brief Use the value attribute to define the spaces between characters in Pixels.
 *  A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
 *
 * Examples:
 * @code
 *
 * textController.Text = "<char-spacing value='5.0f'>Hello world</char-spacing>"; /// Expanded
 *
 * @endcode
 *
 * @code
 *
 * textController.Text = "<char-spacing value='-5.0f'>Hello world</char-spacing>"); /// Condensed
 *
 * @endcode
 */
static const std::string VALUE("value");
} // namespace CHARACTER_SPACING_ATTRIBUTES
namespace BACKGROUND_ATTRIBUTES
{
/**
 * @brief Use the value attribute to define the color of background.
 * The supported attribute values are red, green, blue, yellow, magenta, cyan, white, black, and transparent.
 * Web colors and colors represented in 32-bit hexadecimal 0xAARRGGBB format are also supported.
 *
 * The following example explains how to apply it:
 * @code
 *
 * textController.Text = "<background color='green'>Hello world</background>";
 *
 * @endcode
 */
static const std::string COLOR("color");

} // namespace BACKGROUND_ATTRIBUTES

namespace EMBEDDED_ITEM_ATTRIBUTES
{
/**
 * @brief Use the url attribute to define url path of the image.
 *
 * @note  The url of the image is optional. If there is no image
 * the layout engine will use the width and height to
 * create a space inside the text. This gap can be filled later.
 */
static const std::string URL("url");

/**
 * @brief Use the width attribute to define the width of the item.
 */
static const std::string WIDTH("width");

/**
 * @brief Use the height attribute to define the height of the item.
 */
static const std::string HEIGHT("height");

/**
 * @brief Use the color-blending attribute to define whether the color of the image is multiplied by the color of the text.
 *
 * @note A color blending mode can be set. The default is NONE, the image will use its own color. If MULTIPLY is set, the color
 * of the image will be multiplied by the color of the text.
 */
static const std::string COLOR_BLENDING("color-blending");
} // namespace EMBEDDED_ITEM_ATTRIBUTES

namespace ANCHOR_ATTRIBUTES
{
/**
 * @brief Use the href attribute to define the url of hyperlink.
 */
static const std::string HREF("href");

/**
 * @brief Sets the color for the characters and underlines inside the element.
 */
static const std::string COLOR("color");

/**
 * @brief Sets the clicked color for the characters and underlines inside the element.
 */
static const std::string CLICKED_COLOR("clicked-color");

} // namespace ANCHOR_ATTRIBUTES

} // namespace MARKUP

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUPS_AND_ATTRIBUTES_H
