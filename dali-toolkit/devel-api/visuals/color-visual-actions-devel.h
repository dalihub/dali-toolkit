#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_COLOR_VISUAL_ACTIONS_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_COLOR_VISUAL_ACTIONS_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
namespace DevelColorVisual
{
/**
 * @brief Actions that the color visual can perform. These actions are called through the Visual::Base::DoAction API.
 */
namespace Action
{
/**
 * @brief The available actions for this visual
 */
enum Type
{
  /**
   * @brief Update the properties of the visual.
   */
  UPDATE_PROPERTY = 0
};

} // namespace Action

} // namespace DevelColorVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_COLOR_VISUAL_ACTIONS_DEVEL_H
