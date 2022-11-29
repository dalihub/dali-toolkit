#ifndef DALI_HIDDEN_TEXT_H
#define DALI_HIDDEN_TEXT_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/public-api/controls/text-controls/hidden-input-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{

static constexpr const uint32_t STAR = 0x2A; // Set default substitute character as '*'

/**
 * Class to handle the hidden text
 */
class HiddenText : public ConnectionTracker
{
public:
  class Observer
  {
  public:
    /**
     * @brief Invoked when the time to show last character is expired
     */
    virtual void DisplayTimeExpired() = 0;
  };

  /**
   * @brief Constructor
   * @param[in] observer The Observer pointer.
   */
  HiddenText(Observer* observer);

public: // Intended for internal use
  /**
   * @brief Used to set options of hidden text
   * @param[in] map The property map describing the option
   */
  void SetProperties(const Property::Map& map);

  /**
   * @brief Retrieve property map of hidden text options
   * @param[out] map The hidden text option
   */
  void GetProperties(Property::Map& map);

  /**
   * @brief Convert source text to destination text according to current option
   * @param[in] source The original text
   * @param[out] destination The applied text
   */
  void Substitute(const Vector<Character>& source, Vector<Character>& destination, Length currentCursorIndex);

  /**
   * @brief Initialize the value of PreviousTextCount
   */
  void InitPreviousTextCount();

  /**
   * @brief Returns the hide mode of hidden text.
   * @return The hide mode of hidden text.
   */
  int GetHideMode();

  /**
   * @brief Invoked when the timer is expired
   */
  bool OnTick();

private:
  Timer     mTimer;
  Observer* mObserver;
  int       mHideMode;
  int       mSubstituteText;
  int       mDisplayDuration;
  int       mSubstituteCount;
  Length    mPreviousTextCount;
  bool      mIsLastCharacterShow;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_HIDDEN_TEXT_H
