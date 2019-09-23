#ifndef DALI_TOOLKIT_TEXT_EDITABLE_CONTROL_INTERFACE_H
#define DALI_TOOLKIT_TEXT_EDITABLE_CONTROL_INTERFACE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/input-style.h>

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Text
{

/**
 * @brief An interface that the Text::Controller uses to notify about text changes and add decoration to the text control.
 */
class EditableControlInterface
{
public:

  /**
   * @brief Virtual destructor.
   */
  virtual ~EditableControlInterface()
  {}

  /**
   * @brief Called to signal that text has been inserted or deleted.
   */
  virtual void TextChanged() = 0;

  /**
   * @brief Called when the number of characters to be inserted exceeds the maximum limit
   */
  virtual void MaxLengthReached() = 0;

  /**
   * @brief Called to signal that input style has been changed.
   *
   * @param[in] inputStyleMask Mask with the bits of the input style that has changed.
   */
  virtual void InputStyleChanged( InputStyle::Mask inputStyleMask ) = 0;

  /**
   * @brief Add a decoration.
   *
   * @param[in] decoration The actor displaying a decoration.
   * @param[in] needsClipping Whether the actor needs clipping.
   */
  virtual void AddDecoration( Actor& actor, bool needsClipping ) = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EDITABLE_CONTROL_INTERFACE_H
