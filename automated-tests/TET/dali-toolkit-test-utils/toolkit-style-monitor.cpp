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

#include "toolkit-style-monitor.h"

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace
{
ToolkitStyleMonitor* gToolkitStyleMonitor(NULL);
} // unnamed namespace

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the StyleMonitor
 */
class StyleMonitor : public BaseObject
{
public: // Creation & Destruction

  static Dali::StyleMonitor Get();
  StyleMonitor();
  StyleMonitor(ToolkitStyleMonitor *styleMonitor);
  ~StyleMonitor();

public: // Style Information

  std::string GetDefaultFontFamily() const;
  float GetDefaultFontSize() const;

public: // Signals

  Dali::StyleMonitor::StyleChangeSignalV2& StyleChangeSignal();

  void EmitStyleChangeSignal(StyleChange styleChange)
  {
    mStyleChangeSignal.Emit(Dali::StyleMonitor(this), styleChange);
  }

private:

  Dali::StyleMonitor::StyleChangeSignalV2 mStyleChangeSignal;

  ToolkitStyleMonitor* mToolkitStyleMonitor;
};

Dali::StyleMonitor StyleMonitor::Get()
{
  return gToolkitStyleMonitor->GetStyleMonitor();
}

StyleMonitor::StyleMonitor()
: mToolkitStyleMonitor(NULL)
{
}

StyleMonitor::StyleMonitor(ToolkitStyleMonitor *styleMonitor)
: mToolkitStyleMonitor(styleMonitor)
{
}

StyleMonitor::~StyleMonitor()
{
}

std::string StyleMonitor::GetDefaultFontFamily() const
{
  mToolkitStyleMonitor->mFunctionsCalled.GetDefaultFontFamily = true;
  return ToolkitStyleMonitor::DEFAULT_FONT_FAMILY;
}

float StyleMonitor::GetDefaultFontSize() const
{
  mToolkitStyleMonitor->mFunctionsCalled.GetDefaultFontSize = true;
  return ToolkitStyleMonitor::DEFAULT_FONT_SIZE;
}

Dali::StyleMonitor::StyleChangeSignalV2& StyleMonitor::StyleChangeSignal()
{
  mToolkitStyleMonitor->mFunctionsCalled.SignalStyleChange = true;
  return mStyleChangeSignal;
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string ToolkitStyleMonitor::DEFAULT_FONT_FAMILY("DefaultFont");
const float       ToolkitStyleMonitor::DEFAULT_FONT_SIZE(1.0f);

ToolkitStyleMonitor::ToolkitStyleMonitor()
: mStyleMonitorStub(new Internal::Adaptor::StyleMonitor(this)),
  mStyleMonitor( mStyleMonitorStub )
{
  gToolkitStyleMonitor = this;
}

ToolkitStyleMonitor::~ToolkitStyleMonitor()
{
  gToolkitStyleMonitor = NULL;
}

StyleMonitor ToolkitStyleMonitor::GetStyleMonitor()
{
  return mStyleMonitor;
}

void ToolkitStyleMonitor::EmitSignalStyleChange(StyleChange styleChange)
{
  mStyleMonitorStub->EmitStyleChangeSignal(styleChange);
}

} // namespace Dali
