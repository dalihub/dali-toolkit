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
   * @brief Called to signal that anchor has been clicked.
   */
  virtual void AnchorClicked(const std::string& href) = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ANCHOR_CONTROL_INTERFACE_H
