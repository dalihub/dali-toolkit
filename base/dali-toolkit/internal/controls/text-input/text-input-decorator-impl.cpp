/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-input/text-input-decorator-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/clipboard.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-input/text-input-handles-impl.h>

using namespace Dali;

namespace
{
#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXT_INPUT_DECORATOR");
#endif

const Vector3 DEFAULT_SELECTION_HANDLE_SIZE( 51.0f, 79.0f, 0.0f );
const float TOP_HANDLE_TOP_OFFSET(-1.5f);                                   // Offset between top handle and cutCopyPaste pop-up
const float BOTTOM_HANDLE_BOTTOM_OFFSET(1.5f);                              // Offset between bottom handle and cutCopyPaste pop-up
const float UI_Z_OFFSET( 0.2f );                                            // Text Selection Handles/Cursor z-offset.
const Vector3 UI_OFFSET(0.0f, 0.0f, UI_Z_OFFSET);                           // Text Selection Handles/Cursor offset.
const char* DEFAULT_CURSOR( DALI_IMAGE_DIR "cursor.png" );
const Vector4 DEFAULT_CURSOR_IMAGE_9_BORDER( 2.0f, 2.0f, 2.0f, 2.0f );
const std::size_t CURSOR_BLINK_INTERVAL = 500;                              // Cursor blink interval
const float CURSOR_THICKNESS(6.0f);
const Degree CURSOR_ANGLE_OFFSET(2.0f);                                     // Offset from the angle

const unsigned int SCROLL_TICK_INTERVAL = 50u;
const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 15.f;

/**
 * Whether the given position plus the cursor size offset is inside the given boundary.
 *
 * @param[in] position The given position.
 * @param[in] cursorSize The cursor size.
 * @param[in] controlSize The given boundary.
 * @param[in] threshold imaginary indent around boundary that will trigger the position to be outside of control.
 *
 * @return whether the given position is inside the given boundary.
 */
bool IsPositionWithinControl( const Vector3& position, const Size& cursorSize, const Vector3& controlSize, const Vector2 threshold = Vector2::ZERO )
{
  return ( position.x >= -Math::MACHINE_EPSILON_1000 + threshold.x ) &&
         ( position.x <= controlSize.width - threshold.x + Math::MACHINE_EPSILON_1000 ) &&
         ( position.y - cursorSize.height >= -Math::MACHINE_EPSILON_1000 + threshold.y ) &&
         ( position.y <= controlSize.height + Math::MACHINE_EPSILON_1000 - threshold.y);
}

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

Decorator::Decorator( TextViewCharacterPositioning& textViewManager, TextInputTextStyle& textStyle ):
  mTextViewCharacterPositioning( textViewManager ),
  mTextStyle( textStyle ),
  mSelectionHandleOnePosition(0),
  mSelectionHandleTwoPosition(0),
  mGrabHandlePosition(0),
  mCursorPosition( 0 ),
  mTextHighlight( textViewManager ),
  mCursorBlinkStatus( true ),
  mCursorVisibility( true ),
  mCursorRTLEnabled( false ),
  mIsGrabHandleInScrollArea( false ),
  mIsCursorInScrollArea( false ),
  mGrabHandleEnabled( true )
{
}

Decorator::~Decorator()
{
}

/**
 * Bounding Box
 */
void Decorator::SetBoundingBox( const Rect<float>& boundingRectangle )
{
  // Convert to world coordinates and store as a Vector4 to be compatible with Property Notifications.
  Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  const float originX = boundingRectangle.x - 0.5f * stageSize.width;
  const float originY = boundingRectangle.y - 0.5f * stageSize.height;

  const Vector4 boundary( originX,
                          originY,
                          originX + boundingRectangle.width,
                          originY + boundingRectangle.height );

  mBoundingRectangleWorldCoordinates = boundary;
}

Vector4 Decorator::GetBoundingBox() const
{
  return mBoundingRectangleWorldCoordinates;
}

/**
 * Selection Handles
 */
void Decorator::OnHandlePan(Actor actor, const PanGesture& gesture)
{
  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();

  switch (gesture.state)
  {
    case Gesture::Started:
    // fall through so code not duplicated
    case Gesture::Continuing:
    {
      if ( actor.GetParent() == mTextInputHandles.GetSelectionHandleOne() )
      {
        MoveSelectionHandle( selectionHandleOne, mSelectionHandleOneActualPosition, mSelectionHandleOnePosition, gesture.displacement );
        HidePopUp();
      }
      else if ( actor.GetParent() == mTextInputHandles.GetSelectionHandleTwo() )
      {
        MoveSelectionHandle( selectionHandleTwo, mSelectionHandleTwoActualPosition, mSelectionHandleTwoPosition, gesture.displacement );
        HidePopUp();
      }
      else if ( actor.GetParent() == mTextInputHandles.GetGrabHandle() )
      {
        SetCursorVisibility( true );
        ShowGrabHandle( mGrabHandleVisibility && mIsGrabHandleInScrollArea );
        MoveGrabHandle( gesture.displacement );
        HidePopUp(); // Do not show popup while handle is moving
      }
    }
    break;

    case Gesture::Finished:
    {
      // Revert back to non-pressed selection handle images
      if ( actor.GetParent() == mTextInputHandles.GetSelectionHandleOne() )
      {
         mSelectionHandleOneActualPosition = MoveSelectionHandle( selectionHandleOne, mSelectionHandleOneActualPosition, mSelectionHandleOnePosition, gesture.displacement );
        ShowPopupCutCopyPaste();
      }
      else if ( actor.GetParent() == mTextInputHandles.GetSelectionHandleTwo() )
      {
        mSelectionHandleTwoActualPosition = MoveSelectionHandle( selectionHandleTwo, mSelectionHandleTwoActualPosition, mSelectionHandleTwoPosition, gesture.displacement );
        ShowPopupCutCopyPaste();
      }
      else if ( actor.GetParent() == mTextInputHandles.GetGrabHandle() )
      {
        MoveGrabHandle( gesture.displacement );
        SetCursorVisibility( true );
        ShowPopupCutCopyPaste();
      }
    }
    break;
    default:
      break;
  }
}

void Decorator::CreateSelectionHandles( Actor targetParent )
{
  if ( !mPanGestureDetector )
  {
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &Decorator::OnHandlePan);
  }

  if ( !mTextInputHandles.GetSelectionHandleOne() )
  {
    mTextInputHandles.CreateSelectionHandles();

    mTextInputHandles.AttachSelectionHandlesToGivenPanGesture( mPanGestureDetector );

    targetParent.Add( mTextInputHandles.GetSelectionHandleOne() );
    targetParent.Add( mTextInputHandles.GetSelectionHandleTwo() );

    SetUpHandlePropertyNotifications();
  }
}

void Decorator::RemoveSelectionHandles()
{
  mTextInputHandles.DestorySelectionHandles();
}

Vector3 Decorator::GetSelectionHandleSize()
{
  return DEFAULT_SELECTION_HANDLE_SIZE;
}

std::size_t Decorator::GetHandleOnePosition() const
{
  return mSelectionHandleOnePosition;
}

std::size_t Decorator::GetHandleTwoPosition() const
{
  return mSelectionHandleTwoPosition;
}

Vector3 Decorator::PositionSelectionHandle( Actor selectionHandle, std::size_t position )
{
  bool direction(false);
  Vector3 alternatePosition;
  bool alternatePositionValid(false);

  Vector3 actualPositionOfSelectionHandle = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( position, direction, alternatePosition,alternatePositionValid );

  return PositionSelectionHandle( selectionHandle, actualPositionOfSelectionHandle, position );

}

Vector3 Decorator::PositionSelectionHandle( Actor selectionHandle, Vector3& actualPosition, std::size_t position )
{
  const Vector3 DEFAULT_HANDLE_OFFSET(0.0f, -5.0f, 0.0f);

  selectionHandle.SetPosition( actualPosition += DEFAULT_HANDLE_OFFSET );

  return actualPosition;
}

void Decorator::SetSelectionHandlesVisibility(bool visible )
{
  mTextInputHandles.SetSelectionHandleOneVisibility( visible );
  mTextInputHandles.SetSelectionHandleTwoVisibility( visible );
}

void Decorator::PositionSelectionHandles( std::size_t start, std::size_t end )
{
  mSelectionHandleOnePosition = start;
  mSelectionHandleTwoPosition = end;

  mTextViewCharacterPositioning.UpdateTextLayoutInfo();

  mSelectionHandleOneActualPosition = PositionSelectionHandle( mTextInputHandles.GetSelectionHandleOne(), mSelectionHandleOnePosition );
  mSelectionHandleTwoActualPosition = PositionSelectionHandle( mTextInputHandles.GetSelectionHandleTwo(), mSelectionHandleTwoPosition );
}

Vector3 Decorator::MoveSelectionHandle( Actor selectionHandle,
                                                          Vector3& actualSelectionHandlePosition,
                                                          std::size_t& currentSelectionHandlePosition,
                                                          const Vector2& displacement )
{
  Vector3 actualHandlePosition;
  actualSelectionHandlePosition.x += displacement.x * selectionHandle.GetCurrentScale().x;
  actualSelectionHandlePosition.y += displacement.y * selectionHandle.GetCurrentScale().y;

  // Selection handles should jump to the nearest character
  std::size_t newHandlePosition = 0;
  newHandlePosition = mTextViewCharacterPositioning.ReturnClosestIndex( actualSelectionHandlePosition.GetVectorXY() );

  bool direction(false);
  Vector3 alternatePosition;
  bool alternatePositionValid(false);
  actualHandlePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( newHandlePosition,direction, alternatePosition, alternatePositionValid  );

  bool handleVisible = true;

  if ( handleVisible &&                                          // Ensure the handle is visible.
     ( newHandlePosition != currentSelectionHandlePosition ) &&  // Ensure the handle has moved.
     ( newHandlePosition != mSelectionHandleTwoPosition ) &&     // Ensure new handle position not the same position as an existing handle.
     ( newHandlePosition != mSelectionHandleOnePosition ) )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::MoveSelectionHandle Handle visible and moved]\n");

    currentSelectionHandlePosition = newHandlePosition;

    PositionSelectionHandle( selectionHandle, actualHandlePosition, newHandlePosition );

    ShowUpdatedHighlight();

    // Set Active Style to that of first character in selection
    std::size_t firstHandleInSelection = std::min( mSelectionHandleOnePosition, mSelectionHandleTwoPosition );

    const TextStyle inputStyle = mTextViewCharacterPositioning.GetStyleAt( firstHandleInSelection );
    mTextStyle.SetInputStyle( inputStyle );
  }
  return actualHandlePosition; // Returns Handle position passed in if new value not assigned.
}

/**
 * GrabHandle
 */
void Decorator::PositionGrabHandle( std::size_t positionInText )
{
  bool direction(false);
  Vector3 alternatePosition;
  bool alternatePositionValid(false);

  mGrabHandlePosition = positionInText;

  mTextViewCharacterPositioning.UpdateTextLayoutInfo();
  mActualGrabHandlePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( positionInText, direction, alternatePosition,alternatePositionValid );

  mTextInputHandles.GetGrabHandle().SetPosition( mActualGrabHandlePosition );
}

void Decorator::MoveGrabHandle( const Vector2& displacement /*, std::size_t currentHandlePosition */)
{
  mActualGrabHandlePosition.x += displacement.x;
  mActualGrabHandlePosition.y += displacement.y;

  // Grab handle should jump to the nearest character and take cursor with it
  std::size_t newHandlePosition = mTextViewCharacterPositioning.ReturnClosestIndex( mActualGrabHandlePosition.GetVectorXY() );

  Vector3 actualHandlePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( newHandlePosition );

  bool handleVisible = true;

  if( ( newHandlePosition != mGrabHandlePosition ) &&                           // Only redraw cursor and do updates if position changed
        ( handleVisible ) )// and the new position is visible (if scroll is not enabled, it's always true).
    {
      mActualGrabHandlePosition = actualHandlePosition;
      mTextInputHandles.GetGrabHandle().SetPosition( mActualGrabHandlePosition );

      //PositionGrabHandle( newHandlePosition );
      mGrabHandlePosition = newHandlePosition;
      SetCurrentCursorPosition( mGrabHandlePosition );
      DrawCursor( mGrabHandlePosition );

      const std::size_t cursorPosition = GetCurrentCursorPosition();

      // Let keyboard know the new cursor position so can 're-capture' for prediction.
      mCursorRePositionedSignal.Emit();

      // Set Input Style to that of cursor position
      if ( !mTextViewCharacterPositioning.IsStyledTextEmpty()  && ( cursorPosition > 0 ) )
      {
        DALI_ASSERT_DEBUG( ( 0 <= cursorPosition-1 ) && ( cursorPosition-1 < mTextViewCharacterPositioning.StyledTextSize() ) );
      }
    }
}

void Decorator::ShowGrabHandle( bool visible )
{
  mGrabHandleVisibility = visible;
  mTextInputHandles.SetGrabHandleVisibility( visible );
}

void Decorator::CreateGrabHandle( Actor targetParent )
{
  if ( !mPanGestureDetector )
  {
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &Decorator::OnHandlePan);
  }

  if ( !mTextInputHandles.GetGrabHandle() )
  {
    mTextInputHandles.CreateGrabHandle();
    mTextInputHandles.AttachGrabHandleToGivenPanGesture( mPanGestureDetector );
    targetParent.Add( mTextInputHandles.GetGrabHandle() );
  }
}

void Decorator::SetGrabHandleImage( Image image )
{
  mTextInputHandles.SetGrabHandleImage( image );
}

void Decorator::EnableGrabHandle( bool toggle)
{
  // enables grab handle with will in turn de-activate magnifier
  mGrabHandleEnabled = toggle;
}

bool Decorator::IsGrabHandleEnabled()
{
  // if false then magnifier will be shown instead.
  return mGrabHandleEnabled;
}

/**
 * Cursor
 */
std::size_t Decorator::GetCurrentCursorPosition() const
{
  return mCursorPosition;
}

void Decorator::SetCurrentCursorPosition( std::size_t newCursorPosition )
{
  mCursorPosition = newCursorPosition;
}

void Decorator::SetCursorVisibility( bool visible )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::SetCursorVisibility[%s]\n", ( visible )?"true":"false");

  mCursorVisibility = visible;
  mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea );
  mCursorRTL.SetVisible( mCursorVisibility && mCursorRTLEnabled );
}

void Decorator::DrawCursor(const std::size_t nthChar)
{
  std::size_t cursorPosition = GetCurrentCursorPosition();

  // Get height of cursor and set its size
  Size size( CURSOR_THICKNESS, 0.0f );

  Vector2 min, max; // out parameters for GetRowRectFromCharacterPosition
  size.height = mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( mTextViewCharacterPositioning.GetVisualPosition( cursorPosition ), min, max  ).height;

  mCursor.SetSize(size);

  // If the character is italic then the cursor also tilts.
  if ( !mTextViewCharacterPositioning.IsStyledTextEmpty()  && ( cursorPosition > 0 ) )
  {
    DALI_ASSERT_DEBUG( ( 0 <= cursorPosition-1 ) && ( cursorPosition-1 < mTextViewCharacterPositioning.StyledTextSize() ) );
    const TextStyle styleAtCursor = mTextViewCharacterPositioning.GetStyleAt( cursorPosition-1 );
    mCursor.SetRotation( styleAtCursor.IsItalicsEnabled() ? Degree( styleAtCursor.GetItalicsAngle() - CURSOR_ANGLE_OFFSET ) : Degree( 0.f ), Vector3::ZAXIS );
  }

  DALI_ASSERT_DEBUG( cursorPosition <= mTextViewCharacterPositioning.GetNumberOfCharactersInText() );
  if ( ( cursorPosition <= mTextViewCharacterPositioning.GetNumberOfCharactersInText() ) )
  {
    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid( false );  // Alternate cursor validity flag.
    bool directionRTL( false );      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 position = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( cursorPosition, directionRTL, altPosition, altPositionValid );

    SetAltCursorEnabled( altPositionValid );

    mCursor.SetPosition( position + UI_OFFSET );
  }
}

void Decorator::SetAltCursorEnabled( bool enabled )
{
  mCursorRTLEnabled = enabled;
  mCursorRTL.SetVisible( mCursorVisibility && mCursorRTLEnabled );
}

void Decorator::SetCursorImage(Dali::Image image, const Vector4& border )
{
  DALI_ASSERT_DEBUG ( image && "Create cursor image invalid")

  if ( image )
  {
    mCursor.SetImage( image );
    mCursor.SetNinePatchBorder( border );
  }
}

void Decorator::SetRTLCursorImage( Image image, const Vector4& border )
{
  DALI_ASSERT_DEBUG ( image && "Create cursor image invalid")

  if ( image )
  {
    mCursorRTL.SetImage( image );
    mCursorRTL.SetNinePatchBorder(  border );
  }
}

ImageActor Decorator::CreateCursor( Image cursorImage, const Vector4& border, const std::string& cursorName )
{
  ImageActor cursor;

  if ( cursorImage )
  {
    cursor = ImageActor::New( cursorImage );
  }
  else
  {
    cursor = ImageActor::New( Image::New( DEFAULT_CURSOR ) );
  }

  cursor.SetStyle(ImageActor::STYLE_NINE_PATCH);
  cursor.SetNinePatchBorder( border );
  cursor.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
  cursor.SetVisible(false);
  cursor.SetName( cursorName );
  return cursor;
}

void Decorator::CreateCursors( Actor targetParent )
{
  Image mCursorImage = Image::New( DEFAULT_CURSOR );
  mCursor = CreateCursor (mCursorImage, DEFAULT_CURSOR_IMAGE_9_BORDER , "mainCursor");
  mCursorRTL = CreateCursor ( mCursorImage, DEFAULT_CURSOR_IMAGE_9_BORDER, "rtlCursor");
  targetParent.Add( mCursor );
  targetParent.Add( mCursorRTL );
}

Size Decorator::GetCursorSizeAt( std::size_t positionWithinTextToGetCursorSize )
{
  std::size_t visualPosition = mTextViewCharacterPositioning.GetVisualPosition( positionWithinTextToGetCursorSize );

  Vector2 min, max;

  const Size cursorSize( CURSOR_THICKNESS,
      mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( visualPosition, min, max ).height );

  return cursorSize;
}

void Decorator::StartCursorBlinkTimer()
{
  if ( !mCursorBlinkTimer )
  {
    mCursorBlinkTimer = Timer::New( CURSOR_BLINK_INTERVAL );
    mCursorBlinkTimer.TickSignal().Connect( this, &Decorator::OnCursorBlinkTimerTick );
  }

  if ( !mCursorBlinkTimer.IsRunning() )
  {
    mCursorBlinkTimer.Start();
  }
}

void Decorator::StopCursorBlinkTimer()
{
  if ( mCursorBlinkTimer )
  {
    mCursorBlinkTimer.Stop();
  }
}

bool Decorator::OnCursorBlinkTimerTick()
{
  // Cursor blinking
  mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea && mCursorBlinkStatus );
  if ( mCursorRTLEnabled )
  {
    mCursorRTL.SetVisible( mCursorVisibility && mIsCursorInScrollArea && mCursorBlinkStatus );
  }
  mCursorBlinkStatus = !mCursorBlinkStatus;

  return true;
}

/**
 * Highlight
 */
void Decorator::ShowUpdatedHighlight()
{
  Toolkit::TextView::TextLayoutInfo textLayoutInfo = mTextViewCharacterPositioning.GetLayoutInfo();
  TextHighlight::HighlightInfo highlightInfo = mTextHighlight.CalculateHighlightInfo( mSelectionHandleOnePosition, mSelectionHandleTwoPosition, textLayoutInfo );

  // Clamp highlightInfo so they don't exceed the boundary of the control.
  const Vector3& controlSize = mTextViewCharacterPositioning.GetTextView().GetCurrentSize();
  highlightInfo.Clamp2D( Vector2::ZERO, Vector2(controlSize.x, controlSize.y) );

  mTextHighlight.UpdateHighlight( highlightInfo );
}

void Decorator::CreateHighlight( Actor parent )
{
  DALI_ASSERT_DEBUG( parent && "Highlight target parent does not exist" );

  if ( !mHighlightMeshActor )
  {
    mHighlightMeshActor = MeshActor::New( mTextHighlight.CreateHighLightMesh() );
    mHighlightMeshActor.SetName( "HighlightMeshActor" );
    mHighlightMeshActor.SetAffectedByLighting(false);
    parent.Add( mHighlightMeshActor );
  }
}

void Decorator::RemoveHighlight()
{
  if ( mHighlightMeshActor )
  {
    mHighlightMeshActor.Unparent();
    mHighlightMeshActor.Reset();
    // NOTE: We cannot dereference mHighlightMesh, due to a how the scene-graph MeshRenderer uses the Mesh data.
  }
}

void Decorator::HighlightVisibility( bool visiblility )
{
  if ( mHighlightMeshActor )
  {
    mHighlightMeshActor.SetVisible( visiblility );
  }
}

/**
 *  Callbacks connected to be Property notifications for Boundary checking.
 */
// Note If PropertyNotification signal definition included Actor we would not need to duplicate functions.
void Decorator::OnHandleOneLeavesBoundary( PropertyNotification& source)
{
  mTextInputHandles.GetSelectionHandleOne().SetOpacity(0.0f);
}

void Decorator::OnHandleOneWithinBoundary(PropertyNotification& source)
{
  mTextInputHandles.GetSelectionHandleOne().SetOpacity(1.0f);
}

void Decorator::OnHandleTwoLeavesBoundary( PropertyNotification& source)
{
  mTextInputHandles.GetSelectionHandleTwo().SetOpacity(0.0f);
}

void Decorator::OnHandleTwoWithinBoundary(PropertyNotification& source)
{
  mTextInputHandles.GetSelectionHandleTwo().SetOpacity(1.0f);
}

void Decorator::OnLeftBoundaryExceeded(PropertyNotification& source)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextInputDecorationLayouter::OnLeftBoundaryExceeded\n");
  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  selectionHandleOne.SetScale( -1.0f, 1.0f, 1.0f );
  selectionHandleOne.SetAnchorPoint( AnchorPoint::TOP_LEFT);
}

void Decorator::OnReturnToLeftBoundary(PropertyNotification& source)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextInputDecorationLayouter::OnReturnToLeftBoundary\n");
  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  selectionHandleOne.SetScale( 1.0f, 1.0f, 1.0f );
  selectionHandleOne.SetAnchorPoint( AnchorPoint::TOP_RIGHT);
}

void Decorator::OnRightBoundaryExceeded(PropertyNotification& source)
{
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();
  selectionHandleTwo.SetScale( -1.0f, 1.0f, 1.0f );
  selectionHandleTwo.SetAnchorPoint( AnchorPoint::TOP_RIGHT);
}

void Decorator::OnReturnToRightBoundary(PropertyNotification& source)
{
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();
  selectionHandleTwo.SetScale( 1.0f, 1.0f, 1.0f );
  selectionHandleTwo.SetAnchorPoint( AnchorPoint::TOP_LEFT);
}

void Decorator::SetUpHandlePropertyNotifications()
{
  /* Property notifications for handles exceeding the boundary and returning back within boundary */

  Vector3 handlesize = GetSelectionHandleSize();

  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();

  // Exceeding horizontal boundary
  PropertyNotification leftNotification = selectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_X, LessThanCondition( mBoundingRectangleWorldCoordinates.x + handlesize.x) );
  leftNotification.NotifySignal().Connect( this, &Decorator::OnLeftBoundaryExceeded );

  PropertyNotification rightNotification = selectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_X, GreaterThanCondition( mBoundingRectangleWorldCoordinates.z - handlesize.x ) );
  rightNotification.NotifySignal().Connect( this, &Decorator::OnRightBoundaryExceeded );

  // Within horizontal boundary
  PropertyNotification leftLeaveNotification = selectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_X, GreaterThanCondition( mBoundingRectangleWorldCoordinates.x + 2*handlesize.x ) );
  leftLeaveNotification.NotifySignal().Connect( this, &Decorator::OnReturnToLeftBoundary );

  PropertyNotification rightLeaveNotification = selectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_X, LessThanCondition( mBoundingRectangleWorldCoordinates.z - 2*handlesize.x ) );
  rightLeaveNotification.NotifySignal().Connect( this, &Decorator::OnReturnToRightBoundary );

  // Exceeding vertical boundary
  PropertyNotification verticalExceedNotificationOne = selectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       OutsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                         mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalExceedNotificationOne.NotifySignal().Connect( this, &Decorator::OnHandleOneLeavesBoundary );

  PropertyNotification verticalExceedNotificationTwo = selectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       OutsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                         mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalExceedNotificationTwo.NotifySignal().Connect( this, &Decorator::OnHandleTwoLeavesBoundary );

  // Within vertical boundary
  PropertyNotification verticalWithinNotificationOne = selectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       InsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                        mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalWithinNotificationOne.NotifySignal().Connect( this, &Decorator::OnHandleOneWithinBoundary );

  PropertyNotification verticalWithinNotificationTwo = selectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       InsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                        mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalWithinNotificationTwo.NotifySignal().Connect( this, &Decorator::OnHandleTwoWithinBoundary );
}

/**
 * PopUp
 */
Vector3 Decorator::PositionOfPopUpRelativeToSelectionHandles()
{
  Vector3 position;
  Vector2 min, max;
  Vector3 topHandle;
  Size rowSize;

  // When text is selected, show popup above top handle (and text), or below bottom handle.

  // topHandle: referring to the top most point of the handle or the top line of selection.
  if ( mSelectionHandleTwoActualPosition.y > mSelectionHandleOneActualPosition.y ) // Handle may switch positions so calculate which is top.
  {
    topHandle = mSelectionHandleOneActualPosition;
    rowSize= mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( mSelectionHandleOnePosition, min, max );
  }
  else
  {
    topHandle = mSelectionHandleTwoActualPosition;
    rowSize = mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( mSelectionHandleTwoPosition, min, max );
  }
  topHandle.y += TOP_HANDLE_TOP_OFFSET - rowSize.height;
  position = Vector3(topHandle.x, topHandle.y, 0.0f);

  return position;
}

Vector3 Decorator::AlternatePopUpPositionRelativeToSelectionHandles()
{
  // alternativePosition: referring to the bottom most point of the handle or the bottom line of selection.
  Vector3 alternativePosition;
  alternativePosition.y = std::max ( mSelectionHandleTwoActualPosition.y , mSelectionHandleOneActualPosition.y );
  alternativePosition.y += GetSelectionHandleSize().y + mPopUpPanel.GetSize().y + BOTTOM_HANDLE_BOTTOM_OFFSET;

  return alternativePosition;
}

Vector3 Decorator::PositionOfPopUpRelativeToCursor()
{
  // When no text is selected, show PopUp at position of cursor
  Vector3 position;
  Vector2 min, max;
  std::size_t cursorPosition = GetCurrentCursorPosition();
  position = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( cursorPosition );
  const Size rowSize = mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( cursorPosition, min, max );
  position.y -= rowSize.height;

  return position;
}

Vector3 Decorator::AlternatePopUpPositionRelativeToCursor()
{
  std::size_t cursorPosition = GetCurrentCursorPosition();
  Vector3 alternativePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( cursorPosition );

  if (  mTextInputHandles.GetGrabHandle() )
  {
    // If grab handle enabled then position pop-up below the grab handle.
    alternativePosition.y +=  mTextInputHandles.GetGrabHandle().GetCurrentSize().height  + mPopUpPanel.GetSize().y + BOTTOM_HANDLE_BOTTOM_OFFSET ;
  }
  else
  {
    alternativePosition.y += mPopUpPanel.GetSize().y;
  }

  return alternativePosition;

}

Vector3 Decorator::PositionOfPopUpRelativeToGrabHandle()
{
   return Vector3::ZERO;
}

void Decorator::ShowPopUp()
{
  Vector3 position;
  Vector3 alternativePosition;
  Size rowSize;

  DALI_ASSERT_DEBUG( mPopUpTarget && "PopUp Target Actor does not exist" );

  if( mHighlightMeshActor ) // Text Selection mode
  {
    position = PositionOfPopUpRelativeToSelectionHandles();
  }
  else // Not in Text Selection mode so position relative to cursor.
  {
    position = PositionOfPopUpRelativeToCursor();
  }

  // reposition popup above the desired cursor position.
  mPopUpPanel.Show( mPopUpTarget, true );
  mPopUpPanel.Self().SetPosition( position );
  mPopUpPanel.PressedSignal().Connect( this, &Decorator::OnPopupButtonPressed );

  SetUpPopUpPositionNotifications();
  mPopUpPanel.ApplyConfinementConstraint( mBoundingRectangleWorldCoordinates );
}

void Decorator::ShowPopUp( Actor target )
{
  mPopUpTarget = target;
  ShowPopupCutCopyPaste();
}

void Decorator::ShowPopupCutCopyPaste()
{
  bool isAllTextSelectedAlready = ( mTextViewCharacterPositioning.StyledTextSize() == GetSelectedText().size() );
  bool isTextEmpty = mTextViewCharacterPositioning.IsStyledTextEmpty() ;
  bool isSubsetOfTextAlreadySelected = ( !isAllTextSelectedAlready ) && mHighlightMeshActor;

  Clipboard clipboard = Clipboard::Get();
  bool hasClipboardGotContent = clipboard.NumberOfItems();

  mPopUpPanel.CreateCutCopyPastePopUp( isAllTextSelectedAlready, isTextEmpty, hasClipboardGotContent, isSubsetOfTextAlreadySelected );
  ShowPopUp();
}

void Decorator::HidePopUp( bool animate, bool signalFinished )
{
}

void Decorator::AddPopupOption(const std::string& name, const std::string& caption, const Image icon, bool finalOption)
{
  mPopUpPanel.AddButton(name, caption, icon, finalOption);
}

void Decorator::ClearPopup()
{
  mPopUpPanel.Clear();
}

void Decorator::PopUpLeavesVerticalBoundary( PropertyNotification& source)
{
  Vector3 position, alternativePosition;

  if( mHighlightMeshActor ) // Text Selection mode
  {
    alternativePosition = AlternatePopUpPositionRelativeToSelectionHandles();
  }
  else // Not in Text Selection mode
  {
    alternativePosition = AlternatePopUpPositionRelativeToCursor();
    // if can't be positioned above, then position below row.
  }
  // reposition popup above the desired cursor position.
  mPopUpPanel.Self().SetPosition( alternativePosition );
}

void Decorator::SetUpPopUpPositionNotifications( )
{
  // Note Property notifications ignore any set anchor point so conditions must allow for this.  Default is Top Left.

  // Exceeding vertical boundary
  PropertyNotification verticalExceedNotificationOne = mPopUpPanel.Self().AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       OutsideCondition( mBoundingRectangleWorldCoordinates.y + mPopUpPanel.GetSize().y/2,
                                                                         mBoundingRectangleWorldCoordinates.w - mPopUpPanel.GetSize().y/2 ) );
  verticalExceedNotificationOne.NotifySignal().Connect( this, &Decorator::PopUpLeavesVerticalBoundary );
}

bool Decorator::OnPopupButtonPressed( Toolkit::Button button )
{
  mPopUpButtonPressedSignal.Emit( button );
  return false;
}

Decorator::PressedSignal& Decorator::PopUpButtonPressedSignal()
{
  return mPopUpButtonPressedSignal;
}

Decorator::CursorPositionedSignal& Decorator::CursorRePositionedSignal()
{
  return mCursorRePositionedSignal;
}

/**
 *  Decoration Positioning during Scrolling
 */
void Decorator::TextViewScrolled( Toolkit::TextView textView, Vector2 scrollPosition )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::TextViewScrolled\n");

  const Vector3& controlSize = mTextViewCharacterPositioning.GetTextView().GetCurrentSize(); // todo Could store size and only update in Control Size change.
  Size cursorSize( CURSOR_THICKNESS, 0.f );

  // Updates the cursor and grab handle position and visibility.
  if( mTextInputHandles.GetGrabHandle() || mCursor )
  {
    Vector2 min, max;
    size_t cursorTextPosition = GetCurrentCursorPosition();
    cursorSize.height  = mTextViewCharacterPositioning.GetRowRectFromCharacterPosition( cursorTextPosition, min, max ).height;

    const Vector3 cursorPosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( cursorTextPosition );

    bool mIsCursorInScrollArea = IsPositionWithinControl( cursorPosition, cursorSize, controlSize );
    bool mIsGrabHandleInScrollArea = mIsCursorInScrollArea;

    Vector2 actualGrabHandlePosition = cursorPosition.GetVectorXY();

    if( mTextInputHandles.GetGrabHandle() )
    {
      ShowGrabHandle( mGrabHandleVisibility && mIsGrabHandleInScrollArea );
      PositionGrabHandle( cursorTextPosition );
    }

    if( mCursor )
    {
      mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea );
      DrawCursor( cursorTextPosition );
      mCursor.SetPosition( Vector3(actualGrabHandlePosition) + UI_OFFSET );
    }
  }

  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();

  // Updates the selection handles and highlighted text position and visibility.
  if( mTextInputHandles.GetSelectionHandleOne() && mTextInputHandles.GetSelectionHandleTwo() )
  {
    const Vector3 cursorPositionOne = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( mSelectionHandleOnePosition );
    const Vector3 cursorPositionTwo = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( mSelectionHandleTwoPosition );

    Size cursorSize( GetCursorSizeAt( mSelectionHandleOnePosition ) );
    const bool isSelectionHandleOneVisible = IsPositionWithinControl( cursorPositionOne, cursorSize, controlSize );

    cursorSize =  GetCursorSizeAt( mSelectionHandleTwoPosition );
    const bool isSelectionHandleTwoVisible = IsPositionWithinControl( cursorPositionTwo, cursorSize, controlSize );

    mSelectionHandleOneActualPosition = cursorPositionOne.GetVectorXY();
    mSelectionHandleTwoActualPosition = cursorPositionTwo.GetVectorXY();

    selectionHandleOne.SetVisible( isSelectionHandleOneVisible );
    selectionHandleTwo.SetVisible( isSelectionHandleTwoVisible );

    PositionSelectionHandle( selectionHandleOne, mSelectionHandleOneActualPosition, mSelectionHandleOnePosition );
    PositionSelectionHandle( selectionHandleTwo, mSelectionHandleTwoActualPosition, mSelectionHandleTwoPosition );

    if( mHighlightMeshActor )
    {
      mHighlightMeshActor.SetVisible( true );
      ShowUpdatedHighlight();
    }
  }
}

void Decorator::StartScrollTimer()
{
  if( !mScrollTimer )
  {
    mScrollTimer = Timer::New( SCROLL_TICK_INTERVAL );
    mScrollTimer.TickSignal().Connect( this, &Decorator::OnScrollTimerTick );
  }

  if( !mScrollTimer.IsRunning() )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::StartScrollTimer\n");
    mScrollTimer.Start();
  }
}

void Decorator::StopScrollTimer()
{
  if( mScrollTimer )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::StopScrollTimer\n");

    mScrollTimer.Stop();
    mScrollTimer.Reset();
  }
}

bool Decorator::OnScrollTimerTick()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::OnScrollTimerTick\n");

  if ( mGrabHandleVisibility && mTextInputHandles.GetGrabHandle() )
  {
    std::size_t newGrabHandlePosition = mTextViewCharacterPositioning.ReturnClosestIndex( mActualGrabHandlePosition.GetVectorXY() );
    if ( mGrabHandlePosition != newGrabHandlePosition )
    {
      Vector2 scrollPosition = mTextViewCharacterPositioning.GetScrollPosition();
      Vector2 scrollDelta = ( mActualGrabHandlePosition - mCurrentHandlePosition ).GetVectorXY();
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputDecorationLayouter::OnScrollTimerTick scrollPosition(%f) scrollDelta(%f)\n", scrollPosition.x, scrollDelta.x);
      scrollPosition += scrollDelta;
      mTextViewCharacterPositioning.SetScrollPosition( scrollPosition );

      mActualGrabHandlePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( newGrabHandlePosition ).GetVectorXY();
    }
  }

  Actor selectionHandleOne = mTextInputHandles.GetSelectionHandleOne();
  Actor selectionHandleTwo = mTextInputHandles.GetSelectionHandleTwo();

  if ( selectionHandleOne && selectionHandleTwo )
  {
    std::size_t newHandleOnePosition = mTextViewCharacterPositioning.ReturnClosestIndex( mSelectionHandleOneActualPosition.GetVectorXY() );

    // todo duplicated code should be a function

    if ( mSelectionHandleOnePosition != newHandleOnePosition )
    {
      const Vector3 actualPosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( newHandleOnePosition );

      Vector2 scrollDelta = ( actualPosition - mSelectionHandleOneActualPosition ).GetVectorXY();

      Vector2 scrollPosition = mTextViewCharacterPositioning.GetScrollPosition();
      scrollPosition += scrollDelta;
      mTextViewCharacterPositioning.SetScrollPosition( scrollPosition );

      mSelectionHandleOnePosition = newHandleOnePosition;
      mSelectionHandleOneActualPosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( mSelectionHandleOnePosition ).GetVectorXY();
    }
    else
    {
      mSelectionHandleOneActualPosition.x += mScrollDisplacement.x;
      mSelectionHandleOneActualPosition.y += mScrollDisplacement.y;
    }

    std::size_t newHandleTwoPosition = mTextViewCharacterPositioning.ReturnClosestIndex( mSelectionHandleTwoActualPosition.GetVectorXY() );

    if ( mSelectionHandleTwoPosition != newHandleTwoPosition )
    {
      const Vector3 actualPosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( newHandleTwoPosition );

      Vector2 scrollDelta = ( actualPosition - mSelectionHandleTwoActualPosition ).GetVectorXY();

      Vector2 scrollPosition = mTextViewCharacterPositioning.GetScrollPosition();
      scrollPosition += scrollDelta;
      mTextViewCharacterPositioning.SetScrollPosition( scrollPosition );

      mSelectionHandleTwoPosition = newHandleTwoPosition;
      mCurrentHandlePosition = mTextViewCharacterPositioning.GetActualPositionFromCharacterPosition( mSelectionHandleTwoPosition ).GetVectorXY();

    }
    else
    {
      mSelectionHandleTwoActualPosition.x += mScrollDisplacement.x;
      mSelectionHandleTwoActualPosition.y += mScrollDisplacement.y;
    }
  }

  return true;
}

/**
 *  Text Selection
 */
MarkupProcessor::StyledTextArray Decorator::GetSelectedText()
{
  MarkupProcessor::StyledTextArray currentSelectedText;

  if ( mHighlightMeshActor ) // Text Selected
  {
    MarkupProcessor::StyledTextArray::iterator it = mTextViewCharacterPositioning.GetStyledTextArray().begin() + std::min(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);
    MarkupProcessor::StyledTextArray::iterator end = mTextViewCharacterPositioning.GetStyledTextArray().begin() + std::max(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);

    for(; it != end; ++it)
    {
      MarkupProcessor::StyledText& styledText( *it );
      currentSelectedText.push_back( styledText );
    }
  }
  return currentSelectedText;
}

} // Internal

} // namespace Toolkit

} // namespace Dali

