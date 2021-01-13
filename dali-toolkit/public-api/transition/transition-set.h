#ifndef DALI_TOOLKIT_TRANSITION_SET_H
#define DALI_TOOLKIT_TRANSITION_SET_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/transition/transition-base.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint> // uint32_t, uint8_t

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class TransitionSet;
}

/**
 * @brief
 *
 * TransitionSet is used to control lifetime of multiple Transitions.
 * Transition could be played with multiple other transitions for a scene change.
 * For the case, it is more useful to manage a group of transitions with same lifetime and a finished signal.
 * TransitionSet provides a single Play call and Finished callback for the multiple traisitions those added on it.
 */
class DALI_TOOLKIT_API TransitionSet : public BaseHandle
{
public:
  using TransitionSetSignalType = Signal<void(TransitionSet&)>; ///< TransitionSet finished signal type

  /**
   * @brief Creates an uninitialized TransitionSet; this can be initialized with TransitionSet::New().
   *
   * Calling member functions with an uninitialized TransitionSet handle is not allowed.
   */
  TransitionSet();

  /**
   * @brief Creates an initialized TransitionSet.
   *
   * @return A handle to a newly allocated Dali resource
   * @note durationSeconds can not be negative.
   */
  static TransitionSet New();

  /**
   * @brief Downcasts a handle to TransitionSet handle.
   *
   * If handle points to an TransitionSet object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to an TransitionSet object or an uninitialized handle
   */
  static TransitionSet DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TransitionSet();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  TransitionSet(const TransitionSet& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  TransitionSet& operator=(const TransitionSet& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  TransitionSet(TransitionSet&& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  TransitionSet& operator=(TransitionSet&& rhs);

  /**
   * @brief Add a TransitionBase on this TransitionSet.
   *
   * @param[in] transition TransitionBase to be added.
   */
  void AddTransition(TransitionBase transition);

  /**
   * @brief Retrieves a TransitionBase at the index.
   *
   * @return The TransitionBase of index
   */
  TransitionBase GetTransitionAt(uint32_t index) const;

  /**
   * @brief Retrieves the number of Transitions added in TransitionSet
   *
   * @return The number of Transitions
   */
  uint32_t GetTransitionCount() const;

  /**
   * @brief Play the transition.
   * This method not make property animation instantly.
   * Transition requires some world transform properties.
   * The Transitions currently added on this TransitionSet are queued TransitionQueue
   * and they are played at the end of this tick of event Thread
   */
  void Play();

  /**
   * @brief Connects to this signal to be notified when all TransitionSet's transitions have finished.
   *
   * @return A signal object to connect with
   */
  TransitionSetSignalType& FinishedSignal();

public: // Not intended for use by Application developers
  /// @cond internal
  /**
   * @brief This constructor is used by TransitionSet::New() methods.
   * @param[in] transition A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TransitionSet(Internal::TransitionSet* transition);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TRANSITION_SET_H
