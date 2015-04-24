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
#include <string>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/virtual-keyboard.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

using namespace Dali::Toolkit::Text;

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
  return Toolkit::TextField::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextField, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "rendering-backend",                    INTEGER,   RENDERING_BACKEND                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholder-text",                     STRING,    PLACEHOLDER_TEXT                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "text",                                 STRING,    TEXT                                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "font-family",                          STRING,    FONT_FAMILY                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "font-style",                           STRING,    FONT_STYLE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "point-size",                           FLOAT,     POINT_SIZE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "exceed-policy",                        INTEGER,   EXCEED_POLICY                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "horizontal-alignment",                 STRING,    HORIZONTAL_ALIGNMENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "vertical-alignment",                   STRING,    VERTICAL_ALIGNMENT                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "text-color",                           VECTOR4,   TEXT_COLOR                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "shadow-offset",                        VECTOR2,   SHADOW_OFFSET                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "shadow-color",                         VECTOR4,   SHADOW_COLOR                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "primary-cursor-color",                 VECTOR4,   PRIMARY_CURSOR_COLOR                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "secondary-cursor-color",               VECTOR4,   SECONDARY_CURSOR_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "enable-cursor-blink",                  BOOLEAN,   ENABLE_CURSOR_BLINK                  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursor-blink-interval",                FLOAT,     CURSOR_BLINK_INTERVAL                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursor-blink-duration",                FLOAT,     CURSOR_BLINK_DURATION                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grab-handle-image",                    STRING,    GRAB_HANDLE_IMAGE                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grab-handle-pressed-image",            VECTOR4,   GRAB_HANDLE_PRESSED_IMAGE            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scroll-threshold",                     FLOAT,     SCROLL_THRESHOLD                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scroll-speed",                         FLOAT,     SCROLL_SPEED                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-image-left",          STRING,    SELECTION_HANDLE_IMAGE_LEFT          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-image-right",         STRING,    SELECTION_HANDLE_IMAGE_RIGHT         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-pressed-image-left",  STRING,    SELECTION_HANDLE_PRESSED_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-pressed-image-right", STRING,    SELECTION_HANDLE_PRESSED_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-highlight-color",            STRING,    SELECTION_HIGHLIGHT_COLOR            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "decoration-bounding-box",              RECTANGLE, DECORATION_BOUNDING_BOX              )

DALI_TYPE_REGISTRATION_END()

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
      case Toolkit::TextField::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          //impl.mController->SetPlaceholderText( value.Get< std::string >() ); TODO
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT:
      {
        if( impl.mController )
        {
          impl.mController->SetText( value.Get< std::string >() );
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_FAMILY:
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
      case Toolkit::TextField::Property::FONT_STYLE:
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
      case Toolkit::TextField::Property::POINT_SIZE:
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
      case Toolkit::TextField::Property::EXCEED_POLICY:
      {
        impl.mExceedPolicy = value.Get< int >();
        break;
      }
      case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
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
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
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
      case Toolkit::TextField::Property::TEXT_COLOR:
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
      case Toolkit::TextField::Property::SHADOW_OFFSET:
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
      case Toolkit::TextField::Property::SHADOW_COLOR:
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
      case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetColor( PRIMARY_CURSOR, value.Get< Vector4 >() );
        }
        break;
      }
      case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetColor( SECONDARY_CURSOR, value.Get< Vector4 >() );
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
      {
        if( impl.mController )
        {
          impl.mController->SetEnableCursorBlink( value.Get< bool >() );
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetCursorBlinkInterval( value.Get< float >() );
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetCursorBlinkDuration( value.Get< float >() );
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_THRESHOLD:
      {
        float threshold = value.Get< float >();

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollThreshold( threshold );
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_SPEED:
      {
        float speed = value.Get< float >();

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollSpeed( speed );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        Vector4 color = value.Get< Vector4 >();

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHighlightColor( color );
        }
        break;
      }
      case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          impl.mDecorator->SetBoundingBox( value.Get< Rect<int> >() );
        }
        break;
      }
    } // switch
  } // textfield
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
      case Toolkit::TextField::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetPlaceholderText( text );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::EXCEED_POLICY:
      {
        value = impl.mExceedPolicy;
        break;
      }
      case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::HorizontalAlignment >( impl.mController->GetLayoutEngine().GetHorizontalAlignment(),
                                                                                                                  HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                                  HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT ) );
        }
        break;
      }
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          value = std::string( Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( impl.mController->GetLayoutEngine().GetVerticalAlignment(),
                                                                                                                  VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                                  VERTICAL_ALIGNMENT_STRING_TABLE_COUNT ) );
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetTextColor();
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW_OFFSET:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowOffset();
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowColor();
        }
        break;
      }
      case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( PRIMARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( SECONDARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
      {
        value = impl.mController->GetEnableCursorBlink();
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkInterval();
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkDuration();
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_THRESHOLD:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollThreshold();
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_SPEED:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollSpeed();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetHighlightColor();
        }
        break;
      }
      case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetBoundingBox();
        }
        break;
      }
    } //switch
  }

  return value;
}

void TextField::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( *this );

  mDecorator = Text::Decorator::New( *this, *mController );

  mController->GetLayoutEngine().SetLayout( LayoutEngine::SINGLE_LINE_BOX );

  mController->EnableTextInput( mDecorator );

  // Forward input events to controller
  EnableGestureDetection(Gesture::Tap);
  GetTapGestureDetector().SetMaximumTapsRequired( 2 );
  EnableGestureDetection(Gesture::Pan);

  // Set BoundingBox to stage size if not already set.
  if ( mDecorator->GetBoundingBox().IsEmpty() )
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox( Rect<int>( 0.0f, 0.0f, stageSize.width, stageSize.height ) );
  }

  // Fill-parent area by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
}

void TextField::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange change )
{
  GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
}

Vector3 TextField::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextField::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextField::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  if( mController->Relayout( size ) ||
      !mRenderer )
  {
    if( mDecorator )
    {
      mDecorator->Relayout( size );
    }

    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

    RenderableActor renderableActor;
    if( mRenderer )
    {
      renderableActor = mRenderer->Render( mController->GetView() );
    }

    EnableClipping( (Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == mExceedPolicy), size );

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mRenderableActor );
      mRenderableActor = renderableActor;
    }

    if( mRenderableActor )
    {
      const Vector2 offset = mController->GetScrollPosition() + mController->GetAlignmentOffset();

      mRenderableActor.SetPosition( offset.x, offset.y );

      // Make sure the actor is parented correctly with/without clipping
      if( mClipper )
      {
        mClipper->GetRootActor().Add( mRenderableActor );
      }
      else
      {
        Self().Add( mRenderableActor );
      }
    }
  }
}

void TextField::OnKeyInputFocusGained()
{
  VirtualKeyboard::StatusChangedSignal().Connect( this, &TextField::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();

  if ( imfManager )
  {
    imfManager.EventReceivedSignal().Connect( this, &TextField::OnImfEvent );

    // Notify that the text editing start.
    imfManager.Activate();

    // When window gain lost focus, the imf manager is deactivated. Thus when window gain focus again, the imf manager must be activated.
    imfManager.SetRestoreAfterFocusLost( true );
  }

  mController->KeyboardFocusGainEvent();

  EmitKeyInputFocusSignal( true ); // Calls back into the Control hence done last.
}

void TextField::OnKeyInputFocusLost()
{
  VirtualKeyboard::StatusChangedSignal().Disconnect( this, &TextField::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    // The text editing is finished. Therefore the imf manager don't have restore activation.
    imfManager.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    imfManager.Deactivate();

    imfManager.EventReceivedSignal().Disconnect( this, &TextField::OnImfEvent );
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal( false ); // Calls back into the Control hence done last.
}

void TextField::OnTap( const TapGesture& gesture )
{
  // Show the keyboard if it was hidden.
  if (!VirtualKeyboard::IsVisible())
  {
    VirtualKeyboard::Show();
  }

  SetKeyInputFocus();

  mController->TapEvent( gesture.numberOfTaps, gesture.localPoint.x, gesture.localPoint.y );
}

void TextField::OnPan( const PanGesture& gesture )
{
  mController->PanEvent( gesture.state, gesture.displacement );
}

bool TextField::OnKeyEvent( const KeyEvent& event )
{
  if( Dali::DALI_KEY_ESCAPE == event.keyCode )
  {
    ClearKeyInputFocus();
  }

  return mController->KeyEvent( event );
}

ImfManager::ImfCallbackData TextField::OnImfEvent( Dali::ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent )
{
  switch ( imfEvent.eventName )
  {
    case ImfManager::COMMIT:
    {
      KeyEvent event( "", imfEvent.predictiveString, 0, 0, 0, KeyEvent::Down );
      mController->KeyEvent( event );
      break;
    }
    case ImfManager::PREEDIT: // fall through
    case ImfManager::DELETESURROUNDING:
    case ImfManager::GETSURROUNDING:
    case ImfManager::VOID:
    {
      // do nothing
    }
  } // end switch

  return ImfManager::ImfCallbackData();
}

void TextField::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextField::EnableClipping( bool clipping, const Vector2& size )
{
  if( clipping )
  {
    // Not worth to created clip actor if width or height is equal to zero.
    if( size.width > Math::MACHINE_EPSILON_1000 && size.height > Math::MACHINE_EPSILON_1000 )
    {
      if( !mClipper )
      {
        Actor self = Self();

        mClipper = Clipper::New( size );
        self.Add( mClipper->GetRootActor() );
        self.Add( mClipper->GetImageActor() );
      }
      else if ( mClipper )
      {
        mClipper->Refresh( size );
      }
    }
  }
  else
  {
    // Note - this will automatically remove the root & image actors
    mClipper.Reset();
  }
}

void TextField::KeyboardStatusChanged(bool keyboardShown)
{
  // Just hide the grab handle when keyboard is hidden.
  if (!keyboardShown )
  {
    mController->KeyboardFocusLostEvent();
  }
}

TextField::TextField()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mExceedPolicy( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP )
{
}

TextField::~TextField()
{
  mClipper.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
