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
#include <dali-toolkit/internal/controls/text-input/text-input-handles-impl.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <sstream>
#include <algorithm>
#include <dali/public-api/animation/constraints.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-input/textview-character-positions-impl.h>
#include <dali-toolkit/internal/controls/text-input/text-input-impl.h>
#include <dali-toolkit/internal/controls/text-view/text-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/alignment/alignment.h>

using namespace Dali;

namespace
{
const char* const DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* const DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );
const char* const DEFAULT_SELECTION_HANDLE_ONE_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-left-press.png" );
const char* const DEFAULT_SELECTION_HANDLE_TWO_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-right-press.png" );

const char* const DEFAULT_GRAB_HANDLE( DALI_IMAGE_DIR "insertpoint-icon.png" );

const Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SCALE( 1.5f, 1.5f, 1.0f );
const Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SCALE( 1.5f, 2.0f, 1.0f );

const char* const SELECTION_GRAB_AREA_ONE( "SelectionHandleOneGrabArea");
const char* const SELECTION_GRAB_AREA_TWO( "SelectionHandleTwoGrabArea");
const char* const GRABHANDLE_GRAB_AREA( "GrabHandleGrabArea");

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "TEXT_INPUT_HANDLES" );
#endif

Actor CreateGrabArea( const std::string& name, const Vector3& relativeScale )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: CreateGrabArea\n" );

  Actor handleGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
  handleGrabArea.SetName( name );
  handleGrabArea.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( relativeScale ) ) );  // grab area to be larger than text actor
  handleGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

  return handleGrabArea;
}

ImageActor CreateHandle( const Vector3& anchorPoint, const Image& handleImage, const std::string& name )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: CreateSelectionHandle\n" );

  ImageActor selectionHandle = ImageActor::New( handleImage );
  selectionHandle.SetName( name );
  selectionHandle.SetAnchorPoint( anchorPoint );
  selectionHandle.SetDrawMode( DrawMode::OVERLAY ); // ensure handle above text

  return selectionHandle;
}
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

// Default constructor
TextInputHandles::TextInputHandles():
  mSelectionHandleOne(),
  mSelectionHandleTwo(),
  mSelectionHandleOneOffset( Vector3::ZERO ),
  mSelectionHandleTwoOffset( Vector3::ZERO ),
  mSelectionHandleOneCoordinatePosition( Vector3::ZERO ),
  mSelectionHandleTwoCoordinatePosition( Vector3::ZERO ),
  mSelectionHandleOneStringPosition( 0 ),
  mSelectionHandleTwoStringPosition( 0 ),
  mIsSelectionHandleOneFlipped( false ),
  mIsSelectionHandleTwoFlipped( false )
{
}

TextInputHandles::~TextInputHandles()
{
}

void TextInputHandles::CreateSelectionHandles()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: CreateSelectionHandles\n" );

  mSelectionHandleOneImage = Image::New( DEFAULT_SELECTION_HANDLE_ONE );
  mSelectionHandleOneImagePressed = Image::New( DEFAULT_SELECTION_HANDLE_ONE_PRESSED );
  mSelectionHandleOne = CreateHandle( AnchorPoint::TOP_RIGHT, mSelectionHandleOneImage, "SelectionHandleOne" );
  mIsSelectionHandleOneFlipped = false;

  mHandleOneGrabArea = CreateGrabArea( SELECTION_GRAB_AREA_ONE, DEFAULT_SELECTION_HANDLE_RELATIVE_SCALE );
  mSelectionHandleOne.Add( mHandleOneGrabArea );
  mHandleOneGrabArea.TouchedSignal().Connect(this, &TextInputHandles::OnSelectionHandleTouched);

//  mTapDetector.Attach( mHandleOneGrabArea );

  mSelectionHandleTwoImage = Image::New( DEFAULT_SELECTION_HANDLE_TWO );
  mSelectionHandleTwoImagePressed = Image::New( DEFAULT_SELECTION_HANDLE_TWO_PRESSED );
  mSelectionHandleTwo = CreateHandle( AnchorPoint::TOP_LEFT, mSelectionHandleTwoImage, "SelectionHandleTwo" );
  mIsSelectionHandleTwoFlipped = false;

  mHandleTwoGrabArea = CreateGrabArea( SELECTION_GRAB_AREA_TWO, DEFAULT_SELECTION_HANDLE_RELATIVE_SCALE );
  mSelectionHandleTwo.Add( mHandleTwoGrabArea );
  mHandleTwoGrabArea.TouchedSignal().Connect(this, &TextInputHandles::OnSelectionHandleTouched);

  //  mTapDetector.Attach( mHandleTwoGrabArea );
}

void TextInputHandles::DestorySelectionHandles()
{
  if ( mSelectionHandleOne && mSelectionHandleTwo)
  {
    mSelectionHandleOne.Unparent();
    mSelectionHandleTwo.Unparent();
    mSelectionHandleOneImagePressed.Reset();
    mSelectionHandleOneImage.Reset();
    mSelectionHandleTwoImagePressed.Reset();
    mSelectionHandleTwoImage.Reset();
    mSelectionHandleOne.Reset();
    mSelectionHandleTwo.Reset();
  }
}

void TextInputHandles::SetSelectionHandleOneVisibility( bool visibility )
{
  if ( mSelectionHandleOne )
  {
    mSelectionHandleOne.SetVisible( visibility );
  }
}

void TextInputHandles::SetSelectionHandleTwoVisibility( bool visibility )
{
  if ( mSelectionHandleTwo )
  {
    mSelectionHandleTwo.SetVisible( visibility );
  }
}

void TextInputHandles::AttachSelectionHandlesToGivenPanGesture( PanGestureDetector& panGestureDetector )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: AttachSelectionHandlesToGivenPanGesture\n" );

  panGestureDetector.Attach( mHandleOneGrabArea );
  panGestureDetector.Attach( mHandleTwoGrabArea );
}

void TextInputHandles::AttachSelectionHandlesToGivenTapDetector(TapGestureDetector& tapGestureDetector )
{
  tapGestureDetector.Attach( mHandleOneGrabArea );
  tapGestureDetector.Attach( mHandleTwoGrabArea );
}

void TextInputHandles::AttachGrabHandleToGivenPanGesture( PanGestureDetector& panGestureDetector )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: AttachGrabHandleToGivenPanGesture\n" );

  panGestureDetector.Attach( mGrabHandleGrabArea );
}

Actor TextInputHandles::GetSelectionHandleOne()
{
  return mSelectionHandleOne;
}

Actor TextInputHandles::GetSelectionHandleTwo()
{
  return mSelectionHandleTwo;
}

bool TextInputHandles::OnSelectionHandleTouched(Dali::Actor actor, const TouchEvent& touch)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: OnSelectionHandleTouched\n" );

  Image pressedImage;
  Image normalImage;

  ImageActor handleTouched = ImageActor::DownCast( actor.GetParent() ); // Hit actor would be the GrabArea hence get parent.

  if ( handleTouched == mSelectionHandleOne )
  {
    pressedImage = mSelectionHandleOneImagePressed;
    normalImage = mSelectionHandleOneImage;
  }
  else
  {
    pressedImage = mSelectionHandleTwoImagePressed;
    normalImage = mSelectionHandleTwoImage;
  }

  if (touch.GetPoint(0).state == TouchPoint::Down)
  {
    handleTouched.SetImage( pressedImage );
  }
  else if (touch.GetPoint(0).state == TouchPoint::Up )
  {
    handleTouched.SetImage( normalImage );
  }
  return false;
}

// Grab handle

Actor TextInputHandles::GetGrabHandle()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: GetGrabHandle\n" );

  return mGrabHandle;
}

void TextInputHandles::CreateGrabHandle()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: CreateGrabHandle\n" );

  if ( !mGrabHandle )
  {
    if ( !mGrabHandleImage )
    {
      mGrabHandleImage = Image::New( DEFAULT_GRAB_HANDLE );
    }

    mGrabHandle = CreateHandle( AnchorPoint::TOP_CENTER, mGrabHandleImage, "GrabHandle" );
    mGrabHandleGrabArea = CreateGrabArea( GRABHANDLE_GRAB_AREA, DEFAULT_GRAB_HANDLE_RELATIVE_SCALE );
    mGrabHandle.Add( mGrabHandleGrabArea );
  }
}

void TextInputHandles::DestoryGrabHandle()
{
  if ( mGrabHandle )
  {
    mGrabHandle.Unparent();
    mGrabHandleImage.Reset();
    mGrabHandle.Reset();
  }
}

void TextInputHandles::SetGrabHandleImage( Dali::Image image )
{
  if ( mGrabHandle )
  {
    mGrabHandleImage = image;
    mGrabHandle.SetImage( mGrabHandleImage );
  }
}

void TextInputHandles::SetGrabHandleVisibility( bool visibility )
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextInputHandles: SetGrabHandleVisibility (%s) \n", ( visibility )?"true":"false" );

  if ( mGrabHandle )
  {
    mGrabHandle.SetVisible( visibility );
  }
}

} // Internal

} // namespace Toolkit

} // namespace Dali

