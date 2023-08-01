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

#include "toolkit-clipboard.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Implementation of the Clipboard
 */

class Clipboard :  public Dali::BaseObject
{
public:

  /**
   * @copydoc Dali::ClipboardEventNotifier::Get()
   */
  static Dali::Clipboard Get();

  /**
   * Constructor
   * @param[in] ecoreXwin, The window is created by application.
   */
  Clipboard(/*Ecore_X_Window ecoreXwin*/);
  virtual ~Clipboard();

  /**
   * @copydoc Dali::Clipboard::IsAvailable()
   */
  static bool IsAvailable();

  /**
   * @copydoc Dali::Clipboard::DataSentSignal()
   */
  Dali::Clipboard::DataSentSignalType& DataSentSignal();

  /**
   * @copydoc Dali::Clipboard::DataReceivedSignal()
   */
  Dali::Clipboard::DataReceivedSignalType& DataReceivedSignal();

  /**
   * @copydoc Dali::Clipboard::SetData()
   */
  bool SetData(const Dali::Clipboard::ClipData& clipData);

  /**
   * @copydoc Dali::Clipboard::GetData()
   */
  uint32_t GetData(const std::string &mimeType);

  /**
   * @copydoc Dali::Clipboard::NumberOfClipboardItems()
   */
  size_t NumberOfItems();

  /**
   * @copydoc Dali::Clipboard::ShowClipboard()
   */
  void ShowClipboard();

  /**
   * @copydoc Dali::Clipboard::HideClipboard()
   */
  void HideClipboard();

  /**
  * @copydoc Dali::Clipboard::IsVisible()
  */
  bool IsVisible() const;

private:
  Clipboard( const Clipboard& );
  Clipboard& operator=( Clipboard& );

  static Dali::Clipboard mToolkitClipboard;

  bool        mVisible;
  std::string mMimeType;
  std::string mData;
  uint32_t    mDataId{0};
  size_t      mCount;

  Dali::Clipboard::DataSentSignalType     mDataSentSignal;
  Dali::Clipboard::DataReceivedSignalType mDataReceivedSignal;
}; // class clipboard


Dali::Clipboard Dali::Internal::Adaptor::Clipboard::mToolkitClipboard;


Clipboard::Clipboard()
{
  mVisible = false;
  mCount = 0u;
}

Clipboard::~Clipboard()
{
}

Dali::Clipboard Clipboard::Get()
{
  if(!mToolkitClipboard)
  {
    mToolkitClipboard = Dali::Clipboard(new Dali::Internal::Adaptor::Clipboard());
  }
  return mToolkitClipboard;
}

bool Clipboard::IsAvailable()
{
  if(mToolkitClipboard)
  {
    return true;
  }
  return false;
}

Dali::Clipboard::DataSentSignalType& Clipboard::DataSentSignal()
{
  return mDataSentSignal;
}

Dali::Clipboard::DataReceivedSignalType& Clipboard::DataReceivedSignal()
{
  return mDataReceivedSignal;
}

bool Clipboard::SetData(const Dali::Clipboard::ClipData& clipData)
{
  mMimeType = clipData.GetMimeType();
  mData     = clipData.GetData();
  mCount    = 1u;

  if(mData.empty())
  {
    return false;
  }

  mDataSentSignal.Emit(mMimeType.c_str(), mData.c_str());

  return true;
}

uint32_t Clipboard::GetData(const std::string &mimeType)
{
  if(!mMimeType.compare(mimeType.c_str()))
  {
    mDataId++;
    mDataReceivedSignal.Emit(mDataId, mMimeType.c_str(), mData.c_str());
    return mDataId;
  }
  return 0u;
}

size_t Clipboard::NumberOfItems()
{
  return mCount;
}

void Clipboard::ShowClipboard()
{
  mVisible = true;
}

void Clipboard::HideClipboard()
{
  mVisible = false;
}

bool Clipboard::IsVisible() const
{
  return mVisible;
}

} // namespace Adaptor

} // namespace Internal


inline static Internal::Adaptor::Clipboard& GetImplementation(Dali::Clipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::Clipboard theClipboard = Dali::Clipboard::Get();
  BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<Internal::Adaptor::Clipboard&>(object);
}

inline static const  Internal::Adaptor::Clipboard& GetImplementation(const Dali::Clipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::Clipboard theClipboard = Dali::Clipboard::Get();
  const BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<const Internal::Adaptor::Clipboard&>(object);
}


Clipboard::Clipboard()
{
}
Clipboard::~Clipboard()
{
}
Clipboard::Clipboard(Internal::Adaptor::Clipboard *impl)
  : BaseHandle(impl)
{
}

Clipboard Clipboard::Get()
{
  return Internal::Adaptor::Clipboard::Get();
}

size_t Clipboard::NumberOfItems()
{
  return GetImplementation(*this).NumberOfItems();
}

void Clipboard::ShowClipboard()
{
  GetImplementation(*this).ShowClipboard();
}

void Clipboard::HideClipboard()
{
  GetImplementation(*this).HideClipboard();
}

bool Clipboard::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

} // namespace Dali
