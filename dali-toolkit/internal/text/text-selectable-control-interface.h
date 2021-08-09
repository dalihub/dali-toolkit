#ifndef DALI_TOOLKIT_TEXT_SELECTABLE_CONTROL_INTERFACE_H
#define DALI_TOOLKIT_TEXT_SELECTABLE_CONTROL_INTERFACE_H

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

namespace Dali
{
namespace Toolkit
{
using Uint32Pair = std::pair<uint32_t, uint32_t>;
using string     = std::string;
namespace Text
{
/**
 * @brief An interface that the Text::Controller used for text selection functionality.
 */
class SelectableControlInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~SelectableControlInterface() = default;

  /**
   * @brief Called to set the selection postions in the texts.
   * @param start start selection position (pass NULL to ignore).
   * @param end end selection position (pass NULL to ignore).
   */
  virtual void SetTextSelectionRange(const uint32_t* start, const uint32_t* end) = 0;

  /**
   * @brief Called to return the selection postions in the texts.
   * @return pair contains start and end positions.
   */
  virtual Uint32Pair GetTextSelectionRange() const = 0;

  /**
   * @brief Called to select the whole texts.
   */
  virtual void SelectWholeText() = 0;

  /**
   * @brief Called to unselect the whole texts.
   */
  virtual void SelectNone() = 0;

  /**
   * @brief Called to set the selection postions in the texts.
   * @param start start selection position.
   * @param end end selection position.
   */
  virtual void SelectText(const uint32_t start, const uint32_t end) = 0;

  /**
   * @brief Retrive Selected text.
   * @return The seletced text.
   */
  virtual string GetSelectedText() const = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SELECTABLE_CONTROL_INTERFACE_H
