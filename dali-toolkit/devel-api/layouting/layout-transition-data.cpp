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
 */

#include <dali-toolkit/devel-api/layouting/layout-transition-data.h>
#include <dali-toolkit/internal/layouting/layout-transition-data-impl.h>

namespace Dali
{
namespace Toolkit
{

LayoutTransitionData::LayoutTransitionData()
: BaseHandle()
{
}

LayoutTransitionData::LayoutTransitionData( Internal::LayoutTransitionData* layoutAnimationData )
: BaseHandle( layoutAnimationData )
{
}

LayoutTransitionData LayoutTransitionData::New()
{
  Internal::LayoutTransitionDataPtr layoutAnimationData = Internal::LayoutTransitionData::New();
  return LayoutTransitionData( layoutAnimationData.Get() );
}

LayoutTransitionData LayoutTransitionData::DownCast( BaseHandle handle )
{
  return LayoutTransitionData( dynamic_cast<Internal::LayoutTransitionData*>(handle.GetObjectPtr()) );
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map )
{
  GetImplementation( *this ).AddPropertyAnimator( actor, map );
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation )
{
  GetImplementation( *this ).AddPropertyAnimator( actor, map, keyFrames, interpolation );
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward )
{
  GetImplementation( *this ).AddPropertyAnimator( actor, map, path, forward );
}

LayoutTransitionData::LayoutTransitionSignalType& LayoutTransitionData::FinishedSignal()
{
  return GetImplementation(*this ).FinishedSignal();
}

} // namespace Toolkit
} // namespace Dali
