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

// FILE HEADER
#include <dali-toolkit/internal/text/text-effects-style.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-style-properties-devel.h>
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
const std::string BLUR_RADIUS_KEY( "blurRadius" );
const std::string WIDTH_KEY( "width" );
const std::string HEIGHT_KEY( "height" );
const std::string ENABLE_KEY( "enable" );
const std::string TRUE_TOKEN( "true" );
const std::string FALSE_TOKEN( "false" );
}

bool ParseShadowProperties( const Property::Map& shadowPropertiesMap,
                            bool& colorDefined,
                            Vector4& color,
                            bool& offsetDefined,
                            Vector2& offset,
                            bool& blurRadiusDefined,
                            float& blurRadius )
{
  const unsigned int numberOfItems = shadowPropertiesMap.Count();

  // Parses and applies the style.
  for( unsigned int index = 0u; index < numberOfItems; ++index )
  {
    const KeyValuePair& valueGet = shadowPropertiesMap.GetKeyValue( index );

    if( ( DevelText::Shadow::Property::COLOR == valueGet.first.indexKey ) || ( COLOR_KEY == valueGet.first.stringKey ) )
    {
      /// Color key.
      colorDefined = true;

      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string colorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if( ( DevelText::Shadow::Property::OFFSET == valueGet.first.indexKey ) || ( OFFSET_KEY == valueGet.first.stringKey ) )
    {
      /// Offset key.
      offsetDefined = true;

      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string offsetStr = valueGet.second.Get<std::string>();
        StringToVector2( offsetStr.c_str(), offsetStr.size(), offset );
      }
      else
      {
        offset = valueGet.second.Get<Vector2>();
      }
    }
    else if( ( DevelText::Shadow::Property::BLUR_RADIUS == valueGet.first.indexKey ) || ( BLUR_RADIUS_KEY == valueGet.first.stringKey ) )
    {
      /// Blur radius key.
      blurRadiusDefined = true;

      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string blurRadiusStr = valueGet.second.Get<std::string>();
        blurRadius = StringToFloat( blurRadiusStr.c_str() );
      }
      else
      {
        blurRadius = valueGet.second.Get<float>();
      }
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

    if( ( DevelText::Underline::Property::ENABLE == valueGet.first.indexKey ) || ( ENABLE_KEY == valueGet.first.stringKey ) )
    {
      /// Enable key.
      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string enableStr = valueGet.second.Get<std::string>();
        enabled = Text::TokenComparison( TRUE_TOKEN, enableStr.c_str(), enableStr.size() );
      }
      else
      {
        enabled = valueGet.second.Get<bool>();
      }
    }
    else if( ( DevelText::Underline::Property::COLOR == valueGet.first.indexKey ) || ( COLOR_KEY == valueGet.first.stringKey ) )
    {
      /// Color key.
      colorDefined = true;

      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string colorStr = valueGet.second.Get<std::string>();
        Text::ColorStringToVector4( colorStr.c_str(), colorStr.size(), color );
      }
      else
      {
        color = valueGet.second.Get<Vector4>();
      }
    }
    else if( ( DevelText::Underline::Property::HEIGHT == valueGet.first.indexKey ) || ( HEIGHT_KEY == valueGet.first.stringKey ) )
    {
      /// Height key.
      heightDefined = true;

      if( valueGet.second.GetType() == Dali::Property::STRING )
      {
        const std::string heightStr = valueGet.second.Get<std::string>();
        height = StringToFloat( heightStr.c_str() );
      }
      else
      {
        height = valueGet.second.Get<float>();
      }
    }
  }

  return 0u == numberOfItems;
}

bool ParseOutlineProperties( const Property::Map& underlinePropertiesMap,
                               bool& colorDefined,
                               Vector4& color,
                               bool& widthDefined,
                               uint16_t& width )
{
  const unsigned int numberOfItems = underlinePropertiesMap.Count();

  // Parses and applies the style.
  for( unsigned int index = 0u; index < numberOfItems; ++index )
  {
    const KeyValuePair& valueGet = underlinePropertiesMap.GetKeyValue( index );

    if( ( DevelText::Outline::Property::COLOR == valueGet.first.indexKey ) || ( COLOR_KEY == valueGet.first.stringKey ) )
    {
      /// Color key.
      colorDefined = true;
      color = valueGet.second.Get<Vector4>();
    }
    else if( ( DevelText::Outline::Property::WIDTH == valueGet.first.indexKey ) || ( WIDTH_KEY == valueGet.first.stringKey ) )
    {
      /// Width key.
      widthDefined = true;
      width = static_cast<uint16_t>( valueGet.second.Get<float>() );
    }
  }

  return 0u == numberOfItems;
}

bool ParseBackgroundProperties( const Property::Map& backgroundProperties,
                                bool& enabled,
                                bool& colorDefined,
                                Vector4& color )
{
  const unsigned int numberOfItems = backgroundProperties.Count();

  // Parses and applies the style.
  for( unsigned int index = 0u; index < numberOfItems; ++index )
  {
    const KeyValuePair& valueGet = backgroundProperties.GetKeyValue( index );

    if( ( DevelText::Background::Property::ENABLE == valueGet.first.indexKey ) || ( ENABLE_KEY == valueGet.first.stringKey ) )
    {
      /// Enable key.
      enabled = valueGet.second.Get<bool>();
    }
    else if( ( DevelText::Background::Property::COLOR == valueGet.first.indexKey ) || ( COLOR_KEY == valueGet.first.stringKey ) )
    {
      /// Color key.
      colorDefined = true;
      color = valueGet.second.Get<Vector4>();
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

        bool empty = true;

        if ( propertiesMap.Empty() )
        {
          // Map empty so check if a string provided
          const std::string propertyString = value.Get<std::string>();

          if ( !propertyString.empty() )
          {
            Property::Map parsedStringMap;
            Text::ParsePropertyString( propertyString, parsedStringMap );

            empty = ParseUnderlineProperties( parsedStringMap,
                                              enabled,
                                              colorDefined,
                                              color,
                                              heightDefined,
                                              height );

            controller->UnderlineSetByString( !empty );
          }
        }
        else
        {
           empty = ParseUnderlineProperties( propertiesMap,
                                             enabled,
                                             colorDefined,
                                             color,
                                             heightDefined,
                                             height );

           controller->UnderlineSetByString( false );
        }

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

        if ( controller->IsUnderlineSetByString() )
        {
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
        }
        else
        {
          Property::Map map;

          map.Insert( ENABLE_KEY, enabled );
          map.Insert( COLOR_KEY, color );
          map.Insert( HEIGHT_KEY, height );

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
        bool blurRadiusDefined = false;
        float blurRadius;

        bool empty = true;

        if ( propertiesMap.Empty() )
        {
           // Map empty so check if a string provided
           const std::string propertyString = value.Get<std::string>();

           Property::Map parsedStringMap;
           Text::ParsePropertyString( propertyString, parsedStringMap );

           empty = ParseShadowProperties( parsedStringMap,
                                          colorDefined,
                                          color,
                                          offsetDefined,
                                          offset,
                                          blurRadiusDefined,
                                          blurRadius );

           controller->ShadowSetByString( !empty );

        }
        else
        {
          empty = ParseShadowProperties( propertiesMap,
                                         colorDefined,
                                         color,
                                         offsetDefined,
                                         offset,
                                         blurRadiusDefined,
                                         blurRadius );

          controller->ShadowSetByString( false );
        }

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

          if( blurRadiusDefined && ( controller->GetShadowBlurRadius() != blurRadius ) )
          {
            controller->SetShadowBlurRadius( blurRadius );
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
        const float& blurRadius = controller->GetShadowBlurRadius();

        if ( controller->IsShadowSetByString() )
        {
          std::string shadowProperties = "{";

          std::string colorStr;
          Vector4ToColorString( color, colorStr );
          shadowProperties += "\"color\":\"" + colorStr + "\",";

          std::string offsetStr;
          Vector2ToString( offset, offsetStr );
          shadowProperties += "\"offset\":\"" + offsetStr + "\",";

          std::string blurRadiusStr;
          FloatToString( blurRadius, blurRadiusStr );
          shadowProperties += "\"blurRadius\":\"" + blurRadiusStr + "\"}";

          value = shadowProperties;
        }
        else
        {
          Property::Map map;

          map.Insert( COLOR_KEY, color );
          map.Insert( OFFSET_KEY, offset );
          map.Insert( BLUR_RADIUS_KEY, blurRadius );

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
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool colorDefined = false;
        Vector4 color;
        bool widthDefined = false;
        uint16_t width = 0u;

        bool empty = true;

        if ( propertiesMap.Empty() )
        {
           // Map empty so check if a string provided
           // This is purely to maintain backward compatibility, but we don't parse the string to be a property map.
           const std::string propertyString = value.Get<std::string>();

           // Stores the default outline's properties string to be recovered by the GetOutlineProperties() function.
           controller->SetDefaultOutlineProperties( propertyString );

           controller->OutlineSetByString( true );
        }
        else
        {
           empty = ParseOutlineProperties( propertiesMap,
                                           colorDefined,
                                           color,
                                           widthDefined,
                                           width );

           controller->OutlineSetByString( false );
        }

        if( !empty )
        {
          // Sets the default outline values.
          if( colorDefined && ( controller->GetOutlineColor() != color ) )
          {
            controller->SetOutlineColor( color );
            update = true;
          }

          if( widthDefined && ( controller->GetOutlineWidth() != width ) )
          {
            controller->SetOutlineWidth( width );
            update = true;
          }
        }
        else
        {
          // Disable outline
          if( 0u != controller->GetOutlineWidth() )
          {
            controller->SetOutlineWidth( 0u );
            update = true;
          }
        }
        break;
      }
      case EffectStyle::INPUT:
      {
        const std::string& outlineProperties = value.Get<std::string>();

        controller->SetInputOutlineProperties( outlineProperties );
        break;
      }
    } // switch
  } // if( controller )

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
        if ( controller->IsOutlineSetByString() )
        {
          value = controller->GetDefaultOutlineProperties();
          break;
        }
        else
        {
          const Vector4& color = controller->GetOutlineColor();
          const uint16_t width = controller->GetOutlineWidth();

          Property::Map map;
          map.Insert( COLOR_KEY, color );
          map.Insert( WIDTH_KEY, static_cast<int>( width ) );

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

bool SetBackgroundProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
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

        bool empty = true;

        if ( !propertiesMap.Empty() )
        {
           empty = ParseBackgroundProperties( propertiesMap,
                                              enabled,
                                              colorDefined,
                                              color );
        }

        if( !empty )
        {
          if( enabled != controller->IsBackgroundEnabled() )
          {
            controller->SetBackgroundEnabled( enabled );
            update = true;
          }

          if( colorDefined && ( controller->GetBackgroundColor() != color ) )
          {
            controller->SetBackgroundColor( color );
            update = true;
          }
        }
        else
        {
          // Disable background.
          if( controller->IsBackgroundEnabled() )
          {
            controller->SetBackgroundEnabled( false );
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

void GetBackgroundProperties( ControllerPtr controller, Property::Value& value, EffectStyle::Type type )
{
  if( controller )
  {
    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        const bool enabled = controller->IsBackgroundEnabled();
        const Vector4& color = controller->GetBackgroundColor();

        Property::Map map;
        map.Insert( ENABLE_KEY, enabled );
        map.Insert( COLOR_KEY, color );

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


} // namespace Text

} // namespace Toolkit

} // namespace Dali
