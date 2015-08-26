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

//INTERNAL HEARDER
#include <dali-toolkit/internal/controls/renderers/control-renderer-data-impl.h>

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

void ControlRenderer::SetSize( const Vector2& size )
{
  mImpl->mSize = size;
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

void ControlRenderer::SetOnStage( Actor& actor )
{
  Material material = Material::New( mImpl->mShader );
  mImpl->mRenderer = Renderer::New( mImpl->mGeometry, material );
  mImpl->mRenderer.SetDepthIndex( mImpl->mDepthIndex );
  actor.AddRenderer( mImpl->mRenderer );
}

void ControlRenderer::SetOffStage( Actor& actor )
{
  actor.RemoveRenderer( mImpl->mRenderer );
  mImpl->mRenderer.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
