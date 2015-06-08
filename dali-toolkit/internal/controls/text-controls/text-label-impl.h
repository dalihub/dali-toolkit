#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief A control which renders a short text string.
 */
class TextLabel : public Control, public Text::ControlInterface
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
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Control::OnStyleChange()
   */
  virtual void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change );

  /**
   * @copydoc Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Text::ControlInterface::AddDecoration()
   */
  virtual void AddDecoration( Actor& actor );

  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  virtual void RequestTextRelayout();

  /**
   * @copydoc Text::ControlInterface::TextChanged()
   */
  virtual void TextChanged();

  /**
   * @copydoc Text::ControlInterface::MaxLengthReached()
   */
  virtual void MaxLengthReached();

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

private: // Data

  Text::ControllerPtr mController;
  Text::RendererPtr mRenderer;
  RenderableActor mRenderableActor;

  int mRenderingBackend;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextLabel& GetImpl( Toolkit::TextLabel& textLabel )
{
  DALI_ASSERT_ALWAYS(textLabel);

  Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<Toolkit::Internal::TextLabel&>(handle);
}

inline const Toolkit::Internal::TextLabel& GetImpl( const Toolkit::TextLabel& textLabel )
{
  DALI_ASSERT_ALWAYS(textLabel);

  const Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<const Toolkit::Internal::TextLabel&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H__
