#ifndef DALI_TOOLKIT_TEXT_UNDERLINE_STYLE_PROPERTIES_H
#define DALI_TOOLKIT_TEXT_UNDERLINE_STYLE_PROPERTIES_H

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
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Properties of underline style.
 */
struct UnderlineStyleProperties
{
  // Constructors

  /**
   * Default constructor to set the default values of bitfields
   */
  UnderlineStyleProperties()
  : type{Text::Underline::SOLID},
    color{Color::BLACK},
    height{0u},
    dashGap{1u},
    dashWidth{2u},
    typeDefined{false},
    colorDefined{false},
    heightDefined{false},
    dashGapDefined{false},
    dashWidthDefined{false}
  {
  }

  UnderlineStyleProperties(Text::Underline::Type type,
                           Vector4               color,
                           float                 height,
                           float                 dashGap,
                           float                 dashWidth,
                           bool                  typeDefined,
                           bool                  colorDefined,
                           bool                  heightDefined,
                           bool                  dashGapDefined,
                           bool                  dashWidthDefined)
  : type{type},
    color{color},
    height{height},
    dashGap{dashGap},
    dashWidth{dashWidth},
    typeDefined{typeDefined},
    colorDefined{colorDefined},
    heightDefined{heightDefined},
    dashGapDefined{dashGapDefined},
    dashWidthDefined{dashWidthDefined}
  {
  }

  // Overloading operators

  bool operator==(const UnderlineStyleProperties& other) const
  {
    //The property is similar when both are not defined or when both are defined and have the same value.
    return ((!typeDefined && !other.typeDefined) || ((typeDefined && other.typeDefined) && (type == other.type))) &&
           ((!colorDefined && !other.colorDefined) || ((colorDefined && other.colorDefined) && (color == other.color))) &&
           ((!heightDefined && !other.heightDefined) || ((heightDefined && other.heightDefined) && (Dali::Equals(height, other.height)))) &&
           ((!dashGapDefined && !other.dashGapDefined) || ((dashGapDefined && other.dashGapDefined) && (Dali::Equals(dashGap, other.dashGap)))) &&
           ((!dashWidthDefined && !other.dashWidthDefined) || ((dashWidthDefined && other.dashWidthDefined) && (Dali::Equals(dashWidth, other.dashWidth))));
  }

  bool operator!=(const UnderlineStyleProperties& other) const
  {
    return !(*this == other);
  }

  bool IsHeightEqualTo(const UnderlineStyleProperties& other) const
  {
    return ((!heightDefined && !other.heightDefined) || ((heightDefined && other.heightDefined) && (Dali::Equals(height, other.height))));
  }

  UnderlineStyleProperties& CopyIfNotDefined(const UnderlineStyleProperties& other)
  {
    //Copy only the defined properties in other and not defined in this from other to this
    if(!typeDefined && other.typeDefined)
    {
      type        = other.type;
      typeDefined = true;
    }

    if(!heightDefined && other.heightDefined)
    {
      height        = other.height;
      heightDefined = true;
    }

    if(!colorDefined && other.colorDefined)
    {
      color        = other.color;
      colorDefined = true;
    }

    if(!dashGapDefined && other.dashGapDefined)
    {
      dashGap        = other.dashGap;
      dashGapDefined = true;
    }

    if(!dashWidthDefined && other.dashWidthDefined)
    {
      dashWidth        = other.dashWidth;
      dashWidthDefined = true;
    }

    // to chain this method
    return *this;
  }

  UnderlineStyleProperties& OverrideByDefinedProperties(const UnderlineStyleProperties& other)
  {
    //Copy only the defined properties in other from other to this
    if(other.typeDefined)
    {
      type        = other.type;
      typeDefined = true;
    }

    if(other.heightDefined)
    {
      height        = other.height;
      heightDefined = true;
    }

    if(other.colorDefined)
    {
      color        = other.color;
      colorDefined = true;
    }

    if(other.dashGapDefined)
    {
      dashGap        = other.dashGap;
      dashGapDefined = true;
    }

    if(other.dashWidthDefined)
    {
      dashWidth        = other.dashWidth;
      dashWidthDefined = true;
    }

    // to chain this method
    return *this;
  }

  //Attributes
  Text::Underline::Type type;      ///< The type of underline.
  Vector4               color;     ///< The color of underline.
  float                 height;    ///< The height of underline.
  float                 dashGap;   ///< The dash-gap of underline.
  float                 dashWidth; ///< The height of underline.

  bool typeDefined : 1;      ///< Whether the type is defined.
  bool colorDefined : 1;     ///< Whether the color is defined.
  bool heightDefined : 1;    ///< Whether the height is defined.
  bool dashGapDefined : 1;   ///< Whether the dash-gap is defined.
  bool dashWidthDefined : 1; ///< Whether the dash-width is defined.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UNDERLINE_STYLE_PROPERTIES_H
