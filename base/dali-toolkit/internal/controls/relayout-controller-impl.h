#ifndef __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__

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

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include "relayout-controller.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class RelayoutController;

typedef std::pair< Dali::Toolkit::Control, Vector2 > ControlSizePair;
typedef std::vector< ControlSizePair > ControlStack;

/**
 * @copydoc Toolkit::Internal::RelayoutController
 */
class RelayoutControllerImpl : public Dali::BaseObject, public ConnectionTracker
{
public:

  /**
   * Constructor.
   * We should only create a unique instance.
   * @param relayoutFlag to avoid unnecessary calls inside a single frame
   */
  RelayoutControllerImpl( bool& relayoutFlag );


  /**
   * Destructor
   */
  virtual ~RelayoutControllerImpl();

  /**
   * Request for relayout.
   */
  void Request();

private:

  /**
   * Relayouts controls inside actor tree from bottom to top.
   */
  void Relayout();

  /**
   * Resets the relayout controller.
   */
  void Reset();

  /**
   * Disconnect the Relayout() method from the Stage::EventProcessingFinishedSignal().
   */
  void Disconnect();

  // Undefined
  RelayoutControllerImpl(const RelayoutControllerImpl&);
  RelayoutControllerImpl& operator=(const RelayoutControllerImpl&);

private:

  bool& mRelayoutFlag;               ///< reference to relayout flag to avoid unnecessary calls
  ControlStack mControlStack;        ///< stack for relayouting
  ActorSizeContainer mSizecontainer; ///< size container
  bool mRelayoutConnection:1;        ///< Whether EventProcessingFinishedSignal signal is connected.

};

} // namespace Internal


inline Internal::RelayoutControllerImpl& GetImpl(Dali::Toolkit::Internal::RelayoutController& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::RelayoutControllerImpl&>(handle);
}

inline const Internal::RelayoutControllerImpl& GetImpl(const Dali::Toolkit::Internal::RelayoutController& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::RelayoutControllerImpl&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
