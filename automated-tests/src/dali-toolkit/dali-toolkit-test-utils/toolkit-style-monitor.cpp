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

// HEADER
#include "toolkit-style-monitor.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace
{
const char* DEFAULT_THEME=
  "{\"styles\":{\n"
  "  \"textlabel\":\n"
  "    {\n"
  "      \"fontStyle\":{\"weight\":\"normal\"},\n"
  "      \"pointSize\":18\n"
  "    }\n"
  "  }\n"
  "}\n";

struct NamedTheme
{
  NamedTheme( const std::string& name, const std::string& theme )
  : name(name), theme(theme)
  {
  }

  std::string name;
  std::string theme;
};
typedef std::vector< NamedTheme > NamedThemes;
NamedThemes gThemes;

std::string gTheme;
std::string gFontFamily("LucidaSans");
std::string gFontStyle("Regular");
int         gFontSize(1);
}

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
  std::string GetDefaultFontStyle() const;
  float GetDefaultFontSize() const;
  const std::string& GetTheme() const;
  void SetTheme(std::string theme);
  bool LoadThemeFile( const std::string& filename, std::string& output );

public: // Signals
  Dali::StyleMonitor::StyleChangeSignalType& StyleChangeSignal();

  void EmitStyleChangeSignal(StyleChange::Type styleChange)
  {
    mStyleChangeSignal.Emit(Dali::StyleMonitor(this), styleChange);
  }

private:
  Dali::StyleMonitor::StyleChangeSignalType mStyleChangeSignal;
  static Dali::StyleMonitor mToolkitStyleMonitor;

  std::string mTheme;  ///<< Current theme name
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
  return gFontFamily;
}

std::string StyleMonitor::GetDefaultFontStyle() const
{
  return gFontStyle;
}

float StyleMonitor::GetDefaultFontSize() const
{
  return gFontSize;
}

const std::string& StyleMonitor::GetTheme() const
{
  return mTheme;
}

void StyleMonitor::SetTheme(std::string path)
{
  mTheme = path;
  EmitStyleChangeSignal( StyleChange::THEME_CHANGE );
}

bool StyleMonitor::LoadThemeFile( const std::string& filename, std::string& output )
{
  for( NamedThemes::iterator iter = gThemes.begin(); iter != gThemes.end(); ++iter )
  {
    NamedTheme& theme = *iter;
    if( theme.name == filename )
    {
      output = theme.theme;
      return true;
    }
  }

  if( !gTheme.empty() )
  {
    output = gTheme;
  }
  else
  {
    output = DEFAULT_THEME;
  }
  return true;
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

std::string StyleMonitor::GetDefaultFontStyle() const
{
  return GetImplementation(*this).GetDefaultFontStyle();
}

int StyleMonitor::GetDefaultFontSize() const
{
  return GetImplementation(*this).GetDefaultFontSize();
}

const std::string& StyleMonitor::GetTheme() const
{
  return GetImplementation(*this).GetTheme();
}

void StyleMonitor::SetTheme(const std::string& themeFilePath)
{
  GetImplementation(*this).SetTheme(themeFilePath);
}

StyleMonitor::StyleChangeSignalType& StyleMonitor::StyleChangeSignal()
{
  return GetImplementation(*this).StyleChangeSignal();
}

bool StyleMonitor::LoadThemeFile( const std::string& filename, std::string& output )
{
  return GetImplementation(*this).LoadThemeFile(filename, output);
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

namespace Test
{
namespace StyleMonitor
{

void SetThemeFileOutput( const std::string& name, const std::string& output )
{
  for( NamedThemes::iterator iter = gThemes.begin(); iter != gThemes.end(); ++iter )
  {
    NamedTheme& theme = *iter;
    if( theme.name == name )
    {
      theme.theme = output;
      return;
    }
  }

  gThemes.push_back( NamedTheme( name, output ) );
}

void SetDefaultFontFamily(const std::string& family)
{
  gFontFamily = family;
}

void SetDefaultFontStyle(const std::string& style)
{
  gFontStyle = style;
}

void SetDefaultFontSize( float size )
{
  gFontSize = size;
}

} // StyleMonitor
} // Test
