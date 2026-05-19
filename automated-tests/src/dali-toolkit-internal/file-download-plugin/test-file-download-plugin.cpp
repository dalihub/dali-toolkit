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

#include <dali/devel-api/adaptor-framework/file-download-plugin.h>

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>

namespace
{

constexpr const char* TEST_REMOTE_FILE_DOWNLOAD_PATH = "DALI_TEST_REMOTE_FILE_DOWNLOAD_PATH";

class TestFileDownloadPlugin : public Dali::FileDownloadPlugin
{
public:
  bool InitializePlugin() override
  {
    return true;
  }

  bool DownloadRemoteFileIntoMemory(const std::string&     url,
                                    Dali::Vector<uint8_t>& dataBuffer,
                                    size_t&                dataSize,
                                    size_t                 maximumAllowedSizeBytes) override
  {
    return false;
  }
};

using AsyncCompletionCallback = void (*)(int downloadId, bool success, const char* filePath, void* userData);

} // unnamed namespace

extern "C" Dali::FileDownloadPlugin* CreateFileDownloadPlugin()
{
  return new TestFileDownloadPlugin;
}

extern "C" bool InitializeFileDownloadPlugin(Dali::FileDownloadPlugin* plugin)
{
  return plugin ? plugin->InitializePlugin() : false;
}

extern "C" void DestroyFileDownloadPlugin(Dali::FileDownloadPlugin* plugin)
{
  delete plugin;
}

extern "C" bool FileDownloadPluginIsAsyncDownloadSupported()
{
  return true;
}

extern "C" int FileDownloadPluginStartAsyncDownload(Dali::FileDownloadPlugin* plugin,
                                                    const char*               url,
                                                    size_t                    maxSize,
                                                    AsyncCompletionCallback   callback,
                                                    void*                     userData)
{
  static int  nextDownloadId = 0;
  const int   downloadId     = ++nextDownloadId;
  const char* filePath       = std::getenv(TEST_REMOTE_FILE_DOWNLOAD_PATH);

  if(callback)
  {
    std::thread([downloadId, filePath = std::string(filePath ? filePath : ""), callback, userData]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      callback(downloadId, !filePath.empty(), filePath.c_str(), userData);
    }).detach();
  }

  return downloadId;
}

extern "C" void FileDownloadPluginCancelAsyncDownload(Dali::FileDownloadPlugin* plugin, int downloadId)
{
}
