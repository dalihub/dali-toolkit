#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_PAINTER_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_PAINTER_H__

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
#include "button-painter-impl.h"

namespace Dali
{

// Forward declarations

class Image;

namespace Toolkit
{

// Forward declarations

class PushButton;

namespace Internal
{

// Forward declarations

class PushButtonPainter;

/**
 * PushButtonPainter methods should be implemented in a subclass.
 */
class PushButtonPainter : public ButtonPainter
{
public:
  /**
   * Destructor.
   *
   */
  virtual ~PushButtonPainter() {}

  /**
   * This method is called from the Dali::Toolkit::Internal::PushButton when the
   * \e autorepeating property changes.
   * @param[in] autorepeating property.
   */
  virtual void SetAutoRepeating( bool autorepeating ) = 0;

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton, in which this
   * object is registered, is pressed.
   * @param[inout] button The Dali::Toolkit::PushButton, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Pressed( Toolkit::PushButton& button ) = 0;

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton, in which this
   * object is registered, is released.
   * @param[inout] button The Dali::Toolkit::PushButton, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Released( Toolkit::PushButton& button ) = 0;

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton, in which this
   * object is registered, is clicked.
   * @param[inout] button The Dali::Toolkit::PushButton, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Clicked( Toolkit::PushButton& button ) = 0;

  /**
   * This method is called when the Dali::Toolkit::Internal::PushButton, in which this
   * object is registered, is toggled.
   * @param[inout] button The Dali::Toolkit::PushButton, linked to the internal
   * implementation, in which this object is registered.
   */
  virtual void Toggled( Toolkit::PushButton& button ) = 0;

  /////////////////////////////////////////////////////////////////////////////
  // ButtonPainter interface
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @copydoc ButtonPainter::Initialize( Toolkit::Button& button )
   */
  virtual void Initialize( Toolkit::Button& button ) = 0;

  /**
   * @copydoc ButtonPainter::SetSize( Toolkit::Button& button, const Vector3& size )
   */
  virtual void SetSize( Toolkit::Button& button, const Vector3& size ) = 0;

  /**
   * @copydoc ButtonPainter::SetDimmed( Toolkit::Button& button, bool dimmed )
   */
  virtual void SetDimmed( Toolkit::Button& button, bool dimmed ) = 0;

  /**
   * @copydoc ButtonPainter::SetAnimationTime()
   */
  virtual void SetAnimationTime( float animationTime ) = 0;

  /**
   * @copydoc ButtonPainter::GetAnimationTime()
   */
  virtual float GetAnimationTime() const = 0;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_PAINTER_H__
