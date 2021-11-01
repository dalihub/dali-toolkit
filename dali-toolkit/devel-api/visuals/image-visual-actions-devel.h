#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_ACTIONS_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_ACTIONS_DEVEL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Toolkit
{
namespace DevelImageVisual
{
/**
 * @brief Actions that the image visual can perform.  These actions are called through the Visual::Base::DoAction API.
 */
namespace Action
{
/**
 * @brief The available actions for this visual
 */
enum Type
{
  RELOAD = VISUAL_ACTION_START_INDEX ///< Force reloading of the image, all visuals using this image will get the latest one.
};

} // namespace Action

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_ACTIONS_DEVEL_H
