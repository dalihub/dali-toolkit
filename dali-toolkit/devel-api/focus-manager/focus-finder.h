#ifndef DALI_TOOLKIT_FOCUS_FINDER_H
#define DALI_TOOLKIT_FOCUS_FINDER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace FocusFinder
{

/**
 * Get the nearest focusable actor.
 * @param [in] rootActor The root actor.
 * @param [in] focusedActor The current focused actor.
 * @param [in] direction The direction.
 * @return The nearest focusable actor, or an empty handle if none exists.
 */
DALI_TOOLKIT_API Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor, Toolkit::Control::KeyboardFocus::Direction direction);

} // namespace FocusFinder

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_FOCUS_FINDER_H
