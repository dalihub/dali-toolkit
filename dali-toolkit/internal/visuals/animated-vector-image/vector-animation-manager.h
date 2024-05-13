#ifndef DALI_TOOLKIT_INTERNAL_VECTOR_ANIMATION_MANAGER_H
#define DALI_TOOLKIT_INTERNAL_VECTOR_ANIMATION_MANAGER_H

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

// EXTERNAL INCLUDES
#include <dali/integration-api/ordered-set.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/callback.h>
#include <memory>

// INTERNAL INCLUDES

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class VectorAnimationThread;

/**
 * @brief Vector animation manager
 */
class VectorAnimationManager : public Integration::Processor
{
public:
  /**
   * @brief Constructor.
   */
  VectorAnimationManager();

  /**
   * @brief Destructor.
   */
  ~VectorAnimationManager() override;

  /**
   * Get the vector animation thread.
   * @return A raw pointer pointing to the vector animation thread.
   */
  VectorAnimationThread& GetVectorAnimationThread();

  /**
   * @brief Register a callback.
   *
   * @param callback The callback to register
   * @note Ownership of the callback is passed onto this class.
   */
  void RegisterEventCallback(CallbackBase* callback);

  /**
   * @brief Unregister a previously registered callback
   *
   * @param callback The callback to unregister
   */
  void UnregisterEventCallback(CallbackBase* callback);

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "VectorAnimationManager";
  }

private:
  // Undefined
  VectorAnimationManager(const VectorAnimationManager& manager) = delete;

  // Undefined
  VectorAnimationManager& operator=(const VectorAnimationManager& manager) = delete;

private:
  Dali::Integration::OrderedSet<CallbackBase> mEventCallbacks; ///< Event triggered callback lists (owned)

  std::unique_ptr<VectorAnimationThread> mVectorAnimationThread;
  bool                                   mProcessorRegistered : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VECTOR_ANIMATION_MANAGER_H
