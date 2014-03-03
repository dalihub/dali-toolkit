//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/internal/controls/slider/slider-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

#include <sstream>

using namespace Dali;
using namespace std;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
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

const char* SKINNED_BACKING_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin.png";
const char* SKINNED_HANDLE_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin-handle.png";;
const char* SKINNED_PROGRESS_IMAGE_NAME = DALI_IMAGE_DIR "slider-skin-progress.png";
const char* SKINNED_POPUP_IMAGE_NAME = DALI_IMAGE_DIR "slider-popup.png";
const char* SKINNED_POPUP_ARROW_IMAGE_NAME = DALI_IMAGE_DIR "slider-popup-arrow.png";

const Vector4 SKINNED_BACKING_SCALE9_BORDER( 12.0f, 0.0f, 12.0f, 0.0f );
const Vector4 SKINNED_PROGRESS_SCALE9_BORDER( 14.0f, 0.0f, 0.0f, 0.0f );
const Vector4 SKINNED_POPUP_SCALE9_BORDER( 10.0f, 10.0f, 10.0f, 10.0f );

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


BaseHandle Create()
{
  return Dali::Toolkit::Slider::New();
}

TypeRegistration typeRegistration( typeid(Dali::Toolkit::Slider), typeid(Dali::Toolkit::Control), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::Slider::SIGNAL_VALUE_CHANGED, &Toolkit::Internal::Slider::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, Toolkit::Slider::SIGNAL_MARK, &Toolkit::Internal::Slider::DoConnectSignal );

} // namespace

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
: ControlImpl( true ),
  mState( NORMAL )
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

  // Register properties in a block so the properties are ready for the update functions
  mPropertyHitRegion = self.RegisterProperty( Dali::Toolkit::Slider::HIT_REGION_PROPERTY_NAME, DEFAULT_HIT_REGION, Property::READ_WRITE );
  mPropertyBackingRegion = self.RegisterProperty( Dali::Toolkit::Slider::BACKING_REGION_PROPERTY_NAME, DEFAULT_BACKING_REGION, Property::READ_WRITE );
  mPropertyHandleRegion = self.RegisterProperty( Dali::Toolkit::Slider::HANDLE_REGION_PROPERTY_NAME, DEFAULT_HANDLE_REGION, Property::READ_WRITE );

  mPropertyBackingImageName = self.RegisterProperty( Dali::Toolkit::Slider::BACKING_IMAGE_NAME_PROPERTY_NAME, SKINNED_BACKING_IMAGE_NAME, Property::READ_WRITE );
  mPropertyHandleImageName = self.RegisterProperty( Dali::Toolkit::Slider::HANDLE_IMAGE_NAME_PROPERTY_NAME, SKINNED_HANDLE_IMAGE_NAME, Property::READ_WRITE );

  mPropertyProgressImageName = self.RegisterProperty( Dali::Toolkit::Slider::PROGRESS_IMAGE_NAME_PROPERTY_NAME, SKINNED_PROGRESS_IMAGE_NAME, Property::READ_WRITE );
  mPropertyPopupImageName = self.RegisterProperty( Dali::Toolkit::Slider::POPUP_IMAGE_NAME_PROPERTY_NAME, SKINNED_POPUP_IMAGE_NAME, Property::READ_WRITE );
  mPropertyPopupArrowImageName = self.RegisterProperty( Dali::Toolkit::Slider::POPUP_ARROW_IMAGE_NAME_PROPERTY_NAME, SKINNED_POPUP_ARROW_IMAGE_NAME, Property::READ_WRITE );

  mPropertyBackingScale9Border = self.RegisterProperty( Dali::Toolkit::Slider::BACKING_SCALE9_BORDER_PROPERTY_NAME, SKINNED_BACKING_SCALE9_BORDER, Property::READ_WRITE );
  mPropertyProgressScale9Border = self.RegisterProperty( Dali::Toolkit::Slider::PROGRESS_SCALE9_BORDER_PROPERTY_NAME, SKINNED_PROGRESS_SCALE9_BORDER, Property::READ_WRITE );
  mPropertyPopupScale9Border = self.RegisterProperty( Dali::Toolkit::Slider::POPUP_SCALE9_BORDER_PROPERTY_NAME, SKINNED_POPUP_SCALE9_BORDER, Property::READ_WRITE );

  mPropertyDisableColor = self.RegisterProperty( Dali::Toolkit::Slider::DISABLE_COLOR_PROPERTY_NAME, DEFAULT_DISABLE_COLOR, Property::READ_WRITE );
  mPropertyPopupTextColor = self.RegisterProperty( Dali::Toolkit::Slider::POPUP_TEXT_COLOR_PROPERTY_NAME, DEFAULT_POPUP_TEXT_COLOR, Property::READ_WRITE );

  mPropertyValuePrecision = self.RegisterProperty( Dali::Toolkit::Slider::VALUE_PRECISION_PROPERTY_NAME, DEFAULT_VALUE_PRECISION, Property::READ_WRITE );
  mPropertyShowPopup = self.RegisterProperty( Dali::Toolkit::Slider::SHOW_POPUP_PROPERTY_NAME, DEFAULT_SHOW_POPUP, Property::READ_WRITE );
  mPropertyShowValue = self.RegisterProperty( Dali::Toolkit::Slider::SHOW_VALUE_PROPERTY_NAME, DEFAULT_SHOW_VALUE, Property::READ_WRITE );

  mPropertyEnabled = self.RegisterProperty( Dali::Toolkit::Slider::ENABLED_PROPERTY_NAME, DEFAULT_ENABLED, Property::READ_WRITE );

  mPropertyMarks = self.RegisterProperty( Dali::Toolkit::Slider::MARKS_PROPERTY_NAME, mMarks, Property::READ_WRITE );
  mPropertySnapToMarks = self.RegisterProperty( Dali::Toolkit::Slider::SNAP_TO_MARKS_PROPERTY_NAME, false, Property::READ_WRITE );
  mPropertyMarkTolerance = self.RegisterProperty( Dali::Toolkit::Slider::MARK_TOLERANCE_PROPERTY_NAME, MARK_SNAP_TOLERANCE, Property::READ_WRITE );

  mPropertyLowerBound = self.RegisterProperty( Dali::Toolkit::Slider::LOWER_BOUND_PROPERTY_NAME, DEFAULT_LOWER_BOUND, Property::READ_WRITE );
  mPropertyUpperBound = self.RegisterProperty( Dali::Toolkit::Slider::UPPER_BOUND_PROPERTY_NAME, DEFAULT_UPPER_BOUND, Property::READ_WRITE );
  mPropertyValue = self.RegisterProperty( Dali::Toolkit::Slider::VALUE_PROPERTY_NAME, DEFAULT_VALUE, Property::READ_WRITE );

  ResizeHitRegion( DEFAULT_HIT_REGION );
  SetBackingRegion( DEFAULT_BACKING_REGION );
  UpdateHandleRegion( DEFAULT_HANDLE_REGION );
  CreateBackingImage( SKINNED_BACKING_IMAGE_NAME );
  CreateHandleImage( SKINNED_HANDLE_IMAGE_NAME );
  CreateProgressImage( SKINNED_PROGRESS_IMAGE_NAME );
  CreatePopupImage( SKINNED_POPUP_IMAGE_NAME );
  CreatePopupArrowImage( SKINNED_POPUP_ARROW_IMAGE_NAME );
  SetBackingScale9( SKINNED_BACKING_SCALE9_BORDER );
  SetProgressScale9( SKINNED_PROGRESS_SCALE9_BORDER );
  SetPopupScale9( SKINNED_POPUP_SCALE9_BORDER );
  UpdatePopupTextColor( DEFAULT_POPUP_TEXT_COLOR );
  ShowPopup( DEFAULT_SHOW_POPUP );
  ShowValue( DEFAULT_SHOW_VALUE );
  SetEnabled( DEFAULT_ENABLED );
  UpdateLowerBound( DEFAULT_LOWER_BOUND );
  UpdateUpperBound( DEFAULT_UPPER_BOUND );
  UpdateSkin();
  DisplayValue( DEFAULT_VALUE, false );       // Run this last to display the correct value

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
  }

  return true;
}

void Slider::OnPan( Actor actor, PanGesture gesture )
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
        if( mState == PRESSED && GetSnapToMarks() )
        {
          float value = MapBounds( SnapToMark( MapPercentage( gesture.position ) ), GetLowerBound(), GetUpperBound() );
          SetValue( value );
          DisplayPopup( value );
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

  if( mHandleValueTextView )
  {
    std::stringstream ss;
    ss.precision( GetValuePrecision() );
    ss << fixed << clampledValue;
    mHandleValueTextView.SetText( ss.str() );
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

bool Slider::GetSnapToMarks() const
{
  return Self().GetProperty<bool>( mPropertySnapToMarks );
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

void Slider::CreateBackingImage( const std::string& imageName )
{
  if( mBacking && imageName != String::EMPTY )
  {
    Image image = Image::New( imageName );
    mBacking.SetImage( image );
  }
}

void Slider::SetBackingScale9( const Vector4& border )
{
  if( mBacking )
  {
    mBacking.SetStyle( ImageActor::STYLE_NINE_PATCH );
    mBacking.SetNinePatchBorder( border );
  }
}

void Slider::SetBackingRegionSize( const Vector2& region )
{
  if( mBacking )
  {
    mBacking.SetSize( region );
  }
}

ImageActor Slider::CreateProgress()
{
  ImageActor progress = ImageActor::New();
  progress.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  progress.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  progress.SetZ( PROGRESS_Z );

  return progress;
}

void Slider::CreateProgressImage( const std::string& imageName )
{
  if( mProgress && imageName != String::EMPTY )
  {
    Image image = Image::New( imageName );
    mProgress.SetImage( image );
  }
}

void Slider::CreatePopupImage( const std::string& imageName )
{
  if( mPopup && imageName != String::EMPTY )
  {
    Image image = Image::New( imageName );
    mPopup.SetImage( image );
  }
}

void Slider::CreatePopupArrowImage( const std::string& imageName )
{
  if( mPopupArrow && imageName != String::EMPTY )
  {
    Image image = Image::New( imageName );
    mPopupArrow.SetImage( image );
  }
}

void Slider::SetProgressScale9( const Vector4& border )
{
  if( mProgress )
  {
    mProgress.SetStyle( ImageActor::STYLE_NINE_PATCH );
    mProgress.SetNinePatchBorder( border );
  }
}

void Slider::SetPopupScale9( const Vector4& border )
{
  if( mPopup )
  {
    mPopup.SetStyle( ImageActor::STYLE_NINE_PATCH );
    mPopup.SetNinePatchBorder( border );
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

Toolkit::TextView Slider::CreatePopupText()
{
  Toolkit::TextView textView = Toolkit::TextView::New();
  textView.SetParentOrigin( ParentOrigin::CENTER );
  textView.SetAnchorPoint( AnchorPoint::CENTER );
  textView.SetSizePolicy( Control::Flexible, Control::Flexible );
  textView.SetZ( VALUE_DISPLAY_TEXT_Z );
  return textView;
}

ImageActor Slider::CreatePopup()
{
  ImageActor popup = ImageActor::New();
  popup.SetParentOrigin( ParentOrigin::TOP_CENTER );
  popup.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );

  mValueTextView = CreatePopupText();
  popup.Add( mValueTextView );

  return popup;
}

void Slider::CreateHandleImage( const std::string& imageName )
{
  if( mHandle && imageName != String::EMPTY )
  {
    Image image = Image::New( imageName );
    mHandle.SetImage( image );
  }
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
  if( mHandle && !mHandleValueTextView )
  {
    mHandleValueTextView = Toolkit::TextView::New();
    mHandleValueTextView.SetParentOrigin( ParentOrigin::CENTER );
    mHandleValueTextView.SetAnchorPoint( AnchorPoint::CENTER );
    mHandleValueTextView.SetSize( GetHandleRegion() );
    mHandleValueTextView.SetZ( HANDLE_VALUE_DISPLAY_TEXT_Z );
    mHandle.Add( mHandleValueTextView );
  }
}

void Slider::DestroyHandleValueDisplay()
{
  mHandleValueTextView.Unparent();
  mHandleValueTextView.Reset();
}

void Slider::UpdatePopupTextColor( const Vector4& color )
{
  if( mValueTextView )
  {
    mValueTextView.SetColor( color );
  }
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

void Slider::ResizeHitRegion( const Vector2& size )
{
  if( mHitArea )
  {
    mHitArea.SetSize( size );
  }
}

void Slider::AddPopup()
{
  if( !mValueDisplay )
  {
    mValueDisplay = CreateValueDisplay();
    mValueDisplay.SetVisible( false );
    mHandle.Add( mValueDisplay );

    Actor self = Self();
    CreatePopupImage( self.GetProperty<std::string>( mPropertyPopupImageName ) );
    SetPopupScale9( GetPopupScale9Border() );
    CreatePopupArrowImage( self.GetProperty<std::string>( mPropertyPopupArrowImageName )  );

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
    else
    {
      if( value < mark )
      {
        tail = current - 1;
      }
      else
      {
        head = current + 1;
      }
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

void Slider::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  if( index == mPropertyLowerBound )
  {
    UpdateLowerBound( propertyValue.Get<float>() );
  }
  else if( index == mPropertyUpperBound )
  {
    UpdateUpperBound( propertyValue.Get<float>() );
  }
  else if( index == mPropertyValue )
  {
    DisplayValue( propertyValue.Get<float>(), true );
  }
  else if( index == mPropertyHitRegion )
  {
    ResizeHitRegion( propertyValue.Get<Vector2>() );
  }
  else if( index == mPropertyBackingRegion )
  {
    ResizeBackingRegion( propertyValue.Get<Vector2>() );
  }
  else if( index == mPropertyHandleRegion )
  {
    UpdateHandleRegion( propertyValue.Get<Vector2>() );
  }
  else if( index == mPropertyBackingImageName )
  {
    CreateBackingImage( propertyValue.Get<std::string>() );
  }
  else if( index == mPropertyHandleImageName )
  {
    CreateHandleImage( propertyValue.Get<std::string>() );
  }
  else if( index == mPropertyProgressImageName )
  {
    CreateProgressImage( propertyValue.Get<std::string>() );
  }
  else if( index == mPropertyPopupImageName )
  {
    CreatePopupImage( propertyValue.Get<std::string>() );
  }
  else if( index == mPropertyPopupArrowImageName )
  {
    CreatePopupArrowImage( propertyValue.Get<std::string>() );
  }
  else if( index == mPropertyBackingScale9Border )
  {
    SetBackingScale9( propertyValue.Get<Vector4>() );
  }
  else if( index == mPropertyProgressScale9Border )
  {
    SetProgressScale9( propertyValue.Get<Vector4>() );
  }
  else if( index == mPropertyPopupScale9Border )
  {
    SetPopupScale9( propertyValue.Get<Vector4>() );
  }
  else if( index == mPropertyDisableColor )
  {
    UpdateSkin();
  }
  else if( index == mPropertyPopupTextColor )
  {
    UpdatePopupTextColor( propertyValue.Get<Vector4>() );
  }
  else if( index == mPropertyValuePrecision )
  {
    DisplayValue( GetValue(), false );
  }
  else if( index == mPropertyShowPopup )
  {
    ShowPopup( propertyValue.Get<bool>() );
  }
  else if( index == mPropertyShowValue )
  {
    ShowValue( propertyValue.Get<bool>() );
  }
  else if( index == mPropertyEnabled )
  {
    SetEnabled( propertyValue.Get<bool>() );
  }
  else if( index == mPropertyMarks )
  {
    SetMarks( propertyValue.Get<Property::Array>() );
  }
  else if( index == mPropertySnapToMarks )
  {
    // Nothing
  }
  else if( index == mPropertyMarkTolerance )
  {
    // Nothing
  }
}

void Slider::UpdateLowerBound( float bound )
{
  DisplayValue( GetValue(), false );
}

float Slider::GetLowerBound() const
{
  return Self().GetProperty<float>( mPropertyLowerBound );
}

void Slider::UpdateUpperBound( float bound )
{
  DisplayValue( GetValue(), false );
}

float Slider::GetUpperBound() const
{
  return Self().GetProperty<float>( mPropertyUpperBound );
}

void Slider::SetValue( float value )
{
  Self().SetProperty( mPropertyValue, value );
}

float Slider::GetValue() const
{
  return Self().GetProperty<float>( mPropertyValue );
}

void Slider::SetHitRegion( const Vector2& region )
{
  Self().SetProperty( mPropertyHitRegion, region );
}

Vector2 Slider::GetHitRegion() const
{
  return Self().GetProperty<Vector2>( mPropertyHitRegion );
}

void Slider::SetBackingRegion( const Vector2& region )
{
  Self().SetProperty( mPropertyBackingRegion, region );
}

void Slider::ResizeBackingRegion( const Vector2& region )
{
  SetBackingRegionSize( region );
  ResizeProgressRegion( Vector2( 0.0f, region.y ) );

  mDomain = CalcDomain( region );

  DisplayValue( GetValue(), false );  // Set the progress bar to correct width
}

Vector2 Slider::GetBackingRegion() const
{
  return Self().GetProperty<Vector2>( mPropertyBackingRegion );
}

void Slider::UpdateHandleRegion( const Vector2& region )
{
  ResizeHandleRegion( region );

  Vector2 hitRegion = GetHitRegion();
  hitRegion.x += region.x;
  SetHitRegion( hitRegion );
}

Vector2 Slider::GetHandleRegion() const
{
  return Self().GetProperty<Vector2>( mPropertyHandleRegion );
}

Vector4 Slider::GetBackingScale9Border() const
{
  return Self().GetProperty<Vector4>( mPropertyBackingScale9Border );
}

Vector4 Slider::GetPopupScale9Border() const
{
  return Self().GetProperty<Vector4>( mPropertyPopupScale9Border );
}

Vector4 Slider::GetDisableColor() const
{
  return Self().GetProperty<Vector4>( mPropertyDisableColor );
}

Vector4 Slider::GetPopupTextColor() const
{
  return Self().GetProperty<Vector4>( mPropertyPopupTextColor );
}

int Slider::GetValuePrecision() const
{
  return Self().GetProperty<int>( mPropertyValuePrecision );
}

void Slider::ShowPopup( bool showPopup )
{
  // Value display
  if( showPopup )
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
  return Self().GetProperty<bool>( mPropertyShowPopup );
}

void Slider::ShowValue( bool showValue )
{
  if( showValue )
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
  return Self().GetProperty<bool>( mPropertyShowValue );
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

float Slider::GetMarkTolerance() const
{
  return Self().GetProperty<float>( mPropertyMarkTolerance );
}

// static class method to support script connecting signals

bool Slider::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected = true;
  Toolkit::Slider slider = Toolkit::Slider::DownCast( handle );

  if( signalName == Dali::Toolkit::Slider::SIGNAL_VALUE_CHANGED )
  {
    slider.ValueChangedSignal().Connect( tracker, functor );
  }
  else if( signalName == Dali::Toolkit::Slider::SIGNAL_MARK )
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
  // Value display
  if( mValueTextView )
  {
    std::stringstream ss;
    ss.precision( GetValuePrecision() );
    ss << fixed << value;
    mValueTextView.SetText( ss.str() );
    TextStyle style;
    style.SetTextColor( GetPopupTextColor() );
    mValueTextView.SetStyleToCurrentText( style, TextStyle::COLOR);

    if( mValueDisplay )
    {
      Font font = Font::New();
      float popupWidth = font.MeasureText( ss.str() ).x + VALUE_POPUP_MARGIN * 2.0f;
      if( popupWidth < VALUE_POPUP_MIN_WIDTH )
      {
        popupWidth = VALUE_POPUP_MIN_WIDTH;
      }

      mPopup.SetSize( popupWidth, VALUE_POPUP_HEIGHT );
      mValueDisplay.SetVisible( true );

      mValueTimer.SetInterval( VALUE_VIEW_SHOW_DURATION );
    }
  }
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
