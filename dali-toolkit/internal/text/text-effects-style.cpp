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
const std::string FALSE_TOKEN( "false" );
}

bool ParseShadowProperties( const Property::Map& shadowPropertiesMap,
                            bool& colorDefined,
                            Vector4& color,
                            bool& offsetDefined,
                            Vector2& offset )
{
  const unsigned int numberOfItems = shadowPropertiesMap.Count();

  // Parses and applies the style.
  for( unsigned int index = 0u; index < numberOfItems; ++index )
  {
    const KeyValuePair& valueGet = shadowPropertiesMap.GetKeyValue( index );

    if( COLOR_KEY == valueGet.first.stringKey )
    {
      /// Color key.
      colorDefined = true;

      const std::string colorStr = valueGet.second.Get<std::string>();

      Text::ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
    }
    else if( OFFSET_KEY == valueGet.first.stringKey )
    {
      /// Offset key.
      offsetDefined = true;

      const std::string offsetStr = valueGet.second.Get<std::string>();

      StringToVector2( offsetStr.c_str(), offsetStr.size(), offset );
    }
  }

  return 0u == numberOfItems;
}

bool ParseUnderlineProperties( const Property::Map& underlinePropertiesMap,
                               bool& enabled,
                               bool& colorDefined,
                               Vector4& color,
                               bool& heightDefined,
                               float& height )
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for( unsigned int index = 0u; index < numberOfItems; ++index )
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue( index );

    if( ENABLE_KEY == valueGet.first.stringKey )
    {
      /// Enable key.
      const std::string enableStr = valueGet.second.Get<std::string>();
      enabled = Text::TokenComparison( TRUE_TOKEN, enableStr.c_str(), enableStr.size() );
    }
    else if( COLOR_KEY == valueGet.first.stringKey )
    {
      /// Color key.
      colorDefined = true;

      const std::string colorStr = valueGet.second.Get<std::string>();

      Text::ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
    }
    else if( HEIGHT_KEY == valueGet.first.stringKey )
    {
      /// Height key.
      heightDefined = true;

      const std::string heightStr = valueGet.second.Get<std::string>();

      height = StringToFloat( heightStr.c_str() );
    }
  }

  return 0u == numberOfItems;
}

bool SetUnderlineProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool enabled = false;
        bool colorDefined = false;
        Vector4 color;
        bool heightDefined = false;
        float height = 0.f;

        const bool empty = ParseUnderlineProperties( propertiesMap,
                                                     enabled,
                                                     colorDefined,
                                                     color,
                                                     heightDefined,
                                                     height );

        if( !empty )
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

          if( heightDefined && ( fabsf( controller->GetUnderlineHeight() - height ) > Math::MACHINE_EPSILON_1000 ) )
          {
            controller->SetUnderlineHeight( height );
            update = true;
          }
        }
        else
        {
          // Disable underline.
          if( controller->IsUnderlineEnabled() )
          {
            controller->SetUnderlineEnabled( false );
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& underlineProperties = value.Get<std::string>();

        controller->SetInputUnderlineProperties( underlineProperties );
        break;
      }
    } // switch
  } // if( controller )

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

        Property::Map map;

        const std::string enabledStr = enabled ? TRUE_TOKEN : FALSE_TOKEN;
        map.Insert( ENABLE_KEY, enabledStr );

        std::string colorStr;
        Vector4ToColorString( color, colorStr );
        map.Insert( COLOR_KEY, colorStr );

        std::string heightStr;
        FloatToString( height, heightStr );
        map.Insert( HEIGHT_KEY, heightStr );

        value = map;
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
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool colorDefined = false;
        Vector4 color;
        bool offsetDefined = false;
        Vector2 offset;

        const bool empty = ParseShadowProperties( propertiesMap,
                                                  colorDefined,
                                                  color,
                                                  offsetDefined,
                                                  offset );

        if( !empty )
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
        }
        else
        {
          // Disable shadow.
          if( Vector2::ZERO != controller->GetShadowOffset() )
          {
            controller->SetShadowOffset( Vector2::ZERO );
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& shadowString = value.Get<std::string>();

        controller->SetInputShadowProperties( shadowString );
        break;
      }
    } // switch
  } // if( controller )

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

        Property::Map map;

        std::string colorStr;
        Vector4ToColorString( color, colorStr );
        map.Insert( COLOR_KEY, colorStr );

        std::string offsetStr;
        Vector2ToString( offset, offsetStr );
        map.Insert( OFFSET_KEY, offsetStr );

        value = map;
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
