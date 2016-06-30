#ifndef __DALI_TEST_TOOLKIT_ACCESSIBILITY_ADAPTOR_H__
#define __DALI_TEST_TOOLKIT_ACCESSIBILITY_ADAPTOR_H__

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/accessibility-adaptor.h>
#include <dali/integration-api/events/pan-gesture-event.h>

namespace Test
{
namespace AccessibilityAdaptor
{

void MockSetReadPosition( Dali::AccessibilityAdaptor adaptor, Dali::Vector2& position );
void SetEnabled( Dali::AccessibilityAdaptor adaptor, bool enabled);
void SendPanGesture( Dali::AccessibilityAdaptor adaptor, const Dali::Integration::PanGestureEvent& panEvent );

} // namespace AccessibilityAdaptor
} // namespace Test

#endif //
