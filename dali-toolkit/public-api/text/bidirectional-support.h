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
#include <dali/public-api/common/dali-vector.h>
#include <dali-toolkit/public-api/text/bidirectional-line-info-run.h>
#include <dali-toolkit/public-api/text/bidirectional-paragraph-info-run.h>

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
 * @param[in] text Vector of UTF-32 characters.
 * @param[in] lineBreakInfo The line break info.
 * @param[out] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 */
void SetBidirectionalInfo( const Vector<Character>& text,
                           const Vector<LineBreakInfo>& lineBreakInfo,
                           Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo );

/**
 * Sets the visual to logical and logical to visual map tables.
 *
 * Any map tables previously set are removed.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the bidirectional info indices for each paragraph set.
 * @pre The @p visualModel needs to have the laid-out lines info set.
 *
 * @param[in] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[in] lineRuns The line runs converted to characters.
 * @param[out] lineInfoRuns line runs with the visual to logical and logical to visual conversion maps.
 */
void ReorderLines( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                   const Vector<CharacterRun>& lineRuns,
                   Vector<BidirectionalLineInfoRun>& lineInfoRuns );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__
