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

#include <dali/devel-api/adaptor-framework/virtual-keyboard.h>
#include <dali/public-api/adaptor-framework/key.h>

using namespace Dali;

namespace Dali
{
class Adaptor;

namespace Internal
{
namespace Adaptor
{

namespace VirtualKeyboard
{
Dali::VirtualKeyboard::StatusSignalType gKeyboardStatusSignal;
}

} // Adaptor
} // Internal


bool IsKey( const Dali::KeyEvent& keyEvent, Dali::KEY daliKey)
{
  return true;
}

namespace VirtualKeyboard
{

void ApplySettings( Dali::Property::Map const& )
{
}

bool IsVisible()
{
  return false;
}

void Show()
{
}

StatusSignalType& StatusChangedSignal()
{
  return Internal::Adaptor::VirtualKeyboard::gKeyboardStatusSignal;
}

} // VirtualKeyboard
} // Dali
