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
#include <dali-toolkit/internal/text/decorator/text-decorator.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>

#ifdef DEBUG_ENABLED
#define DECORATOR_DEBUG

#endif

#define MAKE_SHADER(A)#A

namespace
{
const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
uniform   mediump mat4    uMvpMatrix;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform      lowp vec4 uColor;

void main()
{
  gl_FragColor = uColor;
}
);
}

namespace Dali
{
namespace Internal
{
namespace
{
#ifdef DECORATOR_DEBUG
Integration::Log::Filter* gLogFilter( Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_TEXT_DECORATOR") );
#endif
}
}
}


// Local Data
namespace
{
const Dali::Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.25f, 1.5f, 1.0f );
const Dali::Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.25f, 1.5f, 1.0f );
const Dali::Vector3 ACTIVE_LAYER_ANCHOR_POINT( 0.5f, 0.5f, 0.5f );

const Dali::Vector4 LIGHT_BLUE( 0.75f, 0.96f, 1.f, 1.f ); // The text highlight color. TODO: due some problems, maybe with the blending function in the text clipping, the color is fully opaque.

const Dali::Vector4 HANDLE_COLOR( 0.0f, (183.0f / 255.0f), (229.0f / 255.0f), 1.0f  );

const unsigned int CURSOR_BLINK_INTERVAL = 500u; ///< Cursor blink interval in milliseconds.
const float TO_MILLISECONDS = 1000.f;            ///< Converts from seconds to milliseconds.
const float TO_SECONDS = 1.f / TO_MILLISECONDS;  ///< Converts from milliseconds to seconds.

const unsigned int SCROLL_TICK_INTERVAL = 50u; ///< Scroll interval in milliseconds.
const float SCROLL_THRESHOLD = 10.f;           ///< Threshold in pixels close to the edges of the decorator boundaries from where the scroll timer starts to emit signals.
const float SCROLL_SPEED = 300.f;              ///< The scroll speed in pixels/second.

const float SCROLL_DISTANCE = SCROLL_SPEED * SCROLL_TICK_INTERVAL * TO_SECONDS; ///< Distance in pixels scrolled in one second.

const float CURSOR_WIDTH = 1.f; ///< The cursor's width in pixels.

const float POPUP_PADDING = 2.f; ///< Padding space between the highlight box and the text's popup.

typedef Dali::Vector<Dali::Vector4> QuadContainer;

/**
 * @brief Takes a bounding rectangle in the local coordinates of an actor and returns the world coordinates Bounding Box.
 * @param[in] boundingRectangle local bounding
 * @param[out] Vector4 World coordinate bounding Box.
 */
void LocalToWorldCoordinatesBoundingBox( const Dali::Rect<int>& boundingRectangle, Dali::Vector4& boundingBox )
{
  // Convert to world coordinates and store as a Vector4 to be compatible with Property Notifications.
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  const float originX = boundingRectangle.x - 0.5f * stageSize.width;
  const float originY = boundingRectangle.y - 0.5f * stageSize.height;

  boundingBox = Dali::Vector4( originX,
                               originY,
                               originX + boundingRectangle.width,
                               originY + boundingRectangle.height );
}

void WorldToLocalCoordinatesBoundingBox( const Dali::Vector4& boundingBox, Dali::Rect<int>& boundingRectangle )
{
  // Convert to local coordinates and store as a Dali::Rect.
  Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  boundingRectangle.x = boundingBox.x + 0.5f * stageSize.width;
  boundingRectangle.y = boundingBox.y + 0.5f * stageSize.height;
  boundingRectangle.width = boundingBox.z - boundingBox.x;
  boundingRectangle.height = boundingBox.w - boundingBox.y;
}

} // end of namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Decorator::Impl : public ConnectionTracker
{
  enum ScrollDirection
  {
    SCROLL_NONE,
    SCROLL_RIGHT,
    SCROLL_LEFT,
    SCROLL_TOP,
    SCROLL_BOTTOM
  };

  struct CursorImpl
  {
    CursorImpl()
    : color( Dali::Color::BLACK ),
      position(),
      cursorHeight( 0.0f ),
      lineHeight( 0.0f )
    {
    }

    Vector4 color;
    Vector2 position;
    float cursorHeight;
    float lineHeight;
  };

  struct HandleImpl
  {
    HandleImpl()
    : position(),
      globalPosition(),
      size(),
      lineHeight( 0.0f ),
      grabDisplacementX( 0.f ),
      grabDisplacementY( 0.f ),
      active( false ),
      horizontallyVisible( false ),
      verticallyVisible( false ),
      pressed( false ),
      verticallyFlippedPreferred( false ),
      horizontallyFlipped( false ),
      verticallyFlipped( false ),
      verticallyFlippedOnTouch( false )
    {
    }

    ImageView actor;
    Actor grabArea;
    ImageView markerActor;

    Vector2 position;
    Vector2 globalPosition;
    Size    size;
    float   lineHeight;              ///< Not the handle height
    float   grabDisplacementX;
    float   grabDisplacementY;
    bool    active                     : 1;
    bool    horizontallyVisible        : 1;
    bool    verticallyVisible          : 1;
    bool    pressed                    : 1;
    bool    verticallyFlippedPreferred : 1; ///< Whether the handle is preferred to be vertically flipped.
    bool    horizontallyFlipped        : 1; ///< Whether the handle has been horizontally flipped.
    bool    verticallyFlipped          : 1; ///< Whether the handle has been vertically flipped.
    bool    verticallyFlippedOnTouch   : 1; ///< Whether the handle is vertically flipped on touch.
  };

  struct PopupImpl
  {
    PopupImpl()
    : position()
    {
    }

    TextSelectionPopup actor;
    Vector3 position;
  };

  Impl( ControllerInterface& controller,
        TextSelectionPopupCallbackInterface& callbackInterface )
  : mController( controller ),
    mEnabledPopupButtons( TextSelectionPopup::NONE ),
    mTextSelectionPopupCallbackInterface( callbackInterface ),
    mHandleColor( HANDLE_COLOR ),
    mBoundingBox(),
    mHighlightColor( LIGHT_BLUE ),
    mHighlightPosition( Vector2::ZERO ),
    mActiveCursor( ACTIVE_CURSOR_NONE ),
    mCursorBlinkInterval( CURSOR_BLINK_INTERVAL ),
    mCursorBlinkDuration( 0.0f ),
    mCursorWidth( CURSOR_WIDTH ),
    mHandleScrolling( HANDLE_TYPE_COUNT ),
    mHandleReleased( HANDLE_TYPE_COUNT ),
    mScrollDirection( SCROLL_NONE ),
    mScrollThreshold( SCROLL_THRESHOLD ),
    mScrollSpeed( SCROLL_SPEED ),
    mScrollDistance( SCROLL_DISTANCE ),
    mTextDepth( 0u ),
    mActiveCopyPastePopup( false ),
    mPopupSetNewPosition( true ),
    mCursorBlinkStatus( true ),
    mDelayCursorBlink( false ),
    mPrimaryCursorVisible( false ),
    mSecondaryCursorVisible( false ),
    mFlipSelectionHandlesOnCross( false ),
    mFlipLeftSelectionHandleDirection( false ),
    mFlipRightSelectionHandleDirection( false ),
    mIsHandlePanning( false ),
    mIsHandleCurrentlyCrossed( false ),
    mIsHandlePreviouslyCrossed( false ),
    mNotifyEndOfScroll( false ),
    mHorizontalScrollingEnabled( false ),
    mVerticalScrollingEnabled( false ),
    mSmoothHandlePanEnabled( false ),
    mIsHighlightBoxActive( false )
  {
    mQuadVertexFormat[ "aPosition" ] = Property::VECTOR2;
    mHighlightShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    SetupGestures();
  }

  /**
   * Relayout of the decorations owned by the decorator.
   * @param[in] size The Size of the UI control the decorator is adding it's decorations to.
   */
  void Relayout( const Vector2& size )
  {
    mControlSize = size;

    // TODO - Remove this if nothing is active
    CreateActiveLayer();

    // Show or hide the cursors
    CreateCursors();

    if( mPrimaryCursor )
    {
      const CursorImpl& cursor = mCursor[PRIMARY_CURSOR];
      mPrimaryCursorVisible = ( ( mControlSize.width - ( cursor.position.x + mCursorWidth ) > -Math::MACHINE_EPSILON_1000 ) &&
                                ( cursor.position.x > -Math::MACHINE_EPSILON_1000 ) &&
                                ( mControlSize.height - ( cursor.position.y + cursor.cursorHeight ) > -Math::MACHINE_EPSILON_1000 ) &&
                                ( cursor.position.y > -Math::MACHINE_EPSILON_1000 ) );
      if( mPrimaryCursorVisible )
      {
        mPrimaryCursor.SetPosition( cursor.position.x,
                                    cursor.position.y );
        mPrimaryCursor.SetSize( Size( mCursorWidth, cursor.cursorHeight ) );
      }
      mPrimaryCursor.SetVisible( mPrimaryCursorVisible && mCursorBlinkStatus );
    }
    if( mSecondaryCursor )
    {
      const CursorImpl& cursor = mCursor[SECONDARY_CURSOR];
      mSecondaryCursorVisible = ( ( mControlSize.width - ( cursor.position.x + mCursorWidth ) > -Math::MACHINE_EPSILON_1000 ) &&
                                  ( cursor.position.x > -Math::MACHINE_EPSILON_1000 ) &&
                                  ( mControlSize.height - ( cursor.position.y + cursor.cursorHeight ) > -Math::MACHINE_EPSILON_1000 ) &&
                                  ( cursor.position.y > -Math::MACHINE_EPSILON_1000 ) );
      if( mSecondaryCursorVisible )
      {
        mSecondaryCursor.SetPosition( cursor.position.x,
                                      cursor.position.y );
        mSecondaryCursor.SetSize( Size( mCursorWidth, cursor.cursorHeight ) );
      }
      mSecondaryCursor.SetVisible( mSecondaryCursorVisible && mCursorBlinkStatus );
    }

    // Show or hide the grab handle
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    bool newGrabHandlePosition = false;
    grabHandle.horizontallyVisible = false;
    grabHandle.verticallyVisible = false;
    if( grabHandle.active )
    {
      grabHandle.horizontallyVisible = ( ( mControlSize.width - ( grabHandle.position.x + floor( 0.5f * mCursorWidth ) ) > -Math::MACHINE_EPSILON_1000 ) &&
                                         ( grabHandle.position.x > -Math::MACHINE_EPSILON_1000 ) );
      grabHandle.verticallyVisible = ( ( ( mControlSize.height - grabHandle.lineHeight ) - grabHandle.position.y > -Math::MACHINE_EPSILON_1000 ) &&
                                       ( grabHandle.position.y > -Math::MACHINE_EPSILON_1000 ) );

      const bool isVisible = grabHandle.horizontallyVisible && grabHandle.verticallyVisible;
      if( isVisible )
      {
        CreateGrabHandle();

        // Sets the grab handle position and calculate if it needs to be vertically flipped if it exceeds the boundary box.
        SetGrabHandlePosition();

        // Sets the grab handle image according if it's pressed, flipped, etc.
        SetHandleImage( GRAB_HANDLE );

        newGrabHandlePosition = true;
      }

      if( grabHandle.actor )
      {
        grabHandle.actor.SetVisible( isVisible );
      }
    }
    else if( grabHandle.actor )
    {
      grabHandle.actor.Unparent();
    }

    // Show or hide the selection handles/highlight
    HandleImpl& primary = mHandle[ LEFT_SELECTION_HANDLE ];
    HandleImpl& secondary = mHandle[ RIGHT_SELECTION_HANDLE ];
    bool newPrimaryHandlePosition = false;
    bool newSecondaryHandlePosition = false;

    primary.horizontallyVisible = ( ( mControlSize.width - primary.position.x > -Math::MACHINE_EPSILON_1000 ) &&
                                    ( primary.position.x > -Math::MACHINE_EPSILON_1000 ) );
    primary.verticallyVisible = ( ( ( mControlSize.height - primary.lineHeight ) - primary.position.y > -Math::MACHINE_EPSILON_1000 ) &&
                                  ( primary.position.y + ( primary.verticallyFlipped ? 0.f : primary.lineHeight ) > -Math::MACHINE_EPSILON_1000 ) );
    secondary.horizontallyVisible = ( ( mControlSize.width - secondary.position.x > -Math::MACHINE_EPSILON_1000 ) &&
                                      ( secondary.position.x > -Math::MACHINE_EPSILON_1000 ) );
    secondary.verticallyVisible = ( ( ( mControlSize.height - secondary.lineHeight ) - secondary.position.y > -Math::MACHINE_EPSILON_1000 ) &&
                                    ( secondary.position.y + ( secondary.verticallyFlipped ? 0.f : secondary.lineHeight ) > -Math::MACHINE_EPSILON_1000 ) );

    const bool primaryVisible = primary.horizontallyVisible && primary.verticallyVisible;
    const bool secondaryVisible = secondary.horizontallyVisible && secondary.verticallyVisible;

    if( primary.active || secondary.active )
    {
      if( primaryVisible || secondaryVisible )
      {
        CreateSelectionHandles();

        if( primaryVisible )
        {
          SetSelectionHandlePosition( LEFT_SELECTION_HANDLE );

          // Sets the primary handle image according if it's pressed, flipped, etc.
          SetHandleImage( LEFT_SELECTION_HANDLE );

          SetSelectionHandleMarkerSize( primary );

          newPrimaryHandlePosition = true;
        }

        if( secondaryVisible )
        {
          SetSelectionHandlePosition( RIGHT_SELECTION_HANDLE );

          // Sets the secondary handle image according if it's pressed, flipped, etc.
          SetHandleImage( RIGHT_SELECTION_HANDLE );

          SetSelectionHandleMarkerSize( secondary );

          newSecondaryHandlePosition = true;
        }
      }

      if( primary.actor )
      {
        primary.actor.SetVisible( primaryVisible );
      }
      if( secondary.actor )
      {
        secondary.actor.SetVisible( secondaryVisible );
      }

    }
    else
    {
      if( primary.actor )
      {
        primary.actor.Unparent();
      }
      if( secondary.actor )
      {
        secondary.actor.Unparent();
      }
    }

    if( mIsHighlightBoxActive )
    {
      CreateHighlight();
      UpdateHighlight();
    }
    else
    {
      if( mHighlightActor )
      {
        mHighlightActor.Unparent();
      }
    }

    if( newGrabHandlePosition    ||
        newPrimaryHandlePosition ||
        newSecondaryHandlePosition )
    {
      // Setup property notifications to find whether the handles leave the boundaries of the current display.
      SetupActiveLayerPropertyNotifications();
    }

    if( mActiveCopyPastePopup &&
        ( primaryVisible || secondaryVisible ) )
    {
      ShowPopup();
      mPopupSetNewPosition = true;
    }
    else
    {
      if( mCopyPastePopup.actor )
      {
        mCopyPastePopup.actor.HidePopup();
        mPopupSetNewPosition = true;
      }
    }
  }

  void UpdatePositions( const Vector2& scrollOffset )
  {
    mCursor[PRIMARY_CURSOR].position += scrollOffset;
    mCursor[SECONDARY_CURSOR].position += scrollOffset;
    mHandle[ GRAB_HANDLE ].position += scrollOffset;
    mHandle[ LEFT_SELECTION_HANDLE ].position += scrollOffset;
    mHandle[ RIGHT_SELECTION_HANDLE ].position += scrollOffset;
    mHighlightPosition += scrollOffset;
  }

  void ShowPopup()
  {
    if( !mCopyPastePopup.actor )
    {
      return;
    }

    if( !mCopyPastePopup.actor.GetParent() )
    {
      mActiveLayer.Add( mCopyPastePopup.actor );
    }

    mCopyPastePopup.actor.RaiseAbove( mActiveLayer );
    mCopyPastePopup.actor.ShowPopup();
  }

  float CalculateVerticalPopUpPosition( float halfHeight, bool preferBelow )
  {
    float yPosition = 0.f;

    const HandleImpl& primaryHandle = mHandle[LEFT_SELECTION_HANDLE];
    const HandleImpl& secondaryHandle = mHandle[RIGHT_SELECTION_HANDLE];
    const HandleImpl& grabHandle = mHandle[GRAB_HANDLE];

    if( primaryHandle.active || secondaryHandle.active )
    {
      // The origin of the decorator's coordinate system in world coords.
      const Vector3 originWorldCoords = mActiveLayer.GetCurrentWorldPosition() - mActiveLayer.GetCurrentSize() * ACTIVE_LAYER_ANCHOR_POINT;

      if( preferBelow )
      {
        // Find out if there is enough space for the popup at the bottom.
        const float primaryBelowY = primaryHandle.position.y + primaryHandle.lineHeight + primaryHandle.size.height;
        const float secondaryBelowY = secondaryHandle.position.y + secondaryHandle.lineHeight + secondaryHandle.size.height;

        float maxY = std::max( primaryBelowY, secondaryBelowY );

        yPosition = halfHeight + maxY;

        if( originWorldCoords.y + yPosition + halfHeight > mBoundingBox.w )
        {
          // Does not fit below.

          // Try to fit first below the non active handle. Otherwise above the active handle.
          if( RIGHT_SELECTION_HANDLE == mHandleReleased )
          {
            if( primaryBelowY < secondaryBelowY )
            {
              yPosition = halfHeight + primaryBelowY;
            }
            else
            {
              yPosition = primaryHandle.position.y - primaryHandle.size.height - halfHeight;
            }
          }
          else if( LEFT_SELECTION_HANDLE == mHandleReleased )
          {
            if( secondaryBelowY < primaryBelowY )
            {
              yPosition = halfHeight + secondaryBelowY;
            }
            else
            {
              yPosition = secondaryHandle.position.y - secondaryHandle.size.height - halfHeight;
            }
          }

          // Check the handle is whithin the decoration box.
          if( originWorldCoords.y + yPosition < mBoundingBox.y )
          {
            yPosition = mBoundingBox.y - originWorldCoords.y + halfHeight;
          }

          if( originWorldCoords.y + yPosition > mBoundingBox.w )
          {
            yPosition = mBoundingBox.w - originWorldCoords.y - halfHeight;
          }
        }
      } // preferBelow
      else
      {
        // Find out if there is enough space for the popup at the top.
        const float primaryTopY = primaryHandle.position.y - primaryHandle.size.height;
        const float secondaryTopY = secondaryHandle.position.y - secondaryHandle.size.height;

        float minY = std::min( primaryTopY, secondaryTopY );

        yPosition = -halfHeight + minY;
      } // !preferBelow
    } // ( primaryHandle.active || secondaryHandle.active )
    else if( grabHandle.active )
    {
      if( preferBelow )
      {
        yPosition = halfHeight + grabHandle.lineHeight + grabHandle.size.height + grabHandle.position.y;
      }
      else
      {
        yPosition = -halfHeight + grabHandle.position.y - POPUP_PADDING;
      }
    }

    return yPosition;
  }

  void ConstrainPopupPosition( const Vector3& popupHalfSize )
  {
    // Check if the popup is within the boundaries of the decoration box.

    // Check first the horizontal dimension. If is not within the boundaries, it calculates the offset.

    // The origin of the decorator's coordinate system in world coords.
    const Vector3 originWorldCoords = mActiveLayer.GetCurrentWorldPosition() - mActiveLayer.GetCurrentSize() * ACTIVE_LAYER_ANCHOR_POINT;

    // The popup's position in world coords.
    Vector3 popupPositionWorldCoords = originWorldCoords + mCopyPastePopup.position;

    if( popupPositionWorldCoords.x - popupHalfSize.width < mBoundingBox.x )
    {
       mCopyPastePopup.position.x += mBoundingBox.x - ( popupPositionWorldCoords.x - popupHalfSize.width );
    }
    else if( popupPositionWorldCoords.x + popupHalfSize.width > mBoundingBox.z )
    {
       mCopyPastePopup.position.x += mBoundingBox.z - ( popupPositionWorldCoords.x + popupHalfSize.width );
    }

    // Check the vertical dimension. If the popup doesn't fit above the handles, it looks for a valid position below.
    if( popupPositionWorldCoords.y - popupHalfSize.height < mBoundingBox.y )
    {
      mCopyPastePopup.position.y = CalculateVerticalPopUpPosition( popupHalfSize.height, true ); // true -> prefer to set the popup's position below.
    }
  }

  void SetPopupPosition( Actor actor )
  {
    if( !mActiveCopyPastePopup )
    {
      return;
    }

    // Retrieves the popup's size after relayout.
    const Vector3 popupSize( mCopyPastePopup.actor.GetRelayoutSize( Dimension::WIDTH ), mCopyPastePopup.actor.GetRelayoutSize( Dimension::HEIGHT ), 0.0f );
    const Vector3 popupHalfSize = popupSize * 0.5f;

    if( mPopupSetNewPosition )
    {
      const HandleImpl& primaryHandle = mHandle[LEFT_SELECTION_HANDLE];
      const HandleImpl& secondaryHandle = mHandle[RIGHT_SELECTION_HANDLE];
      const HandleImpl& grabHandle = mHandle[GRAB_HANDLE];

      if( primaryHandle.active || secondaryHandle.active )
      {
        const float minHandleXPosition = std::min( primaryHandle.position.x, secondaryHandle.position.x );
        const float maxHandleXPosition = std::max( primaryHandle.position.x, secondaryHandle.position.x );

        mCopyPastePopup.position.x = minHandleXPosition + ( ( maxHandleXPosition - minHandleXPosition ) * 0.5f );

        const float primaryY = -popupHalfSize.height + primaryHandle.position.y - ( primaryHandle.verticallyFlipped ? primaryHandle.size.height : POPUP_PADDING );
        const float secondaryY = -popupHalfSize.height + secondaryHandle.position.y - ( secondaryHandle.verticallyFlipped ? secondaryHandle.size.height : POPUP_PADDING );

        mCopyPastePopup.position.y = std::min( primaryY, secondaryY );
      }
      else if( grabHandle.active )
      {
        mCopyPastePopup.position.x = grabHandle.position.x;

        mCopyPastePopup.position.y = -popupHalfSize.height + grabHandle.position.y - ( grabHandle.verticallyFlipped ? grabHandle.size.height : POPUP_PADDING );
      }
    } // mPopupSetNewPosition

    // It may change the popup's position to fit within the decoration box.
    ConstrainPopupPosition( popupHalfSize );

    SetUpPopupPositionNotifications( popupHalfSize );

    // Prevent pixel mis-alignment by rounding down.
    mCopyPastePopup.position.x = floorf( mCopyPastePopup.position.x );
    mCopyPastePopup.position.y = floorf( mCopyPastePopup.position.y );

    mCopyPastePopup.actor.SetPosition( mCopyPastePopup.position );
    mPopupSetNewPosition = false;
  }

  void CreateCursor( Control& cursor, const Vector4& color )
  {
    cursor = Control::New();
    cursor.SetBackgroundColor( color );
    cursor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    cursor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  }

  // Add or Remove cursor(s) from parent
  void CreateCursors()
  {
    if( mActiveCursor == ACTIVE_CURSOR_NONE )
    {
      if( mPrimaryCursor )
      {
        mPrimaryCursor.Unparent();
      }
      if( mSecondaryCursor )
      {
        mSecondaryCursor.Unparent();
      }
    }
    else
    {
      // Create Primary and or Secondary Cursor(s) if active and add to parent
      if ( mActiveCursor == ACTIVE_CURSOR_PRIMARY ||
           mActiveCursor == ACTIVE_CURSOR_BOTH )
      {
        if ( !mPrimaryCursor )
        {
          CreateCursor( mPrimaryCursor, mCursor[PRIMARY_CURSOR].color );
#ifdef DECORATOR_DEBUG
          mPrimaryCursor.SetName( "PrimaryCursorActor" );
#endif
        }

        if( !mPrimaryCursor.GetParent() )
        {
          mActiveLayer.Add( mPrimaryCursor );
        }
      }

      if ( mActiveCursor == ACTIVE_CURSOR_BOTH )
      {
        if ( !mSecondaryCursor )
        {
          CreateCursor( mSecondaryCursor, mCursor[SECONDARY_CURSOR].color );
#ifdef DECORATOR_DEBUG
          mSecondaryCursor.SetName( "SecondaryCursorActor" );
#endif
        }

        if( !mSecondaryCursor.GetParent() )
        {
          mActiveLayer.Add( mSecondaryCursor );
        }
      }
      else
      {
        if( mSecondaryCursor )
        {
          mSecondaryCursor.Unparent();
        }
      }
    }
  }

  bool OnCursorBlinkTimerTick()
  {
    if( !mDelayCursorBlink )
    {
      // Cursor blinking
      if ( mPrimaryCursor )
      {
        mPrimaryCursor.SetVisible( mPrimaryCursorVisible && mCursorBlinkStatus );
      }
      if ( mSecondaryCursor )
      {
        mSecondaryCursor.SetVisible( mSecondaryCursorVisible && mCursorBlinkStatus );
      }

      mCursorBlinkStatus = !mCursorBlinkStatus;
    }
    else
    {
      // Resume blinking
      mDelayCursorBlink = false;
    }

    return true;
  }

  void SetupGestures()
  {
    // Will consume tap gestures on handles.
    mTapDetector = TapGestureDetector::New();

    // Will consume double tap gestures on handles.
    mTapDetector.SetMaximumTapsRequired( 2u );

    // Will consume long press gestures on handles.
    mLongPressDetector = LongPressGestureDetector::New();

    // Detects pan gestures on handles.
    mPanDetector = PanGestureDetector::New();
    mPanDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnPan );
  }

  void CreateActiveLayer()
  {
    if( !mActiveLayer )
    {
      mActiveLayer = Layer::New();
#ifdef DECORATOR_DEBUG
      mActiveLayer.SetName ( "ActiveLayerActor" );
#endif

      mActiveLayer.SetParentOrigin( ParentOrigin::CENTER );
      mActiveLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      // Add the active layer telling the controller it doesn't need clipping.
      mController.AddDecoration( mActiveLayer, false );
    }

    mActiveLayer.RaiseToTop();
  }

  void SetSelectionHandleMarkerSize( HandleImpl& handle )
  {
    if( handle.markerActor )
    {
      handle.markerActor.SetSize( 0, handle.lineHeight );
    }
  }

  void CreateGrabHandle()
  {
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    if( !grabHandle.actor )
    {
      if( mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        grabHandle.actor = ImageView::New( mHandleImages[GRAB_HANDLE][HANDLE_IMAGE_RELEASED] );
        GetImpl( grabHandle.actor).SetDepthIndex( DepthIndex::DECORATION );
        grabHandle.actor.SetAnchorPoint( AnchorPoint::TOP_CENTER );

        // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
        grabHandle.actor.SetName( "GrabHandleActor" );
        if ( Dali::Internal::gLogFilter->IsEnabledFor( Debug::Verbose ) )
        {
          grabHandle.grabArea = Control::New();
          Toolkit::Control control = Toolkit::Control::DownCast( grabHandle.grabArea );
          control.SetBackgroundColor( Vector4( 1.0f, 1.0f, 1.0f, 0.5f ) );
          grabHandle.grabArea.SetName( "GrabArea" );
        }
        else
        {
          grabHandle.grabArea = Actor::New();
          grabHandle.grabArea.SetName( "GrabArea" );
        }
#else
        grabHandle.grabArea = Actor::New();
#endif

        grabHandle.grabArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
        grabHandle.grabArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
        grabHandle.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
        grabHandle.grabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
        grabHandle.actor.Add( grabHandle.grabArea );
        grabHandle.actor.SetColor( mHandleColor );

        grabHandle.grabArea.TouchSignal().Connect( this, &Decorator::Impl::OnGrabHandleTouched );

        // The grab handle's actor is attached to the tap and long press detectors in order to consume these events.
        // Note that no callbacks are connected to any signal emitted by the tap and long press detectors.
        mTapDetector.Attach( grabHandle.actor );
        mLongPressDetector.Attach( grabHandle.actor );

        // The grab handle's area is attached to the pan detector.
        // The OnPan() method is connected to the signals emitted by the pan detector.
        mPanDetector.Attach( grabHandle.grabArea );

        mActiveLayer.Add( grabHandle.actor );
      }
    }

    if( grabHandle.actor && !grabHandle.actor.GetParent() )
    {
      mActiveLayer.Add( grabHandle.actor );
    }
  }

  void CreateHandleMarker( HandleImpl& handle, Image& image, HandleType handleType )
  {
    if( image )
    {
      handle.markerActor = ImageView::New( image );
      handle.markerActor.SetColor( mHandleColor );
      handle.actor.Add( handle.markerActor );

      handle.markerActor.SetResizePolicy ( ResizePolicy::FIXED, Dimension::HEIGHT );

      if( LEFT_SELECTION_HANDLE == handleType )
      {
        handle.markerActor.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
        handle.markerActor.SetParentOrigin( ParentOrigin::TOP_RIGHT );
      }
      else if( RIGHT_SELECTION_HANDLE == handleType )
      {
        handle.markerActor.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
        handle.markerActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      }
    }
  }

  void CreateSelectionHandles()
  {
    HandleImpl& primary = mHandle[ LEFT_SELECTION_HANDLE ];
    if( !primary.actor )
    {
      if( mHandleImages[LEFT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        primary.actor = ImageView::New( mHandleImages[LEFT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] );
#ifdef DECORATOR_DEBUG
        primary.actor.SetName("SelectionHandleOne");
#endif
        primary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
        GetImpl( primary.actor ).SetDepthIndex( DepthIndex::DECORATION );
        primary.actor.SetColor( mHandleColor );

        primary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
        primary.grabArea.SetName("SelectionHandleOneGrabArea");
#endif
        primary.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
        primary.grabArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
        primary.grabArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
        primary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );

        primary.grabArea.TouchSignal().Connect( this, &Decorator::Impl::OnHandleOneTouched );

        // The handle's actor is attached to the tap and long press detectors in order to consume these events.
        // Note that no callbacks are connected to any signal emitted by the tap and long press detectors.
        mTapDetector.Attach( primary.actor );
        mLongPressDetector.Attach( primary.actor );

        // The handle's area is attached to the pan detector.
        // The OnPan() method is connected to the signals emitted by the pan detector.
        mPanDetector.Attach( primary.grabArea );

        primary.actor.Add( primary.grabArea );

        CreateHandleMarker( primary, mHandleImages[LEFT_SELECTION_HANDLE_MARKER][HANDLE_IMAGE_RELEASED], LEFT_SELECTION_HANDLE );
      }
    }

    if( primary.actor && !primary.actor.GetParent() )
    {
      mActiveLayer.Add( primary.actor );
    }

    HandleImpl& secondary = mHandle[ RIGHT_SELECTION_HANDLE ];
    if( !secondary.actor )
    {
      if( mHandleImages[RIGHT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] )
      {
        secondary.actor = ImageView::New( mHandleImages[RIGHT_SELECTION_HANDLE][HANDLE_IMAGE_RELEASED] );
#ifdef DECORATOR_DEBUG
        secondary.actor.SetName("SelectionHandleTwo");
#endif
        secondary.actor.SetAnchorPoint( AnchorPoint::TOP_LEFT ); // Change to BOTTOM_LEFT if Look'n'Feel requires handle above text.
        GetImpl( secondary.actor ).SetDepthIndex( DepthIndex::DECORATION );
        secondary.actor.SetColor( mHandleColor );

        secondary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
        secondary.grabArea.SetName("SelectionHandleTwoGrabArea");
#endif
        secondary.grabArea.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
        secondary.grabArea.SetParentOrigin( ParentOrigin::TOP_CENTER );
        secondary.grabArea.SetAnchorPoint( AnchorPoint::TOP_CENTER );
        secondary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );

        secondary.grabArea.TouchSignal().Connect( this, &Decorator::Impl::OnHandleTwoTouched );

        // The handle's actor is attached to the tap and long press detectors in order to consume these events.
        // Note that no callbacks are connected to any signal emitted by the tap and long press detectors.
        mTapDetector.Attach( secondary.actor );
        mLongPressDetector.Attach( secondary.actor );

        // The handle's area is attached to the pan detector.
        // The OnPan() method is connected to the signals emitted by the pan detector.
        mPanDetector.Attach( secondary.grabArea );

        secondary.actor.Add( secondary.grabArea );

        CreateHandleMarker( secondary, mHandleImages[RIGHT_SELECTION_HANDLE_MARKER][HANDLE_IMAGE_RELEASED], RIGHT_SELECTION_HANDLE  );
      }
    }

    if( secondary.actor && !secondary.actor.GetParent() )
    {
      mActiveLayer.Add( secondary.actor );
    }
  }

  void CalculateHandleWorldCoordinates( HandleImpl& handle, Vector2& position )
  {
    // Gets the world position of the active layer. The active layer is where the handles are added.
    const Vector3 parentWorldPosition = mActiveLayer.GetCurrentWorldPosition();

    // The grab handle position in world coords.
    // The active layer's world position is the center of the active layer. The origin of the
    // coord system of the handles is the top left of the active layer.
    position.x = parentWorldPosition.x - 0.5f * mControlSize.width + handle.position.x + ( mSmoothHandlePanEnabled ? handle.grabDisplacementX : 0.f );
    position.y = parentWorldPosition.y - 0.5f * mControlSize.height + handle.position.y + ( mSmoothHandlePanEnabled ? handle.grabDisplacementY : 0.f );
  }

  void SetGrabHandlePosition()
  {
    // Reference to the grab handle.
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];

    // Transforms the handle position into world coordinates.
    // @note This is not the same value as grabHandle.actor.GetCurrentWorldPosition()
    // as it's transforming the handle's position set by the text-controller and not
    // the final position set to the actor. Another difference is the GetCurrentWorldPosition()
    // retrieves the position of the center of the actor but the handle's position set
    // by the text controller is not the center of the actor.
    Vector2 grabHandleWorldPosition;
    CalculateHandleWorldCoordinates( grabHandle, grabHandleWorldPosition );

    // Check if the grab handle exceeds the boundaries of the decoration box.
    // At the moment only the height is checked for the grab handle.

    grabHandle.verticallyFlipped = ( grabHandle.verticallyFlippedPreferred &&
                                     ( ( grabHandleWorldPosition.y - grabHandle.size.height ) > mBoundingBox.y ) ) ||
                                   ( grabHandleWorldPosition.y + grabHandle.lineHeight + grabHandle.size.height > mBoundingBox.w );

    // The grab handle 'y' position in local coords.
    // If the grab handle exceeds the bottom of the decoration box,
    // set the 'y' position to the top of the line.
    // The SetGrabHandleImage() method will change the orientation.
    const float yLocalPosition = grabHandle.verticallyFlipped ? grabHandle.position.y : grabHandle.position.y + grabHandle.lineHeight;

    if( grabHandle.actor )
    {
      grabHandle.actor.SetPosition( grabHandle.position.x + floor( 0.5f * mCursorWidth ) + ( mSmoothHandlePanEnabled ? grabHandle.grabDisplacementX : 0.f ),
                                    yLocalPosition + ( mSmoothHandlePanEnabled ? grabHandle.grabDisplacementY : 0.f ) );
    }
  }

  void SetSelectionHandlePosition( HandleType type )
  {
    const bool isPrimaryHandle = LEFT_SELECTION_HANDLE == type;

    // Reference to the selection handle.
    HandleImpl& handle = mHandle[type];

    // Transforms the handle position into world coordinates.
    // @note This is not the same value as handle.actor.GetCurrentWorldPosition()
    // as it's transforming the handle's position set by the text-controller and not
    // the final position set to the actor. Another difference is the GetCurrentWorldPosition()
    // retrieves the position of the center of the actor but the handle's position set
    // by the text controller is not the center of the actor.
    Vector2 handleWorldPosition;
    CalculateHandleWorldCoordinates( handle, handleWorldPosition );

    // Whether to flip the handle (horizontally).
    bool flipHandle = isPrimaryHandle ? mFlipLeftSelectionHandleDirection : mFlipRightSelectionHandleDirection;

    // Whether to flip the handles if they are crossed.
    bool crossFlip = false;
    if( mFlipSelectionHandlesOnCross || !mIsHandlePanning )
    {
      crossFlip = mIsHandleCurrentlyCrossed;
    }

    // Whether the handle was crossed before start the panning.
    const bool isHandlePreviouslyCrossed = mFlipSelectionHandlesOnCross ? false : mIsHandlePreviouslyCrossed;

    // Does not flip if both conditions are true (double flip)
    flipHandle = flipHandle != ( crossFlip || isHandlePreviouslyCrossed );

    // Will flip the handles vertically if the user prefers it.
    bool verticallyFlippedPreferred = handle.verticallyFlippedPreferred;

    if( crossFlip || isHandlePreviouslyCrossed )
    {
      if( isPrimaryHandle )
      {
        verticallyFlippedPreferred = mHandle[RIGHT_SELECTION_HANDLE].verticallyFlippedPreferred;
      }
      else
      {
        verticallyFlippedPreferred = mHandle[LEFT_SELECTION_HANDLE].verticallyFlippedPreferred;
      }
    }

    // Check if the selection handle exceeds the boundaries of the decoration box.
    const bool exceedsLeftEdge = ( isPrimaryHandle ? !flipHandle : flipHandle ) && ( handleWorldPosition.x - handle.size.width < mBoundingBox.x );
    const bool exceedsRightEdge = ( isPrimaryHandle ? flipHandle : !flipHandle ) && ( handleWorldPosition.x + handle.size.width > mBoundingBox.z );

    // Does not flip if both conditions are true (double flip)
    flipHandle = flipHandle != ( exceedsLeftEdge || exceedsRightEdge );

    if( flipHandle )
    {
      if( handle.actor && !handle.horizontallyFlipped )
      {
        // Change the anchor point to flip the image.
        handle.actor.SetAnchorPoint( isPrimaryHandle ? AnchorPoint::TOP_LEFT : AnchorPoint::TOP_RIGHT );

        handle.horizontallyFlipped = true;
      }
    }
    else
    {
      if( handle.actor && handle.horizontallyFlipped )
      {
        // Reset the anchor point.
        handle.actor.SetAnchorPoint( isPrimaryHandle ? AnchorPoint::TOP_RIGHT : AnchorPoint::TOP_LEFT );

        handle.horizontallyFlipped = false;
      }
    }

    // Whether to flip the handle vertically.
    handle.verticallyFlipped = ( verticallyFlippedPreferred &&
                                 ( ( handleWorldPosition.y - handle.size.height ) > mBoundingBox.y ) ) ||
                               ( handleWorldPosition.y + handle.lineHeight + handle.size.height > mBoundingBox.w );

    // The primary selection handle 'y' position in local coords.
    // If the handle exceeds the bottom of the decoration box,
    // set the 'y' position to the top of the line.
    // The SetHandleImage() method will change the orientation.
    const float yLocalPosition = handle.verticallyFlipped ? handle.position.y : handle.position.y + handle.lineHeight;

    if( handle.actor )
    {
      handle.actor.SetPosition( handle.position.x + ( mSmoothHandlePanEnabled ? handle.grabDisplacementX : 0.f ),
                                yLocalPosition + ( mSmoothHandlePanEnabled ? handle.grabDisplacementY : 0.f ) );
    }
  }

  void SetHandleImage( HandleType type )
  {
    HandleImpl& handle = mHandle[type];

    HandleType markerType = HANDLE_TYPE_COUNT;
    // If the selection handle is flipped it chooses the image of the other selection handle. Does nothing for the grab handle.
    if( LEFT_SELECTION_HANDLE == type )
    {
      type = handle.horizontallyFlipped ? RIGHT_SELECTION_HANDLE : LEFT_SELECTION_HANDLE;
      markerType = handle.horizontallyFlipped ? RIGHT_SELECTION_HANDLE_MARKER : LEFT_SELECTION_HANDLE_MARKER;
    }
    else if( RIGHT_SELECTION_HANDLE == type )
    {
      type = handle.horizontallyFlipped ? LEFT_SELECTION_HANDLE : RIGHT_SELECTION_HANDLE;
      markerType = handle.horizontallyFlipped ? LEFT_SELECTION_HANDLE_MARKER : RIGHT_SELECTION_HANDLE_MARKER;
    }

    // Chooses between the released or pressed image. It checks whether the pressed image exists.
    if( handle.actor )
    {
      const HandleImageType imageType = ( handle.pressed ? ( mHandleImages[type][HANDLE_IMAGE_PRESSED] ? HANDLE_IMAGE_PRESSED : HANDLE_IMAGE_RELEASED ) : HANDLE_IMAGE_RELEASED );

      handle.actor.SetImage( mHandleImages[type][imageType] );
    }

    if( HANDLE_TYPE_COUNT != markerType )
    {
      if( handle.markerActor )
      {
        const HandleImageType markerImageType = ( handle.pressed ? ( mHandleImages[markerType][HANDLE_IMAGE_PRESSED] ? HANDLE_IMAGE_PRESSED : HANDLE_IMAGE_RELEASED ) : HANDLE_IMAGE_RELEASED );
        handle.markerActor.SetImage( mHandleImages[markerType][markerImageType] );
      }
    }

    // Whether to flip the handle vertically.
    if( handle.actor )
    {
      handle.actor.SetOrientation( handle.verticallyFlipped ? ANGLE_180 : ANGLE_0, Vector3::XAXIS );
    }
  }

  void CreateHighlight()
  {
    if( !mHighlightActor )
    {
      mHighlightActor = Actor::New();

#ifdef DECORATOR_DEBUG
      mHighlightActor.SetName( "HighlightActor" );
#endif
      mHighlightActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mHighlightActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      mHighlightActor.SetColor( mHighlightColor );
      mHighlightActor.SetColorMode( USE_OWN_COLOR );
    }

    // Add the highlight box telling the controller it needs clipping.
    mController.AddDecoration( mHighlightActor, true );
  }

  void UpdateHighlight()
  {
    if ( mHighlightActor )
    {
      // Sets the position of the highlight actor inside the decorator.
      mHighlightActor.SetPosition( mHighlightPosition.x,
                                   mHighlightPosition.y, -0.0001f );

      const unsigned int numberOfQuads = mHighlightQuadList.Count();
      if( 0u != numberOfQuads )
      {
        // Set the size of the highlighted text to the actor.
        mHighlightActor.SetSize( mHighlightSize );

        // Used to translate the vertices given in decorator's coords to the mHighlightActor's local coords.
        const float offsetX = mHighlightPosition.x + 0.5f * mHighlightSize.width;
        const float offsetY = mHighlightPosition.y + 0.5f * mHighlightSize.height;

        Vector<Vector2> vertices;
        Vector<unsigned short> indices;

        vertices.Reserve( 4u * numberOfQuads );
        indices.Reserve( 6u * numberOfQuads );

        // Index to the vertex.
        unsigned int v = 0u;

        // Traverse all quads.
        for( Vector<Vector4>::ConstIterator it = mHighlightQuadList.Begin(),
               endIt = mHighlightQuadList.End();
             it != endIt;
             ++it, v += 4u )
        {
          const Vector4& quad = *it;

          Vector2 vertex;

          // top-left (v+0)
          vertex.x = quad.x - offsetX;
          vertex.y = quad.y - offsetY;
          vertices.PushBack( vertex );

          // top-right (v+1)
          vertex.x = quad.z - offsetX;
          vertex.y = quad.y - offsetY;
          vertices.PushBack( vertex );

          // bottom-left (v+2)
          vertex.x = quad.x - offsetX;
          vertex.y = quad.w - offsetY;
          vertices.PushBack( vertex );

          // bottom-right (v+3)
          vertex.x = quad.z - offsetX;
          vertex.y = quad.w - offsetY;
          vertices.PushBack( vertex );

          // triangle A (3, 1, 0)
          indices.PushBack( v + 3 );
          indices.PushBack( v + 1 );
          indices.PushBack( v );

          // triangle B (0, 2, 3)
          indices.PushBack( v );
          indices.PushBack( v + 2 );
          indices.PushBack( v + 3 );
        }

        if( ! mQuadVertices )
        {
          mQuadVertices = PropertyBuffer::New( mQuadVertexFormat );
        }

        mQuadVertices.SetData( &vertices[ 0 ], vertices.Size() );

        if( !mQuadGeometry )
        {
          mQuadGeometry = Geometry::New();
          mQuadGeometry.AddVertexBuffer( mQuadVertices );
        }
        mQuadGeometry.SetIndexBuffer( &indices[ 0 ], indices.Size() );

        if( !mHighlightRenderer )
        {
          mHighlightRenderer = Dali::Renderer::New( mQuadGeometry, mHighlightShader );
          mHighlightActor.AddRenderer( mHighlightRenderer );
        }
      }

      mHighlightQuadList.Clear();

      if( mHighlightRenderer )
      {
        mHighlightRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mTextDepth - 2 ); // text is rendered at mTextDepth and text's shadow at mTextDepth -1u.
      }
    }
  }

  void DoPan( HandleImpl& handle, HandleType type, const PanGesture& gesture )
  {
    if( Gesture::Started == gesture.state )
    {
      handle.grabDisplacementX = handle.grabDisplacementY = 0.f;

      handle.globalPosition.x = handle.position.x;
      handle.globalPosition.y = handle.position.y;
    }

    handle.grabDisplacementX += gesture.displacement.x;
    handle.grabDisplacementY += ( handle.verticallyFlipped ? -gesture.displacement.y : gesture.displacement.y );

    const float x = handle.globalPosition.x + handle.grabDisplacementX;
    const float y = handle.globalPosition.y + handle.grabDisplacementY + 0.5f * handle.lineHeight;
    const float yVerticallyFlippedCorrected = y - ( handle.verticallyFlippedOnTouch ? handle.lineHeight : 0.f );

    if( ( Gesture::Started    == gesture.state ) ||
        ( Gesture::Continuing == gesture.state ) )
    {
      Vector2 targetSize;
      mController.GetTargetSize( targetSize );

      if( mHorizontalScrollingEnabled &&
          ( x < mScrollThreshold ) )
      {
        mScrollDirection = SCROLL_RIGHT;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else if( mHorizontalScrollingEnabled &&
               ( x > targetSize.width - mScrollThreshold ) )
      {
        mScrollDirection = SCROLL_LEFT;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else if( mVerticalScrollingEnabled &&
               ( yVerticallyFlippedCorrected < mScrollThreshold ) )
      {
        mScrollDirection = SCROLL_TOP;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else if( mVerticalScrollingEnabled &&
               ( yVerticallyFlippedCorrected + handle.lineHeight > targetSize.height - mScrollThreshold ) )
      {
        mScrollDirection = SCROLL_BOTTOM;
        mHandleScrolling = type;
        StartScrollTimer();
      }
      else
      {
        mHandleScrolling = HANDLE_TYPE_COUNT;
        StopScrollTimer();
        mController.DecorationEvent( type, HANDLE_PRESSED, x, y );
      }

      mIsHandlePanning = true;
    }
    else if( ( Gesture::Finished  == gesture.state ) ||
             ( Gesture::Cancelled == gesture.state ) )
    {
      if( mScrollTimer &&
          ( mScrollTimer.IsRunning() || mNotifyEndOfScroll ) )
      {
        mNotifyEndOfScroll = false;
        mHandleScrolling = HANDLE_TYPE_COUNT;
        StopScrollTimer();
        mController.DecorationEvent( type, HANDLE_STOP_SCROLLING, x, y );
      }
      else
      {
        mController.DecorationEvent( type, HANDLE_RELEASED, x, y );
      }

      if( handle.actor )
      {
        handle.actor.SetImage( mHandleImages[type][HANDLE_IMAGE_RELEASED] );
      }
      handle.pressed = false;

      mIsHandlePanning = false;
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    HandleImpl& primarySelectionHandle = mHandle[LEFT_SELECTION_HANDLE];
    HandleImpl& secondarySelectionHandle = mHandle[RIGHT_SELECTION_HANDLE];

    if( actor == grabHandle.grabArea )
    {
      DoPan( grabHandle, GRAB_HANDLE, gesture );
    }
    else if( actor == primarySelectionHandle.grabArea )
    {
      DoPan( primarySelectionHandle, LEFT_SELECTION_HANDLE, gesture );
    }
    else if( actor == secondarySelectionHandle.grabArea )
    {
      DoPan( secondarySelectionHandle, RIGHT_SELECTION_HANDLE, gesture );
    }
  }

  bool OnGrabHandleTouched( Actor actor, const TouchData& touch )
  {
    HandleImpl& grabHandle = mHandle[GRAB_HANDLE];

    // Switch between pressed/release grab-handle images
    if( touch.GetPointCount() > 0 &&
        grabHandle.actor )
    {
      const PointState::Type state = touch.GetState( 0 );

      if( PointState::DOWN == state )
      {
        grabHandle.pressed = true;
      }
      else if( ( PointState::UP == state ) ||
               ( PointState::INTERRUPTED == state ) )
      {
        grabHandle.pressed = false;
      }

      SetHandleImage( GRAB_HANDLE );
    }

    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  bool OnHandleOneTouched( Actor actor, const TouchData& touch )
  {
    HandleImpl& primarySelectionHandle = mHandle[LEFT_SELECTION_HANDLE];

    // Switch between pressed/release selection handle images
    if( touch.GetPointCount() > 0 &&
        primarySelectionHandle.actor )
    {
      const PointState::Type state = touch.GetState( 0 );

      if( PointState::DOWN == state )
      {
        primarySelectionHandle.pressed = true;
        primarySelectionHandle.verticallyFlippedOnTouch = primarySelectionHandle.verticallyFlipped;
      }
      else if( ( PointState::UP == state ) ||
               ( PointState::INTERRUPTED == state ) )
      {
        primarySelectionHandle.pressed = false;
        mIsHandlePreviouslyCrossed = mIsHandleCurrentlyCrossed;
        mIsHandlePanning = false;
        mHandleReleased = LEFT_SELECTION_HANDLE;
      }

      SetHandleImage( LEFT_SELECTION_HANDLE );
    }

    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  bool OnHandleTwoTouched( Actor actor, const TouchData& touch )
  {
    HandleImpl& secondarySelectionHandle = mHandle[RIGHT_SELECTION_HANDLE];

    // Switch between pressed/release selection handle images
    if( touch.GetPointCount() > 0 &&
        secondarySelectionHandle.actor )
    {
      const PointState::Type state = touch.GetState( 0 );

      if( PointState::DOWN == state )
      {
        secondarySelectionHandle.pressed = true;
        secondarySelectionHandle.verticallyFlippedOnTouch = secondarySelectionHandle.verticallyFlipped;
      }
      else if( ( PointState::UP == state ) ||
               ( PointState::INTERRUPTED == state ) )
      {
        secondarySelectionHandle.pressed = false;
        mIsHandlePreviouslyCrossed = mIsHandleCurrentlyCrossed;
        mIsHandlePanning = false;
        mHandleReleased = RIGHT_SELECTION_HANDLE;
      }

      SetHandleImage( RIGHT_SELECTION_HANDLE );
    }

    // Consume to avoid pop-ups accidentally closing, when handle is outside of pop-up area
    return true;
  }

  void HandleResetPosition( PropertyNotification& source )
  {
    const HandleImpl& grabHandle = mHandle[GRAB_HANDLE];

    if( grabHandle.active )
    {
      // Sets the grab handle position and calculates if it needs to be vertically flipped if it exceeds the boundary box.
      SetGrabHandlePosition();

      // Sets the grab handle image according if it's pressed, flipped, etc.
      SetHandleImage( GRAB_HANDLE );
    }
    else
    {
      // Sets the primary selection handle position and calculates if it needs to be vertically flipped if it exceeds the boundary box.
      SetSelectionHandlePosition( LEFT_SELECTION_HANDLE );

      // Sets the primary handle image according if it's pressed, flipped, etc.
      SetHandleImage( LEFT_SELECTION_HANDLE );

      // Sets the secondary selection handle position and calculates if it needs to be vertically flipped if it exceeds the boundary box.
      SetSelectionHandlePosition( RIGHT_SELECTION_HANDLE );

      // Sets the secondary handle image according if it's pressed, flipped, etc.
      SetHandleImage( RIGHT_SELECTION_HANDLE );
    }
  }

  void SetupActiveLayerPropertyNotifications()
  {
    if( !mActiveLayer )
    {
      return;
    }

    // Vertical notifications.

    // Disconnect any previous connected callback.
    if( mHandleVerticalLessThanNotification )
    {
      mHandleVerticalLessThanNotification.NotifySignal().Disconnect( this, &Decorator::Impl::HandleResetPosition );
      mActiveLayer.RemovePropertyNotification( mHandleVerticalLessThanNotification );
    }

    if( mHandleVerticalGreaterThanNotification )
    {
      mHandleVerticalGreaterThanNotification.NotifySignal().Disconnect( this, &Decorator::Impl::HandleResetPosition );
      mActiveLayer.RemovePropertyNotification( mHandleVerticalGreaterThanNotification );
    }

    const HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    const HandleImpl& primaryHandle = mHandle[LEFT_SELECTION_HANDLE];
    const HandleImpl& secondaryHandle = mHandle[RIGHT_SELECTION_HANDLE];

    if( grabHandle.active )
    {
      if( grabHandle.verticallyFlipped )
      {
        // The grab handle is vertically flipped. Never is going to exceed the bottom edje of the display.
        mHandleVerticalGreaterThanNotification.Reset();

        // The vertical distance from the center of the active layer to the top edje of the display.
        const float topHeight = 0.5f * mControlSize.height - grabHandle.position.y + grabHandle.size.height;

        mHandleVerticalLessThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                    LessThanCondition( mBoundingBox.y + topHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalLessThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalLessThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
      }
      else
      {
        // The grab handle is not vertically flipped. Never is going to exceed the top edje of the display.
        mHandleVerticalLessThanNotification.Reset();

        // The vertical distance from the center of the active layer to the bottom edje of the display.
        const float bottomHeight = -0.5f * mControlSize.height + grabHandle.position.y + grabHandle.lineHeight + grabHandle.size.height;

        mHandleVerticalGreaterThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                       GreaterThanCondition( mBoundingBox.w - bottomHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalGreaterThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalGreaterThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
      }
    }
    else // The selection handles are active
    {
      if( primaryHandle.verticallyFlipped && secondaryHandle.verticallyFlipped )
      {
        // Both selection handles are vertically flipped. Never are going to exceed the bottom edje of the display.
        mHandleVerticalGreaterThanNotification.Reset();

        // The vertical distance from the center of the active layer to the top edje of the display.
        const float topHeight = 0.5f * mControlSize.height + std::max( -primaryHandle.position.y + primaryHandle.size.height, -secondaryHandle.position.y + secondaryHandle.size.height );

        mHandleVerticalLessThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                    LessThanCondition( mBoundingBox.y + topHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalLessThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalLessThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
      }
      else if( !primaryHandle.verticallyFlipped && !secondaryHandle.verticallyFlipped )
      {
        // Both selection handles aren't vertically flipped. Never are going to exceed the top edje of the display.
        mHandleVerticalLessThanNotification.Reset();

        // The vertical distance from the center of the active layer to the bottom edje of the display.
        const float bottomHeight = -0.5f * mControlSize.height + std::max( primaryHandle.position.y + primaryHandle.lineHeight + primaryHandle.size.height,
                                                                           secondaryHandle.position.y + secondaryHandle.lineHeight + secondaryHandle.size.height );

        mHandleVerticalGreaterThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                       GreaterThanCondition( mBoundingBox.w - bottomHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalGreaterThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalGreaterThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
      }
      else
      {
        // Only one of the selection handles is vertically flipped. Both vertical notifications are needed.

        // The vertical distance from the center of the active layer to the top edje of the display.
        const float topHeight = 0.5f * mControlSize.height + ( primaryHandle.verticallyFlipped                              ?
                                                               -primaryHandle.position.y + primaryHandle.size.height        :
                                                               -secondaryHandle.position.y + secondaryHandle.size.height );

        mHandleVerticalLessThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                    LessThanCondition( mBoundingBox.y + topHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalLessThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalLessThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );

        // The vertical distance from the center of the active layer to the bottom edje of the display.
        const float bottomHeight = -0.5f * mControlSize.height + ( primaryHandle.verticallyFlipped                                                       ?
                                                                   secondaryHandle.position.y + secondaryHandle.lineHeight + secondaryHandle.size.height :
                                                                   primaryHandle.position.y + primaryHandle.lineHeight + primaryHandle.size.height );

        mHandleVerticalGreaterThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                       GreaterThanCondition( mBoundingBox.w - bottomHeight ) );

        // Notifies the change from false to true and from true to false.
        mHandleVerticalGreaterThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

        // Connects the signals with the callbacks.
        mHandleVerticalGreaterThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
      }
    }

    // Horizontal notifications.

    // Disconnect any previous connected callback.
    if( mHandleHorizontalLessThanNotification )
    {
      mHandleHorizontalLessThanNotification.NotifySignal().Disconnect( this, &Decorator::Impl::HandleResetPosition );
      mActiveLayer.RemovePropertyNotification( mHandleHorizontalLessThanNotification );
    }

    if( mHandleHorizontalGreaterThanNotification )
    {
      mHandleHorizontalGreaterThanNotification.NotifySignal().Disconnect( this, &Decorator::Impl::HandleResetPosition );
      mActiveLayer.RemovePropertyNotification( mHandleHorizontalGreaterThanNotification );
    }

    if( primaryHandle.active || secondaryHandle.active )
    {
      // The horizontal distance from the center of the active layer to the left edje of the display.
      const float leftWidth = 0.5f * mControlSize.width + std::max( -primaryHandle.position.x + primaryHandle.size.width,
                                                                    -secondaryHandle.position.x + secondaryHandle.size.width );

      mHandleHorizontalLessThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_X,
                                                                                    LessThanCondition( mBoundingBox.x + leftWidth ) );

      // Notifies the change from false to true and from true to false.
      mHandleHorizontalLessThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

      // Connects the signals with the callbacks.
      mHandleHorizontalLessThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );

      // The horizontal distance from the center of the active layer to the right edje of the display.
      const float rightWidth = -0.5f * mControlSize.width + std::max( primaryHandle.position.x + primaryHandle.size.width,
                                                                      secondaryHandle.position.x + secondaryHandle.size.width );

      mHandleHorizontalGreaterThanNotification = mActiveLayer.AddPropertyNotification( Actor::Property::WORLD_POSITION_X,
                                                                                       GreaterThanCondition( mBoundingBox.z - rightWidth ) );

      // Notifies the change from false to true and from true to false.
      mHandleHorizontalGreaterThanNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

      // Connects the signals with the callbacks.
      mHandleHorizontalGreaterThanNotification.NotifySignal().Connect( this, &Decorator::Impl::HandleResetPosition );
    }
  }

  // Popup

  float AlternatePopUpPositionRelativeToCursor( bool topBottom )
  {
    float alternativePosition = 0.0f;

    const float halfPopupHeight = 0.5f * mCopyPastePopup.actor.GetRelayoutSize( Dimension::HEIGHT );

    const HandleImpl& primaryHandle = mHandle[LEFT_SELECTION_HANDLE];
    const HandleImpl& secondaryHandle = mHandle[RIGHT_SELECTION_HANDLE];
    const HandleImpl& grabHandle = mHandle[GRAB_HANDLE];
    const CursorImpl& cursor = mCursor[PRIMARY_CURSOR];

    if( primaryHandle.active || secondaryHandle.active )
    {
      float handleY = 0.f;
      float maxHandleHeight = 0.f;

      const bool primaryVisible = primaryHandle.horizontallyVisible && primaryHandle.verticallyVisible;
      const bool secondaryVisible = secondaryHandle.horizontallyVisible && secondaryHandle.verticallyVisible;

      if( primaryVisible && secondaryVisible )
      {
        handleY = std::max( primaryHandle.position.y, secondaryHandle.position.y );
        maxHandleHeight = std::max( primaryHandle.size.height, secondaryHandle.size.height );
      }
      else if( primaryVisible && !secondaryVisible )
      {
        handleY = primaryHandle.position.y;
        maxHandleHeight = primaryHandle.size.height;
      }
      else if( !primaryVisible && secondaryVisible )
      {
        handleY = secondaryHandle.position.y;
        maxHandleHeight = secondaryHandle.size.height;
      }

      alternativePosition = handleY + ( topBottom ? halfPopupHeight + maxHandleHeight + cursor.lineHeight : -halfPopupHeight - maxHandleHeight );
    }
    else
    {
      alternativePosition = cursor.position.y + ( topBottom ? halfPopupHeight + grabHandle.size.height + cursor.lineHeight : -halfPopupHeight - grabHandle.size.height );
    }

    return alternativePosition;
  }

  void PopUpLeavesTopBoundary( PropertyNotification& source )
  {
    const float popupHeight = mCopyPastePopup.actor.GetRelayoutSize( Dimension::HEIGHT );

    // Sets the position of the popup below.
    mCopyPastePopup.actor.SetY( floorf( CalculateVerticalPopUpPosition( 0.5f * popupHeight, true ) ) );
  }

  void PopUpLeavesBottomBoundary( PropertyNotification& source )
  {
    const float popupHeight = mCopyPastePopup.actor.GetRelayoutSize( Dimension::HEIGHT );

    // Sets the position of the popup above.
    mCopyPastePopup.actor.SetY( floorf( CalculateVerticalPopUpPosition( 0.5f * popupHeight, false ) ) );
  }

  void SetUpPopupPositionNotifications( const Vector3& popupHalfSize )
  {
    // Disconnect any previous connected callback.
    if( mPopupTopExceedNotification )
    {
      mPopupTopExceedNotification.NotifySignal().Disconnect( this, &Decorator::Impl::PopUpLeavesTopBoundary );
      mCopyPastePopup.actor.RemovePropertyNotification( mPopupTopExceedNotification );
    }

    if( mPopupBottomExceedNotification )
    {
      mPopupBottomExceedNotification.NotifySignal().Disconnect( this, &Decorator::Impl::PopUpLeavesBottomBoundary );
      mCopyPastePopup.actor.RemovePropertyNotification( mPopupBottomExceedNotification );
    }

    // Note Property notifications ignore any set anchor point so conditions must allow for this.  Default is Top Left.

    // Exceeding vertical boundary

    mPopupTopExceedNotification = mCopyPastePopup.actor.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                 LessThanCondition( mBoundingBox.y + popupHalfSize.height ) );

    mPopupBottomExceedNotification = mCopyPastePopup.actor.AddPropertyNotification( Actor::Property::WORLD_POSITION_Y,
                                                                                    GreaterThanCondition( mBoundingBox.w - popupHalfSize.height ) );

    // Notifies the change from false to true and from true to false.
    mPopupTopExceedNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );
    mPopupBottomExceedNotification.SetNotifyMode( PropertyNotification::NotifyOnChanged );

    mPopupTopExceedNotification.NotifySignal().Connect( this, &Decorator::Impl::PopUpLeavesTopBoundary );
    mPopupBottomExceedNotification.NotifySignal().Connect( this, &Decorator::Impl::PopUpLeavesBottomBoundary );
  }

  void SetHandleImage( HandleType handleType, HandleImageType handleImageType, Dali::Image image )
  {
    HandleImpl& handle = mHandle[handleType];
    handle.size = Size( image.GetWidth(), image.GetHeight() );

    mHandleImages[handleType][handleImageType] = image;
  }

  void SetScrollThreshold( float threshold )
  {
    mScrollThreshold = threshold;
  }

  float GetScrollThreshold() const
  {
    return mScrollThreshold;
  }

  void SetScrollSpeed( float speed )
  {
    mScrollSpeed = speed;
    mScrollDistance = speed * SCROLL_TICK_INTERVAL * TO_SECONDS;
  }

  float GetScrollSpeed() const
  {
    return mScrollSpeed;
  }

  void NotifyEndOfScroll()
  {
    StopScrollTimer();

    if( mScrollTimer )
    {
      mNotifyEndOfScroll = true;
    }
  }

  /**
   * Creates and starts a timer to scroll the text when handles are close to the edges of the text.
   *
   * It only starts the timer if it's already created.
   */
  void StartScrollTimer()
  {
    if( !mScrollTimer )
    {
      mScrollTimer = Timer::New( SCROLL_TICK_INTERVAL );
      mScrollTimer.TickSignal().Connect( this, &Decorator::Impl::OnScrollTimerTick );
    }

    if( !mScrollTimer.IsRunning() )
    {
      mScrollTimer.Start();
    }
  }

  /**
   * Stops the timer used to scroll the text.
   */
  void StopScrollTimer()
  {
    if( mScrollTimer )
    {
      mScrollTimer.Stop();
    }
  }

  /**
   * Callback called by the timer used to scroll the text.
   *
   * It calculates and sets a new scroll position.
   */
  bool OnScrollTimerTick()
  {
    if( HANDLE_TYPE_COUNT != mHandleScrolling )
    {
      float x = 0.f;
      float y = 0.f;

      switch( mScrollDirection )
      {
        case SCROLL_RIGHT:
        {
          x = mScrollDistance;
          break;
        }
        case SCROLL_LEFT:
        {
          x = -mScrollDistance;
          break;
        }
        case SCROLL_TOP:
        {
          y = mScrollDistance;
          break;
        }
        case SCROLL_BOTTOM:
        {
          y = -mScrollDistance;
          break;
        }
        default:
          break;
      }

      mController.DecorationEvent( mHandleScrolling,
                                   HANDLE_SCROLLING,
                                   x,
                                   y );
    }

    return true;
  }

  ControllerInterface& mController;

  TapGestureDetector       mTapDetector;
  PanGestureDetector       mPanDetector;
  LongPressGestureDetector mLongPressDetector;

  Timer               mCursorBlinkTimer;          ///< Timer to signal cursor to blink
  Timer               mScrollTimer;               ///< Timer used to scroll the text when the grab handle is moved close to the edges.

  Layer                mActiveLayer;                             ///< Layer for active handles and alike that ensures they are above all else.
  PropertyNotification mHandleVerticalLessThanNotification;      ///< Notifies when the 'y' coord of the active layer is less than a given value.
  PropertyNotification mHandleVerticalGreaterThanNotification;   ///< Notifies when the 'y' coord of the active layer is grater than a given value.
  PropertyNotification mHandleHorizontalLessThanNotification;    ///< Notifies when the 'x' coord of the active layer is less than a given value.
  PropertyNotification mHandleHorizontalGreaterThanNotification; ///< Notifies when the 'x' coord of the active layer is grater than a given value.
  PropertyNotification mPopupTopExceedNotification;              ///< Notifies when the popup leaves the bounding box through the top.
  PropertyNotification mPopupBottomExceedNotification;           ///< Notifies when the popup leaves the bounding box through the bottom.
  Control              mPrimaryCursor;
  Control              mSecondaryCursor;

  Actor               mHighlightActor;            ///< Actor to display highlight
  Renderer            mHighlightRenderer;
  Shader              mHighlightShader;           ///< Shader used for highlight
  Property::Map       mQuadVertexFormat;
  PopupImpl           mCopyPastePopup;
  TextSelectionPopup::Buttons mEnabledPopupButtons; /// Bit mask of currently enabled Popup buttons
  TextSelectionPopupCallbackInterface& mTextSelectionPopupCallbackInterface;

  Image               mHandleImages[HANDLE_TYPE_COUNT][HANDLE_IMAGE_TYPE_COUNT];
  Vector4             mHandleColor;

  CursorImpl          mCursor[CURSOR_COUNT];
  HandleImpl          mHandle[HANDLE_TYPE_COUNT];

  PropertyBuffer      mQuadVertices;
  Geometry            mQuadGeometry;
  QuadContainer       mHighlightQuadList;         ///< Sub-selections that combine to create the complete selection highlight.

  Vector4             mBoundingBox;               ///< The bounding box in world coords.
  Vector4             mHighlightColor;            ///< Color of the highlight
  Vector2             mHighlightPosition;         ///< The position of the highlight actor.
  Size                mHighlightSize;             ///< The size of the highlighted text.
  Size                mControlSize;               ///< The control's size. Set by the Relayout.

  unsigned int        mActiveCursor;
  unsigned int        mCursorBlinkInterval;
  float               mCursorBlinkDuration;
  float               mCursorWidth;             ///< The width of the cursors in pixels.
  HandleType          mHandleScrolling;         ///< The handle which is scrolling.
  HandleType          mHandleReleased;          ///< The last handle released.
  ScrollDirection     mScrollDirection;         ///< The direction of the scroll.
  float               mScrollThreshold;         ///< Defines a square area inside the control, close to the edge. A cursor entering this area will trigger scroll events.
  float               mScrollSpeed;             ///< The scroll speed in pixels per second.
  float               mScrollDistance;          ///< Distance the text scrolls during a scroll interval.
  int                 mTextDepth;               ///< The depth used to render the text.

  bool                mActiveCopyPastePopup              : 1;
  bool                mPopupSetNewPosition               : 1;
  bool                mCursorBlinkStatus                 : 1; ///< Flag to switch between blink on and blink off.
  bool                mDelayCursorBlink                  : 1; ///< Used to avoid cursor blinking when entering text.
  bool                mPrimaryCursorVisible              : 1; ///< Whether the primary cursor is visible.
  bool                mSecondaryCursorVisible            : 1; ///< Whether the secondary cursor is visible.
  bool                mFlipSelectionHandlesOnCross       : 1; ///< Whether to flip the selection handles as soon as they cross.
  bool                mFlipLeftSelectionHandleDirection  : 1; ///< Whether to flip the left selection handle image because of the character's direction.
  bool                mFlipRightSelectionHandleDirection : 1; ///< Whether to flip the right selection handle image because of the character's direction.
  bool                mIsHandlePanning                   : 1; ///< Whether any of the handles is moving.
  bool                mIsHandleCurrentlyCrossed          : 1; ///< Whether the handles are crossed.
  bool                mIsHandlePreviouslyCrossed         : 1; ///< Whether the handles where crossed at the last handle touch up.
  bool                mNotifyEndOfScroll                 : 1; ///< Whether to notify the end of the scroll.
  bool                mHorizontalScrollingEnabled        : 1; ///< Whether the horizontal scrolling is enabled.
  bool                mVerticalScrollingEnabled          : 1; ///< Whether the vertical scrolling is enabled.
  bool                mSmoothHandlePanEnabled            : 1; ///< Whether to pan smoothly the handles.
  bool                mIsHighlightBoxActive              : 1; ///< Whether the highlight box is active.
};

DecoratorPtr Decorator::New( ControllerInterface& controller,
                             TextSelectionPopupCallbackInterface& callbackInterface )
{
  return DecoratorPtr( new Decorator( controller,
                                      callbackInterface ) );
}

void Decorator::SetBoundingBox( const Rect<int>& boundingBox )
{
  LocalToWorldCoordinatesBoundingBox( boundingBox, mImpl->mBoundingBox );
}

void Decorator::GetBoundingBox( Rect<int>& boundingBox ) const
{
  WorldToLocalCoordinatesBoundingBox( mImpl->mBoundingBox, boundingBox );
}

void Decorator::Relayout( const Vector2& size )
{
  mImpl->Relayout( size );
}

void Decorator::UpdatePositions( const Vector2& scrollOffset )
{
  mImpl->UpdatePositions( scrollOffset );
}

/** Cursor **/

void Decorator::SetActiveCursor( ActiveCursor activeCursor )
{
  mImpl->mActiveCursor = activeCursor;
}

unsigned int Decorator::GetActiveCursor() const
{
  return mImpl->mActiveCursor;
}

void Decorator::SetPosition( Cursor cursor, float x, float y, float cursorHeight, float lineHeight )
{
  Impl::CursorImpl& cursorImpl = mImpl->mCursor[cursor];

  cursorImpl.position.x = x;
  cursorImpl.position.y = y;
  cursorImpl.cursorHeight = cursorHeight;
  cursorImpl.lineHeight = lineHeight;
}

void Decorator::GetPosition( Cursor cursor, float& x, float& y, float& cursorHeight, float& lineHeight ) const
{
  const Impl::CursorImpl& cursorImpl = mImpl->mCursor[cursor];

  x = cursorImpl.position.x;
  y = cursorImpl.position.y;
  cursorHeight = cursorImpl.cursorHeight;
  lineHeight = cursorImpl.lineHeight;
}

const Vector2& Decorator::GetPosition( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].position;
}

void Decorator::SetCursorColor( Cursor cursor, const Dali::Vector4& color )
{
  mImpl->mCursor[cursor].color = color;
}

const Dali::Vector4& Decorator::GetColor( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].color;
}

void Decorator::StartCursorBlink()
{
  if ( !mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer = Timer::New( mImpl->mCursorBlinkInterval );
    mImpl->mCursorBlinkTimer.TickSignal().Connect( mImpl, &Decorator::Impl::OnCursorBlinkTimerTick );
  }

  if ( !mImpl->mCursorBlinkTimer.IsRunning() )
  {
    mImpl->mCursorBlinkTimer.Start();
  }
}

void Decorator::StopCursorBlink()
{
  if ( mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer.Stop();
  }

  mImpl->mCursorBlinkStatus = true; // Keep cursor permanently shown
}

void Decorator::DelayCursorBlink()
{
  mImpl->mCursorBlinkStatus = true; // Show cursor for a bit longer
  mImpl->mDelayCursorBlink = true;
}

void Decorator::SetCursorBlinkInterval( float seconds )
{
  mImpl->mCursorBlinkInterval = static_cast<unsigned int>( seconds * TO_MILLISECONDS ); // Convert to milliseconds
}

float Decorator::GetCursorBlinkInterval() const
{
  return static_cast<float>( mImpl->mCursorBlinkInterval ) * TO_SECONDS;
}

void Decorator::SetCursorBlinkDuration( float seconds )
{
  mImpl->mCursorBlinkDuration = seconds;
}

float Decorator::GetCursorBlinkDuration() const
{
  return mImpl->mCursorBlinkDuration;
}

void Decorator::SetCursorWidth( int width )
{
  mImpl->mCursorWidth = static_cast<float>( width );
}

int Decorator::GetCursorWidth() const
{
  return static_cast<int>( mImpl->mCursorWidth );
}

/** Handles **/

void Decorator::SetHandleActive( HandleType handleType, bool active )
{
  mImpl->mHandle[handleType].active = active;

  if( !active )
  {
    if( ( LEFT_SELECTION_HANDLE == handleType ) || ( RIGHT_SELECTION_HANDLE == handleType ) )
    {
      mImpl->mIsHandlePreviouslyCrossed = false;
    }

    // TODO: this is a work-around.
    // The problem is the handle actor does not receive the touch event with the Interrupt
    // state when the power button is pressed and the application goes to background.
    mImpl->mHandle[handleType].pressed = false;
    Image imageReleased = mImpl->mHandleImages[handleType][HANDLE_IMAGE_RELEASED];
    ImageView imageView = mImpl->mHandle[handleType].actor;
    if( imageReleased && imageView )
    {
      imageView.SetImage( imageReleased );
    }
  }

}

bool Decorator::IsHandleActive( HandleType handleType ) const
{
  return mImpl->mHandle[handleType].active ;
}

void Decorator::SetHandleImage( HandleType handleType, HandleImageType handleImageType, Dali::Image image )
{
  mImpl->SetHandleImage( handleType, handleImageType, image );
}

Dali::Image Decorator::GetHandleImage( HandleType handleType, HandleImageType handleImageType ) const
{
  return mImpl->mHandleImages[handleType][handleImageType];
}

void Decorator::SetHandleColor( const Vector4& color )
{
  mImpl->mHandleColor = color;
}

const Vector4& Decorator::GetHandleColor() const
{
  return mImpl->mHandleColor;
}

void Decorator::SetPosition( HandleType handleType, float x, float y, float height )
{
  // Adjust handle's displacement
  Impl::HandleImpl& handle = mImpl->mHandle[handleType];

  handle.position.x = x;
  handle.position.y = y;
  handle.lineHeight = height;

  if( mImpl->mSmoothHandlePanEnabled )
  {
    handle.grabDisplacementX = 0.f;
    handle.grabDisplacementY = 0.f;
  }
}

void Decorator::GetPosition( HandleType handleType, float& x, float& y, float& height ) const
{
  Impl::HandleImpl& handle = mImpl->mHandle[handleType];

  x = handle.position.x;
  y = handle.position.y;
  height = handle.lineHeight;
}

const Vector2& Decorator::GetPosition( HandleType handleType ) const
{
  return mImpl->mHandle[handleType].position;
}

void Decorator::FlipHandleVertically( HandleType handleType, bool flip )
{
  mImpl->mHandle[handleType].verticallyFlippedPreferred = flip;
}

bool Decorator::IsHandleVerticallyFlipped( HandleType handleType ) const
{
  return mImpl->mHandle[handleType].verticallyFlippedPreferred;
}

void Decorator::FlipSelectionHandlesOnCrossEnabled( bool enable )
{
  mImpl->mFlipSelectionHandlesOnCross = enable;
}

void Decorator::SetSelectionHandleFlipState( bool indicesSwapped, bool left, bool right )
{
  mImpl->mIsHandleCurrentlyCrossed = indicesSwapped;
  mImpl->mFlipLeftSelectionHandleDirection = left;
  mImpl->mFlipRightSelectionHandleDirection = right;
}

void Decorator::AddHighlight( unsigned int index, const Vector4& quad )
{
  *( mImpl->mHighlightQuadList.Begin() + index ) = quad;
}

void Decorator::SetHighLightBox( const Vector2& position, const Size& size )
{
  mImpl->mHighlightPosition = position;
  mImpl->mHighlightSize = size;
}

void Decorator::ClearHighlights()
{
  mImpl->mHighlightQuadList.Clear();
  mImpl->mHighlightPosition = Vector2::ZERO;
}

void Decorator::ResizeHighlightQuads( unsigned int numberOfQuads )
{
  mImpl->mHighlightQuadList.Resize( numberOfQuads );
}

void Decorator::SetHighlightColor( const Vector4& color )
{
  mImpl->mHighlightColor = color;
}

const Vector4& Decorator::GetHighlightColor() const
{
  return mImpl->mHighlightColor;
}

void Decorator::SetHighlightActive( bool active )
{
  mImpl->mIsHighlightBoxActive = active;
}

bool Decorator::IsHighlightActive() const
{
  return mImpl->mIsHighlightBoxActive;
}

void Decorator::SetTextDepth( int textDepth )
{
  mImpl->mTextDepth = textDepth;
}

void Decorator::SetPopupActive( bool active )
{
  mImpl->mActiveCopyPastePopup = active;
}

bool Decorator::IsPopupActive() const
{
  return mImpl->mActiveCopyPastePopup;
}

void Decorator::SetEnabledPopupButtons( TextSelectionPopup::Buttons& enabledButtonsBitMask )
{
  mImpl->mEnabledPopupButtons = enabledButtonsBitMask;

  if ( !mImpl->mCopyPastePopup.actor )
  {
    mImpl->mCopyPastePopup.actor = TextSelectionPopup::New( &mImpl->mTextSelectionPopupCallbackInterface );
#ifdef DECORATOR_DEBUG
    mImpl->mCopyPastePopup.actor.SetName("mCopyPastePopup");
#endif
    mImpl->mCopyPastePopup.actor.SetAnchorPoint( AnchorPoint::CENTER );
    mImpl->mCopyPastePopup.actor.OnRelayoutSignal().Connect( mImpl, &Decorator::Impl::SetPopupPosition ); // Position popup after size negotiation
  }

  mImpl->mCopyPastePopup.actor.EnableButtons( mImpl->mEnabledPopupButtons );
}

TextSelectionPopup::Buttons& Decorator::GetEnabledPopupButtons()
{
  return mImpl->mEnabledPopupButtons;
}

/** Scroll **/

void Decorator::SetScrollThreshold( float threshold )
{
  mImpl->SetScrollThreshold( threshold );
}

float Decorator::GetScrollThreshold() const
{
  return mImpl->GetScrollThreshold();
}

void Decorator::SetScrollSpeed( float speed )
{
  mImpl->SetScrollSpeed( speed );
}

float Decorator::GetScrollSpeed() const
{
  return mImpl->GetScrollSpeed();
}

void Decorator::NotifyEndOfScroll()
{
  mImpl->NotifyEndOfScroll();
}

void Decorator::SetHorizontalScrollEnabled( bool enable )
{
  mImpl->mHorizontalScrollingEnabled = enable;
}

bool Decorator::IsHorizontalScrollEnabled() const
{
  return mImpl->mHorizontalScrollingEnabled;
}

void Decorator::SetVerticalScrollEnabled( bool enable )
{
  mImpl->mVerticalScrollingEnabled = enable;
}

bool Decorator::IsVerticalScrollEnabled() const
{
  return mImpl->mVerticalScrollingEnabled;
}

void Decorator::SetSmoothHandlePanEnabled( bool enable )
{
  mImpl->mSmoothHandlePanEnabled = enable;
}

bool Decorator::IsSmoothHandlePanEnabled() const
{
  return mImpl->mSmoothHandlePanEnabled;
}

Decorator::~Decorator()
{
  delete mImpl;
}

Decorator::Decorator( ControllerInterface& controller,
                      TextSelectionPopupCallbackInterface& callbackInterface )
: mImpl( NULL )
{
  mImpl = new Decorator::Impl( controller, callbackInterface );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
