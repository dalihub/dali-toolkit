#ifndef DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H

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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/spanned.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
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
class TextLabel : public Control, public Text::ControlInterface, public Text::ScrollerInterface, public Text::AnchorControlInterface
{
public:
  /**
   * @copydoc Dali::Toollkit::TextLabel::New()
   * @param[in] additionalBehaviour custom behavior flags for this TextLabel. Default is CONTROL_BEHAVIOUR_DEFAULT
   */
  static Toolkit::TextLabel New(ControlBehaviour additionalBehaviour = ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);

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

  /**
   * @copydoc Dali::Toollkit::TextLabel::AnchorClickedSignal()
   */
  DevelTextLabel::AnchorClickedSignalType& AnchorClickedSignal();

  /**
   * @copydoc Dali::Toollkit::TextLabel::TextFitChangedSignal()
   */
  DevelTextLabel::TextFitChangedSignalType& TextFitChangedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * @brief Gets text controller
   *
   * @return The text controller
   */
  Text::ControllerPtr GetTextController();

  /**
   * @brief Get the rendered size of a specific text range.
   * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to calculate size for.
   * @param[in] endIndex end index(included) of the text requested to calculate size for.
   * @return list of sizes of the reuested text.
   */
  Vector<Vector2> GetTextSize(const uint32_t startIndex, const uint32_t endIndex) const;

  /**
   * @brief Get the top/left rendered position of a specific text range.
   * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to get position to.
   * @param[in] endIndex end index(included) of the text requested to get position to.
   * @return list of positions of the requested text.
   */
  Vector<Vector2> GetTextPosition(const uint32_t startIndex, const uint32_t endIndex) const;

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetLineBoundingRectangle(const uint32_t lineIndex) const;

  /**
   * @brief Get the character bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetCharacterBoundingRectangle(const uint32_t charIndex) const;

  /**
   * @brief Set the @p spannedText into current textLabel
   * the spanned text contains content (text) and  format (spans with ranges)
   * the text is copied into text-controller and the spans are applied on ranges
   *
   * @param[in] spannedText the text with spans.
   */
  void SetSpannedText(const Text::Spanned& spannedText);

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc Control::OnStyleChange()
   */
  void OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change) override;

  /**
   * @copydoc Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Control::OnTap()
   */
  void OnTap(const TapGesture& tap) override;

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

  /**
   * @copydoc Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

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

public: // From AnchorControlInterface
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  void AnchorClicked(const std::string& href) override;

private: // Implementation
  /**
   * Construct a new TextLabel.
   *
   * @param[in] additionalBehaviour additional behaviour flags for this TextLabel
   */
  TextLabel(ControlBehaviour additionalBehaviour);

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

  /**
   * @brief Callback function for when the layout is changed.
   * @param[in] actor The actor whose layoutDirection is changed.
   * @param[in] type  The layoutDirection.
   */
  void OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type);

  /**
   * @brief Emits TextFitChanged signal.
   */
  void EmitTextFitChangedSignal();
  void OnAccessibilityStatusChanged();

private: // Data
  Text::ControllerPtr   mController;
  Text::TextScrollerPtr mTextScroller;

  Toolkit::Visual::Base mVisual;

  std::vector<Toolkit::TextAnchor> mAnchorActors;

  // Signals
  Toolkit::DevelTextLabel::AnchorClickedSignalType  mAnchorClickedSignal;
  Toolkit::DevelTextLabel::TextFitChangedSignalType mTextFitChangedSignal;

  int  mRenderingBackend;
  bool mTextUpdateNeeded : 1;
  bool mLastAutoScrollEnabled : 1;

protected:
  /**
   * @brief This structure is to connect TextLabel with Accessible functions.
   */
  class TextLabelAccessible : public TextControlAccessible
  {
  public:
    using TextControlAccessible::TextControlAccessible;

    /**
     * @copydoc Dali::Accessibility::Text::GetNameRaw()
     */
    std::string GetNameRaw() const override;

    /**
     * @copydoc Dali::Accessibility::Text::GetNamePropertyIndex()
     */
    Property::Index GetNamePropertyIndex() override;

  protected:
    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::GetTextAnchors()
     */
    const std::vector<Toolkit::TextAnchor>& GetTextAnchors() const override;

    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::GetTextController()
     */
    Toolkit::Text::ControllerPtr GetTextController() const override;
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
