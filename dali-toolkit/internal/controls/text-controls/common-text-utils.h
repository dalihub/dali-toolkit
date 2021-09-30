#ifndef DALI_TOOLKIT_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H
#define DALI_TOOLKIT_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H

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
 */
#include <dali-toolkit/devel-api/controls/text-controls/text-anchor-devel.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>
#include <dali/public-api/actors/actor.h>

#include <dali/public-api/common/vector-wrapper.h>

namespace Dali::Toolkit::Internal
{
class CommonTextUtils
{
public:
  /**
   * Common method to render text, setting up background, foreground actors with decorators/stencil.
   * @param[in] textActor The TextEditor or TextField
   * @param[in] renderer pointer to the text renderer
   * @param[in] controller pointer to the text controller
   * @param[in] decorator pointer to the text decorator
   * @param[in,out] alignmentOffset Alignment offset
   * @param[in,out] renderableActor Actor for rendering text
   * @param[in,out] backgroundActor Actor for rendering background
   * @param[in,out] stencil Clipping actor
   * @param[in,out] clippingDecorationActors Clipping decoration actors
   * @param[in,out] anchorActors Anchor actors
   * @param[in] updateTextType How the text has been updated
   */
  static void RenderText(
    Actor                            textActor,
    Text::RendererPtr                renderer,
    Text::ControllerPtr              controller,
    Text::DecoratorPtr               decorator,
    float&                           alignmentOffset,
    Actor&                           renderableActor,
    Actor&                           backgroundActor,
    Toolkit::Control&                stencil,
    std::vector<Actor>&              clippingDecorationActors,
    std::vector<Toolkit::TextAnchor>& anchorActors,
    Text::Controller::UpdateTextType updateTextType);

  /**
   * Common method to synchronize TextAnchor actors with Anchor objects in text's logical model.
   * @param[in] parent The actor that is a parent of anchor actors
   * @param[in] controller pointer to the text controller
   * @param[in,out] anchorActors Anchor actors
   */
  static void SynchronizeTextAnchorsInParent(
    Actor                             parent,
    Text::ControllerPtr               controller,
    std::vector<Toolkit::TextAnchor>& anchorActors);
};

} // namespace Dali::Toolkit::Internal

#endif //DALI_TOOLKIT_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H
