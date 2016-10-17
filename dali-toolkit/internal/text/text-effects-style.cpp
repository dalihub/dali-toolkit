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
#include <dali-toolkit/internal/text/text-effects-style.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/property-string-parser.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
const std::string COLOR_KEY( "color" );
const std::string OFFSET_KEY( "offset" );
const std::string HEIGHT_KEY( "height" );
const std::string ENABLE_KEY( "enable" );
const std::string TRUE_TOKEN( "true" );
}

bool SetUnderlineProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    const std::string properties = value.Get<std::string>();

    // Parses and applies the style.
    Property::Map map;
    ParsePropertyString( properties, map );

    if( !map.Empty() )
    {
      /// Enable key.
      Property::Value* enableValue = map.Find( ENABLE_KEY );

      bool enabled = false;
      const bool enabledDefined = enableValue != NULL;
      if( enabledDefined )
      {
        const std::string enableStr = enableValue->Get<std::string>();
        enabled = TokenComparison( TRUE_TOKEN, enableStr.c_str(), enableStr.size() );
      }

      /// Color key.
      Property::Value* colorValue = map.Find( COLOR_KEY );

      Vector4 color;
      const bool colorDefined = colorValue != NULL;
      if( colorDefined )
      {
        const std::string colorStr = colorValue->Get<std::string>();

        ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
      }

      /// Height key.
      Property::Value* heightValue = map.Find( HEIGHT_KEY );

      float height = 0.f;
      const bool heightDefined = heightValue != NULL;
      if( heightDefined )
      {
        const std::string heightStr = heightValue->Get<std::string>();

        height = StringToFloat( heightStr.c_str() );
      }

      switch( type )
      {
        case EffectStyle::DEFAULT:
        {
          if( enabled != controller->IsUnderlineEnabled() )
          {
            controller->SetUnderlineEnabled( enabled );
            update = true;
          }
          // Sets the default underline values.
          if( colorDefined && ( controller->GetUnderlineColor() != color ) )
          {
            controller->SetUnderlineColor( color );
            update = true;
          }

          if( heightDefined && fabsf( controller->GetUnderlineHeight() - height ) > Math::MACHINE_EPSILON_1000 )
          {
            controller->SetUnderlineHeight( height );
            update = true;
          }
          break;
        }
        case EffectStyle::INPUT:
        {
          // Sets the input underline values.
          // TODO: to be implemented.
          break;
        }
      }
    }
    else
    {
      switch( type )
      {
        case EffectStyle::DEFAULT:
        {
          // Disable underline.
          if( controller->IsUnderlineEnabled() )
          {
            controller->SetUnderlineEnabled( false );
            update = true;
          }
          break;
        }
        case EffectStyle::INPUT:
        {
          // Sets the input underline values.
          // TODO: to be implemented.
          controller->SetInputUnderlineProperties( properties );
          break;
        }
      }
    }
  }

  return update;
}

void GetUnderlineProperties( ControllerPtr controller, Property::Value& value, EffectStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const bool enabled = controller->IsUnderlineEnabled();
        const Vector4& color = controller->GetUnderlineColor();
        const float height = controller->GetUnderlineHeight();

        std::string underlineProperties = "{\"enable\":";
        const std::string enabledStr = enabled ? "true" : "false";
        underlineProperties += "\"" + enabledStr + "\",";

        std::string colorStr;
        Vector4ToColorString( color, colorStr );
        underlineProperties += "\"color\":\"" + colorStr + "\",";

        std::string heightStr;
        FloatToString( height, heightStr );
        underlineProperties += "\"height\":\"" + heightStr + "\"}";

        value = underlineProperties;
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

bool SetShadowProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    const std::string properties = value.Get< std::string >();

    // Parses and applies the style.
    Property::Map map;
    ParsePropertyString( properties, map );

    if( !map.Empty() )
    {
      /// Color key
      Property::Value* colorValue = map.Find( COLOR_KEY );

      Vector4 color;
      const bool colorDefined = colorValue != NULL;
      if( colorDefined )
      {
        const std::string colorStr = colorValue->Get<std::string>();

        ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
      }

      /// Offset key
      Property::Value* offsetValue = map.Find( OFFSET_KEY );

      Vector2 offset;
      const bool offsetDefined = offsetValue != NULL;
      if( offsetDefined )
      {
        const std::string offsetStr = offsetValue->Get<std::string>();

        StringToVector2( offsetStr.c_str(), offsetStr.size(), offset );
      }

      switch( type )
      {
        case EffectStyle::DEFAULT:
        {
          // Sets the default shadow values.
          if( colorDefined && ( controller->GetShadowColor() != color ) )
          {
            controller->SetShadowColor( color );
            update = true;
          }

          if( offsetDefined && ( controller->GetShadowOffset() != offset ) )
          {
            controller->SetShadowOffset( offset );
            update = true;
          }
          break;
        }
        case EffectStyle::INPUT:
        {
          // Sets the input shadow values.
          // TODO: to be implemented.
          break;
        }
      }
    }
    else
    {
      switch( type )
      {
        case EffectStyle::DEFAULT:
        {
          // Disable shadow.
          if( Vector2::ZERO != controller->GetShadowOffset() )
          {
            controller->SetShadowOffset( Vector2::ZERO );
          }
          break;
        }
        case EffectStyle::INPUT:
        {
          // Sets the input shadow values.
          // TODO: to be implemented.
          controller->SetInputShadowProperties( properties );
          break;
        }
      }
    }
  }

  return update;
}

void GetShadowProperties( ControllerPtr controller, Property::Value& value, EffectStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const Vector4& color = controller->GetShadowColor();
        const Vector2& offset = controller->GetShadowOffset();

        std::string shadowProperties = "{";

        std::string colorStr;
        Vector4ToColorString( color, colorStr );
        shadowProperties += "\"color\":\"" + colorStr + "\",";

        std::string offsetStr;
        Vector2ToString( offset, offsetStr );
        shadowProperties += "\"offset\":\"" + offsetStr + "\"}";

        value = shadowProperties;
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

bool SetEmbossProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    const std::string properties = value.Get< std::string >();

    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        // Stores the default emboss's properties string to be recovered by the GetEmbossProperties() function.
        controller->SetDefaultEmbossProperties( properties );
        break;
      }
      case EffectStyle::INPUT:
      {
        // Stores the input emboss's properties string to be recovered by the GetEmbossProperties() function.
        controller->SetInputEmbossProperties( properties );
        break;
      }
    }
  }

  return update;
}

void GetEmbossProperties( ControllerPtr controller, Property::Value& value, EffectStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        value = controller->GetDefaultEmbossProperties();
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

bool SetOutlineProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    const std::string properties = value.Get< std::string >();

    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        // Stores the default outline's properties string to be recovered by the GetOutlineProperties() function.
        controller->SetDefaultOutlineProperties( properties );
        break;
      }
      case EffectStyle::INPUT:
      {
        // Stores the input outline's properties string to be recovered by the GetOutlineProperties() function.
        controller->SetInputOutlineProperties( properties );
        break;
      }
    }
  }

  return update;
}

void GetOutlineProperties( ControllerPtr controller, Property::Value& value, EffectStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        value = controller->GetDefaultOutlineProperties();
        break;
      }
      case EffectStyle::INPUT:
      {
        value = controller->GetInputOutlineProperties();
        break;
      }
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
