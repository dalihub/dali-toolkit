/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/animated-vector-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const Dali::Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );

} // unnamed namespace

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties )
{
  AnimatedVectorImageVisualPtr visual( new AnimatedVectorImageVisual( factoryCache, shaderFactory, imageUrl ) );
  visual->SetProperties( properties );

  return visual;
}

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
{
  AnimatedVectorImageVisualPtr visual( new AnimatedVectorImageVisual( factoryCache, shaderFactory, imageUrl ) );

  return visual;
}

AnimatedVectorImageVisual::AnimatedVectorImageVisual( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
: Visual::Base( factoryCache ),
  mImageVisualShaderFactory( shaderFactory ),
  mUrl( imageUrl ),
  mVectorRasterizeThread( imageUrl.GetUrl() ),
  mVisualSize(),
  mPlacementActor(),
  mActionStatus( DevelAnimatedVectorImageVisual::Action::STOP )
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;

  mVectorRasterizeThread.SetResourceReadyCallback( new EventThreadCallback( MakeCallback( this, &AnimatedVectorImageVisual::OnResourceReady ) ) );
  mVectorRasterizeThread.SetAnimationFinishedCallback( new EventThreadCallback( MakeCallback( this, &AnimatedVectorImageVisual::OnAnimationFinished ) ) );

  mVectorRasterizeThread.Start();
}

AnimatedVectorImageVisual::~AnimatedVectorImageVisual()
{
}

void AnimatedVectorImageVisual::GetNaturalSize( Vector2& naturalSize )
{
  naturalSize = mVisualSize;
}

void AnimatedVectorImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE );
  if( mUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mUrl.GetUrl() );
  }
  map.Insert( Toolkit::DevelImageVisual::Property::LOOP_COUNT, static_cast< int >( mVectorRasterizeThread.GetLoopCount() ) );
  map.Insert( Toolkit::DevelImageVisual::Property::PLAY_RANGE, static_cast< Vector2 >( mVectorRasterizeThread.GetPlayRange() ) );
  map.Insert( Toolkit::DevelImageVisual::Property::PLAY_STATE, static_cast< int >( mVectorRasterizeThread.GetPlayState() ) );
}

void AnimatedVectorImageVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void AnimatedVectorImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url already passed in from constructor
  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else
    {
       if( keyValue.first == LOOP_COUNT_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::LOOP_COUNT, keyValue.second );
       }
       else if( keyValue.first == PLAY_RANGE_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::PLAY_RANGE, keyValue.second );
       }
    }
  }
}

void AnimatedVectorImageVisual::DoSetProperty( Property::Index index, const Property::Value& value )
{
  switch(index)
  {
    case Toolkit::DevelImageVisual::Property::LOOP_COUNT:
    {
      int32_t loopCount;
      if( value.Get( loopCount ) )
      {
        mVectorRasterizeThread.SetLoopCount( loopCount );
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::PLAY_RANGE:
    {
      Vector2 range;
      if( value.Get( range ) )
      {
        mVectorRasterizeThread.SetPlayRange( range );
      }
      break;
    }
  }
}

void AnimatedVectorImageVisual::DoSetOnStage( Actor& actor )
{
  Shader shader;

  if( mImpl->mCustomShader )
  {
    shader = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource() : mImpl->mCustomShader->mVertexShader,
                          mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource() : mImpl->mCustomShader->mFragmentShader,
                          mImpl->mCustomShader->mHints );

    shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  }
  else
  {
    shader = mImageVisualShaderFactory.GetShader( mFactoryCache, false, true );
  }

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  mImpl->mRenderer = Renderer::New( geometry, shader );

  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer.SetTextures( textureSet );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the rasterization is finished.
  mPlacementActor = actor;

  mVectorRasterizeThread.SetRenderer( mImpl->mRenderer );
}

void AnimatedVectorImageVisual::DoSetOffStage( Actor& actor )
{
  mVectorRasterizeThread.PauseAnimation();

  mActionStatus = DevelAnimatedVectorImageVisual::Action::PAUSE;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED );

    actor.RemoveRenderer( mImpl->mRenderer );
    mImpl->mRenderer.Reset();
  }

  mPlacementActor.Reset();

  // Reset the visual size to zero so that when adding the actor back to stage the rasterization is forced
  mVisualSize = Vector2::ZERO;
}

void AnimatedVectorImageVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );

  if( IsOnStage() )
  {
    if( visualSize != mVisualSize )
    {
      mVisualSize = visualSize;

      uint32_t width = static_cast< uint32_t >( visualSize.width );
      uint32_t height = static_cast< uint32_t >( visualSize.height );

      mVectorRasterizeThread.SetSize( width, height );
    }

    if( mActionStatus == DevelAnimatedVectorImageVisual::Action::PLAY )
    {
      mVectorRasterizeThread.PlayAnimation();

      mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY );
    }
    else
    {
      // Render one frame
      mVectorRasterizeThread.RenderFrame();
    }

    if( mVectorRasterizeThread.IsResourceReady() )
    {
      Actor actor = mPlacementActor.GetHandle();
      if( actor )
      {
        actor.AddRenderer( mImpl->mRenderer );
        mPlacementActor.Reset();
      }

      ResourceReady( Toolkit::Visual::ResourceStatus::READY );
    }
  }
}

void AnimatedVectorImageVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelAnimatedVectorImageVisual::Action::PLAY:
    {
      if( IsOnStage() )
      {
        mVectorRasterizeThread.PlayAnimation();

        mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY );
      }
      mActionStatus = DevelAnimatedVectorImageVisual::Action::PLAY;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::PAUSE:
    {
      mVectorRasterizeThread.PauseAnimation();

      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED );
      }

      mActionStatus = DevelAnimatedVectorImageVisual::Action::PAUSE;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::STOP:
    {
      if( mVectorRasterizeThread.GetPlayState() != DevelImageVisual::PlayState::STOPPED )
      {
        mVectorRasterizeThread.StopAnimation();

        OnAnimationFinished();
      }
      mActionStatus = DevelAnimatedVectorImageVisual::Action::STOP;
      break;
    }
  }
}

void AnimatedVectorImageVisual::OnResourceReady()
{
  // If weak handle is holding a placement actor, it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    actor.AddRenderer( mImpl->mRenderer );
    // reset the weak handle so that the renderer only get added to actor once
    mPlacementActor.Reset();
  }

  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void AnimatedVectorImageVisual::OnAnimationFinished()
{
  if( mImpl->mEventObserver )
  {
    mImpl->mEventObserver->NotifyVisualEvent( *this, DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED );
  }

  mActionStatus = DevelAnimatedVectorImageVisual::Action::STOP;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
