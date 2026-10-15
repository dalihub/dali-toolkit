/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/native-image-queue.h>

// INTERNAL INCLUDES
#include "toolkit-native-image-queue.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

namespace DALI_NAMESPACE
{

const char* NativeImageQueueTest::GetCustomFragmentPrefix()
{
  return "#extension GL_OES_EGL_image_external:require\n";
}

NativeImageQueuePtr NativeImageQueue::New(uint32_t width, uint32_t height, ColorFormat colorFormat)
{
  Any                 empty;
  NativeImageQueuePtr image = new NativeImageQueue(0, width, height, colorFormat, empty);
  return image;
}

NativeImageQueuePtr NativeImageQueue::New(uint32_t queueCount, uint32_t width, uint32_t height, ColorFormat colorFormat)
{
  Any                 empty;
  NativeImageQueuePtr image = new NativeImageQueue(queueCount, width, height, colorFormat, empty);
  return image;
}

NativeImageQueuePtr NativeImageQueue::New(Any nativeImageQueue)
{
  // ColorFormat will be ignored.
  NativeImageQueuePtr image = new NativeImageQueue(0, 0, 0, ColorFormat::BGRA8888, nativeImageQueue);
  return image;
}

Any NativeImageQueue::GetNativeImageQueue()
{
  Any nativeImageQueue;
  return nativeImageQueue;
}

void NativeImageQueue::SetSize(uint32_t width, uint32_t height)
{
}

void NativeImageQueue::IgnoreSourceImage()
{
}

bool NativeImageQueue::CanDequeueBuffer()
{
  return false;
}

uint8_t* NativeImageQueue::DequeueBuffer(uint32_t& width, uint32_t& height, uint32_t& stride)
{
  return nullptr;
}

uint8_t* NativeImageQueue::DequeueBuffer(uint32_t& width, uint32_t& height, uint32_t& stride, BufferAccessType type)
{
  return nullptr;
}

bool NativeImageQueue::EnqueueBuffer(uint8_t* buffer)
{
  return false;
}

void NativeImageQueue::CancelDequeuedBuffer(uint8_t* buffer)
{
}

void NativeImageQueue::FreeReleasedBuffers()
{
}

void NativeImageQueue::SetQueueUsageHint(QueueUsageType type)
{
}

bool NativeImageQueue::CreateResource()
{
  return false;
}

void NativeImageQueue::DestroyResource()
{
}

uint32_t NativeImageQueue::TargetTexture()
{
  return 0;
}

Dali::NativeImageInterface::PrepareTextureResult NativeImageQueue::PrepareTexture()
{
  return Dali::NativeImageInterface::PrepareTextureResult::NO_ERROR;
}

uint32_t NativeImageQueue::GetQueueCount() const
{
  return 0;
}

uint32_t NativeImageQueue::GetWidth() const
{
  return 0;
}

uint32_t NativeImageQueue::GetHeight() const
{
  return 0;
}

bool NativeImageQueue::RequiresBlending() const
{
  return false;
}

int NativeImageQueue::GetTextureTarget() const
{
  return 0;
}

bool NativeImageQueue::ApplyNativeFragmentShader(String& shader, int mask)
{
  shader = String(Dali::NativeImageQueueTest::GetCustomFragmentPrefix()) + shader;
  return true;
}

const char* NativeImageQueue::GetCustomSamplerTypename() const
{
  return "";
}

Any NativeImageQueue::GetNativeImageHandle() const
{
  return nullptr;
}

bool NativeImageQueue::SourceChanged() const
{
  return false;
}

Rect<uint32_t> NativeImageQueue::GetUpdatedArea()
{
  return Rect<uint32_t>(0, 0, 0, 0);
}

void NativeImageQueue::PostRender()
{
}

NativeImageInterface::Extension* NativeImageQueue::GetExtension()
{
  return nullptr;
}

NativeImageQueue::NativeImageQueue(uint32_t queueCount, uint32_t width, uint32_t height, ColorFormat colorFormat, Any nativeImageQueue)
{
}

NativeImageQueue::~NativeImageQueue()
{
}

} //namespace DALI_NAMESPACE
