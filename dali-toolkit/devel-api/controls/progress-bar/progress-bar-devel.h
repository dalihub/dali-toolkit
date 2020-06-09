#ifndef DALI_TOOLKIT_PROGRESS_BAR_DEVEL_H
#define DALI_TOOLKIT_PROGRESS_BAR_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/progress-bar/progress-bar.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelProgressBar
{

/**
 * @brief Enumeration for the style of progress bar.
 */
enum class Style
{
  /**
   * @brief Indicates a linear shape of progress bar.
   */
  LINEAR,

  /**
   * @brief Indicates a circular shape of progress bar.
   */
  CIRCULAR,
};

/**
 * @brief Creates a new ProgressBar with predefined style.
 * @param[in] progressBarStyle A style value that determines the visual properties for ProgressBar.
 * @return A handle to the ProgressBar control.
 */
DALI_TOOLKIT_API Toolkit::ProgressBar New( Style progressBarStyle );

} // namespace DevelProgressBar

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_PROGRESS_BAR_DEVEL_H
