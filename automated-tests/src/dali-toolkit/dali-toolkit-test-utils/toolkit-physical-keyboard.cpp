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
 *
 */

#include "toolkit-physical-keyboard.h"

#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

/********************************************************************************
 * Stub for Dali::Internal::Adaptor::PhysicalKeyboard
 ********************************************************************************/
namespace Internal
{
namespace Adaptor
{
class PhysicalKeyboard : public BaseObject
{
public: // Creation & Destruction

  PhysicalKeyboard();
  ~PhysicalKeyboard();
  static Dali::PhysicalKeyboard Get();

public:
  bool IsAttached() const;

public: // Signals
  Dali::PhysicalKeyboard::PhysicalKeyboardSignalType& StatusChangedSignal();

private:
  Dali::PhysicalKeyboard::PhysicalKeyboardSignalType mStatusChangedSignal;
  bool mIsAttached;
  static Dali::PhysicalKeyboard mPhysicalKeyboard;
};

Dali::PhysicalKeyboard PhysicalKeyboard::mPhysicalKeyboard;

PhysicalKeyboard::PhysicalKeyboard()
: mIsAttached(true)
{
}

PhysicalKeyboard::~PhysicalKeyboard()
{
}

Dali::PhysicalKeyboard PhysicalKeyboard::Get()
{
  if( ! mPhysicalKeyboard )
  {
    mPhysicalKeyboard = Dali::PhysicalKeyboard(new Internal::Adaptor::PhysicalKeyboard());
  }
  return mPhysicalKeyboard;
}

bool PhysicalKeyboard::IsAttached() const
{
  return mIsAttached;
}

Dali::PhysicalKeyboard::PhysicalKeyboardSignalType& PhysicalKeyboard::StatusChangedSignal()
{
  return mStatusChangedSignal;
}

} // namespace Adaptor
} // namespace Internal


/********************************************************************************
 * Stub for Dali::PhysicalKeyboard
 ********************************************************************************/

PhysicalKeyboard::PhysicalKeyboard(){}
PhysicalKeyboard::~PhysicalKeyboard(){}

PhysicalKeyboard PhysicalKeyboard::Get()
{
  // Get the physical keyboard handle
  PhysicalKeyboard handle = Internal::Adaptor::PhysicalKeyboard::Get();
  return handle;
}

bool PhysicalKeyboard::IsAttached() const
{
  const BaseObject& object = GetBaseObject();
  const Internal::Adaptor::PhysicalKeyboard& pyke = static_cast< const Internal::Adaptor::PhysicalKeyboard& >( object );
  return pyke.IsAttached();
}

PhysicalKeyboard::PhysicalKeyboardSignalType& PhysicalKeyboard::StatusChangedSignal()
{
  BaseObject& object = GetBaseObject();
  Internal::Adaptor::PhysicalKeyboard& pyke = static_cast< Internal::Adaptor::PhysicalKeyboard& >( object );
  return pyke.StatusChangedSignal();
}

PhysicalKeyboard::PhysicalKeyboard( Internal::Adaptor::PhysicalKeyboard *impl )
: BaseHandle(impl)
{
}



} // namespace Dali
