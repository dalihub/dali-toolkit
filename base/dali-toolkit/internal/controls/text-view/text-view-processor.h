#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_H__

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
#include "text-view-processor-types.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/**
 * Creates a data structure with info to layout the text, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] text The given styled text.
 * @param[in] layoutParameters Layout configuration.
 * @param[out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 */
void CreateTextInfo( const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData );

/**
 *  Updates the layout data structures with the new inserted text.
 *
 * @note Does nothing if text has no characters.
 * @note It asserts if position is bigger than the number of characters of the whole text.
 *
 * @param[in] position Where the text is going to be inserted.
 * @param[in] text New styled text to be inserted.
 * @param[in] layoutParameters Layout configuration.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 */
void UpdateTextInfo( std::size_t position,
                     const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData );

/**
 *  Updates the layout data structures by removing text.
 *
 * @note Does nothing if number of characters to be deleted is zero.
 * @note It asserts if trying to delete text out of bounds.
 *
 * @param[in] position Position of the first character to be removed.
 * @param[in] numberOfCharacters The number of characters to be removed.
 * @param[in] layoutParameters Layout configuration.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 * @param[in] clearText Whether to clear text-actor's text before insert the text-actors into the cache.
 */
void UpdateTextInfo( std::size_t position,
                     std::size_t numberOfCharacters,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData,
                     TextOperationOnRemove clearText );

/**
 * Updates the layout data structures by replacing text.
 *
 * @param[in] position Position of the first character to be replaced.
 * @param[in] numberOfCharacters The number of characters to be replaced.
 * @param[in] text The new styled text.
 * @param[in] layoutParameters Layout configuration.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 */
void UpdateTextInfo( std::size_t position,
                     std::size_t numberOfCharacters,
                     const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData );

/**
 * Updates the layout data structure by modifying the space between lines.
 *
 * @param[in] lineHeightOffset The new space between lines.
 * @param[in,out] textLayoutInfo Layout info for all groups of characters, words, groups of words, lines and the whole text.
 */
void UpdateTextInfo( float lineHeightOffset,
                     TextLayoutInfo& textLayoutInfo );

/**
 * Updates the text-actor data structures by replacing the style.
 *
 * @note This operation is called only if the new style doesn't modify the metrics. Consequently it doesn't modify any size info.
 *
 * @param[in] style The new style.
 * @param[in] mask The style mask.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 */
void UpdateTextInfo( const TextStyle& style,
                     TextStyle::Mask mask,
                     TextView::RelayoutData& relayoutData );

/**
 * Traverse all text initializing all non initialized text-actor handles.
 *
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 */
void InitializeTextActorInfo( TextView::RelayoutData& relayoutData );

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_H__
