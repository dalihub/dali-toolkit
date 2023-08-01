#ifndef  TOOLKIT_CLIPBOARD_H
#define  TOOLKIT_CLIPBOARD_H

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

#define DALI_CLIPBOARD_H

// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

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
   * @brief Structure that contains information about the clipboard data information.
   */
  struct ClipData
  {
    ClipData(const char* mimeType = nullptr, const char* data = nullptr)
    {
      this->mimeType = mimeType;
      this->data     = data;
    }
    void SetMimeType(const char* mimeType)
    {
      this->mimeType = mimeType;
    }
    const char* GetMimeType() const
    {
      return mimeType;
    }
    void SetData(const char* data)
    {
      this->data = data;
    }
    const char* GetData() const
    {
      return data;
    }

  private:
    const char* mimeType {nullptr}; ///< The mime type of clip data.
    const char* data {nullptr};     ///< The clip data.
  };

  /// @brief Data send completed signal.
  typedef Signal<void(const char*, const char*)> DataSentSignalType;

  /// @brief Data receive completed signal.
  typedef Signal<void(uint32_t, const char*, const char*)> DataReceivedSignalType;


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
   * @brief Checks whether the clipboard is available.
   * @return true, if it is available, false otherwise.
   */
  static bool IsAvailable();

  /**
   * @brief This signal is emitted when the data send complete.
   * @note
   * SetData() opertion does not follow a synchronous call.
   * It follows the sequence below.
   * SetData() -> EcoreEventDataSend() -> SendData() -> DataSentSignal() Emit
   */
  DataSentSignalType& DataSentSignal();

  /**
   * @brief This signal is emitted when the data receive complete.
   * @note
   * GetData() opertion does not follow a synchronous call.
   * It follows the sequence below.
   * GetData() -> EcoreEventOfferDataReady() -> ReceiveData() -> DataReceivedSignal() Emit
   */
  DataReceivedSignalType& DataReceivedSignal();

  /**
   * @brief Send the given data to the clipboard.
   * @param[in] clipData data to send to the clipboard
   * @return bool true if the internal clipboard sending was successful.
   */
  bool SetData(const ClipData& clipData);

  /**
   * @brief Request data from the clipboard.
   * @param[in] mimeType mime type of data to request.
   * @return uint32_t Returns the data request id.
   */
  uint32_t GetData(const std::string& mimeType);

  /**
   * @brief Returns the number of item currently in the clipboard.
   * @return size_t number of clipboard items.
   */
  size_t NumberOfItems();

  /**
   * Show the clipboard window
   */
  void ShowClipboard();

  /**
   * Hide the clipboard window
   */
  void HideClipboard();

  /**
  * @brief Retrieves the clipboard's visibility
  * @return bool true if the clipboard is visible.
  */
  bool IsVisible() const;

};
} // namespace Dali

#endif // TOOLKIT_CLIPBOARD_H
