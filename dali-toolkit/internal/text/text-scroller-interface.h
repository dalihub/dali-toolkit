#ifndef __DALI_TOOLKIT_TEXT_SCROLLER_INTERFACE_H__
#define __DALI_TOOLKIT_TEXT_SCROLLER_INTERFACE_H__

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

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Text
{

/**
 * @brief An interface used by the text-controls which implement auto-scrolling
 */
class ScrollerInterface
{
public:

  /**
   * @brief Constructor.
   */
  ScrollerInterface();

  /**
   * @brief Virtual destructor.
   */
  virtual ~ScrollerInterface();

  /**
   * @brief Called when the scrolling finishes
   */
  virtual void ScrollingFinished() = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SCROLLER_INTERFACE_H__
