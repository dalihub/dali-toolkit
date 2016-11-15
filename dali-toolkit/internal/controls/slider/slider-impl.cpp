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
#include <dali-toolkit/internal/controls/slider/slider-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <sstream>
#include <limits>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // Unnamed namespace
{

BaseHandle Create()
{
  return Dali::Toolkit::Slider::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Slider, Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "lowerBound",             FLOAT,    LOWER_BOUND            )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "upperBound",             FLOAT,    UPPER_BOUND            )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "value",                  FLOAT,    VALUE                  )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "trackVisual",            MAP,      TRACK_VISUAL           )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "handleVisual",           MAP,      HANDLE_VISUAL          )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "progressVisual",         MAP,      PROGRESS_VISUAL        )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "popupVisual",            MAP,      POPUP_VISUAL           )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "popupArrowVisual",       MAP,      POPUP_ARROW_VISUAL     )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "disabledColor",          VECTOR4,  DISABLED_COLOR         )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "valuePrecision",         INTEGER,  VALUE_PRECISION        )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "showPopup",              BOOLEAN,  SHOW_POPUP             )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "showValue",              BOOLEAN,  SHOW_VALUE             )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "marks",                  ARRAY,    MARKS                  )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "snapToMarks",            BOOLEAN,  SNAP_TO_MARKS          )
DALI_PROPERTY_REGISTRATION( Toolkit, Slider, "markTolerance",          FLOAT,    MARK_TOLERANCE         )

DALI_SIGNAL_REGISTRATION(   Toolkit, Slider, "valueChanged",                     SIGNAL_VALUE_CHANGED   )
DALI_SIGNAL_REGISTRATION(   Toolkit, Slider, "mark",                             SIGNAL_MARK            )

DALI_TYPE_REGISTRATION_END()

const float MARK_SNAP_TOLERANCE = 0.05f; // 5% of slider width

const int VALUE_VIEW_SHOW_DURATION = 1000;  // millisec
const int VALUE_VIEW_SHOW_DURATION_LONG = 2000;  // millisec

const float VALUE_VERTICAL_OFFSET = 48.0f;

const float DEFAULT_WIDTH = 0.0f;
const float DEFAULT_HEIGHT = 27.0f;
const float DEFAULT_HIT_HEIGHT = 72.0f;
const float DEFAULT_HANDLE_HEIGHT = DEFAULT_HIT_HEIGHT;
const float POPUP_TEXT_PADDING = 10.0f;

const char* SKINNED_TRACK_VISUAL = DALI_IMAGE_DIR "slider-skin.9.png";
const char* SKINNED_HANDLE_VISUAL = DALI_IMAGE_DIR "slider-skin-handle.png";
const char* SKINNED_PROGRESS_VISUAL = DALI_IMAGE_DIR "slider-skin-progress.9.png";
const char* SKINNED_POPUP_VISUAL = DALI_IMAGE_DIR "slider-popup.9.png";
const char* SKINNED_POPUP_ARROW_VISUAL = DALI_IMAGE_DIR "slider-popup-arrow.png";

const Vector2 DEFAULT_HIT_REGION( DEFAULT_WIDTH, DEFAULT_HIT_HEIGHT );
const Vector2 DEFAULT_TRACK_REGION( DEFAULT_WIDTH, DEFAULT_HEIGHT );
const Vector2 DEFAULT_HANDLE_SIZE( DEFAULT_HANDLE_HEIGHT, DEFAULT_HANDLE_HEIGHT );

const Vector4 DEFAULT_DISABLED_COLOR( 0.5f, 0.5f, 0.5f, 1.0f );

const float VALUE_POPUP_MARGIN = 10.0f;
const float VALUE_POPUP_HEIGHT = 81.0f;
const float VALUE_POPUP_MIN_WIDTH = 54.0f;
const Vector2 VALUE_POPUP_ARROW_SIZE( 18.0f, 18.0f );

const float DEFAULT_LOWER_BOUND = 0.0f;
const float DEFAULT_UPPER_BOUND = 1.0f;
const float DEFAULT_VALUE = 0.0f;
const int DEFAULT_VALUE_PRECISION = 0;
const bool DEFAULT_SHOW_POPUP = false;
const bool DEFAULT_SHOW_VALUE = true;
const bool DEFAULT_ENABLED = true;
const bool DEFAULT_SNAP_TO_MARKS = false;

} // Unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// Slider
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::Slider Slider::New()
{
  // Create the implementation
  SliderPtr slider( new Slider() );

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::Slider handle( *slider );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  slider->Initialize();

  return handle;
}

Slider::Slider()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mState( NORMAL ),
  mPopupVisual(""),
  mPopupArrowVisual(""),
  mTrackVisual(""),
  mHandleVisual(""),
  mProgressVisual(""),
  mPopupMap(),
  mTrackMap(),
  mHandleMap(),
  mPopupArrowMap(),
  mDisabledColor( 0.0f, 0.0f, 0.0f, 0.0f ),
  mHitRegion( 0.0f, 0.0f ),
  mTrackRegion( 0.0f, 0.0f ),
  mHandleSize( 0.0f, 0.0f ),
  mLowerBound( 0.0f ),
  mUpperBound( 0.0f ),
  mValue( 0.0f ),
  mMarkTolerance( 0.0f ),
  mValuePrecision( 0 ),
  mShowPopup( false ),
  mShowValue( false ),
  mSnapToMarks( false )
{
}

Slider::~Slider()
{
}

void Slider::OnInitialize()
{
  // Setup
  CreateChildren();

  // Properties
  Actor self = Self();

  SetHitRegion(     DEFAULT_HIT_REGION     );
  SetTrackRegion(   DEFAULT_TRACK_REGION   );
  SetHandleSize(    DEFAULT_HANDLE_SIZE    );

  SetTrackVisual(            SKINNED_TRACK_VISUAL             );
  SetHandleVisual(           SKINNED_HANDLE_VISUAL            );
  SetProgressVisual(         SKINNED_PROGRESS_VISUAL          );
  SetPopupVisual(            SKINNED_POPUP_VISUAL             );
  SetPopupArrowVisual(       SKINNED_POPUP_ARROW_VISUAL       );

  SetShowPopup( DEFAULT_SHOW_POPUP );
  SetShowValue( DEFAULT_SHOW_VALUE );

  SetEnabled( DEFAULT_ENABLED );
  SetDisabledColor( DEFAULT_DISABLED_COLOR );

  SetSnapToMarks( DEFAULT_SNAP_TO_MARKS );
  SetMarkTolerance( MARK_SNAP_TOLERANCE );

  SetLowerBound( DEFAULT_LOWER_BOUND );
  SetUpperBound( DEFAULT_UPPER_BOUND );
  UpdateSkin();
  SetValuePrecision( DEFAULT_VALUE_PRECISION );
  mValue = DEFAULT_VALUE;
  DisplayValue( mValue, false );       // Run this last to display the correct value

  // Size the Slider actor to a default
  self.SetSize( DEFAULT_HIT_REGION.x, DEFAULT_HIT_REGION.y );

  // Connect to the touch signal
  self.TouchSignal().Connect( this, &Slider::OnTouch );
}

void Slider::OnSizeSet( const Vector3& size )
{
  // Factor in handle overshoot into size of track
  SetHitRegion( Vector2( size.x, GetHitRegion().y ) );
  SetTrackRegion( Vector2( size.x - GetHandleSize().x, GetTrackRegion().y ) );
}

bool Slider::OnTouch(Actor actor, const TouchData& touch)
{
  if( mState != DISABLED )
  {
    const PointState::Type touchState = touch.GetState(0);

    if( touchState == PointState::DOWN )
    {
      mState = PRESSED;

      float percentage = MapPercentage( touch.GetLocalPosition( 0 ) );
      float value = MapBounds( ( GetSnapToMarks() ) ? SnapToMark( percentage ) : MarkFilter( percentage ), GetLowerBound(), GetUpperBound() );
      SetValue( value );
      DisplayPopup( value );
    }
    else if( touchState == PointState::UP )
    {
      if( mState == PRESSED )
      {
        mState = NORMAL;
        mSlidingFinishedSignal.Emit( Toolkit::Slider::DownCast( Self() ), GetValue() );
      }
    }
  }

  return true;
}

void Slider::OnPan( Actor actor, const PanGesture& gesture )
{
  // gesture.position is in local actor coordinates
  if( mState != DISABLED )
  {
    switch( gesture.state )
    {
      case Gesture::Continuing:
      {
        if( mState == PRESSED )
        {
          float value = MapBounds( MarkFilter ( MapPercentage( gesture.position ) ), GetLowerBound(), GetUpperBound() );
          SetValue( value );
          DisplayPopup( value );
        }
        break;
      }
      case Gesture::Finished:
      {
        if( mState == PRESSED  )
        {
          if( GetSnapToMarks() )
          {
            float value = MapBounds( SnapToMark( MapPercentage( gesture.position ) ), GetLowerBound(), GetUpperBound() );
            SetValue( value );
            DisplayPopup( value );
          }
          mSlidingFinishedSignal.Emit( Toolkit::Slider::DownCast( Self() ), GetValue() );
        }

        mState = NORMAL;
        break;
      }
      default:
      {
        break;
      }
    }
  }
}

float Slider::HitSpaceToDomain( float x )
{
  float halfRegionWidth = GetHitRegion().x * 0.5f;
  float halfDomainWidth = ( mDomain.to.x - mDomain.from.x ) * 0.5f;
  float endDiff = halfRegionWidth - halfDomainWidth;

  return x - endDiff;
}

float Slider::MapPercentage( const Vector2& point )
{
  return Clamp( ( HitSpaceToDomain( point.x ) - mDomain.from.x ) / ( mDomain.to.x - mDomain.from.x ), 0.0f, 1.0f );
}

float Slider::MapValuePercentage( float value )
{
  return ( value - GetLowerBound() ) / ( GetUpperBound() - GetLowerBound() );
}

float Slider::MapBounds( float percent, float lowerBound, float upperBound )
{
  return lowerBound + percent * ( upperBound - lowerBound );
}

Slider::Domain Slider::CalcDomain( const Vector2& currentSize )
{
   return Domain( Vector2( 0.0f, 0.0f ), currentSize );
}

void Slider::DisplayValue( float value, bool raiseSignals )
{
  float clampledValue = Clamp( value, GetLowerBound(), GetUpperBound() );

  float percent = MapValuePercentage( clampledValue );

  float x = mDomain.from.x + percent * ( mDomain.to.x - mDomain.from.x );

  mHandle.SetX( x );

  // Progress bar
  if( mProgress )
  {
    mProgress.SetSize( x, GetTrackRegion().y );
  }

  // Signals
  if( raiseSignals )
  {
    Toolkit::Slider self = Toolkit::Slider::DownCast( Self() );
    mValueChangedSignal.Emit( self, clampledValue );

    int markIndex;
    if( MarkReached( percent, markIndex ) )
    {
      mMarkReachedSignal.Emit( self, markIndex );
    }
  }

  if( mHandleValueTextLabel )
  {
    std::stringstream ss;
    ss.precision( GetValuePrecision() );
    ss << std::fixed << clampledValue;

    mHandleValueTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, ss.str() );
  }
}

void Slider::SetMarks( const MarkList& marks )
{
  mMarks = marks;
}

const Slider::MarkList& Slider::GetMarks() const
{
  return mMarks;
}

void Slider::SetSnapToMarks( bool snap )
{
  mSnapToMarks = snap;
}

bool Slider::GetSnapToMarks() const
{
  return mSnapToMarks;
}

Actor Slider::CreateHitRegion()
{
  Actor hitRegion = Actor::New();
  hitRegion.SetParentOrigin( ParentOrigin::CENTER );
  hitRegion.SetAnchorPoint( AnchorPoint::CENTER );
  hitRegion.TouchSignal().Connect( this, &Slider::OnTouch );

  return hitRegion;
}

Toolkit::ImageView Slider::CreateTrack()
{
  Toolkit::ImageView track = Toolkit::ImageView::New();
  track.SetParentOrigin( ParentOrigin::CENTER );
  track.SetAnchorPoint( AnchorPoint::CENTER );

  return track;
}

void Slider::SetTrackVisual( const std::string& filename )
{
  if( mHandle && ( filename.size() > 0 ) )
  {
    mTrack.SetImage( filename );
    mTrackVisual = filename;
  }
}

void Slider::SetTrackVisual( Property::Map map )
{
  Property::Value* imageValue = map.Find( "url" );
  if( imageValue )
  {
    mTrackVisual.clear();
    std::string filename;
    if( imageValue->Get( filename ) )
    {
      if( mTrack && ( filename.size() > 0 ) )
      {
        mTrack.SetImage( filename );
        mTrackMap = map;
      }
    }
  }

  Property::Value* sizeValue = map.Find( "size" );
  if( sizeValue )
  {
    Vector2 size;
    if( sizeValue->Get( size ) )
    {
      mTrackRegion = size;
      if( mTrack )
      {
        mTrack.SetSize( mTrackRegion );
      }

    ResizeProgressRegion( Vector2( 0.0f, mTrackRegion.y ) );

    mDomain = CalcDomain( mTrackRegion );

    // Set the progress bar to correct width
    DisplayValue( GetValue(), false );
    }
  }
}

std::string Slider::GetTrackVisual()
{
  return mTrackVisual;
}

Toolkit::ImageView Slider::CreateProgress()
{
  Toolkit::ImageView progress = Toolkit::ImageView::New();
  progress.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  progress.SetAnchorPoint( AnchorPoint::CENTER_LEFT );

  return progress;
}

void Slider::SetProgressVisual( const std::string& filename )
{
  if( mProgress && ( filename.size() > 0 ) )
  {
    mProgress.SetImage( filename );
    mProgressVisual = filename;
  }
}

void Slider::SetProgressVisual( Property::Map map )
{
  Property::Value* imageValue = map.Find( "url" );
  if( imageValue )
  {
    mProgressVisual.clear();
    std::string filename;
    if( imageValue->Get( filename ) )
    {
      if( mProgress && ( filename.size() > 0 ) )
      {
        mProgress.SetImage( filename );
        mProgressMap = map;
      }
    }
  }
}

std::string Slider::GetProgressVisual()
{
  return mProgressVisual;
}

void Slider::SetPopupVisual( const std::string& filename )
{
  mPopupVisual = filename;
}

void Slider::SetPopupVisual( Property::Map map )
{
  Property::Value* imageValue = map.Find( "url" );
  if( imageValue )
  {
    mPopupVisual.clear();
    std::string filename;
    if( imageValue->Get( filename ) )
    {
      if( mPopup && ( filename.size() > 0 ) )
      {
        mPopup.SetImage( filename );
        mPopupMap = map;
      }
    }
  }
}

std::string Slider::GetPopupVisual()
{
  return mPopupVisual;
}

void Slider::CreatePopupImage( const std::string& filename )
{
  if( mPopup && ( filename.size() > 0 ) )
  {
    Image image = ResourceImage::New( filename );
    mPopup.SetImage( image );
  }
}

void Slider::SetPopupArrowVisual( const std::string& filename )
{
  mPopupArrowVisual = filename;
}

void Slider::SetPopupArrowVisual( Property::Map map )
{
  Property::Value* imageValue = map.Find( "url" );
  if( imageValue )
  {
    mPopupArrowVisual.clear();
    std::string filename;
    if( imageValue->Get( filename ) )
    {
      if( mPopupArrow && ( filename.size() > 0 ) )
      {
        mPopupArrow.SetImage( filename );
        mPopupArrowMap = map;
      }
    }
  }
}

std::string Slider::GetPopupArrowVisual()
{
  return mPopupArrowVisual;
}

void Slider::CreatePopupArrowImage( const std::string& filename )
{
  if( mPopupArrow && ( filename.size() > 0 ) )
  {
    Image image = ResourceImage::New( filename );
    mPopupArrow.SetImage( image );
  }
}

void Slider::ResizeProgressRegion( const Vector2& region )
{
  if( mProgress )
  {
    mProgress.SetSize( region );
  }
}

Toolkit::ImageView Slider::CreateHandle()
{
  Toolkit::ImageView handle = Toolkit::ImageView::New();
  handle.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  handle.SetAnchorPoint( AnchorPoint::CENTER );

  return handle;
}

Toolkit::ImageView Slider::CreatePopupArrow()
{
  Toolkit::ImageView arrow = Toolkit::ImageView::New();
  arrow.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  arrow.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );

  return arrow;
}

Toolkit::TextLabel Slider::CreatePopupText()
{
  Toolkit::TextLabel textLabel = Toolkit::TextLabel::New();
  textLabel.SetParentOrigin( ParentOrigin::CENTER );
  textLabel.SetAnchorPoint( AnchorPoint::CENTER );
  textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  textLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  textLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  textLabel.SetPadding( Padding( POPUP_TEXT_PADDING, POPUP_TEXT_PADDING, 0.0f, 0.0f ) );
  return textLabel;
}

Toolkit::ImageView Slider::CreatePopup()
{
  Toolkit::ImageView popup = Toolkit::ImageView::New();
  popup.SetParentOrigin( ParentOrigin::TOP_CENTER );
  popup.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  popup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH );

  mValueTextLabel = CreatePopupText();
  popup.Add( mValueTextLabel );

  return popup;
}

void Slider::SetHandleVisual( const std::string& filename )
{
  if( mHandle && ( filename.size() > 0 ) )
  {
    mHandle.SetImage( filename );
    mHandleVisual = filename;
  }
}

void Slider::SetHandleVisual( Property::Map map )
{
  Property::Value* imageValue = map.Find( "url" );
  if( imageValue )
  {
    mHandleVisual.clear();
    std::string filename;
    if( imageValue->Get( filename ) )
    {
      if( mHandle && ( filename.size() > 0 ) )
      {
        mHandle.SetImage( filename );
        mHandleMap = map;
      }
    }
  }

  Property::Value* sizeValue = map.Find( "size" );
  if( sizeValue )
  {
    Vector2 size;
    if( sizeValue->Get( size ) )
    {
      mHandleSize = size;
      ResizeHandleSize( mHandleSize );

      Vector2 hitRegion = GetHitRegion();
      hitRegion.x += mHandleSize.x;
      SetHitRegion( hitRegion );
    }
  }
}

std::string Slider::GetHandleVisual()
{
  return mHandleVisual;
}

void Slider::ResizeHandleSize( const Vector2& size )
{
  if( mHandle )
  {
    mHandle.SetSize( size );
  }
}

void Slider::CreateHandleValueDisplay()
{
  if( mHandle && !mHandleValueTextLabel )
  {
    mHandleValueTextLabel = Toolkit::TextLabel::New();
    mHandleValueTextLabel.SetParentOrigin( ParentOrigin::CENTER );
    mHandleValueTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
    mHandleValueTextLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mHandleValueTextLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mHandle.Add( mHandleValueTextLabel );
  }
}

void Slider::DestroyHandleValueDisplay()
{
  UnparentAndReset(mHandleValueTextLabel);
}

Actor Slider::CreateValueDisplay()
{
  Actor popup = Actor::New();
  popup.SetParentOrigin( ParentOrigin::TOP_CENTER );
  popup.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );

  mPopupArrow = CreatePopupArrow();
  popup.Add( mPopupArrow );

  mPopup = CreatePopup();
  mPopup.SetSize( 0.0f, VALUE_POPUP_HEIGHT );
  mPopupArrow.Add( mPopup );

  return popup;
}

Toolkit::Slider::ValueChangedSignalType& Slider::ValueChangedSignal()
{
  return mValueChangedSignal;
}

Toolkit::Slider::ValueChangedSignalType& Slider::SlidingFinishedSignal()
{
  return mSlidingFinishedSignal;
}

Toolkit::Slider::MarkReachedSignalType& Slider::MarkReachedSignal()
{
  return mMarkReachedSignal;
}

void Slider::UpdateSkin()
{
  switch( mState )
  {
    case NORMAL:
    {
      mTrack.SetColor( Color::WHITE );
      mHandle.SetColor( Color::WHITE );
      mProgress.SetColor( Color::WHITE );
      break;
    }
    case DISABLED:
    {
      Vector4 disabledColor = GetDisabledColor();
      mTrack.SetColor( disabledColor );
      mHandle.SetColor( disabledColor );
      mProgress.SetColor( disabledColor );
      break;
    }
    case PRESSED:
    {
      break;
    }
    case FOCUSED:
    {
      break;
    }
  }
}

void Slider::CreateChildren()
{
  Actor self = Self();

  // Hit region
  mHitArea = CreateHitRegion();
  mPanDetector = PanGestureDetector::New();
  mPanDetector.Attach( mHitArea );
  mPanDetector.DetectedSignal().Connect( this, &Slider::OnPan );
  self.Add( mHitArea );

  // Track
  mTrack = CreateTrack();
  self.Add( mTrack );

  // Progress bar
  mProgress = CreateProgress();
  mTrack.Add( mProgress );

  // Handle
  mHandle = CreateHandle();
  mProgress.Add( mHandle );
}

void Slider::SetHitRegion( const Vector2& size )
{
  mHitRegion = size;

  if( mHitArea )
  {
    mHitArea.SetSize( mHitRegion );
  }
}

const Vector2& Slider::GetHitRegion() const
{
  return mHitRegion;
}

void Slider::AddPopup()
{
  if( !mValueDisplay )
  {
    mValueDisplay = CreateValueDisplay();
    mValueDisplay.SetVisible( false );
    mHandle.Add( mValueDisplay );

    CreatePopupImage( GetPopupVisual() );
    CreatePopupArrowImage( GetPopupArrowVisual() );

    mValueTimer = Timer::New( VALUE_VIEW_SHOW_DURATION );
    mValueTimer.TickSignal().Connect( this, &Slider::HideValueView );
  }
}

void Slider::RemovePopup()
{
  if( mValueDisplay )
  {
    mPopup.Unparent();
    mPopup.Reset();

    mPopupArrow.Unparent();
    mPopupArrow.Reset();

    mValueDisplay.Unparent();
    mValueDisplay.Reset();

    mValueTimer.TickSignal().Disconnect( this, &Slider::HideValueView );
    mValueTimer.Reset();
  }
}


float Slider::MarkFilter( float value )
{
  const float MARK_TOLERANCE = GetMarkTolerance();

  float mark;
  for( MarkList::SizeType i = 0; i < mMarks.Count(); ++i)
  {
    const Property::Value& propertyValue = mMarks[i];
    propertyValue.Get( mark );
    mark = MapValuePercentage( mark );

    // If close to a mark, return the mark
    if( fabsf( mark - value ) < MARK_TOLERANCE )
    {
      return mark;
    }
  }

  return value;
}

float Slider::SnapToMark( float value )
{
  float closestMark = value;
  float closestDist = std::numeric_limits<float>::max();

  float mark;
  for( MarkList::SizeType  i = 0; i < mMarks.Count(); ++i)
  {
    const Property::Value& propertyValue = mMarks[i];
    propertyValue.Get( mark );
    mark = MapValuePercentage( mark );

    float dist = fabsf( mark - value );
    if( dist < closestDist )
    {
      closestDist = dist;
      closestMark = mark;
    }
  }

  return closestMark;
}

bool Slider::MarkReached( float value, int& outIndex )
{
  const float MARK_TOLERANCE = GetMarkTolerance();

  // Binary search
  int head = 0,
      tail = mMarks.Size() - 1;
  int current;
  float mark;

  while( head <= tail )
  {
    current = head + ( tail - head ) / 2;

    const Property::Value& propertyValue = mMarks[ current ];
    propertyValue.Get( mark );
    mark = MapValuePercentage( mark );

    if( fabsf( mark - value ) < MARK_TOLERANCE )
    {
      outIndex = current;
      return true;
    }

    if( value < mark )
    {
      tail = current - 1;
    }
    else
    {
      head = current + 1;
    }
  }

  return false;
}

bool Slider::HideValueView()
{
  if( mValueDisplay )
  {
    mValueDisplay.SetVisible( false );
  }

  return false;
}

void Slider::SetLowerBound( float bound )
{
  mLowerBound = bound;
  DisplayValue( GetValue(), false );
}

float Slider::GetLowerBound() const
{
  return mLowerBound;
}

void Slider::SetUpperBound( float bound )
{
  mUpperBound = bound;
  DisplayValue( GetValue(), false );
}

float Slider::GetUpperBound() const
{
  return mUpperBound;
}

void Slider::SetValue( float value )
{
  mValue = value;
  DisplayValue( mValue, true );
}

float Slider::GetValue() const
{
  return mValue;
}

void Slider::SetTrackRegion( const Vector2& region )
{
  mTrackRegion = region;

  if( mTrack )
  {
    mTrack.SetSize( mTrackRegion );
  }

  ResizeProgressRegion( Vector2( 0.0f, mTrackRegion.y ) );

  mDomain = CalcDomain( mTrackRegion );

  DisplayValue( GetValue(), false );  // Set the progress bar to correct width
}

const Vector2& Slider::GetTrackRegion() const
{
  return mTrackRegion;
}

void Slider::SetHandleSize( const Vector2& size )
{
  mHandleSize = size;

  ResizeHandleSize( mHandleSize );

  Vector2 hitRegion = GetHitRegion();
  hitRegion.x += mHandleSize.x;
  SetHitRegion( hitRegion );
}

const Vector2& Slider::GetHandleSize() const
{
  return mHandleSize;
}

void Slider::SetDisabledColor( const Vector4& color )
{
  mDisabledColor = color;

  UpdateSkin();
}

Vector4 Slider::GetDisabledColor() const
{
  return mDisabledColor;
}

void Slider::SetValuePrecision( int precision )
{
  mValuePrecision = precision;
}

int Slider::GetValuePrecision() const
{
  return mValuePrecision;
}

void Slider::SetShowPopup( bool showPopup )
{
  mShowPopup = showPopup;

  // Value display
  if( mShowPopup )
  {
    AddPopup();
  }
  else
  {
    RemovePopup();
  }
}

bool Slider::GetShowPopup() const
{
  return mShowPopup;
}

void Slider::SetShowValue( bool showValue )
{
  mShowValue = showValue;

  if( mShowValue )
  {
    CreateHandleValueDisplay();
  }
  else
  {
    DestroyHandleValueDisplay();
  }
}

bool Slider::GetShowValue() const
{
  return mShowValue;
}

void Slider::SetEnabled( bool enabled )
{
  if( enabled )
  {
    mState = NORMAL;
  }
  else
  {
    mState = DISABLED;
  }

  UpdateSkin();
}

bool Slider::IsEnabled() const
{
  return mState != DISABLED;
}

void Slider::SetMarkTolerance( float tolerance )
{
  mMarkTolerance = tolerance;
}

float Slider::GetMarkTolerance() const
{
  return mMarkTolerance;
}

// Static class method to support script connecting signals
bool Slider::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected = true;
  Toolkit::Slider slider = Toolkit::Slider::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_VALUE_CHANGED ) )
  {
    slider.ValueChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MARK ) )
  {
    slider.MarkReachedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Slider::DisplayPopup( float value )
{
  // Value displayDoConnectSignal
  if( mValueTextLabel )
  {
    std::stringstream ss;
    ss.precision( GetValuePrecision() );
    ss << std::fixed << value;
    mValueTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, ss.str() );

    if( mValueDisplay )
    {
      mValueDisplay.SetVisible( true );

      mValueTimer.SetInterval( VALUE_VIEW_SHOW_DURATION );
    }
  }
}

void Slider::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::Slider slider = Toolkit::Slider::DownCast( Dali::BaseHandle( object ) );

  if ( slider )
  {
    Slider& sliderImpl( GetImpl( slider ) );

    switch ( propertyIndex )
    {
      case Toolkit::Slider::Property::LOWER_BOUND:
      {
        sliderImpl.SetLowerBound( value.Get< float >() );
        break;
      }

      case Toolkit::Slider::Property::UPPER_BOUND:
      {
        sliderImpl.SetUpperBound( value.Get< float >() );
        break;
      }

      case Toolkit::Slider::Property::VALUE:
      {
        sliderImpl.SetValue( value.Get< float >() );
        break;
      }

      case Toolkit::Slider::Property::TRACK_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          sliderImpl.SetTrackVisual( map );
        }
        break;
      }

      case Toolkit::Slider::Property::HANDLE_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          sliderImpl.SetHandleVisual( map );
        }
        break;
      }

      case Toolkit::Slider::Property::PROGRESS_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          sliderImpl.SetProgressVisual( map );
        }
        break;
      }

      case Toolkit::Slider::Property::POPUP_VISUAL:
      {
        std::string imageUrl;
        if( value.Get( imageUrl ) )
        {
          sliderImpl.SetPopupVisual( imageUrl );
        }

        // If it is not a string, then get a Property::Map from the property if possible.
        Property::Map map;
        if( value.Get( map ) )
        {
          sliderImpl.SetPopupVisual( map );
        }

        break;
      }

      case Toolkit::Slider::Property::POPUP_ARROW_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          sliderImpl.SetPopupArrowVisual( map );
        }
        break;
      }

      case Toolkit::Slider::Property::DISABLED_COLOR:
      {
        sliderImpl.SetDisabledColor( value.Get< Vector4 >() );
        break;
      }

      case Toolkit::Slider::Property::VALUE_PRECISION:
      {
        sliderImpl.SetValuePrecision( value.Get< int >() );
        break;
      }

      case Toolkit::Slider::Property::SHOW_POPUP:
      {
        sliderImpl.SetShowPopup( value.Get< bool >() );
        break;
      }

      case Toolkit::Slider::Property::SHOW_VALUE:
      {
        sliderImpl.SetShowValue( value.Get< bool >() );
        break;
      }

      case Toolkit::Slider::Property::MARKS:
      {
        sliderImpl.SetMarks( value.Get< Property::Array >() );
        break;
      }

      case Toolkit::Slider::Property::SNAP_TO_MARKS:
      {
        sliderImpl.SetSnapToMarks( value.Get< bool >() );
        break;
      }

      case Toolkit::Slider::Property::MARK_TOLERANCE:
      {
        sliderImpl.SetMarkTolerance( value.Get< float >() );
        break;
      }
    }
  }
}

Property::Value Slider::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::Slider slider = Toolkit::Slider::DownCast( Dali::BaseHandle( object ) );

  if ( slider )
  {
    Slider& sliderImpl( GetImpl( slider ) );

    switch ( propertyIndex )
    {
      case Toolkit::Slider::Property::LOWER_BOUND:
      {
        value = sliderImpl.GetLowerBound();
        break;
      }

      case Toolkit::Slider::Property::UPPER_BOUND:
      {
        value = sliderImpl.GetUpperBound();
        break;
      }

      case Toolkit::Slider::Property::VALUE:
      {
        value = sliderImpl.GetValue();
        break;
      }

      case Toolkit::Slider::Property::TRACK_VISUAL:
      {
        if( !sliderImpl.mTrackVisual.empty() )
        {
          value = sliderImpl.GetTrackVisual();
        }
        else if( !sliderImpl.mTrackMap.Empty() )
        {
          value = sliderImpl.mTrackMap;
        }
        break;
      }

      case Toolkit::Slider::Property::HANDLE_VISUAL:
      {
        if( !sliderImpl.mHandleVisual.empty() )
        {
          value = sliderImpl.GetHandleVisual();
        }
        else if( !sliderImpl.mHandleMap.Empty() )
        {
          value = sliderImpl.mHandleMap;
        }
        break;
      }

      case Toolkit::Slider::Property::PROGRESS_VISUAL:
      {
        if( !sliderImpl.mProgressVisual.empty() )
        {
          value = sliderImpl.GetProgressVisual();
        }
        else if( !sliderImpl.mProgressMap.Empty() )
        {
          value = sliderImpl.mProgressMap;
        }
        break;
      }

      case Toolkit::Slider::Property::POPUP_VISUAL:
      {
        if( !sliderImpl.mPopupVisual.empty() )
        {
          value = sliderImpl.GetPopupVisual();
        }
        else if( !sliderImpl.mPopupMap.Empty() )
        {
          value = sliderImpl.mPopupMap;
        }
        break;
      }

      case Toolkit::Slider::Property::POPUP_ARROW_VISUAL:
      {
        if( !sliderImpl.mPopupArrowVisual.empty() )
        {
          value = sliderImpl.GetPopupArrowVisual();
        }
        else if( !sliderImpl.mPopupArrowMap.Empty() )
        {
          value = sliderImpl.mPopupArrowMap;
        }
        break;
      }

      case Toolkit::Slider::Property::DISABLED_COLOR:
      {
        value = sliderImpl.GetDisabledColor();
        break;
      }

      case Toolkit::Slider::Property::VALUE_PRECISION:
      {
        value = sliderImpl.GetValuePrecision();
        break;
      }

      case Toolkit::Slider::Property::SHOW_POPUP:
      {
        value = sliderImpl.GetShowPopup();
        break;
      }

      case Toolkit::Slider::Property::SHOW_VALUE:
      {
        value = sliderImpl.GetShowValue();
        break;
      }

      case Toolkit::Slider::Property::MARKS:
      {
        // TODO: Need to be able to return a PropertyArray
        // value = sliderImpl.GetMarks();
        break;
      }

      case Toolkit::Slider::Property::SNAP_TO_MARKS:
      {
        value = sliderImpl.GetSnapToMarks();
        break;
      }

      case Toolkit::Slider::Property::MARK_TOLERANCE:
      {
        value = sliderImpl.GetMarkTolerance();
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
