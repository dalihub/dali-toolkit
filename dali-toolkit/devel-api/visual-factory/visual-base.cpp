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
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

// INTERAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

Visual::Base::Base()
{
}

Visual::Base::~Base()
{
}

Visual::Base::Base( const Visual::Base& handle )
: BaseHandle( handle )
{
}

Visual::Base& Visual::Base::operator=( const Visual::Base& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

Visual::Base::Base(Internal::Visual::Base *impl)
: BaseHandle( impl )
{
}

void Visual::Base::SetSize( const Vector2& size )
{
  GetImplementation( *this ).SetSize( size );
}

const Vector2& Visual::Base::GetSize() const
{
  return GetImplementation( *this ).GetSize();
}

void Visual::Base::GetNaturalSize(Vector2& naturalSize ) const
{
  GetImplementation( *this ).GetNaturalSize( naturalSize );
}

void Visual::Base::SetDepthIndex( float index )
{
  GetImplementation( *this ).SetDepthIndex( index );
}

float Visual::Base::GetDepthIndex() const
{
  return GetImplementation( *this ).GetDepthIndex();
}

void Visual::Base::SetOnStage( Actor& actor )
{
  GetImplementation( *this ).SetOnStage( actor );
}

void Visual::Base::SetOffStage( Actor& actor )
{
  GetImplementation( *this ).SetOffStage( actor );
}

void Visual::Base::RemoveAndReset( Actor& actor )
{
  if( actor && *this )
  {
    SetOffStage( actor );
  }
  Reset();
}

void Visual::Base::CreatePropertyMap( Property::Map& map ) const
{
  GetImplementation( *this ).CreatePropertyMap( map );
}

} // namespace Toolkit

} // namespace Dali
