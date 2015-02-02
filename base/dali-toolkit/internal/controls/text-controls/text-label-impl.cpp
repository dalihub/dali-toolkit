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

namespace
{

} // namespace

namespace Dali
{

namespace Toolkit
{

const Property::Index TextLabel::PROPERTY_TEXT( Internal::TextLabel::TEXTLABEL_PROPERTY_START_INDEX );

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

PropertyRegistration property1( mType, "text", Toolkit::TextLabel::PROPERTY_TEXT, Property::STRING, &TextLabel::SetProperty, &TextLabel::GetProperty );

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

void TextLabel::SetRenderer( Text::RendererPtr renderer )
{
  mRenderer = renderer;
}

void TextLabel::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& labelImpl( GetImpl( label ) );
    switch( index )
    {
      case Toolkit::TextLabel::PROPERTY_TEXT:
      {
        labelImpl.SetText( value.Get< std::string >() );
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
    switch( index )
    {
      case Toolkit::TextLabel::PROPERTY_TEXT:
      {
        DALI_LOG_WARNING( "UTF-8 text representation was discarded\n" );
        break;
      }
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  mController = Text::Controller::New();
}

void TextLabel::SetText( const std::string& text )
{
  if( mController )
  {
    // The Controller updates the View for the renderer
    mController->SetText( text );

    if( mRenderer )
    {
      Actor renderableActor = mRenderer->Render( mController->GetView() );

      if( renderableActor )
      {
        Self().Add( renderableActor );
      }
    }
  }
}

TextLabel::TextLabel()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
