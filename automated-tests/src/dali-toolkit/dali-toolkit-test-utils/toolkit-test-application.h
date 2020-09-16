#ifndef DALI_TOOLKIT_TEST_APPLICATION_H
#define DALI_TOOLKIT_TEST_APPLICATION_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>

namespace Dali
{

class Adaptor;
class Window;

/**
 * Adds some functionality on top of TestApplication that is required by the Toolkit.
 *
 * This includes creation and destruction of the Adaptor and Window classes.
 */
class ToolkitTestApplication : public TestApplication
{
public:

  ToolkitTestApplication( size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                          size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                          float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                          float  verticalDpi   = DEFAULT_VERTICAL_DPI );

  ~ToolkitTestApplication() override;

  /**
   * @brief Executes the idle callbacks.
   *
   * Some controls like the text-field and the text-editor connect callbacks to the
   * idle signal.
   */
  void RunIdles();

private:

  std::unique_ptr<Dali::Window> mMainWindow;
  std::unique_ptr< Adaptor > mAdaptor;
};

} // namespace Dali

#endif // DALI_TOOLKIT_TEST_APPLICATION_H
