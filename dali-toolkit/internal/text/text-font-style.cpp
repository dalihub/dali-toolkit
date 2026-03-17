/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-font-style.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/property-string-parser.h>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const char* WEIGHT_KEY = "weight";
const char* WIDTH_KEY  = "width";
const char* SLANT_KEY  = "slant";
const char* FAMILY_KEY = "family";
const char* TYPE_KEY   = "type";

const char* SYSTEM_TOKEN = "system";

} // namespace

void SetFontFamilyProperty(ControllerPtr controller, const Property::Value& value)
{
  if(controller)
  {
    String fontFamilyString;
    value.Get(fontFamilyString);
    const std::string fontFamilyValue = ToStdString(fontFamilyString);

    if(fontFamilyValue.empty())
    {
      // Resets the default's font family name.
      controller->SetDefaultFontFamily("");
      return;
    }

    Property::Map map;
    ParsePropertyString(fontFamilyValue, map);

    if(map.Empty())
    {
      // There is no map. The font has been passed as a font's family name with no format.
      controller->SetDefaultFontFamily(fontFamilyValue);
    }
    else
    {
      /// Family key
      Property::Value* familyValue = map.Find(FAMILY_KEY);

      String fontFamilyName;
      if(NULL != familyValue)
      {
        fontFamilyName = familyValue->Get<String>();
      }

      /// Type key
      Property::Value* typeValue = map.Find(TYPE_KEY);

      String typeStr;
      if(NULL != typeValue)
      {
        typeStr = typeValue->Get<String>();
      }

      if(TokenComparison(SYSTEM_TOKEN, typeStr.CStr(), typeStr.Size()))
      {
        controller->UpdateAfterFontChange(ToStdString(fontFamilyName));
      }
      else
      {
        controller->SetDefaultFontFamily(ToStdString(fontFamilyName));
      }
    }
  }
}

void SetFontStyleProperty(ControllerPtr controller, const Property::Value& value, FontStyle::Type type)
{
  if(controller)
  {
    Property::Map map;
    if(Property::STRING == value.GetType())
    {
      String fontStyleProperties;
      value.Get(fontStyleProperties);

      ParsePropertyString(ToStdString(fontStyleProperties), map);
      controller->FontStyleSetByString(true);
    }
    else
    {
      map = value.Get<Property::Map>();
      controller->FontStyleSetByString(false);
    }

    if(!map.Empty())
    {
      /// Weight key
      Property::Value* weightValue = map.Find(StringView(WEIGHT_KEY));

      FontWeight weight        = TextAbstraction::FontWeight::NONE;
      const bool weightDefined = weightValue != NULL;
      if(weightDefined)
      {
        const std::string weightStr = ToStdString(*weightValue);

        Scripting::GetEnumeration<FontWeight>(weightStr.c_str(),
                                              FONT_WEIGHT_STRING_TABLE,
                                              FONT_WEIGHT_STRING_TABLE_COUNT,
                                              weight);
      }

      /// Width key
      Property::Value* widthValue = map.Find(WIDTH_KEY);

      FontWidth  width        = TextAbstraction::FontWidth::NONE;
      const bool widthDefined = widthValue != NULL;
      if(widthDefined)
      {
        const std::string widthStr = ToStdString(*widthValue);

        Scripting::GetEnumeration<FontWidth>(widthStr.c_str(),
                                             FONT_WIDTH_STRING_TABLE,
                                             FONT_WIDTH_STRING_TABLE_COUNT,
                                             width);
      }

      /// Slant key
      Property::Value* slantValue = map.Find(SLANT_KEY);

      FontSlant  slant        = TextAbstraction::FontSlant::NONE;
      const bool slantDefined = slantValue != NULL;
      if(slantDefined)
      {
        const std::string slantStr = ToStdString(*slantValue);

        Scripting::GetEnumeration<FontSlant>(slantStr.c_str(),
                                             FONT_SLANT_STRING_TABLE,
                                             FONT_SLANT_STRING_TABLE_COUNT,
                                             slant);
      }

      switch(type)
      {
        case FontStyle::DEFAULT:
        {
          // Sets the default font's style values.
          if(!weightDefined ||
             (weightDefined && (controller->GetDefaultFontWeight() != weight)))
          {
            controller->SetDefaultFontWeight(weight);
          }

          if(!widthDefined ||
             (widthDefined && (controller->GetDefaultFontWidth() != width)))
          {
            controller->SetDefaultFontWidth(width);
          }

          if(!slantDefined ||
             (slantDefined && (controller->GetDefaultFontSlant() != slant)))
          {
            controller->SetDefaultFontSlant(slant);
          }
          break;
        }
        case FontStyle::INPUT:
        {
          // Sets the input font's style values.
          if(!weightDefined ||
             (weightDefined && (controller->GetInputFontWeight() != weight)))
          {
            controller->SetInputFontWeight(weight);
          }

          if(!widthDefined ||
             (widthDefined && (controller->GetInputFontWidth() != width)))
          {
            controller->SetInputFontWidth(width);
          }

          if(!slantDefined ||
             (slantDefined && (controller->GetInputFontSlant() != slant)))
          {
            controller->SetInputFontSlant(slant);
          }
          break;
        }
        case FontStyle::PLACEHOLDER:
        {
          // Sets the placeholder text font's style values.
          if(!weightDefined ||
             (weightDefined && (controller->GetPlaceholderTextFontWeight() != weight)))
          {
            controller->SetPlaceholderTextFontWeight(weight);
          }

          if(!widthDefined ||
             (widthDefined && (controller->GetPlaceholderTextFontWidth() != width)))
          {
            controller->SetPlaceholderTextFontWidth(width);
          }

          if(!slantDefined ||
             (slantDefined && (controller->GetPlaceholderTextFontSlant() != slant)))
          {
            controller->SetPlaceholderTextFontSlant(slant);
          }
          break;
        }
      } // switch
    } // map not empty
    else
    {
      switch(type)
      {
        case FontStyle::DEFAULT:
        {
          controller->SetDefaultFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetDefaultFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetDefaultFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
        case FontStyle::INPUT:
        {
          controller->SetInputFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetInputFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetInputFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
        case FontStyle::PLACEHOLDER:
        {
          controller->SetPlaceholderTextFontWeight(TextAbstraction::FontWeight::NONE);
          controller->SetPlaceholderTextFontWidth(TextAbstraction::FontWidth::NONE);
          controller->SetPlaceholderTextFontSlant(TextAbstraction::FontSlant::NONE);
          break;
        }
      } // switch
    } // map.Empty()
  } // controller
}

void GetFontStyleProperty(ControllerPtr controller, Property::Value& value, FontStyle::Type type)
{
  if(controller)
  {
    const bool isSetbyString = controller->IsFontStyleSetByString();

    bool       weightDefined = false;
    bool       widthDefined  = false;
    bool       slantDefined  = false;
    FontWeight weight        = TextAbstraction::FontWeight::NONE;
    FontWidth  width         = TextAbstraction::FontWidth::NONE;
    FontSlant  slant         = TextAbstraction::FontSlant::NONE;

    switch(type)
    {
      case FontStyle::DEFAULT:
      {
        weightDefined = controller->IsDefaultFontWeightDefined();
        widthDefined  = controller->IsDefaultFontWidthDefined();
        slantDefined  = controller->IsDefaultFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetDefaultFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetDefaultFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetDefaultFontSlant();
        }
        break;
      }
      case FontStyle::INPUT:
      {
        weightDefined = controller->IsInputFontWeightDefined();
        widthDefined  = controller->IsInputFontWidthDefined();
        slantDefined  = controller->IsInputFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetInputFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetInputFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetInputFontSlant();
        }
        break;
      }
      case FontStyle::PLACEHOLDER:
      {
        // The type is FontStyle::PLACEHOLDER
        weightDefined = controller->IsPlaceholderTextFontWeightDefined();
        widthDefined  = controller->IsPlaceholderTextFontWidthDefined();
        slantDefined  = controller->IsPlaceholderTextFontSlantDefined();

        if(weightDefined)
        {
          weight = controller->GetPlaceholderTextFontWeight();
        }

        if(widthDefined)
        {
          width = controller->GetPlaceholderTextFontWidth();
        }

        if(slantDefined)
        {
          slant = controller->GetPlaceholderTextFontSlant();
        }
        break;
      }
    }

    if(!isSetbyString)
    {
      Property::Map map;

      if(weightDefined)
      {
        if(TextAbstraction::FontWeight::NONE != weight)
        {
          const std::string weightStr(GetEnumerationName(weight,
                                                         FONT_WEIGHT_STRING_TABLE,
                                                         FONT_WEIGHT_STRING_TABLE_COUNT));

          Dali::Integration::InsertToMap(map, std::string_view(WEIGHT_KEY), weightStr);
        }
      }

      if(widthDefined)
      {
        if(TextAbstraction::FontWidth::NONE != width)
        {
          const std::string widthStr(GetEnumerationName(width,
                                                        FONT_WIDTH_STRING_TABLE,
                                                        FONT_WIDTH_STRING_TABLE_COUNT));

          Dali::Integration::InsertToMap(map, std::string_view(WIDTH_KEY), widthStr);
        }
      }

      if(slantDefined)
      {
        if(TextAbstraction::FontSlant::NONE != slant)
        {
          const std::string slantStr(GetEnumerationName(slant,
                                                        FONT_SLANT_STRING_TABLE,
                                                        FONT_SLANT_STRING_TABLE_COUNT));

          Dali::Integration::InsertToMap(map, std::string_view(SLANT_KEY), slantStr);
        }
      }

      value = map;
    } // SetbyMAP
    else
    {
      std::string fontStyleProperties = "{";

      if(weightDefined)
      {
        if(TextAbstraction::FontWeight::NONE != weight)
        {
          const std::string weightStr(GetEnumerationName(weight,
                                                         FONT_WEIGHT_STRING_TABLE,
                                                         FONT_WEIGHT_STRING_TABLE_COUNT));

          fontStyleProperties += "\"weight\":\"" + weightStr + "\",";
        }
      }

      if(widthDefined)
      {
        if(TextAbstraction::FontWidth::NONE != width)
        {
          const std::string widthStr(GetEnumerationName(width,
                                                        FONT_WIDTH_STRING_TABLE,
                                                        FONT_WIDTH_STRING_TABLE_COUNT));
          fontStyleProperties += "\"width\":\"" + widthStr + "\",";
        }
      }

      if(slantDefined)
      {
        if(TextAbstraction::FontSlant::NONE != slant)
        {
          const std::string slantStr(GetEnumerationName(slant,
                                                        FONT_SLANT_STRING_TABLE,
                                                        FONT_SLANT_STRING_TABLE_COUNT));

          fontStyleProperties += "\"slant\":\"" + slantStr + "\"";
        }
      }

      // If last character is comma, it will be removed.
      if((*fontStyleProperties.rbegin()) == ',')
      {
        fontStyleProperties = fontStyleProperties.substr(0, fontStyleProperties.size() - 1);
      }
      fontStyleProperties += "}";

      value = Dali::Integration::ToPropertyValue(fontStyleProperties);
    } // SetbyString
  } // controller
}

FontWeight StringToWeight(const char* const weightStr)
{
  FontWeight weight = TextAbstraction::FontWeight::NORMAL;
  Scripting::GetEnumeration<FontWeight>(weightStr,
                                        FONT_WEIGHT_STRING_TABLE,
                                        FONT_WEIGHT_STRING_TABLE_COUNT,
                                        weight);

  return weight;
}

FontWidth StringToWidth(const char* const widthStr)
{
  FontWidth width = TextAbstraction::FontWidth::NORMAL;
  Scripting::GetEnumeration<FontWidth>(widthStr,
                                       FONT_WIDTH_STRING_TABLE,
                                       FONT_WIDTH_STRING_TABLE_COUNT,
                                       width);

  return width;
}

FontSlant StringToSlant(const char* const slantStr)
{
  FontSlant slant = TextAbstraction::FontSlant::NORMAL;
  Scripting::GetEnumeration<FontSlant>(slantStr,
                                       FONT_SLANT_STRING_TABLE,
                                       FONT_SLANT_STRING_TABLE_COUNT,
                                       slant);

  return slant;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
