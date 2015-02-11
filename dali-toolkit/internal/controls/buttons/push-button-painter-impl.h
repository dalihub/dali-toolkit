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
   * @copydoc ButtonPainter::SetDisabled( Toolkit::Button& button, bool disabled )
   */
  virtual void SetDisabled( Toolkit::Button& button, bool disabled ) = 0;

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
