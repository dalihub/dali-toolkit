#ifndef __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__
#define __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#define __DALI_STYLE_MONITOR_H__
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/adaptor-framework/style-change.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class StyleMonitor;
}
}

class StyleMonitor : public BaseHandle
{
public: // Typedefs
  typedef Signal< void (StyleMonitor, StyleChange::Type) > StyleChangeSignalType;
  static const std::string DEFAULT_FONT_FAMILY;
  static const std::string DEFAULT_FONT_STYLE;
  static const float       DEFAULT_FONT_SIZE;

public: // Creation & Destruction
  StyleMonitor();
  StyleMonitor(const StyleMonitor& monitor);
  static StyleMonitor Get();
  ~StyleMonitor();
  static StyleMonitor DownCast( BaseHandle handle );

public: // Style Information
  std::string GetDefaultFontFamily() const;
  std::string GetDefaultFontStyle() const;
  float GetDefaultFontSize() const;
  const std::string& GetTheme() const;
  void SetTheme(std::string themeFilePath);
  bool LoadThemeFile( const std::string& filename, std::string& output );

public: // Signals
  StyleChangeSignalType& StyleChangeSignal();
  void EmitStyleChangeSignal(StyleChange::Type handle);

public: // Operators
  StyleMonitor& operator=(const StyleMonitor& monitor);

public:
  StyleMonitor(Internal::Adaptor::StyleMonitor* styleMonitor);
};


} // namespace Dali

namespace Test
{
namespace StyleMonitor
{
void SetThemeFileOutput( const std::string& output );
void SetDefaultFontFamily(const std::string& family);
void SetDefaultFontStyle(const std::string& style);
void SetDefaultFontSize( float size );

}
}

#endif // __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__
