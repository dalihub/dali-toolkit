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

// CLASS HEADER
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

using Dali::Toolkit::Text::LayoutEngine;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;
}

namespace
{

#if defined ( DEBUG_ENABLED )
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextLabel, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "renderingBackend",     INTEGER, RENDERING_BACKEND      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "text",                 STRING,  TEXT                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "fontFamily",           STRING,  FONT_FAMILY            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "fontStyle",            STRING,  FONT_STYLE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "pointSize",            FLOAT,   POINT_SIZE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "multiLine",            BOOLEAN, MULTI_LINE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "horizontalAlignment",  STRING,  HORIZONTAL_ALIGNMENT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "verticalAlignment",    STRING,  VERTICAL_ALIGNMENT     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "textColor",            VECTOR4, TEXT_COLOR             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadowOffset",         VECTOR2, SHADOW_OFFSET          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadowColor",          VECTOR4, SHADOW_COLOR           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineEnabled",     BOOLEAN, UNDERLINE_ENABLED      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineColor",       VECTOR4, UNDERLINE_COLOR        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineHeight",      FLOAT,   UNDERLINE_HEIGHT       )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "enableMarkup",         BOOLEAN, ENABLE_MARKUP          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "enableAutoScroll",     BOOLEAN, ENABLE_AUTO_SCROLL     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollSpeed",      INTEGER, AUTO_SCROLL_SPEED      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollLoopCount",  INTEGER, AUTO_SCROLL_LOOP_COUNT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollGap",        FLOAT,   AUTO_SCROLL_GAP        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "lineSpacing",          FLOAT,   LINE_SPACING           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underline",            STRING,  UNDERLINE              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadow",               STRING,  SHADOW                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "emboss",               STRING,  EMBOSS                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "outline",              STRING,  OUTLINE                )

DALI_TYPE_REGISTRATION_END()

const int TEXT_VISUAL_PROPERTY_TABLE[] = {
  Dali::Toolkit::TextVisual::Property::RENDERING_BACKEND,
  Dali::Toolkit::TextVisual::Property::TEXT,
  Dali::Toolkit::TextVisual::Property::FONT_FAMILY,
  Dali::Toolkit::TextVisual::Property::FONT_STYLE,
  Dali::Toolkit::TextVisual::Property::POINT_SIZE,
  Dali::Toolkit::TextVisual::Property::MULTI_LINE,
  Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT,
  Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT,
  Dali::Toolkit::TextVisual::Property::TEXT_COLOR,
  Dali::Toolkit::TextVisual::Property::SHADOW,
  Dali::Toolkit::TextVisual::Property::SHADOW,
  Dali::Toolkit::TextVisual::Property::UNDERLINE,
  Dali::Toolkit::TextVisual::Property::UNDERLINE,
  Dali::Toolkit::TextVisual::Property::UNDERLINE,
  Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP,
  Dali::Toolkit::TextVisual::Property::ENABLE_AUTO_SCROLL,
  Dali::Toolkit::TextVisual::Property::AUTO_SCROLL_SPEED,
  Dali::Toolkit::TextVisual::Property::AUTO_SCROLL_LOOP_COUNT,
  Dali::Toolkit::TextVisual::Property::AUTO_SCROLL_GAP,
  Dali::Toolkit::TextVisual::Property::LINE_SPACING,
  Dali::Toolkit::TextVisual::Property::UNDERLINE,
  Dali::Toolkit::TextVisual::Property::SHADOW,
  Dali::Toolkit::TextVisual::Property::EMBOSS,
  Dali::Toolkit::TextVisual::Property::OUTLINE,
  Dali::Toolkit::TextVisual::Property::BATCHING_ENABLED
};

} // namespace

Toolkit::TextLabel TextLabel::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextLabel > impl = new TextLabel();

  // Pass ownership to CustomActor handle
  Toolkit::TextLabel handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextLabel::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& impl = GetImpl( label );

    // Sets the property to the to the text visual.
    impl.mVisual.SetProperty( TEXT_VISUAL_PROPERTY_TABLE[index - Toolkit::TextLabel::PROPERTY_START_INDEX], value );
  }
}

Property::Value TextLabel::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& impl = GetImpl( label );

    if( ( Toolkit::TextLabel::Property::SHADOW_OFFSET == index ) ||
        ( Toolkit::TextLabel::Property::SHADOW_COLOR == index ) ||
        ( Toolkit::TextLabel::Property::UNDERLINE_ENABLED == index ) ||
        ( Toolkit::TextLabel::Property::UNDERLINE_COLOR == index ) ||
        ( Toolkit::TextLabel::Property::UNDERLINE_HEIGHT == index ) )
    {
      // TODO : Branch to be removed when the deprecated properties are finally removed.
      value = impl.mVisual.GetProperty( TEXT_VISUAL_PROPERTY_TABLE[index - Toolkit::TextLabel::PROPERTY_START_INDEX] );

      switch( index )
      {
        case Toolkit::TextLabel::Property::SHADOW_OFFSET: // Converts the deprecated property to the new one.
        {
          bool colorDefined = false;
          Vector4 color;
          bool offsetDefined = false;
          Vector2 offset;
          const bool empty = Text::ParseProperties( value.Get<std::string>(),
                                                    colorDefined,
                                                    color,
                                                    offsetDefined,
                                                    offset );

          if( !empty )
          {
            value = offset;
          }
          break;
        }
        case Toolkit::TextLabel::Property::SHADOW_COLOR: // Converts the deprecated property to the new one.
        {
          bool colorDefined = false;
          Vector4 color;
          bool offsetDefined = false;
          Vector2 offset;
          const bool empty = Text::ParseProperties( value.Get<std::string>(),
                                                    colorDefined,
                                                    color,
                                                    offsetDefined,
                                                    offset );

          if( !empty )
          {
            value = color;
          }
          break;
        }
        case Toolkit::TextLabel::Property::UNDERLINE_ENABLED: // Converts the deprecated property to the new one.
        {
          bool enabled = false;
          bool colorDefined = false;
          Vector4 color;
          bool heightDefined = false;
          float height = 0.f;
          const bool empty = Text::ParseProperties( value.Get<std::string>(),
                                                    enabled,
                                                    colorDefined,
                                                    color,
                                                    heightDefined,
                                                    height );

          if( !empty )
          {
            value = enabled;
          }
          break;
        }
        case Toolkit::TextLabel::Property::UNDERLINE_COLOR: // Converts the deprecated property to the new one.
        {
          bool enabled = false;
          bool colorDefined = false;
          Vector4 color;
          bool heightDefined = false;
          float height = 0.f;
          const bool empty = Text::ParseProperties( value.Get<std::string>(),
                                                    enabled,
                                                    colorDefined,
                                                    color,
                                                    heightDefined,
                                                    height );

          if( !empty && colorDefined )
          {
            value = color;
          }
          break;
        }
        case Toolkit::TextLabel::Property::UNDERLINE_HEIGHT: // Converts the deprecated property to the new one.
        {
          bool enabled = false;
          bool colorDefined = false;
          Vector4 color;
          bool heightDefined = false;
          float height = 0.f;
          const bool empty = Text::ParseProperties( value.Get<std::string>(),
                                                    enabled,
                                                    colorDefined,
                                                    color,
                                                    heightDefined,
                                                    height );

          if( !empty && heightDefined )
          {
            value = height;
          }
          break;
        }
        default:
        {
          // Nothing to do.
          break;
        }
      }
    }
    else
    {
      // Retrieves the property from the text visual.
      value = impl.mVisual.GetProperty( TEXT_VISUAL_PROPERTY_TABLE[index - Toolkit::TextLabel::PROPERTY_START_INDEX] );
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  Actor self = Self();

  // Creates the text's visual.
  Property::Map visualMap;
  visualMap[Toolkit::VisualProperty::TYPE] = Toolkit::Visual::TEXT;
  visualMap[Toolkit::TextVisual::Property::RENDERING_BACKEND] = static_cast<int>( DEFAULT_RENDERING_BACKEND );

  mVisual =  Toolkit::VisualFactory::Get().CreateVisual( visualMap );
  RegisterVisual( Toolkit::TextLabel::Property::TEXT, self, mVisual );

  Internal::Visual::Base& visualBase = Toolkit::GetImplementation( mVisual );
  TextVisual* textVisual = static_cast<TextVisual*>( &visualBase );

  // Sets the text's control interface.
  textVisual->SetTextControlInterface( this );

  // Use height-for-width negotiation by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
}

void TextLabel::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextLabel::OnStyleChange\n");

  switch ( change )
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      // Property system did not set the font so should update it.
      const std::string& newFont = GetImpl( styleManager ).GetDefaultFontFamily();
      DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnStyleChange StyleChange::DEFAULT_FONT_CHANGE newFont(%s)\n", newFont.c_str() );

      const std::string fontString = "{\"family\":\"" + newFont + "\",\"type\":\"system\"}";
      mVisual.SetProperty( Toolkit::TextVisual::Property::FONT_FAMILY, fontString );
      break;
    }
    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
      break;
    }
    case StyleChange::THEME_CHANGE:
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
      break;
    }
  }
}

Vector3 TextLabel::GetNaturalSize()
{
  Vector2 naturalSize;
  mVisual.GetNaturalSize( naturalSize );

  return Vector3( naturalSize );
}

float TextLabel::GetHeightForWidth( float width )
{
  return mVisual.GetHeightForWidth( width );
}

void TextLabel::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnRelayout\n" );

  mVisual.SetSize( size );
}

void TextLabel::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextLabel::OnStageConnection( int depth )
{
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // The depth of the text renderer is set by the text-visual called from OnRelayout().
}

TextLabel::TextLabel()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
