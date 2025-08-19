/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-effects-style.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-style-properties-devel.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/property-string-parser.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const char* COLOR_KEY        = "color";
const char* OFFSET_KEY       = "offset";
const char* BLUR_RADIUS_KEY  = "blurRadius";
const char* WIDTH_KEY        = "width";
const char* HEIGHT_KEY       = "height";
const char* ENABLE_KEY       = "enable";
const char* TYPE_KEY         = "type";
const char* DASH_WIDTH_KEY   = "dashWidth";
const char* DASH_GAP_KEY     = "dashGap";
const char* DIRECTION_KEY    = "direction";
const char* STRENGTH_KEY     = "strength";
const char* LIGHT_COLOR_KEY  = "lightColor";
const char* SHADOW_COLOR_KEY = "shadowColor";
const char* TRUE_TOKEN       = "true";

const char* EMPTY_STRING = "";
} // namespace

bool ParseShadowProperties(const Property::Map& shadowPropertiesMap,
                           bool&                colorDefined,
                           Vector4&             color,
                           bool&                offsetDefined,
                           Vector2&             offset,
                           bool&                blurRadiusDefined,
                           float&               blurRadius)
{
  const unsigned int numberOfItems = shadowPropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = shadowPropertiesMap.GetKeyValue(index);

    if((DevelText::Shadow::Property::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((DevelText::Shadow::Property::OFFSET == valueGet.first.indexKey) || (OFFSET_KEY == valueGet.first.stringKey))
    {
      /// Offset key.
      offsetDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string offsetStr = valueGet.second.Get<std::string>();
        StringToVector2(offsetStr.c_str(), offsetStr.size(), offset);
      }
      else
      {
        offset = valueGet.second.Get<Vector2>();
      }
    }
    else if((DevelText::Shadow::Property::BLUR_RADIUS == valueGet.first.indexKey) || (BLUR_RADIUS_KEY == valueGet.first.stringKey))
    {
      /// Blur radius key.
      blurRadiusDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string blurRadiusStr = valueGet.second.Get<std::string>();
        blurRadius                      = StringToFloat(blurRadiusStr.c_str());
      }
      else
      {
        blurRadius = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseEmbossProperties(const Property::Map& embossPropertiesMap,
                           bool&                enabled,
                           bool&                directionDefined,
                           Vector2&             direction,
                           bool&                strengthDefined,
                           float&               strength,
                           bool&                lightColorDefined,
                           Vector4&             lightColor,
                           bool&                shadowColorDefined,
                           Vector4&             shadowColor)
{
  const unsigned int numberOfItems = embossPropertiesMap.Count();
  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = embossPropertiesMap.GetKeyValue(index);

    if((DevelText::Emboss::Property::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = valueGet.second.Get<std::string>();
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((DevelText::Emboss::Property::DIRECTION == valueGet.first.indexKey) || (DIRECTION_KEY == valueGet.first.stringKey))
    {
      /// Direction key.
      directionDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string directionStr = valueGet.second.Get<std::string>();
        StringToVector2(directionStr.c_str(), directionStr.size(), direction);
      }
      else
      {
        direction = valueGet.second.Get<Vector2>();
      }
    }
    else if((DevelText::Emboss::Property::STRENGTH == valueGet.first.indexKey) || (STRENGTH_KEY == valueGet.first.stringKey))
    {
      /// Strength key.
      strengthDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string strengthStr = valueGet.second.Get<std::string>();
        strength                      = StringToFloat(strengthStr.c_str());
      }
      else
      {
        strength = valueGet.second.Get<float>();
      }
    }
    else if((DevelText::Emboss::Property::LIGHT_COLOR == valueGet.first.indexKey) || (LIGHT_COLOR_KEY == valueGet.first.stringKey))
    {
      /// Light Color key.
      lightColorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string lightColorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4(lightColorStr.c_str(), lightColorStr.size(), lightColor);
      }
      else
      {
        lightColor = valueGet.second.Get<Vector4>();
      }
    }
    else if((DevelText::Emboss::Property::SHADOW_COLOR == valueGet.first.indexKey) || (SHADOW_COLOR_KEY == valueGet.first.stringKey))
    {
      /// Shadow Color key.
      shadowColorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string shadowColorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4(shadowColorStr.c_str(), shadowColorStr.size(), shadowColor);
      }
      else
      {
        shadowColor = valueGet.second.Get<Vector4>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseUnderlineProperties(const Property::Map&   underlinePropertiesMap,
                              bool&                  enabled,
                              bool&                  colorDefined,
                              Vector4&               color,
                              bool&                  heightDefined,
                              float&                 height,
                              bool&                  typeDefined,
                              Text::Underline::Type& type,
                              bool&                  dashWidthDefined,
                              float&                 dashWidth,
                              bool&                  dashGapDefined,
                              float&                 dashGap)
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue(index);

    if((DevelText::Underline::Property::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = valueGet.second.Get<std::string>();
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((DevelText::Underline::Property::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((DevelText::Underline::Property::HEIGHT == valueGet.first.indexKey) || (HEIGHT_KEY == valueGet.first.stringKey))
    {
      /// Height key.
      heightDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string heightStr = valueGet.second.Get<std::string>();
        height                      = StringToFloat(heightStr.c_str());
      }
      else
      {
        height = valueGet.second.Get<float>();
      }
    }
    else if((DevelText::Underline::Property::TYPE == valueGet.first.indexKey) || (TYPE_KEY == valueGet.first.stringKey))
    {
      /// Underline Type key.
      typeDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string typeStr = valueGet.second.Get<std::string>();
        Text::UnderlineTypeStringToTypeValue(typeStr.c_str(), typeStr.size(), type);
      }
      else
      {
        type = valueGet.second.Get<Text::Underline::Type>();
      }
    }
    else if((DevelText::Underline::Property::DASH_WIDTH == valueGet.first.indexKey) || (DASH_WIDTH_KEY == valueGet.first.stringKey))
    {
      /// Dashed Underline Width key.
      dashWidthDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string dashWidthStr = valueGet.second.Get<std::string>();
        dashWidth                      = StringToFloat(dashWidthStr.c_str());
      }
      else
      {
        dashWidth = valueGet.second.Get<float>();
      }
    }
    else if((DevelText::Underline::Property::DASH_GAP == valueGet.first.indexKey) || (DASH_GAP_KEY == valueGet.first.stringKey))
    {
      /// Dashed Underline Gap key.
      dashGapDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string dashGapStr = valueGet.second.Get<std::string>();
        dashGap                      = StringToFloat(dashGapStr.c_str());
      }
      else
      {
        dashGap = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseOutlineProperties(const Property::Map& underlinePropertiesMap,
                            bool&                colorDefined,
                            Vector4&             color,
                            bool&                widthDefined,
                            uint16_t&            width,
                            bool&                offsetDefined,
                            Vector2&             offset,
                            bool&                blurRadiusDefined,
                            float&               blurRadius)
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue(index);

    if((DevelText::Outline::Property::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;
      color        = valueGet.second.Get<Vector4>();
    }
    else if((DevelText::Outline::Property::WIDTH == valueGet.first.indexKey) || (WIDTH_KEY == valueGet.first.stringKey))
    {
      /// Width key.
      widthDefined = true;
      width        = static_cast<uint16_t>(valueGet.second.Get<float>());
    }
    else if((DevelText::Outline::Property::OFFSET == valueGet.first.indexKey) || (OFFSET_KEY == valueGet.first.stringKey))
    {
      /// Offset key.
      offsetDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string offsetStr = valueGet.second.Get<std::string>();
        StringToVector2(offsetStr.c_str(), offsetStr.size(), offset);
      }
      else
      {
        offset = valueGet.second.Get<Vector2>();
      }
    }
    else if((DevelText::Outline::Property::BLUR_RADIUS == valueGet.first.indexKey) || (BLUR_RADIUS_KEY == valueGet.first.stringKey))
    {
      /// Blur radius key.
      blurRadiusDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string blurRadiusStr = valueGet.second.Get<std::string>();
        blurRadius                      = StringToFloat(blurRadiusStr.c_str());
      }
      else
      {
        blurRadius = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseBackgroundProperties(const Property::Map& backgroundProperties,
                               bool&                enabled,
                               bool&                colorDefined,
                               Vector4&             color)
{
  const unsigned int numberOfItems = backgroundProperties.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = backgroundProperties.GetKeyValue(index);

    if((DevelText::Background::Property::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      enabled = valueGet.second.Get<bool>();
    }
    else if((DevelText::Background::Property::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;
      color        = valueGet.second.Get<Vector4>();
    }
  }

  return 0u == numberOfItems;
}

bool ParseStrikethroughProperties(const Property::Map& strikethroughPropertiesMap,
                                  bool&                enabled,
                                  bool&                colorDefined,
                                  Vector4&             color,
                                  bool&                heightDefined,
                                  float&               height)
{
  const unsigned int numberOfItems = strikethroughPropertiesMap.Count();

  // Parses and applies the style.
  for(unsigned int index = 0u; index < numberOfItems; ++index)
  {
    const KeyValuePair& valueGet = strikethroughPropertiesMap.GetKeyValue(index);

    if((DevelText::Strikethrough::Property::ENABLE == valueGet.first.indexKey) || (ENABLE_KEY == valueGet.first.stringKey))
    {
      /// Enable key.
      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string enableStr = valueGet.second.Get<std::string>();
        enabled                     = Text::TokenComparison(TRUE_TOKEN, enableStr.c_str(), enableStr.size());
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if((DevelText::Strikethrough::Property::COLOR == valueGet.first.indexKey) || (COLOR_KEY == valueGet.first.stringKey))
    {
      /// Color key.
      colorDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string colorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4(colorStr.c_str(), colorStr.size(), color);
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if((DevelText::Strikethrough::Property::HEIGHT == valueGet.first.indexKey) || (HEIGHT_KEY == valueGet.first.stringKey))
    {
      /// Height key.
      heightDefined = true;

      if(valueGet.second.GetType() == Dali::Property::STRING)
      {
        const std::string heightStr = valueGet.second.Get<std::string>();
        height                      = StringToFloat(heightStr.c_str());
      }
      else
      {
        height = valueGet.second.Get<float>();
      }
    }
  }
  return 0u == numberOfItems;
}

bool SetUnderlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool                  enabled      = false;
        bool                  colorDefined = false;
        Vector4               color;
        bool                  heightDefined = false;
        float                 height        = 0.f;
        bool                  typeDefined   = false;
        Text::Underline::Type type;
        bool                  dashWidthDefined = false;
        float                 dashWidth        = 2.0f;
        bool                  dashGapDefined   = false;
        float                 dashGap          = 1.0f;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = value.Get<std::string>();

          if(!propertyString.empty())
          {
            Property::Map parsedStringMap;
            Text::ParsePropertyString(propertyString, parsedStringMap);

            empty = ParseUnderlineProperties(parsedStringMap,
                                             enabled,
                                             colorDefined,
                                             color,
                                             heightDefined,
                                             height,
                                             typeDefined,
                                             type,
                                             dashWidthDefined,
                                             dashWidth,
                                             dashGapDefined,
                                             dashGap);

            controller->UnderlineSetByString(!empty);
          }
        }
        else
        {
          empty = ParseUnderlineProperties(propertiesMap,
                                           enabled,
                                           colorDefined,
                                           color,
                                           heightDefined,
                                           height,
                                           typeDefined,
                                           type,
                                           dashWidthDefined,
                                           dashWidth,
                                           dashGapDefined,
                                           dashGap);

          controller->UnderlineSetByString(false);
        }

        if(!empty)
        {
          if(enabled != controller->IsUnderlineEnabled())
          {
            controller->SetUnderlineEnabled(enabled);
            update = true;
          }

          // Sets the default underline values.
          if(colorDefined && (controller->GetUnderlineColor() != color))
          {
            controller->SetUnderlineColor(color);
            update = true;
          }

          if(heightDefined && (fabsf(controller->GetUnderlineHeight() - height) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetUnderlineHeight(height);
            update = true;
          }

          if(typeDefined && (controller->GetUnderlineType() != type))
          {
            controller->SetUnderlineType(type);
            update = true;
          }

          if(dashWidthDefined && (fabsf(controller->GetDashedUnderlineWidth() - dashWidth) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetDashedUnderlineWidth(dashWidth);
            update = true;
          }

          if(dashGapDefined && (fabsf(controller->GetDashedUnderlineGap() - dashGap) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetDashedUnderlineGap(dashGap);
            update = true;
          }
        }
        else
        {
          // Disable underline.
          if(controller->IsUnderlineEnabled())
          {
            controller->SetUnderlineEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& underlineProperties = value.Get<std::string>();

        controller->SetInputUnderlineProperties(underlineProperties);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetUnderlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool                  enabled   = controller->IsUnderlineEnabled();
        const Vector4&              color     = controller->GetUnderlineColor();
        const float                 height    = controller->GetUnderlineHeight();
        const Text::Underline::Type type      = controller->GetUnderlineType();
        const float                 dashWidth = controller->GetDashedUnderlineWidth();
        const float                 dashGap   = controller->GetDashedUnderlineGap();

        if(controller->IsUnderlineSetByString())
        {
          std::string       underlineProperties = "{\"enable\":";
          const std::string enabledStr          = enabled ? "true" : "false";
          underlineProperties += "\"" + enabledStr + "\",";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          underlineProperties += "\"color\":\"" + colorStr + "\",";

          std::string heightStr;
          FloatToString(height, heightStr);
          underlineProperties += "\"height\":\"" + heightStr + "\",";

          std::string typeStr;
          typeStr = GetUnderlineTypeToString(type);
          underlineProperties += "\"type\":\"" + typeStr + "\",";

          std::string dashWidthStr;
          FloatToString(dashWidth, dashWidthStr);
          underlineProperties += "\"dashWidth\":\"" + dashWidthStr + "\",";

          std::string dashGapStr;
          FloatToString(dashGap, dashGapStr);
          underlineProperties += "\"dashGap\":\"" + dashGapStr + "\"}";

          value = underlineProperties;
        }
        else
        {
          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(COLOR_KEY, color);
          map.Insert(HEIGHT_KEY, height);
          map.Insert(TYPE_KEY, type);
          map.Insert(DASH_WIDTH_KEY, dashWidth);
          map.Insert(DASH_GAP_KEY, dashGap);

          value = map;
        }

        break;
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputUnderlineProperties();
        break;
      }
    }
  }
}

bool SetShadowProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    colorDefined = false;
        Vector4 color;
        bool    offsetDefined = false;
        Vector2 offset;
        bool    blurRadiusDefined = false;
        float   blurRadius;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = value.Get<std::string>();

          Property::Map parsedStringMap;
          Text::ParsePropertyString(propertyString, parsedStringMap);

          empty = ParseShadowProperties(parsedStringMap,
                                        colorDefined,
                                        color,
                                        offsetDefined,
                                        offset,
                                        blurRadiusDefined,
                                        blurRadius);

          controller->ShadowSetByString(!empty);
        }
        else
        {
          empty = ParseShadowProperties(propertiesMap,
                                        colorDefined,
                                        color,
                                        offsetDefined,
                                        offset,
                                        blurRadiusDefined,
                                        blurRadius);

          controller->ShadowSetByString(false);
        }

        if(!empty)
        {
          // Sets the default shadow values.
          if(colorDefined && (controller->GetShadowColor() != color))
          {
            controller->SetShadowColor(color);
            update = true;
          }

          if(offsetDefined && (controller->GetShadowOffset() != offset))
          {
            controller->SetShadowOffset(offset);
            update = true;
          }

          if(blurRadiusDefined && (!Dali::Equals(controller->GetShadowBlurRadius(), blurRadius)))
          {
            controller->SetShadowBlurRadius(blurRadius);
            update = true;
          }
        }
        else
        {
          // Disable shadow.
          if(Vector2::ZERO != controller->GetShadowOffset())
          {
            controller->SetShadowOffset(Vector2::ZERO);
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& shadowString = value.Get<std::string>();

        controller->SetInputShadowProperties(shadowString);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetShadowProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Vector4& color      = controller->GetShadowColor();
        const Vector2& offset     = controller->GetShadowOffset();
        const float&   blurRadius = controller->GetShadowBlurRadius();

        if(controller->IsShadowSetByString())
        {
          std::string shadowProperties = "{";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          shadowProperties += "\"color\":\"" + colorStr + "\",";

          std::string offsetStr;
          Vector2ToString(offset, offsetStr);
          shadowProperties += "\"offset\":\"" + offsetStr + "\",";

          std::string blurRadiusStr;
          FloatToString(blurRadius, blurRadiusStr);
          shadowProperties += "\"blurRadius\":\"" + blurRadiusStr + "\"}";

          value = shadowProperties;
        }
        else
        {
          Property::Map map;

          map.Insert(COLOR_KEY, color);
          map.Insert(OFFSET_KEY, offset);
          map.Insert(BLUR_RADIUS_KEY, blurRadius);

          value = map;
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputShadowProperties();
        break;
      }
    }
  }
}

bool SetEmbossProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        if(!propertiesMap.Empty())
        {
          bool    enabled          = false;
          bool    directionDefined = false;
          Vector2 direction;
          bool    strengthDefined   = false;
          float   strength          = 0.f;
          bool    lightColorDefined = false;
          Vector4 lightColor;
          bool    shadowColorDefined = false;
          Vector4 shadowColor;

          bool empty = ParseEmbossProperties(propertiesMap,
                                             enabled,
                                             directionDefined,
                                             direction,
                                             strengthDefined,
                                             strength,
                                             lightColorDefined,
                                             lightColor,
                                             shadowColorDefined,
                                             shadowColor);

          if(!empty)
          {
            if(enabled != controller->IsEmbossEnabled())
            {
              controller->SetEmbossEnabled(enabled);
              update = true;
            }

            // Sets the default emboss values.
            if(directionDefined && (controller->GetEmbossDirection() != direction))
            {
              controller->SetEmbossDirection(direction);
              update = true;
            }

            if(strengthDefined && (fabsf(controller->GetEmbossStrength() - strength) > Math::MACHINE_EPSILON_1000))
            {
              controller->SetEmbossStrength(strength);
              update = true;
            }

            if(lightColorDefined && (controller->GetEmbossLightColor() != lightColor))
            {
              controller->SetEmbossLightColor(lightColor);
              update = true;
            }

            if(shadowColorDefined && (controller->GetEmbossShadowColor() != shadowColor))
            {
              controller->SetEmbossShadowColor(shadowColor);
              update = true;
            }
          }
          else
          {
            // Disable emboss.
            if(controller->IsEmbossEnabled())
            {
              controller->SetEmbossEnabled(false);
              update = true;
            }
          }

          // Note : This code is for keep legacy logics, which is not using Property::Map.
          controller->SetDefaultEmbossProperties(EMPTY_STRING);
        }
        else
        {
          // Disable emboss.
          if(controller->IsEmbossEnabled())
          {
            controller->SetEmbossEnabled(false);
            update = true;
          }

          // Note : This code is for keep legacy logics, which is not using Property::Map.
          const std::string embossString = value.Get<std::string>();

          controller->SetDefaultEmbossProperties(embossString);
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& embossString = value.Get<std::string>();

        controller->SetInputEmbossProperties(embossString);
        break;
      }
    }
  }

  return update;
}

void GetEmbossProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        // Note : This code is for keep legacy logics, which is not using Property::Map.
        const std::string embossString = controller->GetDefaultEmbossProperties();
        if(embossString.empty())
        {
          const bool     enabled     = controller->IsEmbossEnabled();
          const Vector2& direction   = controller->GetEmbossDirection();
          const float    strength    = controller->GetEmbossStrength();
          const Vector4& lightColor  = controller->GetEmbossLightColor();
          const Vector4& shadowColor = controller->GetEmbossShadowColor();

          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(DIRECTION_KEY, direction);
          map.Insert(STRENGTH_KEY, strength);
          map.Insert(LIGHT_COLOR_KEY, lightColor);
          map.Insert(SHADOW_COLOR_KEY, shadowColor);

          value = map;
        }
        else
        {
          value = embossString;
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputEmbossProperties();
        break;
      }
    }
  }
}

bool SetOutlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool     colorDefined = false;
        Vector4  color;
        bool     widthDefined  = false;
        uint16_t width         = 0u;
        bool     offsetDefined = false;
        Vector2  offset;
        bool     blurRadiusDefined = false;
        float    blurRadius;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          // This is purely to maintain backward compatibility, but we don't parse the string to be a property map.
          const std::string propertyString = value.Get<std::string>();

          // Stores the default outline's properties string to be recovered by the GetOutlineProperties() function.
          controller->SetDefaultOutlineProperties(propertyString);

          controller->OutlineSetByString(true);
        }
        else
        {
          empty = ParseOutlineProperties(propertiesMap,
                                         colorDefined,
                                         color,
                                         widthDefined,
                                         width,
                                         offsetDefined,
                                         offset,
                                         blurRadiusDefined,
                                         blurRadius);

          controller->OutlineSetByString(false);
        }

        if(!empty)
        {
          // Sets the default outline values.
          if(colorDefined && (controller->GetOutlineColor() != color))
          {
            controller->SetOutlineColor(color);
            update = true;
          }

          if(widthDefined && (controller->GetOutlineWidth() != width))
          {
            controller->SetOutlineWidth(width);
            update = true;
          }

          if(offsetDefined && (controller->GetOutlineOffset() != offset))
          {
            controller->SetOutlineOffset(offset);
            update = true;
          }

          if(blurRadiusDefined && (!Dali::Equals(controller->GetOutlineBlurRadius(), blurRadius)))
          {
            controller->SetOutlineBlurRadius(blurRadius);
            update = true;
          }
        }
        else
        {
          // Disable outline
          if(0u != controller->GetOutlineWidth())
          {
            controller->SetOutlineWidth(0u);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& outlineProperties = value.Get<std::string>();

        controller->SetInputOutlineProperties(outlineProperties);
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetOutlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        if(controller->IsOutlineSetByString())
        {
          value = controller->GetDefaultOutlineProperties();
          break;
        }
        else
        {
          const Vector4& color      = controller->GetOutlineColor();
          const uint16_t width      = controller->GetOutlineWidth();
          const Vector2& offset     = controller->GetOutlineOffset();
          const float&   blurRadius = controller->GetOutlineBlurRadius();

          Property::Map map;
          map.Insert(COLOR_KEY, color);
          map.Insert(WIDTH_KEY, static_cast<int>(width));
          map.Insert(OFFSET_KEY, offset);
          map.Insert(BLUR_RADIUS_KEY, blurRadius);

          value = map;

          break;
        }
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputOutlineProperties();
        break;
      }
    }
  }
}

bool SetBackgroundProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    enabled      = false;
        bool    colorDefined = false;
        Vector4 color;

        bool empty = true;

        if(!propertiesMap.Empty())
        {
          empty = ParseBackgroundProperties(propertiesMap,
                                            enabled,
                                            colorDefined,
                                            color);
        }

        if(!empty)
        {
          if(enabled != controller->IsBackgroundEnabled())
          {
            controller->SetBackgroundEnabled(enabled);
            update = true;
          }

          if(colorDefined && (controller->GetBackgroundColor() != color))
          {
            controller->SetBackgroundColor(color);
            update = true;
          }
        }
        else
        {
          // Disable background.
          if(controller->IsBackgroundEnabled())
          {
            controller->SetBackgroundEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        // Text background is not supported while inputting yet
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetBackgroundProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool     enabled = controller->IsBackgroundEnabled();
        const Vector4& color   = controller->GetBackgroundColor();

        Property::Map map;
        map.Insert(ENABLE_KEY, enabled);
        map.Insert(COLOR_KEY, color);

        value = map;

        break;
      }
      case EffectStyle::INPUT:
      {
        // Text background is not supported while inputting yet
        break;
      }
    }
  }
}

bool SetStrikethroughProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type)
{
  bool update = false;

  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool    enabled      = false;
        bool    colorDefined = false;
        Vector4 color;
        bool    heightDefined = false;
        float   height        = 0.f;

        bool empty = true;

        if(propertiesMap.Empty())
        {
          // Map empty so check if a string provided
          const std::string propertyString = value.Get<std::string>();

          if(!propertyString.empty())
          {
            Property::Map parsedStringMap;
            Text::ParsePropertyString(propertyString, parsedStringMap);

            empty = ParseStrikethroughProperties(parsedStringMap,
                                                 enabled,
                                                 colorDefined,
                                                 color,
                                                 heightDefined,
                                                 height);

            controller->StrikethroughSetByString(!empty);
          }
        }
        else
        {
          empty = ParseStrikethroughProperties(propertiesMap,
                                               enabled,
                                               colorDefined,
                                               color,
                                               heightDefined,
                                               height);

          controller->StrikethroughSetByString(false);
        }

        if(!empty)
        {
          if(enabled != controller->IsStrikethroughEnabled())
          {
            controller->SetStrikethroughEnabled(enabled);
            update = true;
          }

          // Sets the default strikethrough values.
          if(colorDefined && (controller->GetStrikethroughColor() != color))
          {
            controller->SetStrikethroughColor(color);
            update = true;
          }
          if(heightDefined && (fabsf(controller->GetStrikethroughHeight() - height) > Math::MACHINE_EPSILON_1000))
          {
            controller->SetStrikethroughHeight(height);
            update = true;
          }
        }
        else
        {
          // Disable strikethrough.
          if(controller->IsStrikethroughEnabled())
          {
            controller->SetStrikethroughEnabled(false);
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& strikethroughProperties = value.Get<std::string>();

        controller->SetInputStrikethroughProperties(strikethroughProperties);
        update = true;
        break;
      }
    } // switch
  } // if( controller )

  return update;
}

void GetStrikethroughProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type)
{
  if(controller)
  {
    switch(type)
    {
      case EffectStyle::DEFAULT:
      {
        const bool     enabled = controller->IsStrikethroughEnabled();
        const Vector4& color   = controller->GetStrikethroughColor();
        const float    height  = controller->GetStrikethroughHeight();

        if(controller->IsStrikethroughSetByString())
        {
          std::string       strikethroughProperties = "{\"enable\":";
          const std::string enabledStr              = enabled ? "true" : "false";
          strikethroughProperties += "\"" + enabledStr + "\",";

          std::string colorStr;
          Vector4ToColorString(color, colorStr);
          strikethroughProperties += "\"color\":\"" + colorStr + "\",";

          std::string heightStr;
          FloatToString(height, heightStr);
          strikethroughProperties += "\"height\":\"" + heightStr + "\"}";

          value = strikethroughProperties;
        }
        else
        {
          Property::Map map;

          map.Insert(ENABLE_KEY, enabled);
          map.Insert(COLOR_KEY, color);
          map.Insert(HEIGHT_KEY, height);

          value = map;
        }

        break;
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputStrikethroughProperties();
        break;
      }
    }
  }
}

Underline::Type StringToUnderlineType(const char* const underlineTypeStr)
{
  Underline::Type underlineType = Text::Underline::SOLID;
  Scripting::GetEnumeration<Underline::Type>(underlineTypeStr,
                                             UNDERLINE_TYPE_STRING_TABLE,
                                             UNDERLINE_TYPE_STRING_TABLE_COUNT,
                                             underlineType);

  return underlineType;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
