#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXT_STYLE_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXT_STYLE_H__

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

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/text-input/textview-character-positions-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class TextInputTextStyle;

typedef IntrusivePtr<TextInputTextStyle> TextInputTextStylePtr;

/**
 *  Stores the Input Text Style and provides functions to retrieve and manipulate it.
 */

class TextInputTextStyle : ConnectionTracker
{

public:

  /**
   * @brief Constructor
   */
  TextInputTextStyle(){};

  /**
   * @brief Destructor
   */
   ~TextInputTextStyle(){};

  /**
   * @brief Returns the current Input Style, this is the style that newly inputed text will inherit.
   * @return TextStyle object representing new style.
   */
  TextStyle GetInputStyle() const{return mInputStyle;};

  /**
   * @brief Sets the Input style so newly inputed text will inherit this.
   * @param[in] newStyle the style to now use for Input
   * @return returns true if style changed.  False if new style is the same as current setting.
   */
  bool SetInputStyle( const TextStyle newStyle, const TextStyle::Mask mask = TextStyle::ALL ){return false;};

  /**
   * @brief Gets the Current Font used for newly inputed text
   * @return the Font currently set for new text
   */
  Dali::Font GetInputFont() const{return Dali::Font();};

  /**
   * Signals
   */

  /* Input style changed signal.*/
  typedef SignalV2< void( const TextStyle& style ) > StyleChangedSignalType;

  /**
   * @brief Signal emitted when style changes.
   * @return The signal to connect to
   */
  StyleChangedSignalType& StyleChangedSignal();

private:

  /**
   * @brief Copy Constructor
   * @param[in] textStyle
   * Undefined/Hidden.
   */
  TextInputTextStyle(const TextInputTextStyle& textStyle );

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined/Hidden.
   */
  TextInputTextStyle& operator=(const TextInputTextStyle& rhs);

private:

  TextStyle mInputStyle;                               // Stores the current input style.
  StyleChangedSignalType mStyleChangedSignal;          // Signal emitted when style changes.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_TEXT_STYLE_H__


