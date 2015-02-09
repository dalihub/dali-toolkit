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
   * This method is called from the Dali::Toolkit::Internal::PushButton when the \e disabled property changes.
   *
   * @param[inout] button The button in which all actors that form its appearance are going to be added.
   * @param[in] disabled The disabled state.
   */
  virtual void SetDisabled( Toolkit::Button& button, bool disabled ) = 0;

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
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BUTTON_PAINTER_H__
