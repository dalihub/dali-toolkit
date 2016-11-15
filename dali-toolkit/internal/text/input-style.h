#ifndef __DALI_TOOLKIT_TEXT_INPUT_STYLE_H__
#define __DALI_TOOLKIT_TEXT_INPUT_STYLE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/constants.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * The input text's style.
 */
struct InputStyle
{
  enum Mask
  {
    NONE               = 0x0000,
    INPUT_COLOR        = 0x0001,
    INPUT_FONT_FAMILY  = 0x0002,
    INPUT_POINT_SIZE   = 0x0004,
    INPUT_FONT_WEIGHT  = 0x0008,
    INPUT_FONT_WIDTH   = 0x0010,
    INPUT_FONT_SLANT   = 0x0020,
    INPUT_LINE_SPACING = 0x0040,
    INPUT_UNDERLINE    = 0x0080,
    INPUT_SHADOW       = 0x0100,
    INPUT_EMBOSS       = 0x0200,
    INPUT_OUTLINE      = 0x0400
  };

  InputStyle()
  : textColor( Color::BLACK ),
    familyName(),
    weight( TextAbstraction::FontWeight::NORMAL ),
    width( TextAbstraction::FontWidth::NORMAL ),
    slant( TextAbstraction::FontSlant::NORMAL ),
    size( 0.f ),
    lineSpacing( 0.f ),
    underlineProperties(),
    shadowProperties(),
    embossProperties(),
    outlineProperties(),
    isDefaultColor( true ),
    isFamilyDefined( false ),
    isWeightDefined( false ),
    isWidthDefined( false ),
    isSlantDefined( false ),
    isSizeDefined( false ),
    isLineSpacingDefined( false ),
    isUnderlineDefined( false ),
    isShadowDefined( false ),
    isEmbossDefined( false ),
    isOutlineDefined( false )
    {}

  ~InputStyle()
  {};

  /**
   * @brief
   *
   * Does not copy the font-style, underline, shadow, emboss and outline property strings.
   */
  void Copy( const InputStyle& inputStyle )
  {
    isDefaultColor = inputStyle.isDefaultColor;
    textColor = inputStyle.textColor;

    isFamilyDefined = inputStyle.isFamilyDefined;
    familyName = inputStyle.familyName;

    isWeightDefined = inputStyle.isWeightDefined;
    weight = inputStyle.weight;

    isWidthDefined = inputStyle.isWidthDefined;
    width = inputStyle.width;

    isSlantDefined = inputStyle.isSlantDefined;
    slant = inputStyle.slant;

    isSizeDefined = inputStyle.isSizeDefined;
    size = inputStyle.size;

    isLineSpacingDefined = inputStyle.isLineSpacingDefined;
    lineSpacing = inputStyle.lineSpacing;

    isUnderlineDefined = inputStyle.isUnderlineDefined;
    underlineProperties = inputStyle.underlineProperties;

    isShadowDefined = inputStyle.isShadowDefined;
    shadowProperties = inputStyle.shadowProperties;

    isEmbossDefined = inputStyle.isEmbossDefined;
    embossProperties = inputStyle.embossProperties;

    isOutlineDefined = inputStyle.isOutlineDefined;
    outlineProperties = inputStyle.outlineProperties;
  }

  /**
   * @brief
   *
   * Does not compare the font-style, underline, shadow, emboss and outline property strings.
   */
  bool Equal( const InputStyle& inputStyle ) const
  {
    if( ( isDefaultColor != inputStyle.isDefaultColor )             ||
        ( isFamilyDefined != inputStyle.isFamilyDefined )           ||
        ( isWeightDefined != inputStyle.isWeightDefined )           ||
        ( isWidthDefined != inputStyle.isWidthDefined )             ||
        ( isSlantDefined != inputStyle.isSlantDefined )             ||
        ( isSizeDefined != inputStyle.isSizeDefined )               ||
        ( isLineSpacingDefined != inputStyle.isLineSpacingDefined ) ||
        ( isUnderlineDefined != inputStyle.isUnderlineDefined )     ||
        ( isShadowDefined != inputStyle.isShadowDefined )           ||
        ( isEmbossDefined != inputStyle.isEmbossDefined )           ||
        ( isOutlineDefined != inputStyle.isOutlineDefined )         ||
        ( textColor != inputStyle.textColor )                       ||
        ( familyName != inputStyle.familyName )                     ||
        ( weight != inputStyle.weight )                             ||
        ( width != inputStyle.width )                               ||
        ( slant != inputStyle.slant )                               ||
        ( size != inputStyle.size )                                 ||
        ( lineSpacing != inputStyle.lineSpacing )                   ||
        ( underlineProperties != inputStyle.underlineProperties )   ||
        ( shadowProperties != inputStyle.shadowProperties )         ||
        ( embossProperties != inputStyle.embossProperties )         ||
        ( outlineProperties != inputStyle.outlineProperties ) )
    {
      return false;
    }

    return true;
  }

  Mask GetInputStyleChangeMask( const InputStyle& inputStyle ) const
  {
    Mask mask = NONE;

    if( textColor != inputStyle.textColor )
    {
      mask = static_cast<Mask>( mask | INPUT_COLOR );
    }
    if( familyName != inputStyle.familyName )
    {
      mask = static_cast<Mask>( mask | INPUT_FONT_FAMILY );
    }
    if( weight != inputStyle.weight )
    {
      mask = static_cast<Mask>( mask | INPUT_FONT_WEIGHT );
    }
    if( width != inputStyle.width )
    {
      mask = static_cast<Mask>( mask | INPUT_FONT_WIDTH );
    }
    if( slant != inputStyle.slant )
    {
      mask = static_cast<Mask>( mask | INPUT_FONT_SLANT );
    }
    if( size != inputStyle.size )
    {
      mask = static_cast<Mask>( mask | INPUT_POINT_SIZE );
    }
    if( lineSpacing != inputStyle.lineSpacing )
    {
      mask = static_cast<Mask>( mask | INPUT_LINE_SPACING );
    }
    if( underlineProperties != inputStyle.underlineProperties )
    {
      mask = static_cast<Mask>( mask | INPUT_UNDERLINE );
    }
    if( shadowProperties != inputStyle.shadowProperties )
    {
      mask = static_cast<Mask>( mask | INPUT_SHADOW );
    }
    if( embossProperties != inputStyle.embossProperties )
    {
      mask = static_cast<Mask>( mask | INPUT_EMBOSS );
    }
    if( outlineProperties != inputStyle.outlineProperties )
    {
      mask = static_cast<Mask>( mask | INPUT_OUTLINE );
    }

    return mask;
  }

  Vector4     textColor;           ///< The text's color.
  std::string familyName;          ///< The font's family name.
  FontWeight  weight;              ///< The font's weight.
  FontWidth   width;               ///< The font's width.
  FontSlant   slant;               ///< The font's slant.
  float       size;                ///< The font's size.

  float       lineSpacing;         ///< The line's spacing.

  std::string underlineProperties; ///< The underline properties string.
  std::string shadowProperties;    ///< The shadow properties string.
  std::string embossProperties;    ///< The emboss properties string.
  std::string outlineProperties;   ///< The outline properties string.

  bool        isDefaultColor       : 1; ///< Whether the text's color is the default.
  bool        isFamilyDefined      : 1; ///< Whether the font's family is defined.
  bool        isWeightDefined      : 1; ///< Whether the font's weight is defined.
  bool        isWidthDefined       : 1; ///< Whether the font's width is defined.
  bool        isSlantDefined       : 1; ///< Whether the font's slant is defined.
  bool        isSizeDefined        : 1; ///< Whether the font's size is defined.

  bool        isLineSpacingDefined : 1; ///< Whether the line spacing is defined.
  bool        isUnderlineDefined   : 1; ///< Whether the underline parameters are defined.
  bool        isShadowDefined      : 1; ///< Whether the shadow parameters are defined.
  bool        isEmbossDefined      : 1; ///< Whether the emboss parameters are defined.
  bool        isOutlineDefined     : 1; ///< Whether the outline parameters are defined.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_INPUT_STYLE_H__
