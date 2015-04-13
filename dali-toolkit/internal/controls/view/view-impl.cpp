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
#include "view-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::View::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::View, Toolkit::Control, Create )

DALI_SIGNAL_REGISTRATION( View, "orientation-animation-start", SIGNAL_ORIENTATION_ANIMATION_START )

DALI_TYPE_REGISTRATION_END()

const float ROTATION_ANIMATION_DURATION = 0.5f;

}

Toolkit::View View::New( bool fullscreen )
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< View > internalView = new View(fullscreen);

  // Pass ownership to CustomActor handle
  Toolkit::View view( *internalView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalView->Initialize();

  return view;
}

Layer View::GetContentLayer( unsigned int index ) const
{
  // Returns the layer stored in the layer map.
  Layer layer;

  LayerConstIt it = mContentLayers.find( index );

  if( it != mContentLayers.end() )
  {
    layer = it->second;
  }

  return layer;
}

unsigned int View::AddContentLayer( Layer layer )
{
  // layer must exist.
  DALI_ASSERT_ALWAYS( layer );

  unsigned int index = mNextLayerIndex;
  LayerIt it = FindLayer( layer );

  if( it == mContentLayers.end() )
  {
    // Add layer to the custom actor.
    Self().Add( layer );

    // Store the layer.
    mContentLayers[mNextLayerIndex] = layer;

    // Increase the index.
    ++mNextLayerIndex;
  }

  return index;
}

void View::RemoveContentLayer( Layer layer )
{
  // Check if layer was added in this view.
  LayerIt it = FindLayer( layer );
  if( it != mContentLayers.end() )
  {
    // Remove layer from custom actor.
    Self().Remove( layer );

    // Remove layer from layer map.
    mContentLayers.erase( it );
  }
}

Layer View::GetBackgroundLayer() const
{
  return mBackgroundLayer;
}

void View::SetBackground( ImageActor backgroundImage )
{
  // Create background layer if doesn't exist.

  if( !mBackgroundLayer )
  {
    mBackgroundLayer = Layer::New();

    mBackgroundLayer.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
    mBackgroundLayer.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

    // Add background layer to custom actor.
    Self().Add( mBackgroundLayer );

    // Drop the background layer

    DALI_ASSERT_ALWAYS( mBackgroundLayer.OnStage() ); // We need to be on-stage to drop the layer
    mBackgroundLayer.LowerToBottom();
  }
  else
  {
    // It removes the old background
    if( 0 < mBackgroundLayer.GetChildCount() )
    {
      mBackgroundLayer.Remove( mBackgroundLayer.GetChildAt(0) );
    }
  }

  backgroundImage.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
  backgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  backgroundImage.SetSizeScalePolicy( FILL_WITH_ASPECT_RATIO );
  mBackgroundLayer.Add( backgroundImage );

  RelayoutRequest();
}

void View::SetOrientationFunction( Degree portrait, Degree landscale, Degree portraitInverse, Degree landscapeInverse )
{
  mOrientationFunction[View::PORTRAIT] = portrait;
  mOrientationFunction[View::LANDSCAPE] = landscale;
  mOrientationFunction[View::PORTRAIT_INVERSE] = portraitInverse;
  mOrientationFunction[View::LANDSCAPE_INVERSE] = landscapeInverse;
}

void View::OrientationChanged( Dali::Orientation orientation )
{
  Actor self = Self();

  // Nothing to do if orientation doesn't really change.
  if ( orientation.GetDegrees() == mOrientation || !mAutoRotateEnabled )
  {
    return;
  }

  mOrientation = orientation.GetDegrees();

  // has parent so we expect it to be on stage
  mRotateAnimation = Animation::New( ROTATION_ANIMATION_DURATION );
  mRotateAnimation.AnimateTo( Property( self, Actor::Property::ORIENTATION ), Quaternion( -orientation.GetRadians(), Vector3::ZAXIS ), AlphaFunctions::EaseOut );

  // Resize the view
  if( mFullScreen )
  {
    const Vector2& stageSize( Stage::GetCurrent().GetSize() );
    const Vector3& currentSize( self.GetCurrentSize() );

    float minSize = std::min( stageSize.width, stageSize.height );
    float maxSize = std::max( stageSize.width, stageSize.height );

    Vector3 targetSize;
    View::Orientation viewOrientation = DegreeToViewOrientation( Degree( orientation.GetDegrees() ) );
    switch( viewOrientation )
    {
      case View::PORTRAIT:          // Fallthrough
      case View::PORTRAIT_INVERSE:
        targetSize = Vector3( minSize, maxSize, currentSize.depth );
        break;
      case View::LANDSCAPE:         // Fallthrough
      case View::LANDSCAPE_INVERSE:
        targetSize = Vector3( maxSize, minSize, currentSize.depth );
        break;
      default:
        DALI_ASSERT_ALWAYS( false );
    }

    // if we linearly resize from portrait to landscape halfway through the animation
    // we get size which is square between the both. This would cause a square image to grow
    // if it is fitted to be 100% of view size. Therefore we do a nonlinear size animation
    // where we shrink faster
    // which one grows
    if( targetSize.width > currentSize.width )
    {
      // width grows, shrink height faster
      Vector3 shrink( currentSize );shrink.height = targetSize.height;
      mRotateAnimation.AnimateTo( Property( self, Actor::Property::SIZE ), shrink, AlphaFunctions::EaseOut, TimePeriod( 0.0f, ROTATION_ANIMATION_DURATION * 0.5f ) );
      mRotateAnimation.AnimateTo( Property( self, Actor::Property::SIZE ), targetSize, AlphaFunctions::EaseIn, TimePeriod( 0.0f, ROTATION_ANIMATION_DURATION ) );
    }
    else
    {
      // height grows, shrink width faster
      Vector3 shrink( currentSize );shrink.width = targetSize.width;
      mRotateAnimation.AnimateTo( Property( self, Actor::Property::SIZE ), shrink, AlphaFunctions::EaseOut, TimePeriod( 0.0f, ROTATION_ANIMATION_DURATION * 0.5f ) );
      mRotateAnimation.AnimateTo( Property( self, Actor::Property::SIZE ), targetSize, AlphaFunctions::EaseIn, TimePeriod( 0.0f, ROTATION_ANIMATION_DURATION ) );
    }
  }

  Toolkit::View handle( GetOwner() );
  mOrientationAnimationStartedSignal.Emit( handle, mRotateAnimation, orientation );

  mRotateAnimation.Play();
}

void View::SetAutoRotate( bool enabled )
{
  mAutoRotateEnabled = enabled;
}

Toolkit::View::OrientationAnimationStartedSignalType& View::OrientationAnimationStartedSignal()
{
  return mOrientationAnimationStartedSignal;
}

bool View::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::View view = Toolkit::View::DownCast(handle);

  if( 0 == strcmp( signalName.c_str(), SIGNAL_ORIENTATION_ANIMATION_START ) )
  {
    view.OrientationAnimationStartedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

View::View(bool fullscreen)
: Control( CONTROL_BEHAVIOUR_NONE ),
  mOrientation( -1 ),
  mFullScreen(fullscreen),
  mContentLayers(),
  mNextLayerIndex( 0 ),
  mOrientationFunction(),
  mAutoRotateEnabled( true )
{
  mOrientationFunction[View::PORTRAIT] = 0.f;
  mOrientationFunction[View::LANDSCAPE] = 90.f;
  mOrientationFunction[View::PORTRAIT_INVERSE] = 180.f;
  mOrientationFunction[View::LANDSCAPE_INVERSE] =  270.f;
}

View::~View()
{
}

void View::OnInitialize()
{
  Self().SetAnchorPoint( AnchorPoint::CENTER );
  Self().SetParentOrigin( ParentOrigin::CENTER );

  if( mFullScreen )
  {
    Self().SetSize( Stage::GetCurrent().GetSize() );
  }
}

View::Orientation View::DegreeToViewOrientation( Degree degree )
{
  View::Orientation orientation = PORTRAIT;

  if( fabsf( mOrientationFunction[PORTRAIT] - degree ) <= GetRangedEpsilon( mOrientationFunction[PORTRAIT], degree ) )
  {
    orientation =  PORTRAIT;
  }
  else if( fabsf( mOrientationFunction[LANDSCAPE] - degree ) <= GetRangedEpsilon( mOrientationFunction[LANDSCAPE], degree ) )
  {
    orientation = LANDSCAPE;
  }
  else if( fabsf( mOrientationFunction[PORTRAIT_INVERSE] - degree ) <= GetRangedEpsilon( mOrientationFunction[PORTRAIT_INVERSE], degree ) )
  {
    orientation = PORTRAIT_INVERSE;
  }
  else if( fabsf( mOrientationFunction[LANDSCAPE_INVERSE] - degree ) <= GetRangedEpsilon( mOrientationFunction[LANDSCAPE_INVERSE], degree ) )
  {
    orientation = LANDSCAPE_INVERSE;
  }

  return orientation;
}

View::LayerIt View::FindLayer( Layer layer )
{
  for( LayerIt it = mContentLayers.begin(); it != mContentLayers.end(); ++it )
  {
    if(layer == it->second)
    {
      return it;
    }
  }

  return mContentLayers.end();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
