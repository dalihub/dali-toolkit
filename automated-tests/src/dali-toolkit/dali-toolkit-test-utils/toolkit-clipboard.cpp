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
#include <memory>
#include <string>

namespace Dali
{

struct ClipboardData::Impl
{
  Impl() = default;

  Impl(const Dali::String& mimeType, const Dali::String& content)
  : mMimeType(mimeType),
    mContent(content)
  {
  }

  Dali::String mMimeType;
  Dali::String mContent;
};

// ClipboardData implementation

ClipboardData::ClipboardData()
: mImpl(Dali::MakeUnique<Impl>())
{
}

ClipboardData::ClipboardData(const Dali::String& mimeType, const Dali::String& content)
: mImpl(Dali::MakeUnique<Impl>(mimeType, content))
{
}

ClipboardData::ClipboardData(const ClipboardData& rhs)
: mImpl(rhs.mImpl ? Dali::MakeUnique<Impl>(*rhs.mImpl) : Dali::MakeUnique<Impl>())
{
}

ClipboardData::ClipboardData(ClipboardData&& rhs) noexcept = default;

ClipboardData& ClipboardData::operator=(const ClipboardData& rhs)
{
  if(this != &rhs)
  {
    mImpl = rhs.mImpl ? Dali::MakeUnique<Impl>(*rhs.mImpl) : Dali::MakeUnique<Impl>();
  }
  return *this;
}

ClipboardData& ClipboardData::operator=(ClipboardData&& rhs) noexcept = default;
ClipboardData::~ClipboardData() = default;

void ClipboardData::SetMimeType(const Dali::String& mimeType)
{
  if(!mImpl)
  {
    mImpl = Dali::MakeUnique<Impl>();
  }
  mImpl->mMimeType = mimeType;
}

Dali::String ClipboardData::GetMimeType() const
{
  return mImpl ? mImpl->mMimeType : Dali::String();
}

void ClipboardData::SetContent(const Dali::String& content)
{
  if(!mImpl)
  {
    mImpl = Dali::MakeUnique<Impl>();
  }
  mImpl->mContent = content;
}

Dali::String ClipboardData::GetContent() const
{
  return mImpl ? mImpl->mContent : Dali::String();
}

namespace Internal
{

namespace Adaptor
{

/**
 * Implementation of the Clipboard
 */

class Clipboard : public Dali::BaseObject
{
public:
  /**
   * @copydoc Dali::Clipboard::Get()
   */
  static Dali::Clipboard Get();

  /**
   * Constructor
   */
  Clipboard();
  virtual ~Clipboard();

  /**
   * @copydoc Dali::Clipboard::DataOfferedSignal()
   */
  Dali::Clipboard::DataOfferedSignalType& DataOfferedSignal();

  /**
   * Integration API signals
   */
  Dali::Integration::Clipboard::DataSentSignalType& DataSentSignal();

  Dali::Integration::Clipboard::DataReceivedSignalType& DataReceivedSignal();

  Dali::Integration::Clipboard::DataSelectedSignalType& DataSelectedSignal();

  /**
   * @copydoc Dali::Clipboard::HasType()
   */
  bool HasType(const std::string& mimeType);

  /**
   * @copydoc Dali::Clipboard::SetData()
   */
  bool SetData(const Dali::ClipboardData& clipboardData);

  /**
   * Integration::Clipboard::GetData
   */
  uint32_t GetData(const std::string& mimeType);

  /**
   * @brief Gets the stored mime type.
   */
  const std::string& GetMimeType() const;

  /**
   * @brief Gets the stored data content.
   */
  const std::string& GetDataContent() const;

  /**
   * Integration::Clipboard APIs
   */
  static bool IsAvailable();

  uint32_t GetItemCount();

  void ShowClipboard();

  void HideClipboard();

  bool IsVisible() const;

private:
  Clipboard(const Clipboard&);
  Clipboard& operator=(Clipboard&);

  static Dali::Clipboard mToolkitClipboard;

  bool        mVisible{false};
  std::string mMimeType;
  std::string mData;
  uint32_t    mDataId{0u};
  uint32_t    mCount{0u};

  Dali::Integration::Clipboard::DataSentSignalType     mDataSentSignal;
  Dali::Integration::Clipboard::DataReceivedSignalType mDataReceivedSignal;
  Dali::Clipboard::DataOfferedSignalType               mDataOfferedSignal;
  Dali::Integration::Clipboard::DataSelectedSignalType mDataSelectedSignal;
}; // class clipboard

Dali::Clipboard Dali::Internal::Adaptor::Clipboard::mToolkitClipboard;

Clipboard::Clipboard()
{
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

Dali::Clipboard::DataOfferedSignalType& Clipboard::DataOfferedSignal()
{
  return mDataOfferedSignal;
}

Dali::Integration::Clipboard::DataSelectedSignalType& Clipboard::DataSelectedSignal()
{
  return mDataSelectedSignal;
}

Dali::Integration::Clipboard::DataSentSignalType& Clipboard::DataSentSignal()
{
  return mDataSentSignal;
}

Dali::Integration::Clipboard::DataReceivedSignalType& Clipboard::DataReceivedSignal()
{
  return mDataReceivedSignal;
}

bool Clipboard::HasType(const std::string& mimeType)
{
  return mMimeType == mimeType ? true : false;
}

bool Clipboard::SetData(const Dali::ClipboardData& clipboardData)
{
  Dali::String mimeTypeString = clipboardData.GetMimeType();
  Dali::String contentString  = clipboardData.GetContent();

  mMimeType = mimeTypeString.CStr();
  mData     = contentString.CStr();

  if(mData.empty())
  {
    mCount = 0u;
    return false;
  }

  mCount = 1u;
  mDataSentSignal.Emit(mMimeType.c_str(), mData.c_str());

  return true;
}

uint32_t Clipboard::GetData(const std::string& mimeType)
{
  if(mMimeType == mimeType)
  {
    ++mDataId;
    mDataReceivedSignal.Emit(mDataId, mMimeType.c_str(), mData.c_str());

    return mDataId;
  }

  return 0u;
}

const std::string& Clipboard::GetMimeType() const
{
  return mMimeType;
}

const std::string& Clipboard::GetDataContent() const
{
  return mData;
}

bool Clipboard::IsAvailable()
{
  return static_cast<bool>(mToolkitClipboard);
}

uint32_t Clipboard::GetItemCount()
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
  BaseObject&     object       = theClipboard.GetBaseObject();
  return static_cast<Internal::Adaptor::Clipboard&>(object);
}

inline static const Internal::Adaptor::Clipboard& GetImplementation(const Dali::Clipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::Clipboard   theClipboard = Dali::Clipboard::Get();
  const BaseObject& object       = theClipboard.GetBaseObject();
  return static_cast<const Internal::Adaptor::Clipboard&>(object);
}

// Dali::Clipboard public wrapper implementation

Clipboard::Clipboard()
{
}
Clipboard::~Clipboard()
{
}
Clipboard::Clipboard(Internal::Adaptor::Clipboard* impl)
: BaseHandle(impl)
{
}

Clipboard Clipboard::Get()
{
  return Internal::Adaptor::Clipboard::Get();
}

bool Clipboard::SetData(const ClipboardData& data)
{
  return GetImplementation(*this).SetData(data);
}

bool Clipboard::HasType(const Dali::String& mimeType)
{
  return GetImplementation(*this).HasType(mimeType.CStr());
}

void Clipboard::GetData(const Dali::String& mimeType, CallbackBase* callback)
{
  if(!callback)
  {
    return;
  }

  std::unique_ptr<CallbackBase> callbackHolder(callback);

  Internal::Adaptor::Clipboard& impl = GetImplementation(*this);

  if(mimeType.Empty() || !impl.HasType(mimeType.CStr()))
  {
    Dali::ClipboardData emptyData;
    CallbackBase::Execute<bool, const Dali::ClipboardData&>(*callbackHolder, false, emptyData);
    return;
  }

  Dali::ClipboardData data(Dali::String(impl.GetMimeType().c_str()), Dali::String(impl.GetDataContent().c_str()));
  CallbackBase::Execute<bool, const Dali::ClipboardData&>(*callbackHolder, true, data);
}

Clipboard::DataOfferedSignalType& Clipboard::DataOfferedSignal()
{
  return GetImplementation(*this).DataOfferedSignal();
}

// Integration::Clipboard wrapper implementation

namespace Integration
{
namespace Clipboard
{

bool IsAvailable()
{
  return Dali::Internal::Adaptor::Clipboard::IsAvailable();
}

DataSentSignalType& DataSentSignal(Dali::Clipboard& clipboard)
{
  return GetImplementation(clipboard).DataSentSignal();
}

DataReceivedSignalType& DataReceivedSignal(Dali::Clipboard& clipboard)
{
  return GetImplementation(clipboard).DataReceivedSignal();
}

DataSelectedSignalType& DataSelectedSignal(Dali::Clipboard& clipboard)
{
  return GetImplementation(clipboard).DataSelectedSignal();
}

uint32_t GetData(Dali::Clipboard& clipboard, const Dali::String& mimeType)
{
  return GetImplementation(clipboard).GetData(mimeType.CStr());
}

uint32_t GetItemCount(Dali::Clipboard& clipboard)
{
  return GetImplementation(clipboard).GetItemCount();
}

void ShowClipboard(Dali::Clipboard& clipboard)
{
  GetImplementation(clipboard).ShowClipboard();
}

void HideClipboard(Dali::Clipboard& clipboard, bool skipFirstHide)
{
  GetImplementation(clipboard).HideClipboard();
}

bool IsVisible(const Dali::Clipboard& clipboard)
{
  return GetImplementation(clipboard).IsVisible();
}

} // namespace Clipboard
} // namespace Integration

} // namespace Dali
