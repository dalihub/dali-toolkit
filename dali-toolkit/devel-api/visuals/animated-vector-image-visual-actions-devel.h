#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
  PLAY,            ///< Play the animated vector image.
  PAUSE,           ///< Pause the animated vector image.
  STOP,            ///< Stop the animated vector image. This is also Default playback mode.
  JUMP_TO,         ///< Jump to the specified frame. Property::FLOAT value should be passed.
  UPDATE_PROPERTY  ///< Update the properties of the animated vector image.
};

} // namespace Action

} // namespace DevelAnimatedVectorImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_ANIMATED_VECTOR_IMAGE_VISUAL_ACTIONS_DEVEL_H
