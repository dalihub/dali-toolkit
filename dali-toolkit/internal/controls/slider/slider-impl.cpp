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
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>

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

DALI_PROPERTY_REGISTRATION( Slider, "lower-bound",            FLOAT,    LOWER_BOUND            )
DALI_PROPERTY_REGISTRATION( Slider, "upper-bound",            FLOAT,    UPPER_BOUND            )
DALI_PROPERTY_REGISTRATION( Slider, "value",                  FLOAT,    VALUE                  )
DALI_PROPERTY_REGISTRATION( Slider, "hit-region",             VECTOR2,  HIT_REGION             )
DALI_PROPERTY_REGISTRATION( Slider, "backing-region",         VECTOR2,  BACKING_REGION         )
DALI_PROPERTY_REGISTRATION( Slider, "handle-region",          VECTOR2,  HANDLE_REGION          )
DALI_PROPERTY_REGISTRATION( Slider, "backing-image-name",     STRING,   BACKING_IMAGE_NAME     )
DALI_PROPERTY_REGISTRATION( Slider, "handle-image-name",      STRING,   HANDLE_IMAGE_NAME      )
DALI_PROPERTY_REGISTRATION( Slider, "progress-image-name",    STRING,   PROGRESS_IMAGE_NAME    )
DALI_PROPERTY_REGISTRATION( Slider, "popup-image-name",       STRING,   POPUP_IMAGE_NAME       )
DALI_PROPERTY_REGISTRATION( Slider, "popup-arrow-image-name", STRING,   POPUP_ARROW_IMAGE_NAME )
DALI_PROPERTY_REGISTRATION( Slider, "disable-color",          VECTOR4,  DISABLE_COLOR          )
DALI_PROPERTY_REGISTRATION( Slider, "popup-text-color",       VECTOR4,  POPUP_TEXT_COLOR       )
DALI_PROPERTY_REGISTRATION( Slider, "value-precision",        INTEGER,  VALUE_PRECISION        )
DALI_PROPERTY_REGISTRATION( Slider, "show-popup",             BOOLEAN,  SHOW_POPUP             )
DALI_PROPERTY_REGISTRATION( Slider, "show-value",             BOOLEAN,  SHOW_VALUE             )
DALI_PROPERTY_REGISTRATION( Slider, "enabled",                BOOLEAN,  ENABLED                )
DALI_PROPERTY_REGISTRATION( Slider, "marks",                  ARRAY,    MARKS                  )
DALI_PROPERTY_REGISTRATION( Slider, "snap-to-marks",          BOOLEAN,  SNAP_TO_MARKS          )
DALI_PROPERTY_REGISTRATION( Slider, "mark-tolerance",         FLOAT,    MARK_TOLERANCE         )

DALI_SIGNAL_REGISTRATION(   Slider, "value-changed",                    SIGNAL_VALUE_CHANGED   )
DALI_SIGNAL_REGISTRATION(   Slider, "mark",                             SIGNAL_MARK            )

DALI_TYPE_REGISTRATION_END()

const float BACKING_Z = -0.1f;
const float PROGRESS_Z = 0.1f;
const float HANDLE_Z = 1.0f;
const float VALUE_TEXT_INCREMENT = 0.01f;
const float HANDLE_VALUE_DISPLAY_TEXT_Z = HANDLE_Z + VALUE_TEXT_INCREMENT;
const float VALUE_DISPLAY_TEXT_Z = VALUE_TEXT_INCREMENT + VALUE_TEXT_INCREMENT;  // Put above HANDLE_VALUE_DISPLAY_TEXT_Z (parented to handle)

const float MARK_SNAP_TOLERANCE = 0.05f; // 5% of slider width

const int VALUE_VIEW_SHOW_DURATION = 1000;  // millisec
const int VALUE_VIEW_SHOW_DURATION_LONG = 2000;  // millisec

const float VALUE_VERTICAL_OFFSET = 48.0f;

const float DEFAULT_WIDTH = 0.0f;
const float DEFAULT_HEIGHT = 27.0f;
const float DEFAULT_HIT_HEIGHT = 72.0f;
const float DEFAULT_HANDLE_HEIGHT = DEFAULT_HIT_HEIGHT;
const float POPUP_TEXT_PADDING = 10.0f;

const char* SKINNED_BACKING_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin.9.png";
const char* SKINNED_HANDLE_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin-handle.png";;
const char* SKINNED_PROGRESS_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin-progress.9.png";
const char* SKINNED_POPUP_IMAGE_NAME = DALI_IMAGE_DIR "slider-popup.9.png";
const char* SKINNED_POPUP_ARROW_IMAGE_NAME = DALI_IMAGE_DIR "slider-popup-arrow.png";

const Vector2 DEFAULT_HIT_REGION( DEFAULT_WIDTH, DEFAULT_HIT_HEIGHT );
const Vector2 DEFAULT_BACKING_REGION( DEFAULT_WIDTH, DEFAULT_HEIGHT );
const Vector2 DEFAULT_HANDLE_REGION( DEFAULT_HANDLE_HEIGHT, DEFAULT_HANDLE_HEIGHT );

const Vector4 DEFAULT_DISABLE_COLOR( 0.5f, 0.5f, 0.5f, 1.0f );
const Vector4 DEFAULT_POPUP_TEXT_COLOR( 0.5f, 0.5f, 0.5f, 1.0f );

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
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mState( NORMAL ),
  mDisableColor( 0.0f, 0.0f, 0.0f, 0.0f ),
  mPopupTextColor( 0.0f, 0.0f, 0.0f, 0.0f ),
  mHitRegion( 0.0f, 0.0f ),
  mBackingRegion( 0.0f, 0.0f ),
  mHandleRegionSize( 0.0f, 0.0f ),
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
  SetBackingRegion( DEFAULT_BACKING_REGION );
  SetHandleRegion(  DEFAULT_HANDLE_REGION  );

  SetBackingImageName(    SKINNED_BACKING_IMAGE_NAME     );
  SetHandleImageName(     SKINNED_HANDLE_IMAGE_NAME      );
  SetProgressImageName(   SKINNED_PROGRESS_IMAGE_NAME    );
  SetPopupImageName(      SKINNED_POPUP_IMAGE_NAME       );
  SetPopupArrowImageName( SKINNED_POPUP_ARROW_IMAGE_NAME );

  SetPopupTextColor( DEFAULT_POPUP_TEXT_COLOR );

  SetShowPopup( DEFAULT_SHOW_POPUP );
  SetShowValue( DEFAULT_SHOW_VALUE );

  SetEnabled( DEFAULT_ENABLED );
  SetDisableColor( DEFAULT_DISABLE_COLOR );

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
}

void Slider::OnControlSizeSet( const Vector3& size )
{
  // Factor in handle overshoot into size of backing
  SetHitRegion( Vector2( size.x, GetHitRegion().y ) );
  SetBackingRegion( Vector2( size.x - GetHandleRegion().x, GetBackingRegion().y ) );
}

bool Slider::OnTouchEvent(Actor actor, const TouchEvent& event)
{
  if( mState != DISABLED )
  {
    TouchPoint::State touchState = event.GetPoint(0).state;

    if( touchState == TouchPoint::Down )
    {
      mState = PRESSED;

      float percentage = MapPercentage( event.GetPoint(0).local );
      float value = MapBounds( ( GetSnapToMarks() ) ? SnapToMark( percentage ) : MarkFilter( percentage ), GetLowerBound(), GetUpperBound() );
      SetValue( value );
      DisplayPopup( value );
    }
    else if( touchState == TouchPoint::Up)
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

  mHandle.SetPosition( x, 0.0f, HANDLE_Z );

  // Progress bar
  if( mProgress )
  {
    if( clampledValue > 0.0f )
    {
      mProgress.SetVisible( true ); // Deliberately set this in case multiple SetValues are fired at once
      mProgress.SetSize( x, GetBackingRegion().y );
    }
    else
    {
      mProgress.SetVisible( false );
    }
  }

  // Signals
  if( raiseSignals )
  {
    Toolkit::Slider self = Toolkit::Slider::DownCast( Self() );
    mValueChangedSignal.Emit( self, clampledValue );

    int markIndex;
    if( MarkReached( percent, markIndex ) )
    {
      mMarkSignal.Emit( self, markIndex );
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
  float value;
  for( MarkList::const_iterator it = marks.begin(), itEnd = marks.end(); it != itEnd; ++it )
  {
    const Property::Value& propertyValue = *it;
    propertyValue.Get( value );

    mMarks.push_back( value );
  }
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
  hitRegion.TouchedSignal().Connect( this, &Slider::OnTouchEvent );

  return hitRegion;
}

ImageActor Slider::CreateBacking()
{
  ImageActor backing = ImageActor::New();
  backing.SetParentOrigin( ParentOrigin::CENTER );
  backing.SetAnchorPoint( AnchorPoint::CENTER );
  backing.SetZ( BACKING_Z );

  return backing;
}

void Slider::SetBackingImageName( const std::string& imageName )
{
  if( mBacking && ( imageName.size() > 0 ) )
  {
    Image image = ResourceImage::New( imageName );
    mBacking.SetImage( image );
  }
}

std::string Slider::GetBackingImageName()
{
  if( mBacking )
  {
    return ResourceImage::DownCast( mBacking.GetImage() ).GetUrl();
  }

  return std::string( "" );
}

ImageActor Slider::CreateProgress()
{
  ImageActor progress = ImageActor::New();
  progress.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  progress.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  progress.SetZ( PROGRESS_Z );

  return progress;
}

void Slider::SetProgressImageName( const std::string& imageName )
{
  if( mProgress && ( imageName.size() > 0 ) )
  {
    Image image = ResourceImage::New( imageName );
    mProgress.SetImage( image );
  }
}

std::string Slider::GetProgressImageName()
{
  if( mProgress )
  {
    return ResourceImage::DownCast( mProgress.GetImage()).GetUrl();
  }

  return std::string( "" );
}

void Slider::SetPopupImageName( const std::string& imageName )
{
  mPopupImageName = imageName;
}

std::string Slider::GetPopupImageName()
{
  return mPopupImageName;
}

void Slider::CreatePopupImage( const std::string& imageName )
{
  if( mPopup && ( imageName.size() > 0 ) )
  {
    Image image = ResourceImage::New( imageName );
    mPopup.SetImage( image );
  }
}

void Slider::SetPopupArrowImageName( const std::string& imageName )
{
  mPopupArrowImageName = imageName;
}

std::string Slider::GetPopupArrowImageName()
{
  return mPopupArrowImageName;
}

void Slider::CreatePopupArrowImage( const std::string& imageName )
{
  if( mPopupArrow && ( imageName.size() > 0 ) )
  {
    Image image = ResourceImage::New( imageName );
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

ImageActor Slider::CreateHandle()
{
  ImageActor handle = ImageActor::New();
  handle.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  handle.SetAnchorPoint( AnchorPoint::CENTER );
  handle.SetZ( HANDLE_Z );

  return handle;
}

ImageActor Slider::CreatePopupArrow()
{
  ImageActor arrow = ImageActor::New();
  arrow.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  arrow.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  arrow.SetZ( HANDLE_Z );

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
  textLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, DEFAULT_POPUP_TEXT_COLOR );
  textLabel.SetZ( VALUE_DISPLAY_TEXT_Z );
  textLabel.SetPadding( Padding( POPUP_TEXT_PADDING, POPUP_TEXT_PADDING, 0.0f, 0.0f ) );
  return textLabel;
}

ImageActor Slider::CreatePopup()
{
  ImageActor popup = ImageActor::New();
  popup.SetParentOrigin( ParentOrigin::TOP_CENTER );
  popup.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  popup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH );

  mValueTextLabel = CreatePopupText();
  popup.Add( mValueTextLabel );

  return popup;
}

void Slider::SetHandleImageName( const std::string& imageName )
{
  if( mHandle && ( imageName.size() > 0 ) )
  {
    Image image = ResourceImage::New( imageName );
    mHandle.SetImage( image );
  }
}

std::string Slider::GetHandleImageName()
{
  if( mHandle )
  {
    return ResourceImage::DownCast( mHandle.GetImage() ).GetUrl();
  }

  return std::string( "" );
}

void Slider::ResizeHandleRegion( const Vector2& region )
{
  if( mHandle )
  {
    mHandle.SetSize( region );
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
    mHandleValueTextLabel.SetDrawMode( DrawMode::OVERLAY );
    mHandle.Add( mHandleValueTextLabel );
  }
}

void Slider::DestroyHandleValueDisplay()
{
  UnparentAndReset(mHandleValueTextLabel);
}

void Slider::SetPopupTextColor( const Vector4& color )
{
  mPopupTextColor = color;
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

Toolkit::Slider::MarkSignalType& Slider::MarkSignal()
{
  return mMarkSignal;
}

void Slider::UpdateSkin()
{
  switch( mState )
  {
    case NORMAL:
    {
      mBacking.SetColor( Color::WHITE );
      mHandle.SetColor( Color::WHITE );
      mProgress.SetColor( Color::WHITE );
      break;
    }
    case DISABLED:
    {
      Vector4 disableColor = GetDisableColor();
      mBacking.SetColor( disableColor );
      mHandle.SetColor( disableColor );
      mProgress.SetColor( disableColor );
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

  // Background
  mBacking = CreateBacking();
  self.Add( mBacking );

  // Progress bar
  mProgress = CreateProgress();
  mBacking.Add( mProgress );

  // Handle
  mHandle = CreateHandle();
  mBacking.Add( mHandle );
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

    CreatePopupImage( GetPopupImageName() );
    CreatePopupArrowImage( GetPopupArrowImageName() );

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
  for( MarkList::iterator it = mMarks.begin(), itEnd = mMarks.end(); it != itEnd; ++it )
  {
    const Property::Value& propertyValue = *it;
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
  for( MarkList::iterator it = mMarks.begin(), itEnd = mMarks.end(); it != itEnd; ++it )
  {
    const Property::Value& propertyValue = *it;
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
      tail = mMarks.size() - 1;
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

void Slider::SetBackingRegion( const Vector2& region )
{
  mBackingRegion = region;

  if( mBacking )
  {
    mBacking.SetSize( mBackingRegion );
  }

  ResizeProgressRegion( Vector2( 0.0f, mBackingRegion.y ) );

  mDomain = CalcDomain( mBackingRegion );

  DisplayValue( GetValue(), false );  // Set the progress bar to correct width
}

const Vector2& Slider::GetBackingRegion() const
{
  return mBackingRegion;
}

void Slider::SetHandleRegion( const Vector2& region )
{
  mHandleRegionSize = region;

  ResizeHandleRegion( mHandleRegionSize );

  Vector2 hitRegion = GetHitRegion();
  hitRegion.x += mHandleRegionSize.x;
  SetHitRegion( hitRegion );
}

const Vector2& Slider::GetHandleRegion() const
{
  return mHandleRegionSize;
}

void Slider::SetDisableColor( const Vector4& color )
{
  mDisableColor = color;

  UpdateSkin();
}

Vector4 Slider::GetDisableColor() const
{
  return mDisableColor;
}

Vector4 Slider::GetPopupTextColor() const
{
  return mPopupTextColor;
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
    slider.MarkSignal().Connect( tracker, functor );
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

      case Toolkit::Slider::Property::HIT_REGION:
      {
        sliderImpl.SetHitRegion( value.Get< Vector2 >() );
        break;
      }

      case Toolkit::Slider::Property::BACKING_REGION:
      {
        sliderImpl.SetBackingRegion( value.Get< Vector2 >() );
        break;
      }

      case Toolkit::Slider::Property::HANDLE_REGION:
      {
        sliderImpl.SetHandleRegion( value.Get< Vector2 >() );
        break;
      }

      case Toolkit::Slider::Property::BACKING_IMAGE_NAME:
      {
        sliderImpl.SetBackingImageName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Slider::Property::HANDLE_IMAGE_NAME:
      {
        sliderImpl.SetHandleImageName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Slider::Property::PROGRESS_IMAGE_NAME:
      {
        sliderImpl.SetProgressImageName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Slider::Property::POPUP_IMAGE_NAME:
      {
        sliderImpl.SetPopupImageName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Slider::Property::POPUP_ARROW_IMAGE_NAME:
      {
        sliderImpl.SetPopupArrowImageName( value.Get< std::string >() );
        break;
      }

      case Toolkit::Slider::Property::DISABLE_COLOR:
      {
        sliderImpl.SetDisableColor( value.Get< Vector4 >() );
        break;
      }

      case Toolkit::Slider::Property::POPUP_TEXT_COLOR:
      {
        sliderImpl.SetPopupTextColor( value.Get< Vector4 >() );
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

      case Toolkit::Slider::Property::ENABLED:
      {
        sliderImpl.SetEnabled( value.Get< bool >() );
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

      case Toolkit::Slider::Property::HIT_REGION:
      {
        value = sliderImpl.GetHitRegion();
        break;
      }

      case Toolkit::Slider::Property::BACKING_REGION:
      {
        value = sliderImpl.GetBackingRegion();
        break;
      }

      case Toolkit::Slider::Property::HANDLE_REGION:
      {
        value = sliderImpl.GetHandleRegion();
        break;
      }

      case Toolkit::Slider::Property::BACKING_IMAGE_NAME:
      {
        value = sliderImpl.GetBackingImageName();
        break;
      }

      case Toolkit::Slider::Property::HANDLE_IMAGE_NAME:
      {
        value = sliderImpl.GetHandleImageName();
        break;
      }

      case Toolkit::Slider::Property::PROGRESS_IMAGE_NAME:
      {
        value = sliderImpl.GetProgressImageName();
        break;
      }

      case Toolkit::Slider::Property::POPUP_IMAGE_NAME:
      {
        value = sliderImpl.GetPopupImageName();
        break;
      }

      case Toolkit::Slider::Property::POPUP_ARROW_IMAGE_NAME:
      {
        value = sliderImpl.GetPopupArrowImageName();
        break;
      }

      case Toolkit::Slider::Property::DISABLE_COLOR:
      {
        value = sliderImpl.GetDisableColor();
        break;
      }

      case Toolkit::Slider::Property::POPUP_TEXT_COLOR:
      {
        value = sliderImpl.GetPopupTextColor();
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

      case Toolkit::Slider::Property::ENABLED:
      {
        value = sliderImpl.IsEnabled();
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
