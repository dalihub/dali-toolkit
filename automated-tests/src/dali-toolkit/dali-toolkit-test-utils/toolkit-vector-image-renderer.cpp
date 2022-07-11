/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>
#include <sys/stat.h>
#include <toolkit-application.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-vector-image-renderer.h>
#include <cstring>
#include <memory>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
namespace
{
Dali::Internal::Adaptor::VectorImageRenderer* gVectorImageRenderer = nullptr;

/**
 * @brief Check whether the data contain keyword as substring or not.
 * It can be used as Naive Check-up to determine vector image loaded successfully or not.
 *
 * @param[in] data Raw data
 * @param[in] keyword Keyword to check data holded or not.
 * @return True if data contain keyword.
 */
bool CheckKeywordExist(const Vector<uint8_t>& data, std::string keyword)
{
  std::string trimedString;

  // Remove useless character in data.
  for(const uint8_t& it : data)
  {
    if(keyword.find(it) != std::string::npos)
    {
      trimedString.push_back(it);
    }
  }

  if(trimedString.length() >= keyword.length())
  {
    if(trimedString.find(keyword) != std::string::npos)
    {
      return true;
    }
  }
  return false;
}
} // namespace

class VectorImageRenderer : public Dali::BaseObject
{
public:
  VectorImageRenderer()
  {
  }

  ~VectorImageRenderer()
  {
  }

  bool Load(const Vector<uint8_t>& data, float dpi)
  {
    mLoadCount++;
    if(strncmp(reinterpret_cast<char*>(data.Begin()), "invalid", 7) == 0)
    {
      return false;
    }
    else if(strncmp(reinterpret_cast<char*>(data.Begin()), "Fail to rasterize", strlen("Fail to rasterize")) == 0)
    {
      mRasterizeSuccess = false;
    }
    // Naive check-up whether data is valid format or not. Currently we only check svg and tvg file format.
    else if(!CheckKeywordExist(data, "</svg>") && !CheckKeywordExist(data, "ThorVG"))
    {
      return false;
    }
    mLoadSuccess = true;
    return true;
  }

  bool IsLoaded() const
  {
    return mLoadSuccess;
  }

  Dali::Devel::PixelBuffer Rasterize(uint32_t width, uint32_t height)
  {
    if(mRasterizeSuccess)
    {
      Devel::PixelBuffer pixelBuffer = Devel::PixelBuffer::New(width, height, Dali::Pixel::RGBA8888);
      return pixelBuffer;
    }
    return Dali::Devel::PixelBuffer();
  }

  void GetDefaultSize(uint32_t& width, uint32_t& height) const
  {
    width  = 100;
    height = 100;
  }

public:
  uint32_t mWidth{0};
  uint32_t mHeight{0};
  uint32_t mLoadCount{0};
  bool     mLoadSuccess{false};
  bool     mRasterizeSuccess{true};
};

inline VectorImageRenderer& GetImplementation(Dali::VectorImageRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorImageRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::VectorImageRenderer&>(handle);
}

inline const VectorImageRenderer& GetImplementation(const Dali::VectorImageRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorImageRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::VectorImageRenderer&>(handle);
}

} // namespace Adaptor

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorImageRenderer VectorImageRenderer::New()
{
  Internal::Adaptor::VectorImageRenderer* imageRenderer = new Internal::Adaptor::VectorImageRenderer();

  Internal::Adaptor::gVectorImageRenderer = imageRenderer;

  return VectorImageRenderer(imageRenderer);
}

VectorImageRenderer::VectorImageRenderer()
{
}

VectorImageRenderer::~VectorImageRenderer()
{
}

VectorImageRenderer::VectorImageRenderer(Internal::Adaptor::VectorImageRenderer* internal)
: BaseHandle(internal)
{
}

bool VectorImageRenderer::Load(const Vector<uint8_t>& data, float dpi)
{
  return Internal::Adaptor::GetImplementation(*this).Load(data, dpi);
}

bool VectorImageRenderer::IsLoaded() const
{
  return Internal::Adaptor::GetImplementation(*this).IsLoaded();
}

Dali::Devel::PixelBuffer VectorImageRenderer::Rasterize(uint32_t width, uint32_t height)
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize(width, height);
}

void VectorImageRenderer::GetDefaultSize(uint32_t& width, uint32_t& height) const
{
  Internal::Adaptor::GetImplementation(*this).GetDefaultSize(width, height);
}

} // namespace Dali

namespace Test
{
namespace VectorImageRenderer
{
uint32_t GetLoadCount()
{
  return Dali::Internal::Adaptor::gVectorImageRenderer->mLoadCount;
}

} // namespace VectorImageRenderer
} // namespace Test
