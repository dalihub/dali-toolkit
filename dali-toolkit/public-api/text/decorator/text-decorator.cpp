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
#include <dali-toolkit/public-api/text/decorator/text-decorator.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

#ifdef DEBUG_ENABLED
#define DECORATOR_DEBUG
#endif

// Local Data
namespace
{

const char* DEFAULT_GRAB_HANDLE_IMAGE( DALI_IMAGE_DIR "insertpoint-icon.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );
//const char* DEFAULT_SELECTION_HANDLE_ONE_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-left-press.png" );
//const char* DEFAULT_SELECTION_HANDLE_TWO_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-right-press.png" );

const Dali::Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.5f, 2.0f, 1.0f );
const Dali::Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.5f, 1.5f, 1.0f );

} // end of namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Decorator::Impl : public ConnectionTracker
{
  struct CursorImpl
  {
    CursorImpl()
    : x(0.0f),
      y(0.0f),
      height(0.0f),
      color(Dali::Color::WHITE)
    {
    }

    float x;
    float y;
    float height;

    Vector4 color;
  };

  struct SelectionHandleImpl
  {
    SelectionHandleImpl()
    : x(0.0f),
      y(0.0f),
      cursorHeight(0.0f),
      flipped(false)
    {
    }

    float x;
    float y;
    float cursorHeight; ///< Not the handle height
    bool flipped;

    ImageActor actor;
    Actor grabArea;

    Image pressedImage;
    Image releasedImage;
  };

  Impl( Dali::Toolkit::Internal::Control& parent, Observer& observer )
  : mParent(parent),
    mObserver(observer),
    mActiveCursor(ACTIVE_CURSOR_NONE),
    mActiveGrabHandle(false),
    mCursorBlinkInterval(0.5f),
    mCursorBlinkDuration(0.0f)
  {
  }

  void Relayout( const Vector2& size )
  {
    // Show or hide the grab handle
    if( mActiveGrabHandle )
    {
      SetupTouchEvents();

      CreateActiveLayer();
      CreateGrabHandle();

      mGrabHandle.SetPosition( mCursor[PRIMARY_CURSOR].x, mCursor[PRIMARY_CURSOR].y + mCursor[PRIMARY_CURSOR].height );
    }
    else if( mGrabHandle )
    {
      UnparentAndReset( mGrabHandle );
    }

    // Show or hide the selection handles/highlight
    if( mActiveSelection )
    {
      SetupTouchEvents();

      CreateActiveLayer();
      CreateSelectionHandles();

      SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
      primary.actor.SetPosition( primary.x, primary.y + primary.cursorHeight );

      SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
      secondary.actor.SetPosition( secondary.x, secondary.y + secondary.cursorHeight );

      //CreateHighlight(); TODO
    }
    else
    {
      UnparentAndReset( mSelectionHandle[ PRIMARY_SELECTION_HANDLE ].actor );
      UnparentAndReset( mSelectionHandle[ SECONDARY_SELECTION_HANDLE ].actor );
    }

    // TODO
  }

  void SetupTouchEvents()
  {
    if ( !mTapDetector )
    {
      mTapDetector = TapGestureDetector::New();
      mTapDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnTap );
    }

    if ( !mPanGestureDetector )
    {
      mPanGestureDetector = PanGestureDetector::New();
      mPanGestureDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnPan );
    }
  }

  void CreateActiveLayer()
  {
    if( !mActiveLayer )
    {
      Actor parent = mParent.Self();

      mActiveLayer = Layer::New();
#ifdef DECORATOR_DEBUG
      mActiveLayer.SetName ( "ActiveLayerActor" );
#endif

      mActiveLayer.SetAnchorPoint( AnchorPoint::CENTER);
      mActiveLayer.SetParentOrigin( ParentOrigin::CENTER);
      mActiveLayer.SetSizeMode( SIZE_EQUAL_TO_PARENT );
      mActiveLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );

      parent.Add( mActiveLayer );
    }

    mActiveLayer.RaiseToTop();
  }

  void CreateGrabHandle()
  {
    if( !mGrabHandle )
    {
      if ( !mGrabHandleImage )
      {
        mGrabHandleImage = ResourceImage::New( DEFAULT_GRAB_HANDLE_IMAGE );
      }

      mGrabHandle = ImageActor::New( mGrabHandleImage );
#ifdef DECORATOR_DEBUG
      mGrabHandle.SetName( "GrabHandleActor" );
#endif
      mGrabHandle.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mGrabHandle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mGrabHandle.SetDrawMode( DrawMode::OVERLAY );

      mGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      mGrabArea.SetName( "GrabArea" );
#endif
      mGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
      mGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      mGrabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
      mGrabHandle.Add(mGrabArea);

      mTapDetector.Attach( mGrabArea );
      mPanGestureDetector.Attach( mGrabArea );

      mActiveLayer.Add(mGrabHandle);
    }
  }

  void CreateSelectionHandles()
  {
    SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
    if ( !primary.actor )
    {
      if ( !primary.releasedImage )
      {
        primary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE );
      }

      primary.actor = ImageActor::New( primary.releasedImage );
#ifdef DECORATOR_DEBUG
      primary.actor.SetName("SelectionHandleOne");
#endif
      primary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      primary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      primary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      primary.flipped = false;

      primary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      primary.grabArea.SetName("SelectionHandleOneGrabArea");
#endif
      primary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      primary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      primary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( primary.grabArea );
      mPanGestureDetector.Attach( primary.grabArea );
      primary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleOneTouched );

      primary.actor.Add( primary.grabArea );
      mActiveLayer.Add( primary.actor );
    }

    SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
    if ( !secondary.actor )
    {
      if ( !secondary.releasedImage )
      {
        secondary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO );
      }

      secondary.actor = ImageActor::New( secondary.releasedImage );
#ifdef DECORATOR_DEBUG
      secondary.actor.SetName("SelectionHandleTwo");
#endif
      secondary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      secondary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      secondary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      secondary.flipped = false;

      secondary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      secondary.grabArea.SetName("SelectionHandleTwoGrabArea");
#endif
      secondary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      secondary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      secondary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( secondary.grabArea );
      mPanGestureDetector.Attach( secondary.grabArea );
      secondary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleTwoTouched );

      secondary.actor.Add( secondary.grabArea );
      mActiveLayer.Add( secondary.actor );
    }

    //SetUpHandlePropertyNotifications(); TODO
  }

  void OnTap( Actor actor, const TapGesture& tap )
  {
    if( actor == mGrabHandle )
    {
      // TODO
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( actor == mGrabHandle )
    {
      // TODO
    }
  }

  bool OnHandleOneTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }

  bool OnHandleTwoTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }

  Internal::Control& mParent;
  Observer& mObserver;

  Layer mActiveLayer; ///< Layer for active handles and alike that ensures they are above all else.

  unsigned int mActiveCursor;
  bool         mActiveGrabHandle;
  bool         mActiveSelection;

  CursorImpl mCursor[CURSOR_COUNT];

  ImageActor mGrabHandle;
  Actor mGrabArea;

  SelectionHandleImpl mSelectionHandle[SELECTION_HANDLE_COUNT];

  Image mCursorImage;
  Image mGrabHandleImage;

  TapGestureDetector mTapDetector;
  PanGestureDetector mPanGestureDetector;

  float mCursorBlinkInterval;
  float mCursorBlinkDuration;
};

DecoratorPtr Decorator::New( Internal::Control& parent, Observer& observer )
{
  return DecoratorPtr( new Decorator(parent, observer) );
}

void Decorator::Relayout( const Vector2& size )
{
  mImpl->Relayout( size );
}

void Decorator::SetActiveCursor( ActiveCursor activeCursor )
{
  mImpl->mActiveCursor = activeCursor;
}

unsigned int Decorator::GetActiveCursor() const
{
  return mImpl->mActiveCursor;
}

void Decorator::SetPosition( Cursor cursor, float x, float y, float height )
{
  mImpl->mCursor[cursor].x = x;
  mImpl->mCursor[cursor].y = y;
  mImpl->mCursor[cursor].height = height;
}

void Decorator::GetPosition( Cursor cursor, float& x, float& y, float& height ) const
{
  x = mImpl->mCursor[cursor].x;
  y = mImpl->mCursor[cursor].y;
  height = mImpl->mCursor[cursor].height;
}

void Decorator::SetCursorImage( Dali::Image image )
{
  mImpl->mCursorImage = image;
}

Dali::Image Decorator::GetCursorImage() const
{
  return mImpl->mCursorImage;
}

void Decorator::SetColor( Cursor cursor, const Dali::Vector4& color )
{
  mImpl->mCursor[cursor].color = color;
}

const Dali::Vector4& Decorator::GetColor( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].color;
}

void Decorator::StartCursorBlink()
{
  // TODO
}

void Decorator::StopCursorBlink()
{
  // TODO
}

void Decorator::SetCursorBlinkInterval( float seconds )
{
  mImpl->mCursorBlinkInterval = seconds;
}

float Decorator::GetCursorBlinkInterval() const
{
  return mImpl->mCursorBlinkInterval;
}

void Decorator::SetCursorBlinkDuration( float seconds )
{
  mImpl->mCursorBlinkDuration = seconds;
}

float Decorator::GetCursorBlinkDuration() const
{
  return mImpl->mCursorBlinkDuration;
}

void Decorator::SetGrabHandleActive( bool active )
{
  mImpl->mActiveGrabHandle = active;
}

bool Decorator::IsGrabHandleActive() const
{
  return mImpl->mActiveGrabHandle;
}

void Decorator::SetGrabHandleImage( Dali::Image image )
{
  mImpl->mGrabHandleImage = image;
}

Dali::Image Decorator::GetGrabHandleImage() const
{
  return mImpl->mGrabHandleImage;
}

void Decorator::SetSelectionActive( bool active )
{
  mImpl->mActiveSelection = active;
}

bool Decorator::IsSelectionActive() const
{
  return mImpl->mActiveSelection;
}

void Decorator::SetPosition( SelectionHandle handle, float x, float y, float height )
{
  mImpl->mSelectionHandle[handle].x = x;
  mImpl->mSelectionHandle[handle].y = y;
  mImpl->mSelectionHandle[handle].cursorHeight = height;
}

void Decorator::GetPosition( SelectionHandle handle, float& x, float& y, float& height ) const
{
  x = mImpl->mSelectionHandle[handle].x;
  y = mImpl->mSelectionHandle[handle].y;
  height = mImpl->mSelectionHandle[handle].cursorHeight;
}

void Decorator::SetImage( SelectionHandle handle, SelectionHandleState state, Dali::Image image )
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    mImpl->mSelectionHandle[handle].pressedImage = image;
  }
  else
  {
    mImpl->mSelectionHandle[handle].releasedImage = image;
  }
}

Dali::Image Decorator::GetImage( SelectionHandle handle, SelectionHandleState state ) const
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    return mImpl->mSelectionHandle[handle].pressedImage;
  }

  return mImpl->mSelectionHandle[handle].releasedImage;
}

Decorator::~Decorator()
{
  delete mImpl;
}

Decorator::Decorator( Dali::Toolkit::Internal::Control& parent, Observer& observer )
: mImpl( NULL )
{
  mImpl = new Decorator::Impl( parent, observer );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
