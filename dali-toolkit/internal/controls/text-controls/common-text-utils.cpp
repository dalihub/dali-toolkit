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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/public-api/actors/layer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/hidden-text.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace Dali::Toolkit::Internal
{
void CommonTextUtils::SynchronizeTextAnchorsInParent(
  Actor                             parent,
  Text::ControllerPtr               controller,
  std::vector<Toolkit::TextAnchor>& anchorActors)
{
  for(auto& anchorActor : anchorActors)
  {
    parent.Remove(anchorActor);
  }
  if(Dali::Accessibility::IsUp())
  {
    controller->GetAnchorActors(anchorActors);
    for(auto& anchorActor : anchorActors)
    {
      parent.Add(anchorActor);
    }
  }
}

void CommonTextUtils::RenderText(
  Actor                             textActor,
  Text::RendererPtr                 renderer,
  Text::ControllerPtr               controller,
  Text::DecoratorPtr                decorator,
  float&                            alignmentOffset,
  Actor&                            renderableActor,
  Actor&                            backgroundActor,
  Actor&                            cursorLayerActor,
  Toolkit::Control&                 stencil,
  std::vector<Actor>&               clippingDecorationActors,
  std::vector<Toolkit::TextAnchor>& anchorActors,
  Text::Controller::UpdateTextType  updateTextType)
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

    if(cursorLayerActor)
    {
      cursorLayerActor.RaiseToTop();
    }

    SynchronizeTextAnchorsInParent(textActor, controller, anchorActors);
  }
}

std::size_t TextControlAccessible::GetCharacterCount() const
{
  return GetWholeText().size();
}

std::size_t TextControlAccessible::GetCursorOffset() const
{
  return 0u;
}

Rect<> TextControlAccessible::GetRangeExtents(std::size_t startOffset, std::size_t endOffset, Accessibility::CoordinateType type)
{
  if(!ValidateRange(GetWholeText(), startOffset, endOffset))
  {
    return {0, 0, 0, 0};
  }

  auto rect    = GetTextController()->GetTextBoundingRectangle(startOffset, endOffset - 1);
  auto extents = GetExtents(type);

  rect.x += extents.x;
  rect.y += extents.y;

  return rect;
}

Accessibility::Range TextControlAccessible::GetRangeOfSelection(std::size_t selectionIndex) const
{
  // Since DALi supports only one selection, indices other than 0 are ignored
  if(selectionIndex > 0)
  {
    return {};
  }

  auto indices     = GetTextController()->GetSelectionIndexes();
  auto startOffset = static_cast<std::size_t>(indices.first);
  auto endOffset   = static_cast<std::size_t>(indices.second);
  auto text        = GetText(startOffset, endOffset);

  return {startOffset, endOffset, text};
}

std::string TextControlAccessible::GetText(std::size_t startOffset, std::size_t endOffset) const
{
  auto text = GetWholeText();

  if(!ValidateRange(text, startOffset, endOffset))
  {
    return {};
  }

  if(IsHiddenInput())
  {
    std::uint32_t substituteCharacterUtf32 = GetSubstituteCharacter();
    std::string   substituteCharacterUtf8;
    std::string   substituteText;

    Toolkit::Text::Utf32ToUtf8(&substituteCharacterUtf32, 1, substituteCharacterUtf8);

    while(substituteText.length() < endOffset - startOffset)
    {
      substituteText.append(substituteCharacterUtf8);
    }

    return substituteText;
  }

  return text.substr(startOffset, endOffset - startOffset);
}

Accessibility::Range TextControlAccessible::GetTextAtOffset(std::size_t offset, Accessibility::TextBoundary boundary) const
{
  Accessibility::Range range{};

  if(IsHiddenInput())
  {
    // Returning empty object, as there is no possibility to parse the textfield
    // when its content is hidden.
    return range;
  }

  auto text     = GetWholeText();
  auto textSize = text.size();

  switch(boundary)
  {
    case Dali::Accessibility::TextBoundary::CHARACTER:
    {
      if(offset < textSize)
      {
        range.content     = text[offset];
        range.startOffset = offset;
        range.endOffset   = offset + 1;
      }
      break;
    }

    case Dali::Accessibility::TextBoundary::WORD:
    case Dali::Accessibility::TextBoundary::LINE:
    {
      std::vector<char> breaks(textSize, '\0');

      if(boundary == Dali::Accessibility::TextBoundary::WORD)
      {
        TextAbstraction::Segmentation::Get().GetWordBreakPositionsUtf8(reinterpret_cast<const uint8_t*>(text.c_str()), textSize, breaks.data());
      }
      else
      {
        TextAbstraction::Segmentation::Get().GetLineBreakPositionsUtf8(reinterpret_cast<const uint8_t*>(text.c_str()), textSize, breaks.data());
      }

      std::size_t index   = 0u;
      std::size_t counter = 0u;

      while(index < textSize && counter <= offset)
      {
        auto start = index;
        if(breaks[index])
        {
          while(breaks[index])
          {
            index++;
          }
          counter++;
        }
        else
        {
          if(boundary == Dali::Accessibility::TextBoundary::WORD)
          {
            index++;
          }
          if(boundary == Dali::Accessibility::TextBoundary::LINE)
          {
            counter++;
          }
        }

        if((counter > 0) && ((counter - 1) == offset))
        {
          range.content     = text.substr(start, index - start + 1);
          range.startOffset = start;
          range.endOffset   = index + 1;
        }

        if(boundary == Dali::Accessibility::TextBoundary::LINE)
        {
          index++;
        }
      }
      break;
    }

    case Dali::Accessibility::TextBoundary::SENTENCE:  // Not supported by default
    case Dali::Accessibility::TextBoundary::PARAGRAPH: // Not supported by libunibreak library
    default:
    {
      break;
    }
  }

  return range;
}

bool TextControlAccessible::RemoveSelection(std::size_t selectionIndex)
{
  // Since DALi supports only one selection, indices other than 0 are ignored
  if(selectionIndex > 0)
  {
    return false;
  }

  GetTextController()->SetSelection(0, 0);

  return true;
}

bool TextControlAccessible::SetCursorOffset(std::size_t offset)
{
  return false;
}

bool TextControlAccessible::SetRangeOfSelection(std::size_t selectionIndex, std::size_t startOffset, std::size_t endOffset)
{
  // Since DALi supports only one selection, indices other than 0 are ignored
  if(selectionIndex > 0)
  {
    return false;
  }

  // Lack of ValidateRange() is intentional

  GetTextController()->SetSelection(startOffset, endOffset);

  return true;
}

Accessibility::Hyperlink* TextControlAccessible::GetLink(std::int32_t linkIndex) const
{
  if(linkIndex < 0 || linkIndex >= GetLinkCount())
  {
    return nullptr;
  }

  auto anchor = GetTextAnchors()[linkIndex];

  return Accessibility::Hyperlink::DownCast(Accessibility::Accessible::Get(anchor));
}

std::int32_t TextControlAccessible::GetLinkCount() const
{
  return static_cast<std::int32_t>(GetTextAnchors().size());
}

std::int32_t TextControlAccessible::GetLinkIndex(std::int32_t characterOffset) const
{
  return GetTextController()->GetAnchorIndex(static_cast<std::size_t>(characterOffset));
}

std::string TextControlAccessible::GetWholeText() const
{
  std::string text;

  GetTextController()->GetText(text);

  return text;
}

std::uint32_t TextControlAccessible::GetSubstituteCharacter() const
{
  return Toolkit::Text::STAR;
}

bool TextControlAccessible::IsHiddenInput() const
{
  return false;
}

bool TextControlAccessible::ValidateRange(const std::string& string, std::size_t begin, std::size_t end)
{
  auto size = string.size();

  if(end <= begin || begin >= size || end > size)
  {
    return false;
  }

  // TODO: Check whether the range [begin, end) describes a valid substring:
  // 1. It does not break multi-byte UTF-8 sequences.
  // 2. It does not break graphemes (compound emojis, glyphs with combining characters etc.).

  return true;
}

Accessibility::States EditableTextControlAccessible::CalculateStates()
{
  using Dali::Accessibility::State;

  auto states       = DevelControl::ControlAccessible::CalculateStates();
  auto focusControl = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();

  states[State::EDITABLE]  = true;
  states[State::FOCUSABLE] = true;
  states[State::FOCUSED]   = (Self() == focusControl);

  return states;
}

std::size_t EditableTextControlAccessible::GetCursorOffset() const
{
  return GetTextController()->GetCursorPosition();
}

bool EditableTextControlAccessible::SetCursorOffset(std::size_t offset)
{
  if(offset > GetCharacterCount())
  {
    return false;
  }

  GetTextController()->ResetCursorPosition(offset);
  RequestTextRelayout();

  return true;
}

bool EditableTextControlAccessible::CopyText(std::size_t startPosition, std::size_t endPosition)
{
  auto text = GetWholeText();

  if(!ValidateRange(text, startPosition, endPosition))
  {
    return false;
  }

  GetTextController()->CopyStringToClipboard(text.substr(startPosition, endPosition - startPosition));

  return true;
}

bool EditableTextControlAccessible::CutText(std::size_t startPosition, std::size_t endPosition)
{
  if(!CopyText(startPosition, endPosition))
  {
    return false;
  }

  return DeleteText(startPosition, endPosition);
}

bool EditableTextControlAccessible::DeleteText(std::size_t startPosition, std::size_t endPosition)
{
  auto text = GetWholeText();

  if(!ValidateRange(text, startPosition, endPosition))
  {
    return false;
  }

  return SetTextContents(std::move(text.erase(startPosition, endPosition - startPosition)));
}

bool EditableTextControlAccessible::InsertText(std::size_t startPosition, std::string newText)
{
  auto text = GetWholeText();

  if(!ValidateRange(text, startPosition, startPosition + 1) && !(startPosition == text.size()))
  {
    return false;
  }

  return SetTextContents(std::move(text.insert(startPosition, newText)));
}

bool EditableTextControlAccessible::SetTextContents(std::string newContents)
{
  GetTextController()->SetText(std::move(newContents));

  return true;
}

} // namespace Dali::Toolkit::Internal
