#ifndef DALI_TOOLKIT_DEVEL_LAYOUT_CONTROLLER_H
#define DALI_TOOLKIT_DEVEL_LAYOUT_CONTROLLER_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/common/dali-common.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class LayoutController;
}

/**
 * @brief Controls the process of layouting the control tree. It provides methods to enable
 * controls to re-layout and internal systems to separately measure and layout controls.
 */
class DALI_TOOLKIT_API LayoutController : public BaseHandle
{
public:
  /**
   * @brief Constructor - create an uninitialized handle.
   *
   * This can be initialized with LayoutController::Get().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  LayoutController();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~LayoutController();

  /**
   * @brief Gets the singleton of the LayoutController object.
   *
   * @return A handle to the LayoutController object.
   */
  static LayoutController Get();

  /**
   * @brief Request for a particular layout (wrapping a control or a visual) to be measured and laid out.
   * @param[in] layout The layout to measure & relayout.
   */
  void RequestLayout( LayoutItem layout );

public:
  /// @cond internal
  /**
   * @brief This constructor is used by LayoutController::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL LayoutController( Internal::LayoutController *impl );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_LAYOUT_CONTROLLER_H
