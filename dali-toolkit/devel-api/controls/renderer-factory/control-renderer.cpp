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
#include "control-renderer.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>

namespace Dali
{

namespace Toolkit
{

ControlRenderer::ControlRenderer()
{
}

ControlRenderer::~ControlRenderer()
{
}

ControlRenderer::ControlRenderer( const ControlRenderer& handle )
: BaseHandle( handle )
{
}

ControlRenderer& ControlRenderer::operator=( const ControlRenderer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

ControlRenderer::ControlRenderer(Internal::ControlRenderer *impl)
: BaseHandle(impl)
{
}

void ControlRenderer::SetSize( const Vector2& size )
{
  GetImplementation( *this ).SetSize(size);
}

const Vector2& ControlRenderer::GetSize() const
{
  return GetImplementation( *this ).GetSize();
}

void ControlRenderer::GetNaturalSize(Vector2& naturalSize ) const
{
  GetImplementation( *this ).GetNaturalSize( naturalSize );
}

void ControlRenderer::SetDepthIndex( float index )
{
  GetImplementation( *this ).SetDepthIndex(index);
}

float ControlRenderer::GetDepthIndex() const
{
  return GetImplementation( *this ).GetDepthIndex();
}

void ControlRenderer::SetOnStage( Actor& actor )
{
  GetImplementation( *this ).SetOnStage(actor);
}

void ControlRenderer::SetOffStage( Actor& actor )
{
  GetImplementation( *this ).SetOffStage(actor);
}

void ControlRenderer::RemoveAndReset( Actor& actor )
{
  if( actor && *this )
  {
    SetOffStage( actor );
  }
  Reset();
}

void ControlRenderer::CreatePropertyMap( Property::Map& map ) const
{
  GetImplementation( *this ).CreatePropertyMap( map );
}

} // namespace Toolkit

} // namespace Dali
