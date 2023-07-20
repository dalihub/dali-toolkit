#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_IMAGE_VISUAL_ACTIONS_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_IMAGE_VISUAL_ACTIONS_DEVEL_H

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
#include <dali-toolkit/devel-api/toolkit-action-index-ranges.h>
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelAnimatedImageVisual
{
/**
 * @brief Actions that the animated image visual can perform.  These actions are called through the Visual::Base::DoAction API.
 */
namespace Action
{
/**
 * @brief The available actions for this visual
 */
enum Type
{
  PLAY = DevelImageVisual::Action::IMAGE_VISUAL_ACTION_END_INDEX, ///< Play the animated GIF. This is also Default playback mode.
  PAUSE,                                                          ///< Pause the animated GIF.
  STOP,                                                           ///< Stop the animated GIF.
  JUMP_TO,                                                        ///< Jump to the specified frame. Property::INTEGER value should be passed.

  ANIMATED_IMAGE_VISUAL_ACTION_END_INDEX = DevelImageVisual::Action::IMAGE_VISUAL_ACTION_END_INDEX + 1000  ///< End of animated image visual action index.
};

} // namespace Action

} // namespace DevelAnimatedImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_IMAGE_VISUAL_ACTIONS_DEVEL_H
