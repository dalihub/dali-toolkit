#ifndef __DALI_TOOLKIT_INTERNAL_RELAYOUT_HELPER_H__
#define __DALI_TOOLKIT_INTERNAL_RELAYOUT_HELPER_H__

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
#include <dali/dali.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace RelayoutHelper
{

/**
 * Gets the natural size of the given actor.
 *
 * If the actor is a Control, it returns the natural size of the Control. @see Control::GetNaturalSize()
 * If the actor is an ImageActor, it returns the size of the image.
 * If the actor is a TextActor, it returns the size of the text. @see Font::MeasureText()
 * Otherwise it returns the actor's current size. @see Actor::GetCurrentSize()
 *
 * @param[in] actor The actor.
 *
 * @return The natural size.
 */
Vector3 GetNaturalSize( Actor actor );

/**
 * Gets the actor's height for the given width.
 *
 * If the actor is a Control it returns the height for width. @see Control::GetHeightForWidth()
 * If the actor is an ImageActor or a TextActor it scales the natural size to fit the given width. @see GetNaturalSize()
 * Otherwise it returns the actor's current size. @see Actor::GetCurrentSize()
 *
 * @param[in] actor The actor.
 * @param[in] width The width.
 *
 * @return The actor's height for given width.
 */
float GetHeightForWidth( Actor actor, float width );

} // namespace RelayoutHelper

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_RELAYOUT_HELPER_H__
