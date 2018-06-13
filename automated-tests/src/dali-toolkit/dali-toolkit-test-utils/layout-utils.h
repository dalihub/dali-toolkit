#ifndef __DALI_TOOLKIT_LAYOUT_UTILS_H__
#define __DALI_TOOLKIT_LAYOUT_UTILS_H__

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

#include <dali-toolkit/dali-toolkit.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates an empty leaf control filled with red color for layout tests
 *
 * @param[in] width The width of the control.
 * @param[in] height The height of the control.
 * @param[out] Control The control.
 */
Control CreateLeafControl( int width, int height );

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_LAYOUT_UTILS_H__
