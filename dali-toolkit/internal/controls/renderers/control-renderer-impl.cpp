/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "control-renderer-impl.h"

// EXTERNAL HEADER
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

//INTERNAL HEARDER
#include <dali-toolkit/internal/controls/renderers/control-renderer-data-impl.h>

namespace
{
//custom shader
const char * const CUSTOM_SHADER( "shader" );
const char * const CUSTOM_VERTEX_SHADER( "vertexShader" );
const char * const CUSTOM_FRAGMENT_SHADER( "fragmentShader" );
const char * const CUSTOM_SUBDIVIDE_GRID_X( "subdivideGridX" );
const char * const CUSTOM_SUBDIVIDE_GRID_Y( "subdivideGridY" );
const char * const CUSTOM_SHADER_HINTS( "hints" ); ///< type INTEGER; (bitfield) values from enum Shader::Hints
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ControlRenderer::ControlRenderer( RendererFactoryCache& factoryCache )
: mImpl( new Impl() ),
  mFactoryCache( factoryCache )
{
}

ControlRenderer::~ControlRenderer()
{
  delete mImpl;
}

void ControlRenderer::SetCustomShader( const Property::Map& shaderMap )
{
  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->SetPropertyMap( shaderMap );
  }
  else
  {
   mImpl->mCustomShader = new Impl::CustomShader( shaderMap );
  }
}

void ControlRenderer::Initialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* customShaderValue = propertyMap.Find( CUSTOM_SHADER );
  if( customShaderValue )
  {
    Property::Map shaderMap;
    if( customShaderValue->Get( shaderMap ) )
    {
      SetCustomShader( shaderMap );
    }
  }

  DoInitialize( actor, propertyMap );
}

void ControlRenderer::SetSize( const Vector2& size )
{
  mImpl->mSize = size;
}

const Vector2& ControlRenderer::GetSize() const
{
  return mImpl->mSize;
}

void ControlRenderer::GetNaturalSize( Vector2& naturalSize ) const
{
  naturalSize = Vector2::ZERO;
}

void ControlRenderer::SetClipRect( const Rect<int>& clipRect )
{
}

void ControlRenderer::SetOffset( const Vector2& offset )
{
  mImpl->mOffset = offset;
}

void ControlRenderer::SetDepthIndex( float index )
{
  mImpl->mDepthIndex = index;
  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex );
  }
}

float ControlRenderer::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void ControlRenderer::SetOnStage( Actor& actor )
{
  DoSetOnStage( actor );

  mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
  mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex );
  actor.AddRenderer( mImpl->mRenderer );
  mImpl->mFlags |= Impl::IS_ON_STAGE;
}

void ControlRenderer::SetOffStage( Actor& actor )
{
  if( GetIsOnStage() )
  {
    DoSetOffStage( actor );

    mImpl->mFlags &= ~Impl::IS_ON_STAGE;
  }
}

void ControlRenderer::EnablePreMultipliedAlpha( bool preMultipled )
{
  if(preMultipled)
  {
    mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Impl::IS_PREMULTIPLIED_ALPHA;
  }

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, preMultipled);
  }
}

bool ControlRenderer::IsPreMultipliedAlphaEnabled() const
{
  return mImpl->mFlags & Impl::IS_PREMULTIPLIED_ALPHA;
}

void ControlRenderer::DoSetOnStage( Actor& actor )
{
}

void ControlRenderer::DoSetOffStage( Actor& actor )
{
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

void ControlRenderer::CreatePropertyMap( Property::Map& map ) const
{
  DoCreatePropertyMap( map );

  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->CreatePropertyMap( map );
  }
}

bool ControlRenderer::GetIsOnStage() const
{
  return mImpl->mFlags & Impl::IS_ON_STAGE;
}

bool ControlRenderer::GetIsFromCache() const
{
  return mImpl->mFlags & Impl::IS_FROM_CACHE;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
