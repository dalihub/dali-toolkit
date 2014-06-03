#ifndef  __TOOLKIT_CLIPBOARD_H__
#define  __TOOLKIT_CLIPBOARD_H__

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

#define __DALI_CLIPBOARD_H__

// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{

namespace Adaptor
{
class Clipboard;
}
}

/**
 * The Clipboard can operate using various funtion.
 * Clipboard can manage it's item and set show / hide status.
 */
class Clipboard : public BaseHandle
{
public:
  /**
   * Create an uninitialized Clipboard;
   *  this can be initialized with one of the derived Clipboard' New() methods
   */
  Clipboard();

  /**
   * Virtual destructor.
   */
  virtual ~Clipboard();

  /**
   * This constructor is used by Adaptor::GetClipboard().
   * @param[in] clipboard A pointer to the clipboard.
   */
  Clipboard( Internal::Adaptor::Clipboard* clipboard );

  /**
   * Retrieve a handle to the ClipboardEventNotifier instance
   * @return A handle to the Clipboard
   */
  static Clipboard Get();

  /**
   * Send the given string to the clipboard
   * @param[in] itemData string to send to clip board
   * @return bool true if the internal clip board sending was successful.
   */
  bool SetItem( const std::string& itemData );

  /**
   * Retreive the string at the given index in the clip board.
   * @param[in] index item in clipboard list to retrieve
   * @return string the text item at the current index.
   */
  std::string GetItem( unsigned int index );

  /**
   * Returns the number of item currently in the clipboard
   * @return unsigned int number of clipboard items
   */
  unsigned int NumberOfItems();

  /**
   * Show the clipboard window
   */
  void ShowClipboard();

  /**
   * Hide the clipboard window
   */
  void HideClipboard();

};
} // namespace Dali

#endif // __TOOLKIT_CLIPBOARD_H__
