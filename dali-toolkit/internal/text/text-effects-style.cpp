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
const std::string THICKNESS_KEY( "thickness" );
}

bool SetUnderlineProperties( ControllerPtr controller, const Property::Value& value, EffectStyle::Type type )
{
  bool update = false;

  if( controller )
  {
    const std::string properties = value.Get< std::string >();

    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        // Stores the default underline's properties string to be recovered by the GetUnderlineProperties() function.
        controller->SetDefaultUnderlineProperties( properties );
        break;
      }
      case EffectStyle::INPUT:
      {
        // Stores the input underline's properties string to be recovered by the GetUnderlineProperties() function.
        controller->SetInputUnderlineProperties( properties );
        break;
      }
    }

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

      /// Thickness key
      Property::Value* thicknessValue = map.Find( THICKNESS_KEY );

      float thickness = 0.f;
      const bool thicknessDefined = thicknessValue != NULL;
      if( thicknessDefined )
      {
        const std::string thicknessStr = thicknessValue->Get<std::string>();

        thickness = StringToFloat( thicknessStr.c_str() );
      }

      switch( type )
      {
        case EffectStyle::DEFAULT:
        {
          if( !controller->IsUnderlineEnabled() )
          {
            controller->SetUnderlineEnabled( true );
            update = true;
          }
          // Sets the default underline values.
          if( colorDefined && ( controller->GetUnderlineColor() != color ) )
          {
            controller->SetUnderlineColor( color );
            update = true;
          }

          if( thicknessDefined &&  fabsf( controller->GetUnderlineHeight() - thickness ) > Math::MACHINE_EPSILON_1000 )
          {
            controller->SetUnderlineHeight( thickness );
            update = true;
          }
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
      // Disable underline.
      if( controller->IsUnderlineEnabled() )
      {
        controller->SetUnderlineEnabled( false );
        update = true;
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
        value = controller->GetDefaultUnderlineProperties();
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

    switch( type )
    {
      case EffectStyle::DEFAULT:
      {
        // Stores the default shadow's properties string to be recovered by the GetShadowProperties() function.
        controller->SetDefaultShadowProperties( properties );
        break;
      }
      case EffectStyle::INPUT:
      {
        // Stores the input shadow's properties string to be recovered by the GetShadowProperties() function.
        controller->SetInputShadowProperties( properties );
        break;
      }
    }

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

        StringOffsetToVector2( offsetStr, offset );
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
        }
        case EffectStyle::INPUT:
        {
          // Sets the input shadow values.
          // TODO: to be implemented.
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
        value = controller->GetDefaultShadowProperties();
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
