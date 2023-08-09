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

// CLASS HEADER
#include <toolkit-texture-upload-manager.h>

// EXTERNAL INCLUDE
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/texture-integ.h>

// INTERNAL INCLUDE
#include <test-graphics-controller.h>
#include <toolkit-application.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class TextureUploadManager : public Dali::BaseObject
{
public:
  using ResourceId = Dali::Devel::TextureUploadManager::ResourceId;

public:
  static Dali::Devel::TextureUploadManager Get();
  TextureUploadManager();
  ~TextureUploadManager() = default;

  Dali::Texture GenerateTexture2D();

public: // Update thread called method
  bool ResourceUpload();

  void InitalizeGraphicsController(Dali::Graphics::Controller& graphicsController);

private:                                                             // Update thread called method
  using UploadRequestItem  = std::pair<ResourceId, Dali::PixelData>; ///< TODO : PixelData? PixelBuffer?
  using RequestUploadQueue = std::vector<UploadRequestItem>;

  bool ProcessUploadQueue(RequestUploadQueue&& queue);

public: // Worker thread called method
  /**
   * @copydoc Dali::Devel::TextureUploadManager::RequestUpload()
   */
  void RequestUpload(ResourceId id, Dali::PixelData pixelData);

private:
  Dali::Graphics::Controller* mGraphicsController;
  RequestUploadQueue          mRequestUploadQueue{};

public:
  ResourceId gUniqueResourceId{0u};
};

inline Internal::Adaptor::TextureUploadManager& GetImplementation(Dali::Devel::TextureUploadManager& obj)
{
  DALI_ASSERT_ALWAYS(obj && "TextureUploadManager is empty");

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::Adaptor::TextureUploadManager&>(handle);
}

inline const Internal::Adaptor::TextureUploadManager& GetImplementation(const Dali::Devel::TextureUploadManager& obj)
{
  DALI_ASSERT_ALWAYS(obj && "TextureUploadManager is empty");

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::Adaptor::TextureUploadManager&>(handle);
}

/********************************************************************************/
/*********************************  INTERNAL CLASS  *****************************/
/********************************************************************************/

namespace
{
Devel::TextureUploadManager gTextureUploadManager;
} // namespace

Devel::TextureUploadManager TextureUploadManager::Get()
{
  if(!gTextureUploadManager)
  {
    gTextureUploadManager = Devel::TextureUploadManager(new TextureUploadManager());
  }
  return gTextureUploadManager;
}

TextureUploadManager::TextureUploadManager()
: mGraphicsController{nullptr}
{
}

Dali::Texture TextureUploadManager::GenerateTexture2D()
{
  ResourceId resourceId = ++gUniqueResourceId;

  Dali::Texture ret = Dali::Integration::NewTextureWithResourceId(Dali::TextureType::TEXTURE_2D, resourceId);

  return ret;
}

// Called by update thread

bool TextureUploadManager::ResourceUpload()
{
  DALI_ASSERT_DEBUG(mGraphicsController && "GraphicsController is not prepared!");

  // Upload.
  bool uploaded = ProcessUploadQueue(std::move(mRequestUploadQueue));
  mRequestUploadQueue.clear();

  return uploaded;
}

void TextureUploadManager::InitalizeGraphicsController(Dali::Graphics::Controller& graphicsController)
{
  mGraphicsController = &graphicsController;
}

bool TextureUploadManager::ProcessUploadQueue(RequestUploadQueue&& queue)
{
  bool uploaded = false;

  if(!queue.empty())
  {
    for(auto& requests : queue)
    {
      auto& resourceId = requests.first;
      auto& pixelData  = requests.second;

      Graphics::Texture* graphicsTexture = nullptr;

      // TODO : Could we detect TEXTURE_2D or TEXTURE_CUBE case in future?
      {
        // We always need to create new one
        auto createInfo = Graphics::TextureCreateInfo();
        createInfo
          .SetTextureType(Dali::Graphics::ConvertTextureType(Dali::TextureType::TEXTURE_2D))
          .SetUsageFlags(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE))
          .SetFormat(Dali::Graphics::ConvertPixelFormat(pixelData.GetPixelFormat()))
          .SetSize({pixelData.GetWidth(), pixelData.GetHeight()})
          .SetLayout(Graphics::TextureLayout::LINEAR)
          .SetData(nullptr)
          .SetDataSize(0u)
          .SetNativeImage(nullptr)
          .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

        graphicsTexture = mGraphicsController->CreateTextureByResourceId(resourceId, createInfo);
      }

      if(graphicsTexture)
      {
        Graphics::TextureUpdateInfo info{};

        info.dstTexture   = graphicsTexture;
        info.dstOffset2D  = {0u, 0u};
        info.layer        = 0u;
        info.level        = 0u;
        info.srcReference = 0;
        info.srcExtent2D  = {pixelData.GetWidth(), pixelData.GetHeight()};
        info.srcOffset    = 0;
        info.srcSize      = Dali::Integration::GetPixelDataBuffer(pixelData).bufferSize;
        info.srcStride    = pixelData.GetStride();
        info.srcFormat    = Dali::Graphics::ConvertPixelFormat(pixelData.GetPixelFormat());

        Graphics::TextureUpdateSourceInfo updateSourceInfo{};
        updateSourceInfo.sourceType                = Graphics::TextureUpdateSourceInfo::Type::PIXEL_DATA;
        updateSourceInfo.pixelDataSource.pixelData = pixelData;

        mGraphicsController->UpdateTextures({info}, {updateSourceInfo});

        uploaded = true;
      }
      else
      {
        // Invalidate resouce id! ignore.
      }
    }

    if(uploaded)
    {
      // Flush here
      Graphics::SubmitInfo submitInfo;
      submitInfo.cmdBuffer.clear(); // Only flush
      submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
      mGraphicsController->SubmitCommandBuffers(submitInfo);
    }
  }

  return uploaded;
}

// Called by worker thread

void TextureUploadManager::RequestUpload(Dali::Devel::TextureUploadManager::ResourceId resourceId, Dali::PixelData pixelData)
{
  mRequestUploadQueue.push_back(std::move(UploadRequestItem(resourceId, pixelData)));
}

} // namespace Adaptor

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

namespace Devel
{
// Called by main thread

TextureUploadManager::TextureUploadManager() = default;

TextureUploadManager::~TextureUploadManager() = default;

TextureUploadManager TextureUploadManager::Get()
{
  return Internal::Adaptor::TextureUploadManager::Get();
}

Dali::Texture TextureUploadManager::GenerateTexture2D()
{
  return Internal::Adaptor::GetImplementation(*this).GenerateTexture2D();
}

TextureUploadManager::TextureUploadManager(Internal::Adaptor::TextureUploadManager* impl)
: BaseHandle(impl)
{
}

// Called by update thread

bool TextureUploadManager::ResourceUpload()
{
  return Internal::Adaptor::GetImplementation(*this).ResourceUpload();
}

// Called by worker thread

void TextureUploadManager::RequestUpload(ResourceId resourceId, PixelData pixelData)
{
  Internal::Adaptor::GetImplementation(*this).RequestUpload(resourceId, pixelData);
}

} // namespace Devel

} // namespace Dali

namespace Test
{
namespace TextureUploadManager
{
void InitalizeGraphicsController(Dali::Graphics::Controller& graphicsController)
{
  auto textureUploadManager = Dali::Devel::TextureUploadManager::Get();
  Internal::Adaptor::GetImplementation(textureUploadManager).InitalizeGraphicsController(graphicsController);
}
} // namespace TextureUploadManager
} // namespace Test
