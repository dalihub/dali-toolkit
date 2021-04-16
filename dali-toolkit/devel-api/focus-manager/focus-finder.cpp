/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "focus-finder.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/focus-manager/focus-finder-impl.h>

namespace Dali
{
namespace Toolkit
{
FocusFinder::FocusFinder()
{
}

FocusFinder::~FocusFinder()
{
}

FocusFinder FocusFinder::Get()
{
  FocusFinder finder;

  // Check whether the focus finder is already created
  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(FocusFinder));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus finder
      finder = FocusFinder(dynamic_cast<Internal::FocusFinder*>(handle.GetObjectPtr()));
    }

    if(!finder)
    {
      // If not, create the focus finder and register it as a singleton
      finder = FocusFinder(new Internal::FocusFinder());
      singletonService.Register(typeid(finder), finder);
    }
  }

  return finder;
}

FocusFinder::FocusFinder(Internal::FocusFinder* impl)
: BaseHandle(impl)
{
}

Actor FocusFinder::GetNearestFocusableActor(Actor focusedActor, Toolkit::Control::KeyboardFocus::Direction direction)
{
  return GetImpl(*this).GetNearestFocusableActor(focusedActor, direction);
}

} // namespace Toolkit

} // namespace Dali
