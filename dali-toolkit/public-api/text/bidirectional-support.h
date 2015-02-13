#ifndef __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__
#define __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class LogicalModel;
class VisualModel;

/**
 * Sets the bidirectional info into the logical model.
 *
 * Any bidirectional info previously set is removed.
 *
 * @pre The @p model needs to have a text set.
 * @pre The @p model needs to have the line break info set.
 *
 * @param[in,out] model The text's logical model.
 */
void SetBidirectionalInfo( LogicalModel& model );

/**
 * Sets the visual to logical and logical to visual map tables.
 *
 * Any map tables previously set are removed.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the line break info set.
 * @pre The @p visualModel needs to have the laid-out lines info set.
 *
 * @param[in,out] logicalModel The text's logical model.
 * @param[in] visualModel The text's visual model.
 */
void ReorderLines( LogicalModel& logicalModel,
                   const VisualModel& visualModel );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__
