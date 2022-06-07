/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include "style-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/internal/builder/builder-impl.h>
#include <dali-toolkit/internal/feedback/feedback-style.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/styling/style-manager.h>

namespace
{
//const char* LANDSCAPE_QUALIFIER = "landscape";
const char* PORTRAIT_QUALIFIER  = "portrait";
const char* FONT_SIZE_QUALIFIER = "fontsize";

const char* DEFAULT_THEME_FILE_NAME = "dali-toolkit-default-theme.json";

const char* PACKAGE_PATH_KEY              = "PACKAGE_PATH";
const char* APPLICATION_RESOURCE_PATH_KEY = "APPLICATION_RESOURCE_PATH";

const char* DEFAULT_TOOLKIT_PACKAGE_PATH = "/toolkit/";

static constexpr int32_t COUNT_BROKEN_IMAGE_MAX = 3;

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_STYLE");
#endif

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  BaseHandle handle = StyleManager::Get();

  if(!handle)
  {
    SingletonService singletonService(SingletonService::Get());
    if(singletonService)
    {
      Toolkit::StyleManager manager = Toolkit::StyleManager(new Internal::StyleManager());
      singletonService.Register(typeid(manager), manager);
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Toolkit::StyleManager, Dali::BaseHandle, Create, true)
DALI_TYPE_REGISTRATION_END()

} // namespace

Toolkit::StyleManager StyleManager::Get()
{
  Toolkit::StyleManager manager;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    // Check whether the style manager is already created
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(Toolkit::StyleManager));
    if(handle)
    {
      // If so, downcast the handle of singleton
      manager = Toolkit::StyleManager(dynamic_cast<StyleManager*>(handle.GetObjectPtr()));
    }
  }

  return manager;
}

StyleManager::StyleManager()
: mDefaultFontSize(-1),
  mDefaultFontFamily(""),
  mDefaultThemeFilePath(),
  mFeedbackStyle(nullptr)
{
  // Add theme builder constants
  const std::string dataReadOnlyDir                     = AssetManager::GetDaliDataReadOnlyPath();
  mThemeBuilderConstants[PACKAGE_PATH_KEY]              = dataReadOnlyDir + DEFAULT_TOOLKIT_PACKAGE_PATH;
  mThemeBuilderConstants[APPLICATION_RESOURCE_PATH_KEY] = Application::GetResourcePath();

  mStyleMonitor = StyleMonitor::Get();
  if(mStyleMonitor)
  {
    mStyleMonitor.StyleChangeSignal().Connect(this, &StyleManager::StyleMonitorChange);
    mDefaultFontSize = mStyleMonitor.GetDefaultFontSize();
  }

  // Set the full path for the default style theme.
  const std::string styleDirPath = AssetManager::GetDaliStylePath();
  mDefaultThemeFilePath          = styleDirPath + DEFAULT_THEME_FILE_NAME;

  // Sound & haptic style
  mFeedbackStyle = new FeedbackStyle();

  // Initialize BrokenImages
  mBrokenImageUrls.assign(COUNT_BROKEN_IMAGE_MAX, "");
}

StyleManager::~StyleManager()
{
  delete mFeedbackStyle;
}

void StyleManager::ApplyTheme(const std::string& themeFile)
{
  SetTheme(themeFile);
}

void StyleManager::ApplyDefaultTheme()
{
  SetTheme(mDefaultThemeFilePath);
}

const std::string& StyleManager::GetDefaultFontFamily() const
{
  return mDefaultFontFamily;
}

void StyleManager::SetStyleConstant(const std::string& key, const Property::Value& value)
{
  mStyleBuilderConstants[key] = value;
}

bool StyleManager::GetStyleConstant(const std::string& key, Property::Value& valueOut)
{
  Property::Value* value = mStyleBuilderConstants.Find(key);
  if(value)
  {
    valueOut = *value;
    return true;
  }

  return false;
}

void StyleManager::ApplyThemeStyle(Toolkit::Control control)
{
  if(!mThemeBuilder)
  {
    ApplyDefaultTheme();
  }

  if(mThemeBuilder)
  {
    ApplyStyle(mThemeBuilder, control);
  }
}

void StyleManager::ApplyThemeStyleAtInit(Toolkit::Control control)
{
  ApplyThemeStyle(control);

  if(mFeedbackStyle)
  {
    mFeedbackStyle->ObjectCreated(control);
  }
}

void StyleManager::ApplyStyle(Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName)
{
  bool builderReady = false;

  // First look in the cache
  Toolkit::Builder builder = FindCachedBuilder(jsonFileName);
  if(builder)
  {
    builderReady = true;
  }
  else
  {
    // Merge theme and style constants
    Property::Map constants(mThemeBuilderConstants);
    constants.Merge(mStyleBuilderConstants);

    // Create it
    builder = CreateBuilder(constants);

    if(LoadJSON(builder, jsonFileName))
    {
      CacheBuilder(builder, jsonFileName);
      builderReady = true;
    }
  }

  // Apply the style to the control
  if(builderReady)
  {
    builder.ApplyStyle(styleName, control);
  }
}

Toolkit::StyleManager::StyleChangedSignalType& StyleManager::StyleChangedSignal()
{
  return mStyleChangedSignal;
}

Toolkit::StyleManager::StyleChangedSignalType& StyleManager::ControlStyleChangeSignal()
{
  return mControlStyleChangeSignal;
}

Toolkit::DevelStyleManager::BrokenImageChangedSignalType& StyleManager::BrokenImageChangedSignal()
{
  return mBrokenImageChangedSignal;
}

void StyleManager::SetTheme(const std::string& themeFile)
{
  bool themeLoaded = false;
  bool loading     = false;

  // If we haven't loaded a theme, or the stored theme file is empty, or
  // the previously loaded theme is different to the requested theme,
  // first reset the builder and load the default theme.
  if(!mThemeBuilder || mThemeFile.empty() || mThemeFile.compare(themeFile) != 0)
  {
    loading       = true;
    mThemeBuilder = CreateBuilder(mThemeBuilderConstants);
    themeLoaded   = LoadJSON(mThemeBuilder, mDefaultThemeFilePath); // Sets themeLoaded to true if theme exists
  }

  if(themeFile.compare(mDefaultThemeFilePath) != 0)
  {
    // The theme is different to the default: Merge it
    loading = true;
    themeLoaded |= LoadJSON(mThemeBuilder, themeFile);
  }

  if(loading)
  {
    mThemeFile = themeFile;

    if(themeLoaded)
    {
      // We've successfully loaded the theme file
      if(mFeedbackStyle)
      {
        mFeedbackStyle->StyleChanged(mThemeFile, StyleChange::THEME_CHANGE);
      }

      EmitStyleChangeSignals(StyleChange::THEME_CHANGE);
    }
    else
    {
      // We tried to load a theme, but it failed. Ensure the builder is reset
      mThemeBuilder.Reset();
      mThemeFile.clear();
    }
  }
}

const Property::Map& StyleManager::GetConfigurations()
{
  DALI_LOG_STREAM(gLogFilter, Debug::Concise, "GetConfigurations()\n On entry, mThemeBuilder: " << (bool(mThemeBuilder) ? "Created" : "Empty") << "  mThemeFile: " << mThemeFile);

  if(!mThemeBuilder)
  {
    DALI_LOG_STREAM(gLogFilter, Debug::Concise, "GetConfigurations()  Loading default theme");

    mThemeBuilder = CreateBuilder(mThemeBuilderConstants);

    // Load default theme because this is first try to load stylesheet.
#if defined(DEBUG_ENABLED)
    bool themeLoaded = LoadJSON(mThemeBuilder, mDefaultThemeFilePath);
    DALI_LOG_STREAM(gLogFilter, Debug::Concise, "  themeLoaded" << (themeLoaded ? "success" : "failure"));
#else
    LoadJSON(mThemeBuilder, mDefaultThemeFilePath);
#endif

    mThemeFile = mDefaultThemeFilePath;
  }

#if defined(DEBUG_ENABLED)
  Property::Map result = mThemeBuilder.GetConfigurations();
  DALI_LOG_STREAM(gLogFilter, Debug::Concise, "GetConfigurations()\n On exit, result Count: " << (result.Count() != 0));
  DALI_LOG_STREAM(gLogFilter, Debug::Verbose, "          result: " << result);
#endif

  return mThemeBuilder.GetConfigurations();
}

void StyleManager::SetBrokenImageUrl(DevelStyleManager::BrokenImageType brokenImageType, const std::string& brokenImageUrl)
{
  int brokenType                     = static_cast<int>(brokenImageType);
  mBrokenImageUrls[brokenType]       = brokenImageUrl;
  Toolkit::StyleManager styleManager = StyleManager::Get();
  mBrokenImageChangedSignal.Emit(styleManager);
}

std::string StyleManager::GetBrokenImageUrl(DevelStyleManager::BrokenImageType brokenImageType)
{
  int brokenType = static_cast<int>(brokenImageType);
  return mBrokenImageUrls[brokenType];
}

std::vector<std::string> StyleManager::GetBrokenImageUrlList()
{
  // create a list for brokenImage
  std::vector<std::string> brokenImageUrlList;
  for(int i = 0; i < COUNT_BROKEN_IMAGE_MAX; i++)
  {
    if(!mBrokenImageUrls[i].empty())
    {
      brokenImageUrlList.push_back(mBrokenImageUrls[i]);
    }
  }
  return brokenImageUrlList;
}

bool StyleManager::LoadFile(const std::string& filename, std::string& stringOut)
{
  DALI_ASSERT_DEBUG(0 != filename.length());

  // as toolkit is platform agnostic, it cannot load files from filesystem
  // ask style monitor to load the style sheet
  if(mStyleMonitor)
  {
    return mStyleMonitor.LoadThemeFile(filename, stringOut);
  }

  return false;
}

Toolkit::Builder StyleManager::CreateBuilder(const Property::Map& constants)
{
  Toolkit::Builder builder = Toolkit::Builder::New();
  builder.AddConstants(constants);

  return builder;
}

bool StyleManager::LoadJSON(Toolkit::Builder builder, const std::string& jsonFilePath)
{
  std::string fileString;
  if(LoadFile(jsonFilePath, fileString))
  {
    builder.LoadFromString(fileString);
    return true;
  }
  else
  {
    DALI_LOG_WARNING("Error loading file '%s'\n", jsonFilePath.c_str());
    return false;
  }
}

static void CollectQualifiers(std::vector<std::string>& qualifiersOut)
{
  // Append the relevant qualifier for orientation
  // int orientation = 0; // Get the orientation from the system
  /*
  //// To Do /////
  Getting orientation from the system, and determine Qualifie LANDSCAPE or PORTRAIT
  orientation  0, 180 : PORTRAIT_QUALIFIER (default)
  orientation 90, 270 : LANDSCAPE_QUALIFIER
  */

  qualifiersOut.push_back(std::string(PORTRAIT_QUALIFIER));
}

/**
 * @brief Construct a qualified style name out of qualifiers
 *
 * A qualifed style name will be in the format: style-qualifier0-qualifier1-qualifierN
 *
 * @param[in] styleName The root name of the style
 * @param[in] qualifiers List of qualifier names
 * @param[out] qualifiedStyleOut The qualified style name
 */
static void BuildQualifiedStyleName(
  const std::string&              styleName,
  const std::vector<std::string>& qualifiers,
  std::string&                    qualifiedStyleOut)
{
  qualifiedStyleOut.append(styleName);

  for(std::vector<std::string>::const_iterator it    = qualifiers.begin(),
                                               itEnd = qualifiers.end();
      it != itEnd;
      ++it)
  {
    const std::string& str = *it;

    qualifiedStyleOut.append("-");
    qualifiedStyleOut.append(str);
  }
}

static bool GetStyleNameForControl(Toolkit::Builder builder, Toolkit::Control control, std::string& styleName)
{
  styleName = control.GetStyleName();

  if(styleName.empty())
  {
    styleName = control.GetTypeName();
  }

  // Apply the style after choosing the correct actual style (e.g. landscape or portrait)
  std::vector<std::string> qualifiers;
  CollectQualifiers(qualifiers);

  bool        found = 0;
  std::string qualifiedStyleName;
  do
  {
    qualifiedStyleName.clear();
    BuildQualifiedStyleName(styleName, qualifiers, qualifiedStyleName);

    // Break if style found or we have tried the root style name (qualifiers is empty)
    if(GetImpl(builder).LookupStyleName(qualifiedStyleName))
    {
      found = true;
      break;
    }
    if(qualifiers.size() == 0)
    {
      break;
    }
    // Remove the last qualifier in an attempt to find a style that is valid
    qualifiers.pop_back();
  } while(!found);

  if(found)
  {
    styleName = qualifiedStyleName;
  }
  return found;
}

void StyleManager::ApplyStyle(Toolkit::Builder builder, Toolkit::Control control)
{
  std::string styleName = control.GetStyleName();
  if(GetStyleNameForControl(builder, control, styleName))
  {
    builder.ApplyStyle(styleName, control);
  }

  if(mDefaultFontSize >= 0)
  {
    // Apply the style for logical font size
    std::stringstream fontSizeQualifier;
    fontSizeQualifier << styleName << FONT_SIZE_QUALIFIER << mDefaultFontSize;
    builder.ApplyStyle(fontSizeQualifier.str(), control);
  }
}

const StylePtr StyleManager::GetRecordedStyle(Toolkit::Control control)
{
  if(mThemeBuilder)
  {
    std::string styleName = control.GetStyleName();

    if(GetStyleNameForControl(mThemeBuilder, control, styleName))
    {
      const StylePtr style = GetImpl(mThemeBuilder).GetStyle(styleName);
      return style;
    }
  }
  return StylePtr(NULL);
}

Toolkit::Builder StyleManager::FindCachedBuilder(const std::string& key)
{
  BuilderMap::iterator builderIt = mBuilderCache.find(key);
  if(builderIt != mBuilderCache.end())
  {
    return builderIt->second;
  }

  return Toolkit::Builder();
}

void StyleManager::CacheBuilder(Toolkit::Builder builder, const std::string& key)
{
  mBuilderCache[key] = builder;
}

void StyleManager::StyleMonitorChange(StyleMonitor styleMonitor, StyleChange::Type styleChange)
{
  switch(styleChange)
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      mDefaultFontFamily = styleMonitor.GetDefaultFontFamily();
      break;
    }

    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      mDefaultFontSize = styleMonitor.GetDefaultFontSize();
      break;
    }

    case StyleChange::THEME_CHANGE:
    {
      SetTheme(styleMonitor.GetTheme());
      break;
    }
  }
  EmitStyleChangeSignals(styleChange);
}

void StyleManager::EmitStyleChangeSignals(StyleChange::Type styleChange)
{
  Toolkit::StyleManager styleManager = StyleManager::Get();

  // Update Controls first
  mControlStyleChangeSignal.Emit(styleManager, styleChange);

  // Inform application last
  mStyleChangedSignal.Emit(styleManager, styleChange);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
