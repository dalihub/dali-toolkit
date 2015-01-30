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
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{


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
  ~StyleMonitor();

public: // Style Information
  std::string GetDefaultFontFamily() const;
  float GetDefaultFontSize() const;
  const std::string& GetTheme() const;
  void SetTheme(std::string theme);

public: // Signals
  Dali::StyleMonitor::StyleChangeSignalType& StyleChangeSignal();

  void EmitStyleChangeSignal(StyleChange styleChange)
  {
    mStyleChangeSignal.Emit(Dali::StyleMonitor(this), styleChange);
  }

private:
  Dali::StyleMonitor::StyleChangeSignalType mStyleChangeSignal;
  static Dali::StyleMonitor mToolkitStyleMonitor;
  std::string mTheme;
};

Dali::StyleMonitor StyleMonitor::mToolkitStyleMonitor;

Dali::StyleMonitor StyleMonitor::Get()
{
  if( ! mToolkitStyleMonitor )
  {
    mToolkitStyleMonitor = Dali::StyleMonitor( new Dali::Internal::Adaptor::StyleMonitor() );
  }
  return mToolkitStyleMonitor;
}

StyleMonitor::StyleMonitor()
: mTheme("default")
{
}

StyleMonitor::~StyleMonitor()
{
}

std::string StyleMonitor::GetDefaultFontFamily() const
{
  return Dali::StyleMonitor::DEFAULT_FONT_FAMILY;
}

float StyleMonitor::GetDefaultFontSize() const
{
  return Dali::StyleMonitor::DEFAULT_FONT_SIZE;
}

const std::string& StyleMonitor::GetTheme() const
{
  return mTheme;
}

void StyleMonitor::SetTheme(std::string path)
{
  StyleChange styleChange;
  styleChange.themeChange = true;
  styleChange.themeFilePath = path;
  mTheme = path;
  EmitStyleChangeSignal(styleChange);
}

Dali::StyleMonitor::StyleChangeSignalType& StyleMonitor::StyleChangeSignal()
{
  return mStyleChangeSignal;
}

} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

Internal::Adaptor::StyleMonitor& GetImplementation(Dali::StyleMonitor& monitor)
{
  BaseObject& object = monitor.GetBaseObject();
  return static_cast<Internal::Adaptor::StyleMonitor&>(object);
}
const Internal::Adaptor::StyleMonitor& GetImplementation(const Dali::StyleMonitor& monitor)
{
  const BaseObject& object = monitor.GetBaseObject();
  return static_cast<const Internal::Adaptor::StyleMonitor&>(object);
}

const std::string Dali::StyleMonitor::DEFAULT_FONT_FAMILY("DefaultFont");
const float       Dali::StyleMonitor::DEFAULT_FONT_SIZE(1.0f);

StyleMonitor::StyleMonitor()
{
}

StyleMonitor::StyleMonitor(const StyleMonitor& monitor)
: BaseHandle(monitor)
{
}

StyleMonitor StyleMonitor::StyleMonitor::Get()
{
  return Internal::Adaptor::StyleMonitor::Get();
}

StyleMonitor::~StyleMonitor()
{
}

std::string StyleMonitor::GetDefaultFontFamily() const
{
  return GetImplementation(*this).GetDefaultFontFamily();
}

float StyleMonitor::GetDefaultFontSize() const
{
  return GetImplementation(*this).GetDefaultFontSize();
}

const std::string& StyleMonitor::GetTheme() const
{
  return GetImplementation(*this).GetTheme();
}

void StyleMonitor::SetTheme(std::string themeFilePath)
{
  GetImplementation(*this).SetTheme(themeFilePath);
}

StyleMonitor::StyleChangeSignalType& StyleMonitor::StyleChangeSignal()
{
  return GetImplementation(*this).StyleChangeSignal();
}

void StyleMonitor::EmitStyleChangeSignal(StyleChange styleChange)
{
  GetImplementation(*this).EmitStyleChangeSignal(styleChange);
}

StyleMonitor& StyleMonitor::operator=(const StyleMonitor& monitor)
{
  if( *this != monitor )
  {
    BaseHandle::operator=(monitor);
  }
  return *this;
}

StyleMonitor::StyleMonitor(Internal::Adaptor::StyleMonitor* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
