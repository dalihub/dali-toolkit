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
#include <dali-toolkit/internal/controls/renderers/visual-impl.h>
#include "visual.h"

namespace Dali
{

namespace Toolkit
{

Visual::Visual()
{
}

Visual::~Visual()
{
}

Visual::Visual( const Visual& handle )
: BaseHandle( handle )
{
}

Visual& Visual::operator=( const Visual& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

Visual::Visual(Internal::Visual *impl)
: BaseHandle( impl )
{
}

void Visual::SetSize( const Vector2& size )
{
  GetImplementation( *this ).SetSize( size );
}

const Vector2& Visual::GetSize() const
{
  return GetImplementation( *this ).GetSize();
}

void Visual::GetNaturalSize(Vector2& naturalSize ) const
{
  GetImplementation( *this ).GetNaturalSize( naturalSize );
}

void Visual::SetDepthIndex( float index )
{
  GetImplementation( *this ).SetDepthIndex( index );
}

float Visual::GetDepthIndex() const
{
  return GetImplementation( *this ).GetDepthIndex();
}

void Visual::SetOnStage( Actor& actor )
{
  GetImplementation( *this ).SetOnStage( actor );
}

void Visual::SetOffStage( Actor& actor )
{
  GetImplementation( *this ).SetOffStage( actor );
}

void Visual::RemoveAndReset( Actor& actor )
{
  if( actor && *this )
  {
    SetOffStage( actor );
  }
  Reset();
}

void Visual::CreatePropertyMap( Property::Map& map ) const
{
  GetImplementation( *this ).CreatePropertyMap( map );
}

} // namespace Toolkit

} // namespace Dali
