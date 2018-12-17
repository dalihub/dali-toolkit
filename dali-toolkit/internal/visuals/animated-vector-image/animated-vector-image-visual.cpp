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
#include <dali/devel-api/common/stage-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const Dali::Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );
constexpr auto LOOP_FOREVER = -1;

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
: Visual::Base( factoryCache, Visual::FittingMode::FILL ),
  mImageVisualShaderFactory( shaderFactory ),
  mUrl( imageUrl ),
  mVisualSize(),
  mPlayRange( 0.0f, 1.0f ),
  mPlacementActor(),
  mVectorRasterizeThread(),
  mLoopCount( LOOP_FOREVER ),
  mActionStatus( DevelAnimatedVectorImageVisual::Action::STOP )
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
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
  map.Insert( Toolkit::DevelImageVisual::Property::LOOP_COUNT, static_cast< int >( mLoopCount ) );
  map.Insert( Toolkit::DevelImageVisual::Property::PLAY_RANGE, static_cast< Vector2 >( mPlayRange ) );
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
        mLoopCount = loopCount;
        if( mVectorRasterizeThread )
        {
          mVectorRasterizeThread->SetLoopCount( loopCount );
        }
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::PLAY_RANGE:
    {
      Vector2 range;
      if( value.Get( range ) )
      {
        // Make sure the range specified is between 0.0 and 1.0
        if( range.x >= 0.0f && range.x <= 1.0f && range.y >= 0.0f && range.y <= 1.0f )
        {
          Vector2 orderedRange( range );
          // If the range is not in order swap values
          if( range.x > range.y )
          {
            orderedRange = Vector2( range.y, range.x );
          }

          mPlayRange = orderedRange;

          if( mVectorRasterizeThread )
          {
            mVectorRasterizeThread->SetPlayRange( mPlayRange );
          }
        }
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

  // This visual needs it's size set before it can be rasterized hence set ResourceReady once on stage
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void AnimatedVectorImageVisual::DoSetOffStage( Actor& actor )
{
  if( mVectorRasterizeThread )
  {
    mVectorRasterizeThread->PauseAnimation();
    DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::IF_REQUIRED );
    mActionStatus = DevelAnimatedVectorImageVisual::Action::PAUSE;
  }

  if( mImpl->mRenderer )
  {
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

      if( !mVectorRasterizeThread )
      {
        uint32_t width = static_cast< uint32_t >( visualSize.width );
        uint32_t height = static_cast< uint32_t >( visualSize.height );

        mVectorRasterizeThread = std::unique_ptr< VectorRasterizeThread >( new VectorRasterizeThread( mUrl.GetUrl(), mImpl->mRenderer, width, height ) );

        mVectorRasterizeThread->SetResourceReadyCallback( new EventThreadCallback( MakeCallback( this, &AnimatedVectorImageVisual::OnResourceReady ) ) );
        mVectorRasterizeThread->SetLoopCount( mLoopCount );
        mVectorRasterizeThread->SetPlayRange( mPlayRange );

        mVectorRasterizeThread->Start();

        if( mActionStatus == DevelAnimatedVectorImageVisual::Action::PLAY )
        {
          mVectorRasterizeThread->StartAnimation();
          DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::CONTINUOUSLY );
        }
        else
        {
          // Render one frame
          mVectorRasterizeThread->RenderFrame();
        }
      }
      else
      {
        uint32_t width = static_cast< uint32_t >( visualSize.width );
        uint32_t height = static_cast< uint32_t >( visualSize.height );

        mVectorRasterizeThread->SetSize( width, height );
      }
    }
  }
}

void AnimatedVectorImageVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  if( actionId == mActionStatus )
  {
    return;
  }

  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelAnimatedVectorImageVisual::Action::PLAY:
    {
      if( IsOnStage())
      {
        if( mVectorRasterizeThread )
        {
          mVectorRasterizeThread->StartAnimation();
          DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::CONTINUOUSLY );   //TODO: Should manage this globally
        }
      }
      mActionStatus = DevelAnimatedVectorImageVisual::Action::PLAY;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::PAUSE:
    {
      if( mVectorRasterizeThread )
      {
        mVectorRasterizeThread->PauseAnimation();
        DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::IF_REQUIRED );
      }
      mActionStatus = DevelAnimatedVectorImageVisual::Action::PAUSE;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::RESUME:
    {
      if( mVectorRasterizeThread )
      {
        mVectorRasterizeThread->ResumeAnimation();
        DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::CONTINUOUSLY );
      }
      mActionStatus = DevelAnimatedVectorImageVisual::Action::RESUME;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::STOP:
    {
      if( mVectorRasterizeThread )
      {
        mVectorRasterizeThread->StopAnimation();
        DevelStage::SetRenderingBehavior( Stage::GetCurrent(), DevelStage::Rendering::IF_REQUIRED );
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

    Stage::GetCurrent().KeepRendering( 0.0f );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
