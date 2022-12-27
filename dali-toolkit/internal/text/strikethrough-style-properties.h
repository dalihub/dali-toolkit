#ifndef DALI_TOOLKIT_TEXT_STRIKETHROUGH_STYLE_PROPERTIES_H
#define DALI_TOOLKIT_TEXT_STRIKETHROUGH_STYLE_PROPERTIES_H

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
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Properties of strikethrough style.
 */
struct StrikethroughStyleProperties
{
  // Constructors

  /**
   * Default constructor to set the default values of bitfields
   */
  StrikethroughStyleProperties()
  : color{Color::BLACK},
    height{0u},
    colorDefined{false},
    heightDefined{false}
  {
  }

  StrikethroughStyleProperties(Vector4 color,
                               float   height,
                               bool    colorDefined,
                               bool    heightDefined)
  : color{color},
    height{height},
    colorDefined{colorDefined},
    heightDefined{heightDefined}

  {
  }

  // Overloading operators

  bool operator==(const StrikethroughStyleProperties& other) const
  {
    //The property is similar when both are not defined or when both are defined and have the same value.
    return ((!colorDefined && !other.colorDefined) || ((colorDefined && other.colorDefined) && (color == other.color))) &&
           ((!heightDefined && !other.heightDefined) || ((heightDefined && other.heightDefined) && (Dali::Equals(height, other.height))));
  }

  bool operator!=(const StrikethroughStyleProperties& other) const
  {
    return !(*this == other);
  }

  bool IsHeightEqualTo(const StrikethroughStyleProperties& other) const
  {
    return ((!heightDefined && !other.heightDefined) || ((heightDefined && other.heightDefined) && (Dali::Equals(height, other.height))));
  }

  StrikethroughStyleProperties& CopyIfNotDefined(const StrikethroughStyleProperties& other)
  {
    //Copy only the defined properties in other and not defined in this from other to this

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

    // to chain this method
    return *this;
  }

  StrikethroughStyleProperties& OverrideByDefinedProperties(const StrikethroughStyleProperties& other)
  {
    //Copy only the defined properties in other from other to this

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

    // to chain this method
    return *this;
  }

  //Attributes
  Vector4 color;  ///< The color of strikethrough.
  float   height; ///< The height of strikethrough.

  bool colorDefined : 1;  ///< Whether the color is defined.
  bool heightDefined : 1; ///< Whether the height is defined.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_STRIKETHROUGH_STYLE_PROPERTIES_H
