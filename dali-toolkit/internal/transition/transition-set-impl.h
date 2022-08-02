#ifndef DALI_TOOLKIT_INTERNAL_TRANSITION_SET_H
#define DALI_TOOLKIT_INTERNAL_TRANSITION_SET_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/transition/transition-impl.h>
#include <dali-toolkit/public-api/transition/transition-set.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using TransitionSetPtr = IntrusivePtr<TransitionSet>;

class TransitionSet : public BaseObject, public ConnectionTracker, public Integration::Processor
{
public:
  /**
   * Create a new TransitionSet object.
   * @return A smart-pointer to the newly allocated TransitionSet.
   */
  static TransitionSetPtr New();

  /**
   * @copydoc Dali::Toolkit::TransitionSet::AddTransition(TransitionPtr transition)
   */
  void AddTransition(TransitionBasePtr transition);

  /**
   * @copydoc Dali::Toolkit::TransitionSet::GetTransitionAt(uint32_t index)
   */
  TransitionBase* GetTransitionAt(uint32_t index) const;

  /**
   * @copydoc Dali::Toolkit::TransitionSet::GetTransitionCount()
   */
  uint32_t GetTransitionCount() const;

  /**
   * @brief Make ready this transition set to play.
   * Transitions in this transition set will be create property animations at the end of this tick of main thread.
   */
  void Play();

  /**
   * @copydoc Dali::Toolkit::TransitionSet::FinishedSignal()
   */
  Dali::Toolkit::TransitionSet::TransitionSetSignalType& FinishedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

private:
  /**
   * @brief Set pre process of each transition.
   */
  void TransitionPreProcess();

  /**
   * @brief Start to play each of Transition.
   * This method called at the end of event thread tick, and this method call OnPlay method of TransitionBase internally.
   */
  void TransitionStart();

  /**
   * @brief Remove each finished TransitionBase from play list.
   * If all transitions are finished emit Finished signal.
   */
  void TransitionFinished(Dali::Animation& source);

  /**
   * Emit the Finished signal
   */
  void EmitFinishedSignal();

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

protected:
  /**
   * Construct a new TransitionSet.
   */
  TransitionSet();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~TransitionSet() override;

private:
  // Undefined
  TransitionSet(const TransitionSet&);

  // Undefined
  TransitionSet& operator=(const TransitionSet& rhs);

private:
  Dali::Toolkit::TransitionSet::TransitionSetSignalType mFinishedSignal{};
  std::vector<TransitionBasePtr>                        mTransitions;
  Dali::Animation                                       mAnimation;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TransitionSet& GetImplementation(Dali::Toolkit::TransitionSet& transitionSet)
{
  DALI_ASSERT_ALWAYS(transitionSet && "TransitionSet handle is empty");

  BaseObject& handle = transitionSet.GetBaseObject();

  return static_cast<Internal::TransitionSet&>(handle);
}

inline const Internal::TransitionSet& GetImplementation(const Dali::Toolkit::TransitionSet& transitionSet)
{
  DALI_ASSERT_ALWAYS(transitionSet && "TransitionSet handle is empty");

  const BaseObject& handle = transitionSet.GetBaseObject();

  return static_cast<const Internal::TransitionSet&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TRANSITION_SET_H
