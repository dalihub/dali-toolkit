#ifndef __DALI_TOOLKIT_INTERNAL_BUTTON_PAINTER_H__
#define __DALI_TOOLKIT_INTERNAL_BUTTON_PAINTER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES

namespace Dali
{

// Forward declarations

struct Vector3;

namespace Toolkit
{

// Forward declarations

class Button;

namespace Internal
{

// Forward declarations

class ButtonPainter;

// Type definitions

typedef IntrusivePtr<ButtonPainter> ButtonPainterPtr;

/**
 * ButtonPainter is an interface which provides common functionality to all painters.
 */
class ButtonPainter : public RefObject, public ConnectionTracker
{
public:
  /**
   * Destructor.
   *
   */
  virtual ~ButtonPainter() {}

  /**
   * Initializes the painter.
   *
   * This method is called from the Dali::Toolkit::Internal::Button.
   *
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   */
  virtual void Initialize( Toolkit::Button& button ) = 0;

  /**
   * Sets the new size.
   *
   * This method is called from the Dali::Toolkit::Internal::Button when its size changes.
   *
   * @param[inout] button The button which stores button's images.
   * @param[in] size The new size.
   */
  virtual void SetSize( Toolkit::Button& button, const Vector3& size ) = 0;

  /**
   * This method is called from the Dali::Toolkit::Internal::Button when the \e disabled property changes.
   *
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] disabled The disabled state.
   */
  virtual void SetDisabled( Toolkit::Button& button, bool disabled ) = 0;

  /**
   * This method is called from the Dali::Toolkit::Internal::PushButton when the \e autorepeating property changes.
   * @param[in] autorepeating property.
   */
  virtual void SetAutoRepeating( bool autorepeating ) {}

  /**
   * Sets the animation time.
   * @param [in] animationTime The animation time in seconds.
   */
  virtual void SetAnimationTime( float animationTime ) = 0;

  /**
   * Retrieves the animation time.
   * @return The animation time in seconds.
   */
  virtual float GetAnimationTime() const = 0;

  /**
   * Sets the label.
   * It adds the label to the root actor.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] label Button label.
   */
  virtual void SetLabel( Toolkit::Button& button, Actor label ) {}

  /**
   * Sets the button image.
   * It adds the button image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The button image.
   */
  virtual void SetButtonImage( Toolkit::Button& button, Actor image ) {}

  /**
   * Sets the selected image.
   * It adds the selected image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The selected image.
   */
  virtual void SetSelectedImage( Toolkit::Button& button, Actor image ) {}

  /**
   * Sets the background image.
   * It adds the background image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The background image.
   */
  virtual void SetBackgroundImage( Toolkit::Button& button, Actor image ) {}

  /**
   * Sets the disabled image.
   * It adds the disabled image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The disabled button image.
   */
  virtual void SetDisabledImage( Toolkit::Button& button, Actor image ) {}

  /**
   * Sets the disabled selected image.
   * It adds the disabled selected image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The disabled selected image.
   */
  virtual void SetDisabledSelectedImage( Toolkit::Button& button, Actor image ) {}

  /**
   * Sets the disabled background image.
   * It adds the disabled background image to the root actor and creates the image transition if needed.
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] image The disabled background image.
   */
  virtual void SetDisabledBackgroundImage( Toolkit::Button& button, Actor image ) {}

  /**
   * This method is called when the Dali::Toolkit::Internal::Button, in which this
   * object is registered, is pressed.
   * @param[inout] button The Dali::Toolkit::Button, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Pressed( Toolkit::Button& button ) {}

  /**
   * This method is called when the Dali::Toolkit::Internal::Button, in which this
   * object is registered, is released.
   * @param[inout] button The Dali::Toolkit::Button, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Released( Toolkit::Button& button ) {}

  /**
   * This method is called when the Dali::Toolkit::Internal::Button, in which this
   * object is registered, is clicked.
   * @param[inout] button The Dali::Toolkit::Button, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Clicked( Toolkit::Button& button ) {}

  /**
   * This method is called when the Dali::Toolkit::Internal::Button, in which this
   * object is registered, changes its state.
   * @param[inout] button The Dali::Toolkit::Button, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Selected( Toolkit::Button& button ) {}
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BUTTON_PAINTER_H__
