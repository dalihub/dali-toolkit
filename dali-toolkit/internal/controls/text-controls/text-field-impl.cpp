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
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/layouts/layout-engine.h>
#include <dali-toolkit/public-api/text/rendering/text-backend.h>

using namespace Dali::Toolkit::Text;

namespace
{

const unsigned int DEFAULT_RENDERING_BACKEND = 0;

} // namespace


namespace Dali
{

namespace Toolkit
{

const Property::Index TextField::PROPERTY_RENDERING_BACKEND(      Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX );
const Property::Index TextField::PROPERTY_PLACEHOLDER_TEXT(       Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 1 );
const Property::Index TextField::PROPERTY_TEXT(                   Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 2 );
const Property::Index TextField::PROPERTY_CURSOR_IMAGE(           Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 3 );
const Property::Index TextField::PROPERTY_PRIMARY_CURSOR_COLOR(   Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 4 );
const Property::Index TextField::PROPERTY_SECONDARY_CURSOR_COLOR( Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 5 );
const Property::Index TextField::PROPERTY_ENABLE_CURSOR_BLINK(    Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 6 );
const Property::Index TextField::PROPERTY_CURSOR_BLINK_INTERVAL(  Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 7 );
const Property::Index TextField::PROPERTY_CURSOR_BLINK_DURATION(  Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 8 );
const Property::Index TextField::PROPERTY_GRAB_HANDLE_IMAGE(      Internal::TextField::TEXTFIELD_PROPERTY_START_INDEX + 9 );

namespace Internal
{

namespace
{

// Type registration
BaseHandle Create()
{
  return Toolkit::TextField::New();
}

TypeRegistration mType( typeid(Toolkit::TextField), typeid(Toolkit::Control), Create );

PropertyRegistration property01( mType, "rendering-backend",      Toolkit::TextField::PROPERTY_RENDERING_BACKEND,      Property::INTEGER,          &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property02( mType, "placeholder-text",       Toolkit::TextField::PROPERTY_PLACEHOLDER_TEXT,       Property::STRING,           &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property03( mType, "text",                   Toolkit::TextField::PROPERTY_TEXT,                   Property::STRING,           &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property04( mType, "cursor-image",           Toolkit::TextField::PROPERTY_CURSOR_IMAGE,           Property::STRING,           &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property05( mType, "primary-cursor-color",   Toolkit::TextField::PROPERTY_PRIMARY_CURSOR_COLOR,   Property::VECTOR4,          &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property06( mType, "secondary-cursor-color", Toolkit::TextField::PROPERTY_SECONDARY_CURSOR_COLOR, Property::VECTOR4,          &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property07( mType, "enable-cursor-blink",    Toolkit::TextField::PROPERTY_ENABLE_CURSOR_BLINK,    Property::BOOLEAN,          &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property08( mType, "cursor-blink-interval",  Toolkit::TextField::PROPERTY_CURSOR_BLINK_INTERVAL,  Property::FLOAT,            &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property09( mType, "cursor-blink-duration",  Toolkit::TextField::PROPERTY_CURSOR_BLINK_DURATION,  Property::FLOAT,            &TextField::SetProperty, &TextField::GetProperty );
PropertyRegistration property10( mType, "grab-handle-image",      Toolkit::TextField::PROPERTY_GRAB_HANDLE_IMAGE,      Property::STRING,           &TextField::SetProperty, &TextField::GetProperty );

} // namespace

Toolkit::TextField TextField::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextField > impl = new TextField();

  // Pass ownership to CustomActor handle
  Toolkit::TextField handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextField::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextField textField = Toolkit::TextField::DownCast( Dali::BaseHandle( object ) );

  if( textField )
  {
    TextField& impl( GetImpl( textField ) );

    switch( index )
    {
      case Toolkit::TextField::PROPERTY_RENDERING_BACKEND:
      {
        unsigned int backend = value.Get< unsigned int >();

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          //impl.mController->SetPlaceholderText( value.Get< std::string >() ); TODO
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_TEXT:
      {
        if( impl.mController )
        {
          impl.mController->SetText( value.Get< std::string >() );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetCursorImage( image );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetColor( PRIMARY_CURSOR, value.Get< Vector4 >() );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetColor( SECONDARY_CURSOR, value.Get< Vector4 >() );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_ENABLE_CURSOR_BLINK:
      {
        if( impl.mController )
        {
          //impl.mController->SetEnableCursorBlink( value.Get< bool >() ); TODO
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetCursorBlinkInterval( value.Get< float >() );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetCursorBlinkDuration( value.Get< float >() );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_GRAB_HANDLE_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetGrabHandleImage( image );
        }
        break;
      }
    }
  }
}

Property::Value TextField::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextField textField = Toolkit::TextField::DownCast( Dali::BaseHandle( object ) );

  if( textField )
  {
    TextField& impl( GetImpl( textField ) );

    switch( index )
    {
      case Toolkit::TextField::PROPERTY_RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextField::PROPERTY_PLACEHOLDER_TEXT:
      {
        DALI_LOG_WARNING( "UTF-8 text representation was discarded\n" );
        break;
      }
      case Toolkit::TextField::PROPERTY_TEXT:
      {
        DALI_LOG_WARNING( "UTF-8 text representation was discarded\n" );
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetCursorImage() );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( PRIMARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( SECONDARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_ENABLE_CURSOR_BLINK:
      {
        //value = impl.mController->GetEnableCursorBlink(); TODO
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkInterval();
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkDuration();
        }
        break;
      }
      case Toolkit::TextField::PROPERTY_GRAB_HANDLE_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetCursorImage() );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
    }
  }

  return value;
}

void TextField::OnInitialize()
{
  mController = Text::Controller::New( *this );

  mDecorator = Text::Decorator::New( *this, *mController );

  mController->GetLayoutEngine().SetLayout( LayoutEngine::SINGLE_LINE_BOX );

  mController->EnableTextInput( mDecorator );

  // Forward input events to controller
  EnableGestureDetection( Gesture::Tap );

  // TODO - Fix TapGestureDetector to support single and double tap
  mDoubleTapDetector = TapGestureDetector::New();
  mDoubleTapDetector.SetTapsRequired( 2 );
  mDoubleTapDetector.DetectedSignal().Connect( this, &TextField::OnDoubleTap );
  mDoubleTapDetector.Attach(Self());
}

void TextField::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  if( mController->Relayout( size ) )
  {
    if( mDecorator )
    {
      mDecorator->Relayout( size );
    }

    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

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

void TextField::OnTap( const TapGesture& tap )
{
  mController->TapEvent( tap.numberOfTaps, tap.localPoint.x, tap.localPoint.y );
}

void TextField::OnDoubleTap( Actor actor, const TapGesture& tap )
{
  mController->TapEvent( tap.numberOfTaps, tap.localPoint.x, tap.localPoint.y );
}

void TextField::RequestTextRelayout()
{
  RelayoutRequest();
}

TextField::TextField()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND )
{
}

TextField::~TextField()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
