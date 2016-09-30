/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/visuals/transition-data-impl.h>

namespace Dali
{
namespace Toolkit
{

TransitionData::TransitionData()
{
}

TransitionData::~TransitionData()
{
}

TransitionData TransitionData::New( const Property::Array& transition )
{
  Internal::TransitionDataPtr transitionData = Internal::TransitionData::New( transition );
  return TransitionData( transitionData.Get() );
}

TransitionData TransitionData::New( const Property::Map& transition )
{
  Internal::TransitionDataPtr transitionData = Internal::TransitionData::New( transition );
  return TransitionData( transitionData.Get() );
}

TransitionData TransitionData::DownCast( BaseHandle handle )
{
  return TransitionData( dynamic_cast<Dali::Toolkit::Internal::TransitionData*>(handle.GetObjectPtr()));
}

TransitionData::TransitionData( const TransitionData& handle )
: BaseHandle( handle )
{
}

TransitionData& TransitionData::operator=( const TransitionData& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

size_t TransitionData::Count() const
{
  return GetImplementation( *this ).Count();
}

Property::Map TransitionData::GetAnimatorAt( size_t index )
{
  return GetImplementation( *this ).GetAnimatorAt( index );
}

TransitionData::TransitionData( Internal::TransitionData* pointer )
: BaseHandle( pointer )
{
}

} // namespace Toolkit
} // namespace Dali
