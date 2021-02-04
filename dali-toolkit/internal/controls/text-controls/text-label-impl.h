#ifndef DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-scroller-interface.h>
#include <dali-toolkit/internal/text/text-scroller.h>
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief A control which renders a short text string.
 */
class TextLabel : public Control, public Text::ControlInterface, public Text::ScrollerInterface
{
public:
  /**
   * @copydoc Dali::Toollkit::TextLabel::New()
   */
  static Toolkit::TextLabel New();

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  Text::ControllerPtr getController();

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Control::OnStyleChange()
   */
  void OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change) override;

  /**
   * @copydoc Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Control::OnPropertySet()
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  // From ControlInterface

  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

private: // from TextScroller
  /**
   * @copydoc Text::ScrollerInterface::ScrollingFinished()
   */
  void ScrollingFinished() override;

private: // Implementation
  /**
   * Construct a new TextLabel.
   */
  TextLabel();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextLabel();

private:
  // Undefined copy constructor and assignment operators
  TextLabel(const TextLabel&);
  TextLabel& operator=(const TextLabel& rhs);

  /**
   * @brief Set up Autoscrolling
   */
  void SetUpAutoScrolling();

  /**
   * Creates a text-scroller if one has not been created.
   * @return The text scroller.
   */
  Text::TextScrollerPtr GetTextScroller()
  {
    if(!mTextScroller)
    {
      mTextScroller = Text::TextScroller::New(*this);
    }
    return mTextScroller;
  }

private: // Data
  Text::ControllerPtr   mController;
  Text::TextScrollerPtr mTextScroller;

  Toolkit::Visual::Base mVisual;

  int  mRenderingBackend;
  bool mTextUpdateNeeded : 1;

protected:
  struct AccessibleImpl : public Control::Impl::AccessibleImpl,
                          public virtual Dali::Accessibility::Text
  {
    using Control::Impl::AccessibleImpl::AccessibleImpl;

    std::string          GetText(size_t startOffset, size_t endOffset) override;
    size_t               GetCharacterCount() override;
    size_t               GetCaretOffset() override;
    bool                 SetCaretOffset(size_t offset) override;
    Accessibility::Range GetTextAtOffset(size_t offset, Accessibility::TextBoundary boundary) override;
    Accessibility::Range GetSelection(size_t selectionNum) override;
    bool                 RemoveSelection(size_t selectionNum) override;
    bool                 SetSelection(size_t selectionNum, size_t startOffset, size_t endOffset) override;
    std::string          GetNameRaw() override;
    Property::Index      GetNamePropertyIndex() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextLabel& GetImpl(Toolkit::TextLabel& textLabel)
{
  DALI_ASSERT_ALWAYS(textLabel);

  Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<Toolkit::Internal::TextLabel&>(handle);
}

inline const Toolkit::Internal::TextLabel& GetImpl(const Toolkit::TextLabel& textLabel)
{
  DALI_ASSERT_ALWAYS(textLabel);

  const Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<const Toolkit::Internal::TextLabel&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H
