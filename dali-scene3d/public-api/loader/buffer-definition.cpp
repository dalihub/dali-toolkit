/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/buffer-definition.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali::Scene3D::Loader
{
namespace
{
static constexpr std::string_view EMBEDDED_DATA_PREFIX                 = "data:";
static constexpr std::string_view EMBEDDED_DATA_APPLICATION_MEDIA_TYPE = "application/";
static constexpr std::string_view EMBEDDED_DATA_BASE64_ENCODING_TYPE   = "base64,";
} // namespace

struct BufferDefinition::Impl
{
  std::vector<uint8_t>              buffer;
  std::shared_ptr<Dali::FileStream> stream;
};

BufferDefinition::BufferDefinition(std::vector<uint8_t>&& buffer)
: mImpl{new BufferDefinition::Impl}
{
  mImpl->buffer = std::move(buffer);
  mImpl->stream = std::make_shared<Dali::FileStream>(reinterpret_cast<uint8_t*>(mImpl->buffer.data()), mImpl->buffer.size(), FileStream::READ | FileStream::BINARY);
  mIsEmbedded   = true;
}

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
  return mImpl->stream.get()->GetStream();
}

Dali::String BufferDefinition::GetUri()
{
  std::string result = ToStdString(mResourcePath) + ((mIsEmbedded) ? std::string() : ToStdString(mUri));
  return ToDaliString(result);
}

bool BufferDefinition::IsAvailable()
{
  LoadBuffer();
  return mImpl->stream != nullptr;
}

void BufferDefinition::LoadBuffer()
{
  if(mImpl->stream == nullptr)
  {
    std::string uri = ToStdString(mUri);
    if(uri.find(EMBEDDED_DATA_PREFIX.data()) == 0 && uri.find(EMBEDDED_DATA_APPLICATION_MEDIA_TYPE.data(), EMBEDDED_DATA_PREFIX.length()) == EMBEDDED_DATA_PREFIX.length())
    {
      auto position = uri.find(EMBEDDED_DATA_BASE64_ENCODING_TYPE.data(), EMBEDDED_DATA_PREFIX.length() + EMBEDDED_DATA_APPLICATION_MEDIA_TYPE.length());
      if(position != std::string::npos)
      {
        position += EMBEDDED_DATA_BASE64_ENCODING_TYPE.length();
        std::string_view data = std::string_view(uri).substr(position);
        mImpl->buffer.clear();
        Dali::Toolkit::DecodeBase64FromString(data, mImpl->buffer);
        mImpl->stream = std::make_shared<Dali::FileStream>(reinterpret_cast<uint8_t*>(mImpl->buffer.data()), mByteLength, FileStream::READ | FileStream::BINARY);
        mIsEmbedded   = true;
      }
    }
    else
    {
      std::string fullPath = ToStdString(mResourcePath) + uri;
      mImpl->stream        = std::make_shared<Dali::FileStream>(fullPath, FileStream::READ | FileStream::BINARY);
      if(mImpl->stream == nullptr)
      {
        DALI_LOG_ERROR("Failed to load %s\n", fullPath.c_str());
      }
    }
  }
}

} // namespace Dali::Scene3D::Loader
