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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor-font.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/text-font-style.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
const std::string XHTML_FAMILY_ATTRIBUTE("family");
const std::string XHTML_SIZE_ATTRIBUTE("size");
const std::string XHTML_WEIGHT_ATTRIBUTE("weight");
const std::string XHTML_WIDTH_ATTRIBUTE("width");
const std::string XHTML_SLANT_ATTRIBUTE("slant");

const unsigned int MAX_FONT_ATTRIBUTE_SIZE = 15u; ///< The maximum length of any of the possible 'weight', 'width' or 'slant' values.
}

void ProcessFontTag( const Tag& tag, FontDescriptionRun& fontRun )
{
  for( Vector<Attribute>::ConstIterator it = tag.attributes.Begin(),
         endIt = tag.attributes.End();
       it != endIt;
       ++it )
  {
    const Attribute& attribute( *it );
    if( TokenComparison( XHTML_FAMILY_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength ) )
    {
      fontRun.familyDefined = true;
      fontRun.familyLength = attribute.valueLength;
      fontRun.familyName = new char[fontRun.familyLength];
      memcpy( fontRun.familyName, attribute.valueBuffer, fontRun.familyLength );
      // The memory is freed when the font run is removed from the logical model.
    }
    else if( TokenComparison( XHTML_SIZE_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength ) )
    {
      // 64.f is used to convert from point size to 26.6 pixel format.
      fontRun.size = static_cast<PointSize26Dot6>( StringToFloat( attribute.valueBuffer ) * 64.f );
      fontRun.sizeDefined = true;
    }
    else if( TokenComparison( XHTML_WEIGHT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength ) )
    {
      // The StringToWeight() uses the Scripting::GetEnumeration() function which requires the input string to end with a '\0' char.
      char value[MAX_FONT_ATTRIBUTE_SIZE+1u];
      const Length length = attribute.valueLength > MAX_FONT_ATTRIBUTE_SIZE ? MAX_FONT_ATTRIBUTE_SIZE : attribute.valueLength;
      memcpy( value, attribute.valueBuffer, length );
      value[length] = 0;

      fontRun.weight = StringToWeight( value );
      fontRun.weightDefined = true;
    }
    else if( TokenComparison( XHTML_WIDTH_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength ) )
    {
      // The StringToWidth() uses the Scripting::GetEnumeration() function which requires the input string to end with a '\0' char.
      char value[MAX_FONT_ATTRIBUTE_SIZE+1u];
      const Length length = attribute.valueLength > MAX_FONT_ATTRIBUTE_SIZE ? MAX_FONT_ATTRIBUTE_SIZE : attribute.valueLength;
      memcpy( value, attribute.valueBuffer, length );
      value[length] = 0;

      fontRun.width = StringToWidth( value );
      fontRun.widthDefined = true;
    }
    else if( TokenComparison( XHTML_SLANT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength ) )
    {
      // The StringToSlant() uses the Scripting::GetEnumeration() function which requires the input string to end with a '\0' char.
      char value[MAX_FONT_ATTRIBUTE_SIZE+1u];
      const Length length = attribute.valueLength > MAX_FONT_ATTRIBUTE_SIZE ? MAX_FONT_ATTRIBUTE_SIZE : attribute.valueLength;
      memcpy( value, attribute.valueBuffer, length );
      value[length] = 0;

      fontRun.slant = StringToSlant( value );
      fontRun.slantDefined = true;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
