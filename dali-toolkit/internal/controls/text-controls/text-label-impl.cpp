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

const Property::Index TextLabel::PROPERTY_RENDERING_BACKEND( Internal::TextLabel::TEXTLABEL_PROPERTY_START_INDEX );
const Property::Index TextLabel::PROPERTY_TEXT(              Internal::TextLabel::TEXTLABEL_PROPERTY_START_INDEX + 1 );
const Property::Index TextLabel::PROPERTY_MULTI_LINE(        Internal::TextLabel::TEXTLABEL_PROPERTY_START_INDEX + 2 );

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

TypeRegistration mType( typeid(Toolkit::TextLabel), typeid(Toolkit::Control), Create );

PropertyRegistration property1( mType, "rendering-backend", Toolkit::TextLabel::PROPERTY_RENDERING_BACKEND, Property::INTEGER,          &TextLabel::SetProperty, &TextLabel::GetProperty );
PropertyRegistration property2( mType, "text",              Toolkit::TextLabel::PROPERTY_TEXT,              Property::STRING,           &TextLabel::SetProperty, &TextLabel::GetProperty );
PropertyRegistration property3( mType, "multi-line",        Toolkit::TextLabel::PROPERTY_MULTI_LINE,        Property::STRING,           &TextLabel::SetProperty, &TextLabel::GetProperty );

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
      case Toolkit::TextLabel::PROPERTY_RENDERING_BACKEND:
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
      case Toolkit::TextLabel::PROPERTY_TEXT:
      {
        if( impl.mController )
        {
          impl.mController->SetText( value.Get< std::string >() );
        }
        break;
      }
      case Toolkit::TextLabel::PROPERTY_MULTI_LINE:
      {
        if( impl.mController )
        {
          LayoutEngine::Layout layout = value.Get< bool >() ? LayoutEngine::MULTI_LINE_BOX : LayoutEngine::SINGLE_LINE_BOX;
          impl.mController->GetLayoutEngine().SetLayout( layout );
          impl.RequestTextRelayout();
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
      case Toolkit::TextLabel::PROPERTY_RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }

      case Toolkit::TextLabel::PROPERTY_TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          value = text;
        }

        DALI_LOG_WARNING( "UTF-8 text representation was discarded\n" );
        break;
      }

      case Toolkit::TextLabel::PROPERTY_MULTI_LINE:
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
