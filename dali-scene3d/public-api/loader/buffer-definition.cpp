/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/buffer-definition.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
static constexpr std::string_view EMBEDDED_DATA_PREFIX                 = "data:";
static constexpr std::string_view EMBEDDED_DATA_APPLICATION_MEDIA_TYPE = "application/";
static constexpr std::string_view EMBEDDED_DATA_BASE64_ENCODING_TYPE   = "base64,";
} // namespace

struct BufferDefinition::Impl
{
  std::vector<uint8_t>             buffer;
  std::shared_ptr<Dali::FileStream> stream;
};

BufferDefinition::BufferDefinition()
: mImpl{new BufferDefinition::Impl}
{
}

BufferDefinition::~BufferDefinition() = default;

BufferDefinition::BufferDefinition(BufferDefinition&& other)
: mResourcePath(std::move(other.mResourcePath)),
  mUri(std::move(other.mUri)),
  mByteLength(std::move(other.mByteLength)),
  mName(std::move(other.mName)),
  mImpl(std::move(other.mImpl))
{
}

std::iostream& BufferDefinition::GetBufferStream()
{
  LoadBuffer();
  return mImpl.get()->stream.get()->GetStream();
}

std::string BufferDefinition::GetUri()
{
  return mResourcePath + ((mIsEmbedded) ? std::string() : mUri);
}

bool BufferDefinition::IsAvailable()
{
  LoadBuffer();
  return mImpl.get()->stream != nullptr;
}

void BufferDefinition::LoadBuffer()
{
  if(mImpl.get()->stream == nullptr)
  {
    if(mUri.find(EMBEDDED_DATA_PREFIX.data()) == 0 && mUri.find(EMBEDDED_DATA_APPLICATION_MEDIA_TYPE.data(), EMBEDDED_DATA_PREFIX.length()) == EMBEDDED_DATA_PREFIX.length())
    {
      uint32_t position = mUri.find(EMBEDDED_DATA_BASE64_ENCODING_TYPE.data(), EMBEDDED_DATA_PREFIX.length() + EMBEDDED_DATA_APPLICATION_MEDIA_TYPE.length());
      if(position != std::string::npos)
      {
        position += EMBEDDED_DATA_BASE64_ENCODING_TYPE.length();
        std::string data = mUri.substr(position);
        mImpl.get()->buffer.clear();
        Dali::Toolkit::DecodeBase64PropertyData(data, mImpl.get()->buffer);
        mImpl.get()->stream = std::make_shared<Dali::FileStream>(reinterpret_cast<uint8_t*>(mImpl.get()->buffer.data()), mByteLength, FileStream::READ | FileStream::BINARY);
        mIsEmbedded         = true;
      }
    }
    else
    {
      mImpl.get()->stream = std::make_shared<Dali::FileStream>(mResourcePath + mUri, FileStream::READ | FileStream::BINARY);
      if(mImpl.get()->stream == nullptr)
      {
        DALI_LOG_ERROR("Failed to load %s\n", (mResourcePath + mUri).c_str());
      }
    }
  }
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
