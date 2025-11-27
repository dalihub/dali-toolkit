#ifndef DALI_TOOLKIT_TEXT_ANCHOR_CONTROL_INTERFACE_H
#define DALI_TOOLKIT_TEXT_ANCHOR_CONTROL_INTERFACE_H

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
namespace Text
{
/**
 * @brief An interface that the Text::Controller used for anchor functionality.
 */
class AnchorControlInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~AnchorControlInterface() = default;

  /**
   * @brief Called to notify that an anchor has been clicked.
   * @param[in] cursorPosition Checks if an anchor exists at the given cursor position.
   * @param[out] href If an anchor exists at the given cursor position, the href is written.
   * @return True if an anchor exists at the given cursor position, false otherwise.
   */
  virtual bool AnchorClicked(uint32_t cursorPosition, std::string& href) = 0;

  /**
   * @brief Called to emit an anchor clicked signal.
   */
  virtual void EmitAnchorClickedSignal(const std::string& href) = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ANCHOR_CONTROL_INTERFACE_H
