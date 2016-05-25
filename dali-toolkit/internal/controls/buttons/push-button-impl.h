#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * PushButton implementation class.
 *
 * \sa Dali::Toolkit::PushButton
 */
class PushButton : public Button
{
public:

  /**
   * Create a new PushButton.
   * @return A smart-pointer to the newly allocated PushButton.
   */
  static Dali::Toolkit::PushButton New();

protected:

  /**
   * Construct a new PushButton.
   */
  PushButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PushButton();

public:

  // Properties

  /**
   * Enum for the alignment modes of the icon.
   */
  enum IconAlignment
  {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    DEFAULT = RIGHT
  };

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
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

private: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionIn( Actor actor )
   */
  virtual void PrepareForTranstionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionOut( Actor actor )
   */
  virtual void PrepareForTranstionOut( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionIn( Toolkit::Visual::Base& visual )
   */
  virtual void OnTransitionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionOut( Toolkit::Visual::Base& visual )
   */
  virtual void OnTransitionOut( Actor actor );


private:

  /**
   * @brief Sets the alignment mode to use to align the icon to the label.
   *
   * @param[in] iconAlignment The alignment mode to use
   */
  void SetIconAlignment( const PushButton::IconAlignment iconAlignment );

  /**
   * @brief Gets the alignment mode used to align the icon to the label.
   *
   * @return The alignment mode in use
   */
  const PushButton::IconAlignment GetIconAlignment() const;

private:

  // Undefined
  PushButton( const PushButton& );

  // Undefined
  PushButton& operator=( const PushButton& );

private:

  IconAlignment  mIconAlignment;                 ///< The alignment of the icon against the label.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::PushButton& GetImplementation( Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::PushButton&>( handle );
}

inline const Toolkit::Internal::PushButton& GetImplementation( const Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::PushButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
