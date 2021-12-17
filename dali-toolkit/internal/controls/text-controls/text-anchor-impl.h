#ifndef DALI_TOOLKIT_INTERNAL_TEXT_ANCHOR_H
#define DALI_TOOLKIT_INTERNAL_TEXT_ANCHOR_H

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
#include <dali/devel-api/atspi-interfaces/hyperlink.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-anchor-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief A control which renders anchor (hyperlink) in hypertext.
 */
class TextAnchor : public Control
{
public:
  /**
   * @copydoc Dali::Toollkit::TextAnchor::New()
   */
  static Toolkit::TextAnchor New();

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

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Control::OnPropertySet()
   */
  // void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Control::OnAccessibilityActivated()
   */
  bool OnAccessibilityActivated() override;

private: // Implementation
  /**
   * Construct a new TextAnchor.
   */
  TextAnchor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextAnchor();

private:
  // Undefined copy constructor and assignment operators
  TextAnchor(const TextAnchor&);
  TextAnchor& operator=(const TextAnchor& rhs);

  //Data
  int         mStartCharacterIndex;
  int         mEndCharacterIndex;
  std::string mUri;

protected:
  /**
   * @brief This structure is to connect TextAnchor with Accessible functions.
   */
  struct AccessibleImpl : public DevelControl::AccessibleImpl,
                          public virtual Dali::Accessibility::Hyperlink
  {
    using DevelControl::AccessibleImpl::AccessibleImpl;
    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetEndIndex()
     */
    int32_t GetEndIndex() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetStartIndex()
     */
    int32_t GetStartIndex() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorCount()
     */
    int32_t GetAnchorCount() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorAccessible()
     */
    Accessible* GetAnchorAccessible(int32_t anchorIndex) const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorUri()
     */
    std::string GetAnchorUri(int32_t anchorIndex) const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::IsValid()
     */
    bool IsValid() const override;
  };
};

inline Toolkit::Internal::TextAnchor& GetImpl(Toolkit::TextAnchor& textAnchor)
{
  DALI_ASSERT_ALWAYS(textAnchor);

  Dali::RefObject& handle = textAnchor.GetImplementation();

  return static_cast<Toolkit::Internal::TextAnchor&>(handle);
}

inline const Toolkit::Internal::TextAnchor& GetImpl(const Toolkit::TextAnchor& textAnchor)
{
  DALI_ASSERT_ALWAYS(textAnchor);

  const Dali::RefObject& handle = textAnchor.GetImplementation();

  return static_cast<const Toolkit::Internal::TextAnchor&>(handle);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_ANCHOR_H
