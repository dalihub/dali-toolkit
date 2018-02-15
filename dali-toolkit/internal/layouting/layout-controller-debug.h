#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_DEBUG_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_DEBUG_H

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

#include <dali/public-api/actors/actor.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

#if defined(DEBUG_ENABLED)

extern void LayoutDebugMeasureState( Actor root );
extern void LayoutDebugAfterLayout( Actor root );


#define LAYOUT_DEBUG_MEASURE_STATES( root ) \
  LayoutDebugMeasureState( root )

#define LAYOUT_DEBUG_AFTER_LAYOUT( root ) \
  LayoutDebugAfterLayout( root )

#else


#define LAYOUT_DEBUG_MEASURE_STATES( root )
#define LAYOUT_DEBUG_AFTER_LAYOUT( root )

#endif

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_DEBUG_H
