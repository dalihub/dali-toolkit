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
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>

using Dali::Toolkit::Text::LayoutEngine;
using Dali::Toolkit::Text::Backend;

namespace
{

const unsigned int DEFAULT_RENDERING_BACKEND = 0;

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextLabel, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( TextLabel, "rendering-backend", INTEGER, RENDERING_BACKEND )
DALI_PROPERTY_REGISTRATION( TextLabel, "text",              STRING,  TEXT              )
DALI_PROPERTY_REGISTRATION( TextLabel, "font-family",       STRING,  FONT_FAMILY       )
DALI_PROPERTY_REGISTRATION( TextLabel, "font-style",        STRING,  FONT_STYLE        )
DALI_PROPERTY_REGISTRATION( TextLabel, "point-size",        FLOAT,   POINT_SIZE        )
DALI_PROPERTY_REGISTRATION( TextLabel, "multi-line",        BOOLEAN, MULTI_LINE        )

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
        unsigned int backend = value.Get< int >();

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = static_cast< unsigned int >( backend );
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

          if( impl.mController->GetDefaultPointSize() != pointSize /*TODO - epsilon*/ )
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
            impl.mController->GetLayoutEngine().SetLayout( layout );
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
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  mController = Text::Controller::New( *this );
}

Vector3 TextLabel::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextLabel::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextLabel::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  if( mController->Relayout( size ) ||
      !mRenderer )
  {
    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

    RenderableActor renderableActor;
    if( mRenderer )
    {
      renderableActor = mRenderer->Render( mController->GetView() );
    }

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mRenderableActor );

      if( renderableActor )
      {
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

TextLabel::TextLabel()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
