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
const char * const CUSTOM_VERTEX_SHADER( "vertex-shader" );
const char * const CUSTOM_FRAGMENT_SHADER( "fragment-shader" );
const char * const CUSTOM_SUBDIVIDE_GRID_X( "subdivide-grid-x" );
const char * const CUSTOM_SUBDIVIDE_GRID_Y( "subdivide-grid-y" );
const char * const CUSTOM_SHADER_HINTS( "hints" ); ///< type INTEGER; (bitfield) values from enum Shader::Hints
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ControlRenderer::ControlRenderer()
: mImpl( new Impl() )
{
}

ControlRenderer::~ControlRenderer()
{
  delete mImpl;
}

void ControlRenderer::Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap )
{
  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->SetPropertyMap( propertyMap );
  }
  else
  {
    Property::Value* customShaderValue = propertyMap.Find( CUSTOM_SHADER );
    if( customShaderValue )
    {
      Property::Map customShader;
      if( customShaderValue->Get( customShader ) )
      {
        mImpl->mCustomShader = new Impl::CustomShader( propertyMap );
      }
    }
  }
  DoInitialize( factoryCache, propertyMap );
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
  mImpl->mClipRect = clipRect;
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
    mImpl->mRenderer.SetDepthIndex( mImpl->mDepthIndex );
  }
}

float ControlRenderer::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void ControlRenderer::SetOnStage( Actor& actor )
{
  Material material = Material::New( mImpl->mShader );
  mImpl->mRenderer = Renderer::New( mImpl->mGeometry, material );
  mImpl->mRenderer.SetDepthIndex( mImpl->mDepthIndex );
  actor.AddRenderer( mImpl->mRenderer );
  mImpl->mIsOnStage = true;

  DoSetOnStage( actor );
}

void ControlRenderer::SetOffStage( Actor& actor )
{
  if( mImpl->mIsOnStage )
  {
    DoSetOffStage( actor );

    actor.RemoveRenderer( mImpl->mRenderer );
    mImpl->mRenderer.Reset();

    mImpl->mIsOnStage = false;
  }
}

void ControlRenderer::DoSetOnStage( Actor& actor )
{
}

void ControlRenderer::DoSetOffStage( Actor& actor )
{
}

void ControlRenderer::CreatePropertyMap( Property::Map& map ) const
{
  if( mImpl->mCustomShader )
  {
    mImpl->mCustomShader->CreatePropertyMap( map );
  }
  DoCreatePropertyMap( map );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
