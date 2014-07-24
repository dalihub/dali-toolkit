#ifndef __DALI_TOOLKIT_INTERNAL_SPLIT_BY_WORD_POLICIES_H__
#define __DALI_TOOLKIT_INTERNAL_SPLIT_BY_WORD_POLICIES_H__

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

// INTERNAL INCLUDES
#include "text-view-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace SplitByWord
{

/**
 * Sets text-actor's size and position accordingly with the given text-view's size and layout parameters.
 * Visible text-actors are added to the text-view. Non visible actors are not added.
 *
 * @param[in] textView The handle to the text-view actor.
 * @param[in] relayoutOperationMask Mask which defines which operations need to be done in the relayout process.
 * @param[in] layoutParameters The layout parameters.
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in] relayoutData The text-view's data structures which are modified by this function.
 */
void Relayout( Actor textView,
               TextView::RelayoutOperationMask relayoutOperationMask,
               const TextView::LayoutParameters& layoutParameters,
               const TextView::VisualParameters& visualParameters,
               TextView::RelayoutData& relayoutData );

} // namespace SplitByWord

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SPLIT_BY_WORD_POLICIES_H__
