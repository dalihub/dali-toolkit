#ifndef __DALI_TOOLKIT_INTERNAL_UTIL_H__
#define __DALI_TOOLKIT_INTERNAL_UTIL_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali/dali.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace Util
{

/**
 * @brief Callback that the EventProcessingFinishedSignal signal calls
 *
 * Priorities calls to managers
 */
inline void EventProcessingFinishedSignalPrioritizer()
{
  // Priority 0: Set all styles
  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  if( GetImpl(styleManager).IsThemeRequestPending() )
  {
    GetImpl(styleManager).SetTheme();
  }

  // Todo: Priority 1: Do relayout after styles have been set
}

/**
 * @brief Connect to the EventProcessingFinishedSignal
 *
 * Needs to be called only once, but will still operate successfully if called multiple times.
 * Makes the assumption that this is the only thing connecting to the EventProcessingFinishedSignal.
 */
inline void ConnectEventProcessingFinishedSignal()
{
  Stage stage = Stage::GetCurrent();

  // This is only intended for one purpose!
  if( stage.EventProcessingFinishedSignal().GetConnectionCount() == 0 )
  {
    stage.EventProcessingFinishedSignal().Connect( &EventProcessingFinishedSignalPrioritizer );
  }
}

} // namespace Util

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_STYLE_MANAGER_H__

