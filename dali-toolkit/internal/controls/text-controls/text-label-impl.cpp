/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

using Dali::Toolkit::Text::LayoutEngine;
using Dali::Toolkit::Text::Backend;

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

const Scripting::StringEnum< Toolkit::Text::LayoutEngine::HorizontalAlignment > HORIZONTAL_ALIGNMENT_STRING_TABLE[] =
{
  { "BEGIN",  Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_BEGIN  },
  { "CENTER", Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_CENTER },
  { "END",    Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_END    },
};
const unsigned int HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE ) / sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE[0] );

const Scripting::StringEnum< Toolkit::Text::LayoutEngine::VerticalAlignment > VERTICAL_ALIGNMENT_STRING_TABLE[] =
{
  { "TOP",    Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_TOP    },
  { "CENTER", Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_CENTER },
  { "BOTTOM", Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_BOTTOM },
};
const unsigned int VERTICAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( VERTICAL_ALIGNMENT_STRING_TABLE ) / sizeof( VERTICAL_ALIGNMENT_STRING_TABLE[0] );

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextLabel, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "rendering-backend",    INTEGER, RENDERING_BACKEND    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "text",                 STRING,  TEXT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "font-family",          STRING,  FONT_FAMILY          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "font-style",           STRING,  FONT_STYLE           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "point-size",           FLOAT,   POINT_SIZE           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "multi-line",           BOOLEAN, MULTI_LINE           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "horizontal-alignment", STRING,  HORIZONTAL_ALIGNMENT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "vertical-alignment",   STRING,  VERTICAL_ALIGNMENT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "text-color",           VECTOR4, TEXT_COLOR           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadow-offset",        VECTOR2, SHADOW_OFFSET        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadow-color",         VECTOR4, SHADOW_COLOR         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underline-enabled",    BOOLEAN, UNDERLINE_ENABLED    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underline-color",      VECTOR4, UNDERLINE_COLOR      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underline-height",     FLOAT,   UNDERLINE_HEIGHT     )

DALI_TYPE_REGISTRATION_END()

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
    TextLabel& impl( GetImpl( label ) );
    switch( index )
    {
      case Toolkit::TextLabel::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        if( impl.mController )
        {
          impl.mController->SetText( value.Get< std::string >() );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          std::string fontFamily = value.Get< std::string >();

          if( impl.mController->GetDefaultFontFamily() != fontFamily )
          {
            impl.mController->SetDefaultFontFamily( fontFamily );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_STYLE:
      {
        if( impl.mController )
        {
          std::string fontStyle = value.Get< std::string >();

          if( impl.mController->GetDefaultFontStyle() != fontStyle )
          {
            impl.mController->SetDefaultFontStyle( fontStyle );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          float pointSize = value.Get< float >();

          if( !Equals( impl.mController->GetDefaultPointSize(), pointSize ) )
          {
            impl.mController->SetDefaultPointSize( pointSize );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        if( impl.mController )
        {
          LayoutEngine& engine = impl.mController->GetLayoutEngine();
          LayoutEngine::Layout layout = value.Get< bool >() ? LayoutEngine::MULTI_LINE_BOX : LayoutEngine::SINGLE_LINE_BOX;

          if( engine.GetLayout() != layout )
          {
            engine.SetLayout( layout );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
      {
        LayoutEngine& engine = impl.mController->GetLayoutEngine();
        const LayoutEngine::HorizontalAlignment alignment = Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::HorizontalAlignment >( value.Get< std::string >().c_str(),
                                                                                                                                           HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                                                           HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );

        if( engine.GetHorizontalAlignment() != alignment )
        {
          engine.SetHorizontalAlignment( alignment );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
      {
        LayoutEngine& engine = impl.mController->GetLayoutEngine();
        const LayoutEngine::VerticalAlignment alignment = Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::VerticalAlignment >( value.Get< std::string >().c_str(),
                                                                                                                                       VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                                                       VERTICAL_ALIGNMENT_STRING_TABLE_COUNT );

        if( engine.GetVerticalAlignment() != alignment )
        {
          engine.SetVerticalAlignment( alignment );
          impl.RequestTextRelayout();
        }
        break;
      }

      case Toolkit::TextLabel::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          Vector4 textColor = value.Get< Vector4 >();
          if ( impl.mController->GetTextColor() != textColor )
          {
            impl.mController->SetTextColor( textColor );
            impl.RequestTextRelayout();
          }
        }
        break;
      }

      case Toolkit::TextLabel::Property::SHADOW_OFFSET:
      {
        if( impl.mController )
        {
          Vector2 shadowOffset = value.Get< Vector2 >();
          if ( impl.mController->GetShadowOffset() != shadowOffset )
          {
            impl.mController->SetShadowOffset( shadowOffset );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_COLOR:
      {
        if( impl.mController )
        {
          Vector4 shadowColor = value.Get< Vector4 >();
          if ( impl.mController->GetShadowColor() != shadowColor )
          {
            impl.mController->SetShadowColor( shadowColor );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_COLOR:
      {
        if( impl.mController )
        {
          Vector4 color = value.Get< Vector4 >();
          if ( impl.mController->GetUnderlineColor() != color )
          {
            impl.mController->SetUnderlineColor( color );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_ENABLED:
      {
        if( impl.mController )
        {
          bool enabled = value.Get< bool >();
          if ( impl.mController->IsUnderlineEnabled() != enabled )
          {
            impl.mController->SetUnderlineEnabled( enabled );
            impl.RequestTextRelayout();
          }
        }
        break;
      }

      case Toolkit::TextLabel::Property::UNDERLINE_HEIGHT:
      {
        if( impl.mController )
        {
          float height = value.Get< float >();
          if ( impl.mController->GetUnderlineHeight() != height )
          {
            impl.mController->SetUnderlineHeight( height );
            impl.RequestTextRelayout();
          }
        }
        break;
      }
    }
  }
}

Property::Value TextLabel::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& impl( GetImpl( label ) );
    switch( index )
    {
      case Toolkit::TextLabel::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          value = text;
        }
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        if( impl.mController )
        {
          value = static_cast<bool>( LayoutEngine::MULTI_LINE_BOX == impl.mController->GetLayoutEngine().GetLayout() );
        }
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::HorizontalAlignment >( impl.mController->GetLayoutEngine().GetHorizontalAlignment(),
                                                                                                                  HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                                  HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT ) );
        }
        break;
      }
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( impl.mController->GetLayoutEngine().GetVerticalAlignment(),
                                                                                                                VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                                VERTICAL_ALIGNMENT_STRING_TABLE_COUNT ) );
        }
        break;
      }
      case Toolkit::TextLabel::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetTextColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_OFFSET:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowOffset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetUnderlineColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_ENABLED:
      {
        if ( impl.mController )
        {
          value = impl.mController->IsUnderlineEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_HEIGHT:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetUnderlineHeight();
        }
        break;
      }
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( *this );

  // Use height-for-width negotiation by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
}

void TextLabel::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange change )
{
  GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
}

Vector3 TextLabel::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextLabel::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextLabel::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  if( mController->Relayout( size ) ||
      !mRenderer )
  {
    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

    Actor renderableActor;
    if( mRenderer )
    {
      renderableActor = mRenderer->Render( mController->GetView() );
    }

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mRenderableActor );

      if( renderableActor )
      {
        const Vector2& alignmentOffset = mController->GetAlignmentOffset();
        renderableActor.SetPosition( alignmentOffset.x, alignmentOffset.y );

        Self().Add( renderableActor );
      }

      mRenderableActor = renderableActor;
    }
  }
}

void TextLabel::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextLabel::MaxLengthReached()
{
  // Pure Virtual from TextController Interface, only needed when inputting text
}

TextLabel::TextLabel()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
