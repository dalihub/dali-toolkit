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
#include "test-media-frame-source.h"

namespace
{
/**
 * @brief Connects the unit-test source without a platform MMFW dependency.
 */
class TestSourcePlugin : public Dali::Integration::MediaFrameSourcePlugin
{
public:
  TestSourcePlugin() = default;

  ~TestSourcePlugin() override
  {
    Detach();
  }

  TestSourcePlugin(const TestSourcePlugin&)            = delete;
  TestSourcePlugin& operator=(const TestSourcePlugin&) = delete;

  bool Attach(Dali::Any source, Dali::Integration::MediaFrameConsumer& consumer) override
  {
    Detach();
    if(!source.IsType<TestMediaFrameSource*>())
    {
      return false;
    }
    auto* testSource = source.Get<TestMediaFrameSource*>();
    if(!testSource || testSource->consumer)
    {
      return false;
    }
    mSource           = testSource;
    mSource->consumer = &consumer;
    return true;
  }

  void Detach() override
  {
    if(mSource)
    {
      mSource->consumer = nullptr;
      mSource           = nullptr;
    }
  }

private:
  TestMediaFrameSource* mSource{nullptr};
};
} //namespace

extern "C" DALI_EXPORT_API Dali::Integration::MediaFrameSourcePlugin* CreateMediaFrameSourcePlugin()
{
  return new TestSourcePlugin;
}

extern "C" DALI_EXPORT_API void DestroyMediaFrameSourcePlugin(Dali::Integration::MediaFrameSourcePlugin* plugin)
{
  delete plugin;
}
