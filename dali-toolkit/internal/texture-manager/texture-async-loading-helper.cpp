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
#include <dali-toolkit/internal/texture-manager/texture-async-loading-helper.h>

// EXTERNAL HEADERS
#include <dali/devel-api/adaptor-framework/async-task-manager.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/adaptor-framework/file-download/remote-file-download-manager.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>
#include <dali-toolkit/internal/image-loader/remote-decode-task.h>
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>

// STD INCLUDES
#include <string>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gTextureManagerLogFilter; ///< Define at texture-manager-impl.cpp
#endif
namespace
{
constexpr size_t MAXIMUM_DOWNLOAD_IMAGE_SIZE = 50 * 1024 * 1024;

void SubmitRemoteDecodeTask(Dali::AsyncTaskManager                  asyncTaskManager,
                            TextureManager::TextureId               textureId,
                            const std::string&                      localFilePath,
                            const Dali::ImageDimensions&            desiredSize,
                            Dali::SamplingMode::Type                samplingMode,
                            bool                                    orientationCorrection,
                            DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                            bool                                    loadYuvPlanes,
                            CallbackBase*                           callback)
{
  if(DALI_UNLIKELY(!asyncTaskManager || !callback))
  {
    DALI_LOG_ERROR("TextureAsyncLoadingHelper: textureId[%d] download callback without valid decode context\n", textureId);
    delete callback;
    return;
  }

  RemoteDecodeTaskPtr remoteDecodeTask = new RemoteDecodeTask(textureId, localFilePath, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, callback);
  asyncTaskManager.AddTask(remoteDecodeTask);
}
}

TextureAsyncLoadingHelper::TextureAsyncLoadingHelper(TextureManager& textureManager)
: mTextureManager(textureManager),
  mLoadTaskId(0u)
{
}

void TextureAsyncLoadingHelper::LoadAnimatedImage(const TextureManager::TextureId                textureId,
                                                  Dali::AnimatedImageLoading                     animatedImageLoading,
                                                  const uint32_t                                 frameIndex,
                                                  const Dali::ImageDimensions&                   desiredSize,
                                                  const Dali::SamplingMode::Type                 samplingMode,
                                                  const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                                                  const bool                                     loadYuvPlanes)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex, desiredSize, samplingMode, preMultiplyOnLoad, loadYuvPlanes, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::Load(const TextureManager::TextureId                textureId,
                                     const VisualUrl&                               url,
                                     const Dali::ImageDimensions&                   desiredSize,
                                     const Dali::SamplingMode::Type                 samplingMode,
                                     const bool                                     orientationCorrection,
                                     const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                                     const bool                                     loadYuvPlanes)
{
  if(DALI_UNLIKELY(url.IsBufferResource()))
  {
    LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, mTextureManager.GetEncodedImageBuffer(url), desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
    loadingTask->SetTextureId(textureId);
    Dali::AsyncTaskManager::Get().AddTask(loadingTask);
    return;
  }

  // For remote URLs, route through RemoteFileDownloadManager when async download is supported.
  // This avoids blocking a Worker Thread on network I/O.
  if(!url.IsLocalResource() && Dali::RemoteFileDownloadManager::IsAsyncDownloadSupported())
  {
    DALI_LOG_DEBUG_INFO("TextureAsyncLoadingHelper::Load textureId[%d] via RemoteFileDownloadManager url[%s]\n", textureId, url.GetEllipsedUrl().c_str());

    TextureAsyncLoadingHelper* asyncLoadingHelper = this;
    Dali::AsyncTaskManager     asyncTaskManager   = Dali::AsyncTaskManager::Get();

    Dali::RemoteFileDownloadManager::Get().StartDownload(
      url.GetUrl(),
      MAXIMUM_DOWNLOAD_IMAGE_SIZE,
      [asyncLoadingHelper, asyncTaskManager, textureId, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes](bool success, const std::string& localFilePath) {
        // Called on the download completion thread. Only enqueue AsyncTaskManager tasks here.
        if(!success || localFilePath.empty())
        {
          DALI_LOG_DEBUG_INFO("TextureAsyncLoadingHelper: textureId[%d] download failed, notifying load failure\n", textureId);
          SubmitRemoteDecodeTask(asyncTaskManager, textureId, {}, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, MakeCallback(asyncLoadingHelper, &TextureAsyncLoadingHelper::RemoteDecodeComplete));
          return;
        }

        // Submit a decode-only LoadingTask using the locally cached file.
        DALI_LOG_DEBUG_INFO("TextureAsyncLoadingHelper: textureId[%d] download done, submitting decode task filePath[%s]\n", textureId, localFilePath.c_str());
        SubmitRemoteDecodeTask(asyncTaskManager, textureId, localFilePath, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, MakeCallback(asyncLoadingHelper, &TextureAsyncLoadingHelper::RemoteDecodeComplete));
      });
    return;
  }

  // Default: submit directly (Worker Thread will handle download synchronously for curl, or decode for local).
  DALI_LOG_DEBUG_INFO("TextureAsyncLoadingHelper::Load textureId[%d] via AsyncTaskManager url[%s]\n", textureId, url.GetEllipsedUrl().c_str());
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, url, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::ApplyMask(const TextureManager::TextureId                textureId,
                                          Devel::PixelBuffer                             pixelBuffer,
                                          Devel::PixelBuffer                             maskPixelBuffer,
                                          const float                                    contentScale,
                                          const bool                                     cropToMask,
                                          const DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad, MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::AsyncLoadComplete(LoadingTaskPtr task)
{
  // Call TextureManager::AsyncLoadComplete
  if(task->textureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.AsyncLoadComplete(task->textureId, task->pixelBuffers);
  }
}

void TextureAsyncLoadingHelper::RemoteDecodeComplete(Dali::AsyncTaskPtr asyncTask)
{
  auto* remoteDecodeTask = static_cast<RemoteDecodeTask*>(asyncTask.Get());
  if(remoteDecodeTask && remoteDecodeTask->textureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.AsyncLoadComplete(remoteDecodeTask->textureId, remoteDecodeTask->pixelBuffers);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
