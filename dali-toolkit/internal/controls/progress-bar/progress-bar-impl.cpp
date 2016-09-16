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
#include <dali-toolkit/internal/controls/progress-bar/progress-bar-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <sstream>
#include <algorithm>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/public-api/math/math-utils.h>

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
  return Dali::Toolkit::ProgressBar::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ProgressBar, Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "progressValue",          FLOAT,    PROGRESS_VALUE         )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "trackVisual",            MAP,      TRACK_VISUAL           )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "progressVisual",         MAP,      PROGRESS_VISUAL        )
DALI_SIGNAL_REGISTRATION(   Toolkit, ProgressBar, "valueChanged",                     SIGNAL_VALUE_CHANGED   )

DALI_TYPE_REGISTRATION_END()

const char* SKINNED_TRACK_VISUAL = DALI_IMAGE_DIR "slider-skin.9.png";
const char* SKINNED_PROGRESS_VISUAL = DALI_IMAGE_DIR "slider-skin-progress.9.png";

float DEFAULT_VALUE = 0.0f;
float DEFAULT_LOWER_BOUND = 0.0f;
float DEFAULT_UPPER_BOUND = 1.0f;
float DEFAULT_PADDING = 24.0f;

} // Unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// ProgressBar
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::ProgressBar ProgressBar::New()
{
  // Create the implementation
  ProgressBarPtr progressBar( new ProgressBar() );

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::ProgressBar handle( *progressBar );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  progressBar->Initialize();

  return handle;
}

ProgressBar::ProgressBar()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mTrackVisual(""),
  mProgressVisual(""),
  mTrackMap(),
  mTrackVisualSize(),
  mProgressVisualSize(),
  mValue( DEFAULT_VALUE )
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::OnInitialize()
{
  // Setup
  CreateChildren();

  // Properties
  SetTrackVisual( SKINNED_TRACK_VISUAL );
  SetProgressVisual( SKINNED_PROGRESS_VISUAL );

  DisplayValue( mValue, false );       // Run this last to display the correct value
}

void ProgressBar::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Vector2 trackSize( size );
  trackSize.width = std::max( 0.0f, size.width - DEFAULT_PADDING ); // Ensure we don't go negative

  // Track
  if( mTrack )
  {
    container.Add( mTrack, trackSize );

    // mValueTextLabel will have its relayout method called automatically as it's a child of mTrack,
    // which is added to the container
  }

  // Progress bar
  if( mProgress )
  {
    mDomain = CalcDomain( trackSize );

    Vector2 progressSize( trackSize );

    // If no progress, then we do not want a n-patch image shown incorrectly
    progressSize.width = std::max( mProgressVisualSize.width, mDomain.from.x + mValue * ( mDomain.to.x - mDomain.from.x ) );
    progressSize.width = std::min( progressSize.width, trackSize.width ); // We should not exceed given size

    container.Add( mProgress, progressSize );
  }
}

Vector3 ProgressBar::GetNaturalSize()
{
  // Return the maximum width/height combinations of our visuals

  Vector3 naturalSize;
  naturalSize.width = std::max( mTrackVisualSize.width, mProgressVisualSize.width );
  naturalSize.height = std::max( mTrackVisualSize.height, mProgressVisualSize.height );
  return naturalSize;
}

ProgressBar::Domain ProgressBar::CalcDomain( const Vector2& currentSize )
{
   return Domain( Vector2( 0.0f, 0.0f ), currentSize );
}

void ProgressBar::DisplayValue( float value, bool raiseSignals )
{
  // Signals
  if( raiseSignals )
  {
    Toolkit::ProgressBar self = Toolkit::ProgressBar::DownCast( Self() );
    mValueChangedSignal.Emit( self, value );
  }

  // Change the value of the text label
  if( mValueTextLabel )
  {
    std::stringstream ss;
    ss.precision( 0 );
    ss << std::fixed << ( value * 100 ) << "%";

    std::string label = mValueTextLabel.GetProperty<std::string>( Toolkit::TextLabel::Property::TEXT );
    if( label.compare(ss.str()) )
    {
      mValueTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, ss.str() );
    }
  }
}

Toolkit::ImageView ProgressBar::CreateTrack()
{
  Toolkit::ImageView track = Toolkit::ImageView::New();
  track.SetParentOrigin( ParentOrigin::CENTER );
  track.SetAnchorPoint( AnchorPoint::CENTER );
  track.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::ALL_DIMENSIONS );

  return track;
}

void ProgressBar::SetTrackVisual( const std::string& filename )
{
  if( mTrack && filename.size() > 0 )
  {
    mTrack.SetImage( filename );
    mTrackVisual = filename;
    mTrackVisualSize = Vector2::ZERO;
    RelayoutRequest();
  }
}

void ProgressBar::SetTrackVisual( Property::Map map )
{
  bool relayoutRequired = false;

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
        relayoutRequired = true;
      }
    }
  }

  Property::Value* sizeValue = map.Find( "size" );
  if( sizeValue )
  {
    Vector2 size;
    if( sizeValue->Get( size ) )
    {
      mTrackVisualSize = size;
      relayoutRequired = true;
    }
  }

  // Visual and/or visual size changed so we need to relayout
  if( relayoutRequired )
  {
    RelayoutRequest();
  }
}

std::string ProgressBar::GetTrackVisual()
{
  return mTrackVisual;
}

Toolkit::ImageView ProgressBar::CreateProgress()
{
  Toolkit::ImageView progress = Toolkit::ImageView::New();
  progress.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  progress.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  progress.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::ALL_DIMENSIONS );

  return progress;
}

void ProgressBar::SetProgressVisual( const std::string& filename )
{
  if( mProgress && ( filename.size() > 0 ) )
  {
    mProgress.SetImage( filename );
    mProgressVisual = filename;
    mProgressVisualSize = Vector2::ZERO;
    RelayoutRequest();
  }
}

void ProgressBar::SetProgressVisual( Property::Map map )
{
  bool relayoutRequired = false;

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
        relayoutRequired = true;
      }
    }
  }

  Property::Value* sizeValue = map.Find( "size" );
  if( sizeValue )
  {
    Vector2 size;
    if( sizeValue->Get( size ) )
    {
      mProgressVisualSize = size;
      relayoutRequired = true;
    }
  }

  // Visual and/or visual size changed so we need to relayout
  if( relayoutRequired )
  {
    RelayoutRequest();
  }
}

std::string ProgressBar::GetProgressVisual()
{
  return mProgressVisual;
}

Toolkit::ProgressBar::ValueChangedSignalType& ProgressBar::ValueChangedSignal()
{
  return mValueChangedSignal;
}

void ProgressBar::CreateChildren()
{
  Actor self = Self();

  // Track
  mTrack = CreateTrack();
  self.Add( mTrack ); // Needs to be a direct child as we want to manipulate its size

  // Progress bar
  mProgress = CreateProgress();
  self.Add( mProgress ); // Needs to be a direct child as we want to manipulate its size

  // Value Text
  mValueTextLabel = Toolkit::TextLabel::New();
  mValueTextLabel.SetName( "ProgressBarValueTextLabel" );
  mValueTextLabel.SetStyleName( "ProgressBarValueTextLabel" );
  mValueTextLabel.SetParentOrigin( ParentOrigin::CENTER );
  mValueTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
  mValueTextLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  mValueTextLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  mTrack.Add( mValueTextLabel ); // Add to mTrack and let it automatically set its size
}

void ProgressBar::SetProgressValue( float value )
{
  // update the progress bar value (taking float precision errors into account)
  if( ( mValue != value ) &&
      ( ( value >= DEFAULT_LOWER_BOUND ) || ( Equals( value, DEFAULT_LOWER_BOUND ) ) ) &&
      ( ( value <= DEFAULT_UPPER_BOUND ) || ( Equals( value, DEFAULT_UPPER_BOUND ) ) ) )
  {
    mValue = Clamp( value, DEFAULT_LOWER_BOUND, DEFAULT_UPPER_BOUND );
    DisplayValue( mValue, true );
    RelayoutRequest();
  }
}

float ProgressBar::GetProgressValue() const
{
  return mValue;
}

// Static class method to support script connecting signals
bool ProgressBar::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected = true;
  Toolkit::ProgressBar ProgressBar = Toolkit::ProgressBar::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_VALUE_CHANGED ) )
  {
    ProgressBar.ValueChangedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ProgressBar::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::ProgressBar progressBar = Toolkit::ProgressBar::DownCast( Dali::BaseHandle( object ) );

  if ( progressBar )
  {
    ProgressBar& progressBarImpl( GetImpl( progressBar ) );

    switch ( propertyIndex )
    {
      case Toolkit::ProgressBar::Property::PROGRESS_VALUE:
      {
        progressBarImpl.SetProgressValue( value.Get< float >() );
        break;
      }

      case Toolkit::ProgressBar::Property::TRACK_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          progressBarImpl.SetTrackVisual( map );
        }
        break;
      }

      case Toolkit::ProgressBar::Property::PROGRESS_VISUAL:
      {
        Property::Map map;
        if( value.Get( map ) )
        {
          progressBarImpl.SetProgressVisual( map );
        }
        break;
      }
    }
  }
}

Property::Value ProgressBar::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::ProgressBar progressBar = Toolkit::ProgressBar::DownCast( Dali::BaseHandle( object ) );

  if ( progressBar )
  {
    ProgressBar& progressBarImpl( GetImpl( progressBar ) );

    switch ( propertyIndex )
    {
      case Toolkit::ProgressBar::Property::PROGRESS_VALUE:
      {
        value = progressBarImpl.GetProgressValue();
        break;
      }

      case Toolkit::ProgressBar::Property::TRACK_VISUAL:
      {
        if( !progressBarImpl.mTrackVisual.empty() )
        {
          value = progressBarImpl.GetTrackVisual();
        }
        else if( !progressBarImpl.mTrackMap.Empty() )
        {
          value = progressBarImpl.mTrackMap;
        }
        break;
      }

      case Toolkit::ProgressBar::Property::PROGRESS_VISUAL:
      {
        if( !progressBarImpl.mProgressVisual.empty() )
        {
          value = progressBarImpl.GetProgressVisual();
        }
        else if( !progressBarImpl.mProgressMap.Empty() )
        {
          value = progressBarImpl.mProgressMap;
        }
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
