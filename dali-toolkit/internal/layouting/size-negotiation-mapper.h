#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_SIZE_NEGOTIATION_MAPPER_IMPL_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_SIZE_NEGOTIATION_MAPPER_IMPL_H

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
 */

// EXTERNAL HEADERS
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using LayoutItemPtr = IntrusivePtr<LayoutItem>;
/**
 *  @brief Maps ResizePolicies used in SizeNegotiation to the Layout system
 *
 */
namespace SizeNegotiationMapper
{
/**
 * @brief Sets the child layout parameters on the control by mapping it's ResizePolicy to a MeasureSpecification
 * @param[out] control the control to set the layout params on
 * @param[out] layout the layout for the given control
 * @param[in] dimension the dimension or dimensions the resize policy applies to.
 * @note If Dimension::ALL_DIMENSIONS is provided as the dimension parameter then Dimension::WIDTH is used for
 *       both height and width mapping.
 */
void SetLayoutParametersUsingResizePolicy( Toolkit::Control control, Toolkit::Internal::LayoutItemPtr layout, const Dimension::Type dimension );

/**
 * @brief Sets the child layout parameters on the control using the a ResizePolicy that is dependant on it's parent
 * @param[out] control the control to set the layout params on
 * @param[in] parentWidthSpecification the parent's width measure specification
 * @param[in] parentHeightSpecification the parent's height measure specification
 * @param[out] childWidth the resulting child width
 * @param[out] childHeight the resulting child height
 * @note This method should be used after a child has been parented and a parent measure spec is available.
 */
void GetSizeofChildForParentDependentResizePolicy( Toolkit::Control control, const MeasureSpec parentWidthSpecification, const MeasureSpec parentHeightSpecification, LayoutLength& childWidth, LayoutLength& childHeight );

} // namespace SizeNegotiationMapper

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_SIZE_NEGOTIATION_MAPPER_IMPL_H