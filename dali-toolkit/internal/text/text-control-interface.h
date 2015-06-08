#ifndef __DALI_TOOLKIT_TEXT_CONTROL_INTERFACE_H__
#define __DALI_TOOLKIT_TEXT_CONTROL_INTERFACE_H__

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

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Text
{

/**
 * @brief An interface that the Text::Controller uses to request a text relayout.
 */
class ControlInterface
{
public:

  /**
   * @brief Constructor.
   */
  ControlInterface();

  /**
   * @brief Virtual destructor.
   */
  virtual ~ControlInterface();

  /**
   * @brief Add a decoration.
   *
   * @param[in] decoration The actor displaying a decoration.
   */
  virtual void AddDecoration( Actor& actor ) = 0;

  /**
   * @brief Called to request a text relayout.
   */
  virtual void RequestTextRelayout() = 0;

  /**
   * @brief Called to signal that text has been inserted or deleted.
   */
  virtual void TextChanged() = 0;

  /**
   * @brief Called when the number of characters to be inserted exceeds the maximum limit
   */
  virtual void MaxLengthReached() = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CONTROL_INTERFACE_H__
