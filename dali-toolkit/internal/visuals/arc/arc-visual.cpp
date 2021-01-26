/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/arc/arc-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// cap
DALI_ENUM_TO_STRING_TABLE_BEGIN( CAP )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelArcVisual::Cap, BUTT )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelArcVisual::Cap, ROUND )
DALI_ENUM_TO_STRING_TABLE_END( CAP )

}

ArcVisualPtr ArcVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  ArcVisualPtr arcVisualPtr( new ArcVisual( factoryCache ) );
  arcVisualPtr->SetProperties( properties );
  return arcVisualPtr;
}

ArcVisual::ArcVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, static_cast<Toolkit::Visual::Type>( Toolkit::DevelVisual::ARC ) ),
  mThickness( 0.0f ),
  mRadius( 0.0f ),
  mStartAngle( 0.0f ),
  mSweepAngle( 360.0f ),
  mRadiusIndex( Property::INVALID_INDEX ),
  mThicknessIndex( Property::INVALID_INDEX ),
  mStartAngleIndex( Property::INVALID_INDEX ),
  mSweepAngleIndex( Property::INVALID_INDEX ),
  mCapType( DevelArcVisual::Cap::BUTT )
{
}

ArcVisual::~ArcVisual()
{
}

void ArcVisual::DoSetProperties( const Property::Map& propertyMap )
{
  Property::Value* thicknessValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::THICKNESS, THICKNESS_NAME );
  if( thicknessValue )
  {
    if( !thicknessValue->Get( mThickness ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: THICKNESS property has incorrect type: %d\n", thicknessValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mThicknessIndex, mThickness );

        // Need to calculate radius again
        OnSetTransform();
      }
    }
  }

  Property::Value* startAngleValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::START_ANGLE, START_ANGLE_NAME );
  if( startAngleValue )
  {
    if( !startAngleValue->Get( mStartAngle ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: START_ANGLE property has incorrect type: %d\n", startAngleValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mStartAngleIndex, mStartAngle );
      }
    }
  }

  Property::Value* sweepAngleValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::SWEEP_ANGLE, SWEEP_ANGLE_NAME );
  if( sweepAngleValue )
  {
    if( !sweepAngleValue->Get( mSweepAngle ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: SWEEP_ANGLE property has incorrect type: %d\n", sweepAngleValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mSweepAngleIndex, mSweepAngle );
      }
    }
  }

  Property::Value* capValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::CAP, CAP_NAME );
  if( capValue )
  {
    int capType = 0;
    Scripting::GetEnumerationProperty( *capValue, CAP_TABLE, CAP_TABLE_COUNT, capType );
    mCapType = Toolkit::DevelArcVisual::Cap::Type( capType );
  }
}

void ArcVisual::DoSetOnScene( Actor& actor )
{
  InitializeRenderer();

  actor.AddRenderer( mImpl->mRenderer );

  // Arc Visual generated and ready to display
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void ArcVisual::DoSetOffScene(Actor& actor)
{
  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    mThickness  = mImpl->mRenderer.GetProperty<float>(mThicknessIndex);
    mStartAngle = mImpl->mRenderer.GetProperty<float>(mStartAngleIndex);
    mSweepAngle = mImpl->mRenderer.GetProperty<float>(mSweepAngleIndex);
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mImpl->mRenderer.Reset();

  mThicknessIndex  = Property::INVALID_INDEX;
  mStartAngleIndex = Property::INVALID_INDEX;
  mSweepAngleIndex = Property::INVALID_INDEX;
}

void ArcVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  float thickness, startAngle, sweepAngle;
  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    thickness  = mImpl->mRenderer.GetProperty<float>(mThicknessIndex);
    startAngle = mImpl->mRenderer.GetProperty<float>(mStartAngleIndex);
    sweepAngle = mImpl->mRenderer.GetProperty<float>(mSweepAngleIndex);
  }
  else
  {
    thickness  = mThickness;
    startAngle = mStartAngle;
    sweepAngle = mSweepAngle;
  }

  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ARC );
  map.Insert(Toolkit::DevelArcVisual::Property::THICKNESS, thickness);
  map.Insert(Toolkit::DevelArcVisual::Property::START_ANGLE, startAngle);
  map.Insert(Toolkit::DevelArcVisual::Property::SWEEP_ANGLE, sweepAngle);
  map.Insert( Toolkit::DevelArcVisual::Property::CAP, mCapType );
}

void ArcVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void ArcVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );
  mRadius = ( std::min( visualSize.width, visualSize.height ) - mThickness ) / 2.0f;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( mRadiusIndex, mRadius );
  }
}

void ArcVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelArcVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if( map )
      {
        DoSetProperties( *map );
      }
      break;
    }
  }
}

void ArcVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  Shader shader;
  if( mCapType == DevelArcVisual::Cap::BUTT )
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::ARC_BUTT_CAP_SHADER );
    if( !shader )
    {
      shader = Shader::New( Dali::Shader::GetVertexShaderPrefix() + SHADER_ARC_VISUAL_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_ARC_VISUAL_BUTT_CAP_SHADER_FRAG.data() );
      mFactoryCache.SaveShader( VisualFactoryCache::ARC_BUTT_CAP_SHADER, shader );
    }
  }
  else
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::ARC_ROUND_CAP_SHADER );
    if( !shader )
    {
      shader = Shader::New( Dali::Shader::GetVertexShaderPrefix() + SHADER_ARC_VISUAL_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_ARC_VISUAL_ROUND_CAP_SHADER_FRAG.data() );
      mFactoryCache.SaveShader( VisualFactoryCache::ARC_ROUND_CAP_SHADER, shader );
    }
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  mThicknessIndex  = mImpl->mRenderer.RegisterProperty(DevelArcVisual::Property::THICKNESS, THICKNESS_NAME, mThickness);
  mStartAngleIndex = mImpl->mRenderer.RegisterProperty(DevelArcVisual::Property::START_ANGLE, START_ANGLE_NAME, mStartAngle);
  mSweepAngleIndex = mImpl->mRenderer.RegisterProperty(DevelArcVisual::Property::SWEEP_ANGLE, SWEEP_ANGLE_NAME, mSweepAngle);

  mRadiusIndex = mImpl->mRenderer.RegisterProperty( RADIUS_NAME, mRadius );

  mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
