#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include "text-view-processor-types.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

void dbgPrint( const WordLayoutInfo& word );
void dbgPrint( const WordGroupLayoutInfo& group );
void dbgPrint( const LineLayoutInfo& line );
void dbgPrint( const TextLayoutInfo& textInfo );
void dbgPrint( const TextStyle& style );
void dbgPrint( const TextInfoIndices& indices );
void dbgPrint( const MarkupProcessor::StyledTextArray& text );

void dbgPrintText( const WordLayoutInfo& word );

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__
