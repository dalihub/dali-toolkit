#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/toolkit-action-index-ranges.h>
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali/public-api/signals/callback.h>
#include <string>

namespace Dali
{
namespace Toolkit
{
namespace DevelAnimatedVectorImageVisual
{
/**
 * @brief Actions that the animated vector image visual can perform.  These actions are called through the Visual::Base::DoAction API.
 */
namespace Action
{
/**
 * @brief The available actions for this visual
 */
enum Type
{
  // Shared actions with AnimatedImageVisual
  PLAY    = DevelAnimatedImageVisual::Action::PLAY,    ///< Play the animated vector image.
  PAUSE   = DevelAnimatedImageVisual::Action::PAUSE,   ///< Pause the animated vector image.
  STOP    = DevelAnimatedImageVisual::Action::STOP,    ///< Stop the animated vector image. This is also Default playback mode.
  JUMP_TO = DevelAnimatedImageVisual::Action::JUMP_TO, ///< Jump to the specified frame. Property::INTEGER value should be passed.

  // AnimatedVectorImageVisual only actions
  SET_DYNAMIC_PROPERTY = DevelAnimatedImageVisual::Action::ANIMATED_IMAGE_VISUAL_ACTION_END_INDEX ///< Set the dynamic property.
};

} // namespace Action

/**
 * @brief The dynamic property info
 *
 * @note A callback of the following type may be used:
 * @code
 *   Property::Value MyFunction(int32_t id, VectorAnimationRenderer::VectorProperty property, uint32_t frameNumber);
 * @endcode
 *
 * id  The id to specify the callback.
 * property The property that represent what you are trying to change.
 * frameNumber The current frame number.
 * It returns a Property::Value to set according to the property type.
 *
 * The callback will be called on the worker thread. You MUST not call other DALi methods in the callback.
 * And the object must still be alive when the callback occurs if you make the callback from a class member function.
 */
struct DynamicPropertyInfo
{
  int32_t       id;       ///< The Id to specify the callback. It should be unique and will be passed when the callback is called.
  std::string   keyPath;  ///< The key path used to target a specific content or a set of contents that will be updated.
  int32_t       property; ///< The property to set.
  CallbackBase* callback; ///< The callback that gets called every time the animation is rendered. Ownership of the callback is passed onto the visual.
};

} // namespace DevelAnimatedVectorImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H
