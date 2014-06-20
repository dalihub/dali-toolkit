#ifndef __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_H__
#define __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_H__

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

#include <dali/dali.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class RelayoutControllerImpl;

/**
 * RelayoutController
 * This singleton class provides functionality to trigger relayouting of toolkit controls in the dali scene graph.
 */
class RelayoutController : public Dali::BaseHandle
{
public:

  /**
   * Constructor.
   * We should only create a unique instance.
   */
  RelayoutController();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RelayoutController();

  /**
   * Request to relayout.
   */
  static void Request();

private:

  RelayoutController(Internal::RelayoutControllerImpl* impl);

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_H__
