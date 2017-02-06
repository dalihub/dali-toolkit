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
#include <dali-toolkit/devel-api/align-enums.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

#include <dali/integration-api/debug.h>

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

DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "progressValue",                     FLOAT,    PROGRESS_VALUE                   )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "secondaryProgressValue",            FLOAT,    SECONDARY_PROGRESS_VALUE         )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "indeterminate",                     BOOLEAN,  INDETERMINATE                    )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "trackVisual",                       MAP,      TRACK_VISUAL                     )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "progressVisual",                    MAP,      PROGRESS_VISUAL                  )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "secondaryProgressVisual",           MAP,      SECONDARY_PROGRESS_VISUAL        )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "indeterminateVisual",               MAP,      INDETERMINATE_VISUAL             )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "indeterminateVisualAnimation",      ARRAY,    INDETERMINATE_VISUAL_ANIMATION   )
DALI_PROPERTY_REGISTRATION( Toolkit, ProgressBar, "labelVisual",                       MAP,      LABEL_VISUAL                     )
DALI_SIGNAL_REGISTRATION(   Toolkit, ProgressBar, "valueChanged",                      SIGNAL_VALUE_CHANGED                       )

DALI_TYPE_REGISTRATION_END()

struct ProgressDepthIndex
{
  // Enum to make sure the visual order
  enum
  {
    TRACK_VISUAL,
    SECONDARY_PROGRESS_VISUAL,
    PROGRESS_VISUAL,
    LABEL_VISUAL,
    INDETERMINATE_VISUAL
  };
};

float DEFAULT_VALUE = 0.0f;
float DEFAULT_LOWER_BOUND = 0.0f;
float DEFAULT_UPPER_BOUND = 1.0f;
float DEFAULT_FONT_SIZE = 12.0f;

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
  mProgressValue( DEFAULT_VALUE ),
  mSecondaryProgressValue( DEFAULT_VALUE ),
  mIndeterminate( false )
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::OnInitialize()
{
}

void ProgressBar::OnRelayout( const Vector2& size, RelayoutContainer& container )
{

  Vector2 trackSize( size );
  mDomain = CalcDomain( size );

  trackSize.width = std::max( 0.0f, size.width ); // Ensure we don't go negative

  Toolkit::Visual::Base trackVisual = GetVisual( Toolkit::ProgressBar::Property::TRACK_VISUAL );
  Toolkit::Visual::Base secondProgressVisual = GetVisual( Toolkit::ProgressBar::Property::SECONDARY_PROGRESS_VISUAL );
  Toolkit::Visual::Base progressVisual = GetVisual( Toolkit::ProgressBar::Property::PROGRESS_VISUAL );
  Toolkit::Visual::Base labelVisual = GetVisual( Toolkit::ProgressBar::Property::LABEL_VISUAL );
  Toolkit::Visual::Base indeterminateVisual = GetVisual( Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL );

  if( trackVisual )
  {
    Property::Map visualTransform;

    visualTransform.Add( Toolkit::DevelVisual::Transform::Property::SIZE, trackSize )
                   .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_SIZE_MODE, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );  // Use Absolute size
    trackVisual.SetTransformAndSize( visualTransform, trackSize );
  }

  if( secondProgressVisual )
  {
    Property::Map visualTransform;

    visualTransform.Add( Toolkit::DevelVisual::Transform::Property::SIZE, Vector2( mDomain.from.x + mSecondaryProgressValue * ( mDomain.to.x - mDomain.from.x ), trackSize.height  ) )
                   .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_SIZE_MODE, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) )  // Use Absolute size
                   .Add( Toolkit::DevelVisual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN )
                   .Add( Toolkit::DevelVisual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN );
    secondProgressVisual.SetTransformAndSize( visualTransform, trackSize );
  }

  if( progressVisual )
  {
    Property::Map visualTransform;

    visualTransform.Add( Toolkit::DevelVisual::Transform::Property::SIZE, Vector2( mDomain.from.x + mProgressValue * ( mDomain.to.x - mDomain.from.x ), trackSize.height ) )
                   .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_SIZE_MODE, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) )  // Use Absolute size
                   .Add( Toolkit::DevelVisual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN )
                   .Add( Toolkit::DevelVisual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN );
    progressVisual.SetTransformAndSize( visualTransform, trackSize );
  }

  if( labelVisual )
  {
    Property::Map visualTransform;

    visualTransform.Add( Toolkit::DevelVisual::Transform::Property::SIZE, trackSize )
                   .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_SIZE_MODE, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );  // Use Absolute size

    labelVisual.SetTransformAndSize( visualTransform, trackSize );
  }

  if( indeterminateVisual )
  {
    Property::Map visualTransform;

    visualTransform.Add( Toolkit::DevelVisual::Transform::Property::SIZE, trackSize )
                   .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_SIZE_MODE, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );  // Use Absolute size
    indeterminateVisual.SetTransformAndSize( visualTransform, trackSize );
  }

}

Vector3 ProgressBar::GetNaturalSize()
{
  // Return the bigger size after comparing trackVisual naturalSize and labelVisual naturalSize
  Toolkit::Visual::Base trackVisual =  GetVisual( Toolkit::ProgressBar::Property::TRACK_VISUAL );
  Toolkit::Visual::Base labelVisual =  GetVisual( Toolkit::ProgressBar::Property::LABEL_VISUAL );

  Size trackSize;
  Size labelSize;

  if ( trackVisual )
  {
    trackVisual.GetNaturalSize( trackSize );
  }
  if ( labelVisual )
  {
    labelVisual.GetNaturalSize( labelSize );
  }

  Vector3 naturalSize;
  naturalSize.width = ( trackSize.width > labelSize.width ) ? trackSize.width: labelSize.width;
  naturalSize.height = ( trackSize.height > labelSize.height ) ? trackSize.height: labelSize.height;

  return naturalSize;
}

ProgressBar::Domain ProgressBar::CalcDomain( const Vector2& currentSize )
{
   return Domain( Vector2( 0.0f, 0.0f ), currentSize );
}

Toolkit::ProgressBar::ValueChangedSignalType& ProgressBar::ValueChangedSignal()
{
  return mValueChangedSignal;
}

void ProgressBar::SetProgressValue( float value )
{
  // update the progress bar value (taking float precision errors into account)
  // TODO : it seems 0.0f cannot into this statement.
  if( ( mProgressValue != value ) &&
      ( ( value >= DEFAULT_LOWER_BOUND ) || ( Equals( value, DEFAULT_LOWER_BOUND ) ) ) &&
      ( ( value <= DEFAULT_UPPER_BOUND ) || ( Equals( value, DEFAULT_UPPER_BOUND ) ) ) )
  {
    mProgressValue = Clamp( value, DEFAULT_LOWER_BOUND, DEFAULT_UPPER_BOUND );

    Toolkit::ProgressBar self = Toolkit::ProgressBar::DownCast( Self() );
    mValueChangedSignal.Emit( self, mProgressValue, mSecondaryProgressValue );
    RelayoutRequest();
  }
}

float ProgressBar::GetProgressValue() const
{
  return mProgressValue;
}

void ProgressBar::SetSecondaryProgressValue( float value )
{
  // update the progress bar value (taking float precision errors into account)
  // TODO : it seems 0.0f cannot into this statement.
  if( ( mSecondaryProgressValue != value ) &&
      ( ( value >= DEFAULT_LOWER_BOUND ) || ( Equals( value, DEFAULT_LOWER_BOUND ) ) ) &&
      ( ( value <= DEFAULT_UPPER_BOUND ) || ( Equals( value, DEFAULT_UPPER_BOUND ) ) ) )
  {
    mSecondaryProgressValue = Clamp( value, DEFAULT_LOWER_BOUND, DEFAULT_UPPER_BOUND );

    Toolkit::ProgressBar self = Toolkit::ProgressBar::DownCast( Self() );
    mValueChangedSignal.Emit( self, mProgressValue, mSecondaryProgressValue );

    RelayoutRequest();
  }
}

float ProgressBar::GetSecondaryProgressValue() const
{
  return mSecondaryProgressValue;
}

void ProgressBar::SetIndeterminate( bool value )
{
  mIndeterminate = value;
  EnableVisual( Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL, mIndeterminate );

  if( mIndeterminate )
  {
    RelayoutRequest();
    if( mIndeterminateVisualTransition )
    {
      PlayIndeterminateVisualTransition();
    }
  }
  else
  {
    if( mIndeterminateVisualAni )
    {
      mIndeterminateVisualAni.Stop();
    }
  }
}

bool ProgressBar::GetIndeterminate() const
{
  return mIndeterminate;
}

void ProgressBar::SetIndeterminateVisualTransition( Toolkit::TransitionData transition )
{
  mIndeterminateVisualTransition = transition;
  if( mIndeterminate )
  {
    PlayIndeterminateVisualTransition();
  }
}

void ProgressBar::PlayIndeterminateVisualTransition()
{
  if( mIndeterminateVisualAni )
  {
    mIndeterminateVisualAni.Stop();
    mIndeterminateVisualAni.Clear();
  }

  mIndeterminateVisualAni = CreateTransition( mIndeterminateVisualTransition );

  if( mIndeterminate && mIndeterminateVisualAni )
  {
    mIndeterminateVisualAni.SetLooping(true);
    mIndeterminateVisualAni.Play();
  }
}

Toolkit::TransitionData ProgressBar::ConvertPropertyToTransition( const Property::Value& value )
{
  Toolkit::TransitionData transitionData;

  if( value.GetType() == Property::ARRAY )
  {
    transitionData = Toolkit::TransitionData::New( *value.GetArray() );
  }
  else if( value.GetType() == Property::MAP )
  {
    transitionData = Toolkit::TransitionData::New( *value.GetMap() );
  }

  return transitionData;
}

/**
 * Create Visual for given index from a property map or url.
 * 1) Check if value passed in is a url and create visual
 * 2) Create visual from map if step (1) is false
 * 3) Register visual with control with false for enable flag.
 * 4) Unregister visual if empty map was provided. This is the method to remove a visual
 */

void ProgressBar::CreateVisualsForComponent( Property::Index index, const Property::Value& value, const float visualDepth )
{
  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base progressVisual;

  std::string imageUrl;
  if( value.Get( imageUrl ) )
  {
    if ( !imageUrl.empty() )
    {
      progressVisual = visualFactory.CreateVisual(  imageUrl, ImageDimensions()  );
    }
  }
  else // Does this code make text-visual can be accepted as visual?
  {
    // if its not a string then get a Property::Map from the property if possible.
    Property::Map *map = value.GetMap();
    if( map && !map->Empty()  ) // Empty map results in current visual removal.
    {
      progressVisual = visualFactory.CreateVisual( *map );
    }
  }

  if ( progressVisual )
  {
    progressVisual.SetDepthIndex( visualDepth );
    if( index == Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL )
    {
      RegisterVisual( index, progressVisual, mIndeterminate );
    }
    else
    {
      RegisterVisual( index, progressVisual, true );
    }
  }
  else
  {
    UnregisterVisual( index );
  }
}

bool ProgressBar::GetPropertyMapForVisual( Property::Index visualIndex, Property::Map& retreivedMap ) const
{
  bool success = false;
  Toolkit::Visual::Base visual = GetVisual( visualIndex );

  if ( visual )
  {
    visual.CreatePropertyMap( retreivedMap );
    success = true;
  }

  return success;
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
      case Toolkit::ProgressBar::Property::TRACK_VISUAL:
      {
        progressBarImpl.CreateVisualsForComponent( propertyIndex, value, ProgressDepthIndex::TRACK_VISUAL );
        break;
      }

      case Toolkit::ProgressBar::Property::SECONDARY_PROGRESS_VISUAL:
      {
        progressBarImpl.CreateVisualsForComponent( propertyIndex, value, ProgressDepthIndex::SECONDARY_PROGRESS_VISUAL );
        break;
      }

      case Toolkit::ProgressBar::Property::PROGRESS_VISUAL:
      {
        progressBarImpl.CreateVisualsForComponent( propertyIndex, value, ProgressDepthIndex::PROGRESS_VISUAL );
        break;
      }

      case Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL:
      {
        progressBarImpl.CreateVisualsForComponent( propertyIndex, value, ProgressDepthIndex::INDETERMINATE_VISUAL );
        break;
      }

      case Toolkit::ProgressBar::Property::LABEL_VISUAL:
      {
        Property::Map map;
        std::string textString;

        if ( value.Get( textString ) )
        {
          // set new text string as TEXT property
          Property::Map newTextMap;
          Toolkit::Visual::Base label = progressBarImpl.GetVisual( Toolkit::ProgressBar::Property::LABEL_VISUAL );

          if( label )
          {
            label.CreatePropertyMap( map );
          }

          // if LABEL_VISUAL doesn't set before, add Visual property "TYPE" to create new text Visual
          if( map.Empty() )
          {
            newTextMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT );
            newTextMap.Add( Toolkit::TextVisual::Property::POINT_SIZE, DEFAULT_FONT_SIZE );
          }
          newTextMap.Add( Toolkit::TextVisual::Property::TEXT, textString );

          map.Merge( newTextMap );
        }
        else
        {
          value.Get( map );
        }

        if( !map.Empty() )
        {
          progressBarImpl.CreateVisualsForComponent( propertyIndex, map, ProgressDepthIndex::LABEL_VISUAL );
        }
        break;
      }

      case Toolkit::ProgressBar::Property::PROGRESS_VALUE:
      {
        progressBarImpl.SetProgressValue( value.Get< float >() );
        break;
      }

      case Toolkit::ProgressBar::Property::SECONDARY_PROGRESS_VALUE:
      {
        progressBarImpl.SetSecondaryProgressValue( value.Get< float >() );
        break;
      }

      case Toolkit::ProgressBar::Property::INDETERMINATE:
      {
        progressBarImpl.SetIndeterminate( value.Get< bool >() );
        break;
      }

      case Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL_ANIMATION:
      {
        progressBarImpl.SetIndeterminateVisualTransition( progressBarImpl.ConvertPropertyToTransition( value ) );
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
      case Toolkit::ProgressBar::Property::TRACK_VISUAL:
      case Toolkit::ProgressBar::Property::PROGRESS_VISUAL:
      case Toolkit::ProgressBar::Property::SECONDARY_PROGRESS_VISUAL:
      case Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL:
      case Toolkit::ProgressBar::Property::LABEL_VISUAL:
      {
        Property::Map visualProperty;
        if ( progressBarImpl.GetPropertyMapForVisual( propertyIndex, visualProperty ) )
        {
          value = visualProperty;
        }
        break;
      }

      case Toolkit::ProgressBar::Property::PROGRESS_VALUE:
      {
        value = progressBarImpl.GetProgressValue();
        break;
      }

      case Toolkit::ProgressBar::Property::SECONDARY_PROGRESS_VALUE:
      {
        value = progressBarImpl.GetSecondaryProgressValue();
        break;
      }

      case Toolkit::ProgressBar::Property::INDETERMINATE:
      {
        value = progressBarImpl.GetIndeterminate();
        break;
      }

      case Toolkit::ProgressBar::Property::INDETERMINATE_VISUAL_ANIMATION:
      {
        //TODO: Return INDETERMINATE_VISUAL_TRANSITION;
      }
    }
  }

  return value;
}

void ProgressBar::OnStageConnection( int depth )
{
  // Chain up first (ensures visuals are ready to draw)
  Control::OnStageConnection( depth );

  if( mIndeterminate )
  {
    PlayIndeterminateVisualTransition();
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
