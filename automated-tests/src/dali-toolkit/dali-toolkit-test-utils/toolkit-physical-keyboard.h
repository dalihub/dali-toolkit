#ifndef __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__
#define __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class PhysicalKeyboard;
}
}

class PhysicalKeyboard : public BaseHandle
{
public:
  typedef SignalV2< void (PhysicalKeyboard) > Signal;
  PhysicalKeyboard();
  virtual ~PhysicalKeyboard();
  static PhysicalKeyboard Get();
  bool IsAttached() const;
  Signal& StatusChangedSignal();
  PhysicalKeyboard( Internal::Adaptor::PhysicalKeyboard* impl );
};


} // namespace Dali

#endif // __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__
