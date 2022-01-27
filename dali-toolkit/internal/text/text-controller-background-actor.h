#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_BACKGROUND_ACTOR_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_BACKGROUND_ACTOR_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali::Toolkit::Text
{
class View;

/**
 * @brief Create an actor that renders the text background color
 *
 * @param[in] textView The text view.
 * @param[in] textVisualModel The text visual model.
 * @param[in] textLogicalModel The text logical model.
 * @param[in] textShaderBackground The text shader for background.
 *
 * @return the created actor or an empty handle if no background color needs to be rendered.
 */
Actor CreateControllerBackgroundActor(const View& textView, const VisualModelPtr& textVisualModel, const LogicalModelPtr& textLogicalModel, Shader& textShaderBackground);

} // namespace Dali::Toolkit::Text

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_BACKGROUND_ACTOR_H
