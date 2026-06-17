#ifndef TOOLKIT_CLIPBOARD_H
#define TOOLKIT_CLIPBOARD_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// Prevent real clipboard headers from being included
#define DALI_CLIPBOARD_H
#define DALI_CLIPBOARD_DATA_H
#define DALI_INTEGRATION_CLIPBOARD_INTEG_H

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief ClipboardData stores clipboard MIME type and content.
 *
 * Fake implementation for test purposes.
 */
class ClipboardData
{
public:
  ClipboardData();
  ClipboardData(const Dali::String& mimeType, const Dali::String& content);
  ClipboardData(const ClipboardData& rhs);
  ClipboardData(ClipboardData&& rhs) noexcept;
  ClipboardData& operator=(const ClipboardData& rhs);
  ClipboardData& operator=(ClipboardData&& rhs) noexcept;
  ~ClipboardData();

  void SetMimeType(const Dali::String& mimeType);
  Dali::String GetMimeType() const;

  void SetContent(const Dali::String& content);
  Dali::String GetContent() const;

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

namespace Internal DALI_INTERNAL
{

namespace Adaptor
{
class Clipboard;
}
} //namespace Internal DALI_INTERNAL

/**
 * The Clipboard can operate using various funtion.
 * Clipboard can manage it's item and set show / hide status.
 */
class Clipboard : public BaseHandle
{
public:
  using DataOfferedSignalType = Signal<void(const Dali::String&)>;

  /**
   * Create an uninitialized Clipboard;
   *  this can be initialized with one of the derived Clipboard' New() methods
   */
  Clipboard();

  /**
   * Non virtual destructor.
   */
  ~Clipboard();

  /**
   * This constructor is used by Adaptor::GetClipboard().
   * @param[in] clipboard A pointer to the clipboard.
   */
  Clipboard(Internal::Adaptor::Clipboard* clipboard);

  /**
   * Retrieve a handle to the ClipboardEventNotifier instance
   * @return A handle to the Clipboard
   */
  static Clipboard Get();

  /**
   * @brief Send the given data to the clipboard.
   * @param[in] data data to send to the clipboard
   * @return bool true if the internal clipboard sending was successful.
   */
  bool SetData(const ClipboardData& data);

  /**
   * @brief Check if there is data in the clipboard with a given mime type.
   * @param[in] mimeType mime type to search for.
   * @return bool true if there is data, otherwise false.
   */
  bool HasType(const Dali::String& mimeType);

  /**
   * @brief Request data from the clipboard.
   * @param[in] mimeType mime type of data to request.
   * @param[in] callback Callback to receive the data. Ownership is transferred.
   */
  void GetData(const Dali::String& mimeType, CallbackBase* callback);

  /**
   * @brief Signal emitted when clipboard data is offered.
   */
  DataOfferedSignalType& DataOfferedSignal();
};

} // namespace Dali DALI_IMPORT_API

namespace Dali
{
namespace Integration
{
namespace Clipboard
{

using DataSentSignalType     = Signal<void(const char*, const char*)>;
using DataReceivedSignalType = Signal<void(uint32_t, const char*, const char*)>;
using DataSelectedSignalType = Signal<void(const char*)>;

bool IsAvailable();

DataSentSignalType& DataSentSignal(Dali::Clipboard& clipboard);

DataReceivedSignalType& DataReceivedSignal(Dali::Clipboard& clipboard);

DataSelectedSignalType& DataSelectedSignal(Dali::Clipboard& clipboard);

uint32_t GetData(Dali::Clipboard& clipboard, const Dali::String& mimeType);

uint32_t GetItemCount(Dali::Clipboard& clipboard);

void ShowClipboard(Dali::Clipboard& clipboard);

void HideClipboard(Dali::Clipboard& clipboard, bool skipFirstHide = false);

bool IsVisible(const Dali::Clipboard& clipboard);

} // namespace Clipboard
} // namespace Integration
} // namespace Dali

#endif // TOOLKIT_CLIPBOARD_H
