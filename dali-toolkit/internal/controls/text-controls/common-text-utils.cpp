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

#include <dali/public-api/actors/layer.h>

#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace Dali::Toolkit::Internal
{
void CommonTextUtils::RenderText(
  Actor                            textActor,
  Text::RendererPtr                renderer,
  Text::ControllerPtr              controller,
  Text::DecoratorPtr               decorator,
  float&                           alignmentOffset,
  Actor&                           renderableActor,
  Actor&                           backgroundActor,
  Toolkit::Control&                stencil,
  std::vector<Actor>&              clippingDecorationActors,
  Text::Controller::UpdateTextType updateTextType)
{
  Actor newRenderableActor;

  if(Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType))
  {
    if(renderer)
    {
      newRenderableActor = renderer->Render(controller->GetView(),
                                            textActor,
                                            Property::INVALID_INDEX, // Animatable property not supported
                                            alignmentOffset,
                                            DepthIndex::CONTENT);
    }

    if(renderableActor != newRenderableActor)
    {
      UnparentAndReset(backgroundActor);
      UnparentAndReset(renderableActor);
      renderableActor = newRenderableActor;

      if(renderableActor)
      {
        backgroundActor = controller->CreateBackgroundActor();
      }
    }
  }

  if(renderableActor)
  {
    const Vector2& scrollOffset = controller->GetTextModel()->GetScrollPosition();

    float renderableActorPositionX, renderableActorPositionY;

    if(stencil)
    {
      renderableActorPositionX = scrollOffset.x + alignmentOffset;
      renderableActorPositionY = scrollOffset.y;
    }
    else
    {
      Extents padding;
      padding = textActor.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

      // Support Right-To-Left of padding
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(textActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      renderableActorPositionX = scrollOffset.x + alignmentOffset + padding.start;
      renderableActorPositionY = scrollOffset.y + padding.top;
    }

    renderableActor.SetProperty(Actor::Property::POSITION, Vector2(renderableActorPositionX, renderableActorPositionY));

    // Make sure the actors are parented correctly with/without clipping
    Actor self = stencil ? stencil : textActor;

    Actor highlightActor;

    for(std::vector<Actor>::iterator it    = clippingDecorationActors.begin(),
                                     endIt = clippingDecorationActors.end();
        it != endIt;
        ++it)
    {
      self.Add(*it);
      it->LowerToBottom();

      if(it->GetProperty<std::string>(Dali::Actor::Property::NAME) == "HighlightActor")
      {
        highlightActor = *it;
      }
    }
    clippingDecorationActors.clear();

    self.Add(renderableActor);

    if(backgroundActor)
    {
      if(decorator && decorator->IsHighlightVisible())
      {
        self.Add(backgroundActor);
        backgroundActor.SetProperty(Actor::Property::POSITION, Vector2(renderableActorPositionX, renderableActorPositionY)); // In text field's coords.
        backgroundActor.LowerBelow(highlightActor);
      }
      else
      {
        renderableActor.Add(backgroundActor);
        backgroundActor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f)); // In renderable actor's coords.
        backgroundActor.LowerToBottom();
      }
    }
  }
}

} // namespace Dali::Toolkit::Internal
