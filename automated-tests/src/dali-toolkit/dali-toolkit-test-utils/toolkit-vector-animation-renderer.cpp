/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-array.h>
#include <toolkit-application.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-vector-animation-renderer.h>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
namespace
{
Dali::Internal::Adaptor::VectorAnimationRenderer* gVectorAnimationRenderer = nullptr;
}

class VectorAnimationRenderer : public Dali::BaseObject
{
public:
  VectorAnimationRenderer()
  : mUrl(),
    mRenderer(),
    mWidth(0),
    mHeight(0),
    mDefaultWidth(0),
    mDefaultHeight(0),
    mTotalFrameNumber(VECTOR_ANIMATION_TOTAL_FRAME_NUMBER),
    mPreviousFrame(0),
    mDelayTime(0),
    mDroppedFrames(0),
    mFrameRate(60.0f),
    mTestFrameDrop(false),
    mNeedDroppedFrames(false),
    mUseNativeImage(false),
    mEventThreadCallback(new EventThreadCallback(MakeCallback(this, &VectorAnimationRenderer::OnTriggered)))
  {
    mCount++;

    if(mCount == 2)
    {
      mFrameRate = 0.1f;
    }
  }

  ~VectorAnimationRenderer()
  {
    mCount--;
  }

  bool Load(const std::string& url)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mUrl = url;
    if(mUrl == "invalid.json")
    {
      mLoadFailed = true;
      return false;
    }
    else if(mUrl == "framedrop.json")
    {
      // Change total frame number for test
      mTotalFrameNumber = 200;
    }

    mDefaultWidth  = 100;
    mDefaultHeight = 100;

    return true;
  }

  bool Load(const Dali::Vector<uint8_t>& data)
  {
    Dali::Mutex::ScopedLock lock(mMutex);

    mDefaultWidth  = 100;
    mDefaultHeight = 100;

    return true;
  }

  void SetRenderer(Dali::Renderer renderer)
  {
    mRenderer = renderer;
  }

  void SetSize(uint32_t width, uint32_t height)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mWidth  = width;
    mHeight = height;

    if(!mLoadFailed)
    {
      mNeedTrigger   = true;
      mResourceReady = false;
    }
  }

  bool Render(uint32_t frameNumber)
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    if(mWidth == 0 || mHeight == 0)
    {
      return false;
    }

    if(mTestFrameDrop)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int32_t>(mDelayTime)));
      mTestFrameDrop     = false;
      mNeedDroppedFrames = true;
    }
    else if(mNeedDroppedFrames)
    {
      mDroppedFrames     = (frameNumber > mPreviousFrame) ? frameNumber - mPreviousFrame - 1 : frameNumber + (mTotalFrameNumber - mPreviousFrame) - 1;
      mNeedTrigger       = true;
      mNeedDroppedFrames = false;
    }

    if(!mDynamicPropertyCallbacks.empty())
    {
      for(auto&& dynamicPropertyCallbackPair : mDynamicPropertyCallbacks)
      {
        CallbackBase::ExecuteReturn<Property::Value>(*dynamicPropertyCallbackPair.second, dynamicPropertyCallbackPair.first, 0, frameNumber);
      }
    }

    if(mNeedTrigger)
    {
      mEventThreadCallback->Trigger();
      mNeedTrigger = false;
    }

    if(frameNumber == 1 && mPreviousFrame != frameNumber)
    {
      mPreviousFrame = frameNumber;
      // For test corverage
      return false;
    }
    mPreviousFrame = frameNumber;
    return true;
  }

  uint32_t GetTotalFrameNumber() const
  {
    return mTotalFrameNumber;
  }

  float GetFrameRate() const
  {
    return mFrameRate;
  }

  void GetDefaultSize(uint32_t& width, uint32_t& height) const
  {
    width  = mDefaultWidth;
    height = mDefaultHeight;
  }

  bool GetMarkerInfo(const std::string& marker, uint32_t& startFrame, uint32_t& endFrame) const
  {
    if(marker.compare(VECTOR_ANIMATION_MARKER_NAME_1) == 0)
    {
      startFrame = VECTOR_ANIMATION_MARKER_START_FRAME_1;
      endFrame   = VECTOR_ANIMATION_MARKER_END_FRAME_1;
    }
    else if(marker.compare(VECTOR_ANIMATION_MARKER_NAME_2) == 0)
    {
      startFrame = VECTOR_ANIMATION_MARKER_START_FRAME_2;
      endFrame   = VECTOR_ANIMATION_MARKER_END_FRAME_2;
    }
    else
    {
      return false;
    }
    return true;
  }

  void InvalidateBuffer()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    if(mResourceReady)
    {
      mNeedTrigger   = true;
      mResourceReady = false;
    }
  }

  void AddPropertyValueCallback(const std::string& keyPath, Dali::VectorAnimationRenderer::VectorProperty property, CallbackBase* callback, int32_t id)
  {
    mDynamicPropertyCallbacks.emplace_back(id, std::unique_ptr<CallbackBase>(callback));
  }

  void KeepRasterizedBuffer()
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mEnableFixedCache = true;
  }

  Dali::VectorAnimationRenderer::UploadCompletedSignalType& UploadCompletedSignal()
  {
    return mUploadCompletedSignal;
  }

  void OnTriggered()
  {
    if(!mResourceReady)
    {
      mResourceReady = true;

      Dali::TextureSet textureSet = mRenderer.GetTextures();

      if(mUseNativeImage)
      {
        Dali::NativeImageSourcePtr nativeImageSource = Dali::NativeImageSource::New(mWidth, mHeight, Dali::NativeImageSource::COLOR_DEPTH_32);
        Dali::Texture              texture           = Dali::Texture::New(*nativeImageSource);
        textureSet.SetTexture(0, texture);
      }
      else
      {
        Dali::Texture texture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, mWidth, mHeight);
        textureSet.SetTexture(0, texture);

        Devel::PixelBuffer pixelBuffer = Devel::PixelBuffer::New(mWidth, mHeight, Pixel::RGBA8888);
        Dali::PixelData    pixelData   = Devel::PixelBuffer::Convert(pixelBuffer);
        texture.Upload(pixelData);
      }

      mUploadCompletedSignal.Emit();
    }
  }

public:
  static uint32_t mCount;

  std::string                                                    mUrl;
  Dali::Renderer                                                 mRenderer;
  Dali::Mutex                                                    mMutex;
  std::vector<std::pair<int32_t, std::unique_ptr<CallbackBase>>> mDynamicPropertyCallbacks;

  uint32_t mWidth;
  uint32_t mHeight;
  uint32_t mDefaultWidth;
  uint32_t mDefaultHeight;
  uint32_t mTotalFrameNumber;
  uint32_t mPreviousFrame;
  uint32_t mDelayTime;
  uint32_t mDroppedFrames;
  float    mFrameRate;
  bool     mTestFrameDrop;
  bool     mNeedDroppedFrames;
  bool     mLoadFailed{false};
  bool     mResourceReady{false};
  bool     mNeedTrigger{true};
  bool     mEnableFixedCache{false};
  bool     mUseNativeImage{false};

  Dali::VectorAnimationRenderer::UploadCompletedSignalType mUploadCompletedSignal;
  std::unique_ptr<EventThreadCallback>                     mEventThreadCallback;
};

uint32_t VectorAnimationRenderer::mCount = 0;

inline VectorAnimationRenderer& GetImplementation(Dali::VectorAnimationRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorAnimationRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::VectorAnimationRenderer&>(handle);
}

inline const VectorAnimationRenderer& GetImplementation(const Dali::VectorAnimationRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorAnimationRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::VectorAnimationRenderer&>(handle);
}

} // namespace Adaptor

} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorAnimationRenderer VectorAnimationRenderer::New()
{
  Internal::Adaptor::VectorAnimationRenderer* animationRenderer = new Internal::Adaptor::VectorAnimationRenderer();

  Internal::Adaptor::gVectorAnimationRenderer = animationRenderer;

  return VectorAnimationRenderer(animationRenderer);
}

VectorAnimationRenderer::VectorAnimationRenderer()
{
}

VectorAnimationRenderer::~VectorAnimationRenderer()
{
}

VectorAnimationRenderer::VectorAnimationRenderer(Internal::Adaptor::VectorAnimationRenderer* internal)
: BaseHandle(internal)
{
}

VectorAnimationRenderer::VectorAnimationRenderer(const VectorAnimationRenderer& handle)
: BaseHandle(handle)
{
}

VectorAnimationRenderer& VectorAnimationRenderer::operator=(const VectorAnimationRenderer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void VectorAnimationRenderer::Finalize()
{
}

bool VectorAnimationRenderer::Load(const std::string& url)
{
  return Internal::Adaptor::GetImplementation(*this).Load(url);
}

bool VectorAnimationRenderer::Load(const Dali::Vector<uint8_t>& data)
{
  return Internal::Adaptor::GetImplementation(*this).Load(data);
}

void VectorAnimationRenderer::SetRenderer(Renderer renderer)
{
  Internal::Adaptor::GetImplementation(*this).SetRenderer(renderer);
}

void VectorAnimationRenderer::SetSize(uint32_t width, uint32_t height)
{
  Internal::Adaptor::GetImplementation(*this).SetSize(width, height);
}

bool VectorAnimationRenderer::Render(uint32_t frameNumber)
{
  return Internal::Adaptor::GetImplementation(*this).Render(frameNumber);
}

void VectorAnimationRenderer::RenderStopped()
{
}

uint32_t VectorAnimationRenderer::GetTotalFrameNumber() const
{
  return Internal::Adaptor::GetImplementation(*this).GetTotalFrameNumber();
}

float VectorAnimationRenderer::GetFrameRate() const
{
  return Internal::Adaptor::GetImplementation(*this).GetFrameRate();
}

void VectorAnimationRenderer::GetDefaultSize(uint32_t& width, uint32_t& height) const
{
  Internal::Adaptor::GetImplementation(*this).GetDefaultSize(width, height);
}

void VectorAnimationRenderer::GetLayerInfo(Property::Map& map) const
{
}

bool VectorAnimationRenderer::GetMarkerInfo(const std::string& marker, uint32_t& startFrame, uint32_t& endFrame) const
{
  return Internal::Adaptor::GetImplementation(*this).GetMarkerInfo(marker, startFrame, endFrame);
}

void VectorAnimationRenderer::GetMarkerInfo(Property::Map& map) const
{
  map.Add(VECTOR_ANIMATION_MARKER_NAME_1, Property::Array({VECTOR_ANIMATION_MARKER_START_FRAME_1, VECTOR_ANIMATION_MARKER_END_FRAME_1}));
  map.Add(VECTOR_ANIMATION_MARKER_NAME_2, Property::Array({VECTOR_ANIMATION_MARKER_START_FRAME_2, VECTOR_ANIMATION_MARKER_END_FRAME_2}));
}

void VectorAnimationRenderer::InvalidateBuffer()
{
  return Internal::Adaptor::GetImplementation(*this).InvalidateBuffer();
}

void VectorAnimationRenderer::AddPropertyValueCallback(const std::string& keyPath, VectorProperty property, CallbackBase* callback, int32_t id)
{
  Internal::Adaptor::GetImplementation(*this).AddPropertyValueCallback(keyPath, property, callback, id);
}

void VectorAnimationRenderer::KeepRasterizedBuffer()
{
  Internal::Adaptor::GetImplementation(*this).KeepRasterizedBuffer();
}

VectorAnimationRenderer::UploadCompletedSignalType& VectorAnimationRenderer::UploadCompletedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).UploadCompletedSignal();
}

} // namespace Dali

namespace Test
{
namespace VectorAnimationRenderer
{
void DelayRendering(uint32_t delay)
{
  Dali::Internal::Adaptor::gVectorAnimationRenderer->mDelayTime     = delay;
  Dali::Internal::Adaptor::gVectorAnimationRenderer->mTestFrameDrop = true;
}

uint32_t GetDroppedFrames()
{
  return Dali::Internal::Adaptor::gVectorAnimationRenderer->mDroppedFrames;
}

void UseNativeImageTexture(bool useNativeImage)
{
  Dali::Internal::Adaptor::gVectorAnimationRenderer->mUseNativeImage = useNativeImage;
}

} // namespace VectorAnimationRenderer
} // namespace Test
