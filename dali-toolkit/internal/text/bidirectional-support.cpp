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

// FILE HEADER
#include <dali-toolkit/internal/text/bidirectional-support.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void SetBidirectionalInfo( const Vector<Character>& text,
                           const Vector<LineBreakInfo>& lineBreakInfo,
                           Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo )
{
}

void ReplaceBidirectionalInfo( LogicalModel& model,
                               CharacterIndex characterIndex,
                               Length numberOfCharactersToRemove,
                               Length numberOfCharactersToInsert )
{
}

void ReorderLines( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                   const Vector<CharacterRun>& lineRuns,
                   Vector<BidirectionalLineInfoRun>& lineInfoRuns )
{
}

void ReorderLines( LogicalModel& logicalModel,
                   const VisualModel& visualModel,
                   CharacterIndex characterIndex,
                   Length numberOfCharactersToRemove,
                   Length numberOfCharactersToInsert )
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
