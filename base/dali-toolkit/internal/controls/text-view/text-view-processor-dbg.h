#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__

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
#include <dali-toolkit/public-api/markup-processor/markup-processor.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

//Forward declarations.
class TextStyle;

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

//Forward declarations.
struct WordLayoutInfo;
struct ParagraphLayoutInfo;
struct TextLayoutInfo;
struct TextInfoIndices;

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gTextViewProcessorLogFilter;
#endif

void dbgPrint( const WordLayoutInfo& word );
void dbgPrint( const ParagraphLayoutInfo& paragraph );
void dbgPrint( const TextLayoutInfo& textInfo );
void dbgPrint( const TextStyle& style );
void dbgPrint( const TextInfoIndices& indices );
void dbgPrint( const MarkupProcessor::StyledTextArray& text );

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_DBG_H__
