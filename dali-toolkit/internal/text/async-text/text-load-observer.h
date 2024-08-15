#ifndef DALI_TOOLKIT_INTERNAL_TEXT_LOAD_OBSERVER_H
#define DALI_TOOLKIT_INTERNAL_TEXT_LOAD_OBSERVER_H

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
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Base class used to observe the load status of a async text.
 *
 * Derived class must implement the LoadComplete method which is
 * executed once the text is ready to render.
 */
class TextLoadObserver
{
public:
  typedef Signal<void(TextLoadObserver*)> DestructionSignalType; ///< Signal prototype for the Destruction Signal.

  struct TextInformation
  {
    TextInformation(const Text::AsyncTextRenderInfo& renderInfo, const Text::AsyncTextParameters& parameters);
    TextInformation();

    Text::AsyncTextRenderInfo renderInfo;
    Text::AsyncTextParameters parameters;
  };

public:
  /**
   * @brief Constructor.
   */
  TextLoadObserver();

  /**
   * @brief Virtual destructor.
   */
  virtual ~TextLoadObserver();

  /**
   * The action to be taken once the async load has finished.
   * And in case of text loading, this method is called after off screen rendering.
   * This should be overridden by the deriving class.
   *
   * @param[in] loadSuccess True if the text load was successful.
   * @param[in] textureInformation Structure that contains loaded text information.
   */
  virtual void LoadComplete(bool loadSuccess, const TextInformation& textInformation) = 0;

  /**
   * @brief Returns the destruction signal.
   * This is emitted when the observer is destroyed.
   * This is used by the observer notifier to mark this observer as destroyed (IE. It no longer needs notifying).
   */
  DestructionSignalType& DestructionSignal();

  /**
   * @brief Each time DestructionSignal is connected, the count increases by 1.
   */
  void ConnectDestructionSignal();

  /**
   * @brief Each time DestructionSignal is disconnected, the count decreases by 1.
   * @return If the count reaches 0, true is returned.
   */
  bool DisconnectDestructionSignal();

public:
private:
  DestructionSignalType mDestructionSignal;        ///< The destruction signal emitted when the observer is destroyed.
  int                   mDestructionSignalConnect; ///< The number of times DestructionSignal is connected.
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_LOAD_OBSERVER_H
