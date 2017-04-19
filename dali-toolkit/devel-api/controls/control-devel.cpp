/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "control-devel.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelControl
{

ResourceReadySignalType&  ResourceReadySignal( Control& control )
{

  Internal::Control& internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlImpl = Internal::Control::Impl::Get( internalControl );

  return controlImpl.mResourceReadySignal;
}

bool IsResourceReady( const Control& control )
{
  const Internal::Control& internalControl = Toolkit::Internal::GetImplementation(control);
  const Internal::Control::Impl& controlImpl = Internal::Control::Impl::Get( internalControl );

  return controlImpl.IsResourceReady();
}


} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali
