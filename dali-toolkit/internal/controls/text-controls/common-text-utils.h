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

// EXTERNAL INCLUDES
#include <dali/devel-api/atspi-interfaces/editable-text.h>
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-accessible.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-anchor-devel.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

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
    Actor&                           cursorLayerActor,
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

class TextControlAccessible : public DevelControl::ControlAccessible,
                              public virtual Dali::Accessibility::Text,
                              public virtual Dali::Accessibility::Hypertext
{
public:
  using DevelControl::ControlAccessible::ControlAccessible;

  // Text

  /**
   * @copydoc Dali::Accessibility::Text::GetCharacterCount()
   */
  std::size_t GetCharacterCount() const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetCursorOffset()
   */
  std::size_t GetCursorOffset() const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetRangeExtents()
   */
  Rect<> GetRangeExtents(std::size_t startOffset, std::size_t endOffset, Accessibility::CoordinateType type) override;

  /**
   * @copydoc Dali::Accessibility::Text::GetRangeOfSelection()
   */
  Accessibility::Range GetRangeOfSelection(std::size_t selectionIndex) const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetText()
   */
  std::string GetText(std::size_t startOffset, std::size_t endOffset) const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetTextAtOffset()
   */
  Accessibility::Range GetTextAtOffset(std::size_t offset, Accessibility::TextBoundary boundary) const override;

  /**
   * @copydoc Dali::Accessibility::Text::RemoveSelection()
   */
  bool RemoveSelection(std::size_t selectionIndex) override;

  /**
   * @copydoc Dali::Accessibility::Text::SetCursorOffset()
   */
  bool SetCursorOffset(std::size_t offset) override;

  /**
   * @copydoc Dali::Accessibility::Text::SetRangeOfSelection()
   */
  bool SetRangeOfSelection(std::size_t selectionIndex, std::size_t startOffset, std::size_t endOffset) override;

  // Hypertext

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLink()
   */
  Accessibility::Hyperlink* GetLink(std::int32_t linkIndex) const override;

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLinkCount()
   */
  std::int32_t GetLinkCount() const override;

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLinkIndex()
   */
  std::int32_t GetLinkIndex(std::int32_t characterOffset) const override;

protected:
  /**
   * @brief Gets whole text.
   *
   * @return The text
   */
  std::string GetWholeText() const;

  /**
   * @brief Gets text anchors.
   *
   * @return Text anchors
   */
  virtual const std::vector<Toolkit::TextAnchor>& GetTextAnchors() const = 0;

  /**
   * @brief Gets text controller.
   *
   * @return The text controller
   */
  virtual Toolkit::Text::ControllerPtr GetTextController() const = 0;

  /**
   * @brief Get substitute character for hidden text.
   *
   * @return The substitute character (Unicode codepoint)
   */
  virtual std::uint32_t GetSubstituteCharacter() const;

  /**
   * @brief Checks whether text should be hidden (replaced with substitute characters).
   *
   * @return True if text should be hidden, false otherwise
   */
  virtual bool IsHiddenInput() const;

  /**
   * @brief Checks whether [startPosition, endPosition) is a valid, non-empty range within a given string.
   *
   * @param string Source string
   * @param begin Start index (inclusive)
   * @param end End index (exclusive)
   * @return true if the range is valid, false otherwise
   */
  static bool ValidateRange(const std::string& string, std::size_t begin, std::size_t end);
};

class EditableTextControlAccessible : public TextControlAccessible,
                                      public virtual Dali::Accessibility::EditableText
{
public:
  using TextControlAccessible::TextControlAccessible;

  /**
   * @copydoc Dali::Toolkit::DevelControl::ControlAccessible::CalculateStates()
   */
  Accessibility::States CalculateStates() override;

  // Text

  /**
   * @copydoc Dali::Accessibility::Text::GetCursorOffset()
   */
  std::size_t GetCursorOffset() const override;

  /**
   * @copydoc Dali::Accessibility::Text::SetCursorOffset()
   */
  bool SetCursorOffset(std::size_t offset) override;

  // EditableText

  /**
   * @copydoc Dali::Accessibility::EditableText::CopyText()
   */
  bool CopyText(size_t startPosition, size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::CutText()
   */
  bool CutText(size_t startPosition, size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::DeleteText()
   */
  bool DeleteText(size_t startPosition, size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::InsertText()
   */
  bool InsertText(size_t startPosition, std::string text) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::SetTextContents()
   */
  bool SetTextContents(std::string newContents) override;

protected:
  /**
   * @brief Requests text relayout.
   */
  virtual void RequestTextRelayout() = 0;
};

} // namespace Dali::Toolkit::Internal

#endif //DALI_TOOLKIT_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H
