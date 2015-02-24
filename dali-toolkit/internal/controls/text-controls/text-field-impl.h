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
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>
#include <dali-toolkit/public-api/text/decorator/text-decorator.h>
#include <dali-toolkit/public-api/text/text-controller.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief A control which renders a short text string.
 */
class TextField : public Control
{
public:

  // Properties
  enum
  {
    TEXTFIELD_PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    TEXTFIELD_PROPERTY_END_INDEX = TEXTFIELD_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices
  };

  /**
   * @copydoc Dali::Toollkit::TextField::New()
   */
  static Toolkit::TextField New();

  /**
   * @copydoc Dali::Toollkit::TextField::SetRenderer()
   */
  void SetRenderer( Text::RendererPtr renderer );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
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
   * @copydoc Control::OnInitialize()
   */
  virtual void OnRelayout( const Vector2& size, ActorSizeContainer& container );

private: // Implementation

  /**
   * Helper for SetProperty.
   * @param[in] text The new "text" property value.
   */
  void SetText( const std::string& text );

  /**
   * Construct a new TextField.
   */
  TextField();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextField();

private:

  // Undefined copy constructor and assignment operators
  TextField(const TextField&);
  TextField& operator=(const TextField& rhs);

private: // Data

  Text::ControllerPtr mController;
  Text::RendererPtr mRenderer;
  Text::DecoratorPtr mDecorator;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextField& GetImpl( Toolkit::TextField& textLabel )
{
  DALI_ASSERT_ALWAYS(textLabel);

  Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<Toolkit::Internal::TextField&>(handle);
}

inline const Toolkit::Internal::TextField& GetImpl( const Toolkit::TextField& textLabel )
{
  DALI_ASSERT_ALWAYS(textLabel);

  const Dali::RefObject& handle = textLabel.GetImplementation();

  return static_cast<const Toolkit::Internal::TextField&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_LABEL_H__
