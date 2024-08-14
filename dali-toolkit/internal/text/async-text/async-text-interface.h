#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_INTERFACE_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_INTERFACE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>

namespace Dali
{
class Actor;

namespace Toolkit
{
namespace Text
{
/**
 * @brief An interface used by the text-controls which implement async text.
 */
class AsyncTextInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~AsyncTextInterface() = default;

  /**
   * @brief Setup auto scrolling.
   *
   * Passes the render info generated in the worker thread to the text-control.
   */
  virtual void AsyncSetupAutoScroll(Text::AsyncTextRenderInfo renderInfo) = 0;

  /**
   * @brief Called when the text fit changed.
   */
  virtual void AsyncTextFitChanged(float pointSize) = 0;

  /**
   * @brief Called when the async load complete.
   */
  virtual void AsyncLoadComplete(Text::AsyncTextRenderInfo renderInfo) = 0;

  /**
   * @brief Called when the async size computed.
   */
  virtual void AsyncSizeComputed(Text::AsyncTextRenderInfo renderInfo) = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_INTERFACE_H
