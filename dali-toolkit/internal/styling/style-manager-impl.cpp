/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/singleton-service.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/application.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/internal/feedback/feedback-style.h>

namespace
{

const char* LANDSCAPE_QUALIFIER = "landscape";
const char* PORTRAIT_QUALIFIER  = "portrait";
const char* FONT_SIZE_QUALIFIER = "fontsize";

const char* DEFAULT_THEME = DALI_STYLE_DIR "dali-toolkit-default-theme.json";

const char* PACKAGE_PATH_KEY = "PACKAGE_PATH";
const char* APPLICATION_RESOURCE_PATH_KEY = "APPLICATION_RESOURCE_PATH";

const char* DEFAULT_PACKAGE_PATH = DALI_DATA_READ_ONLY_DIR "/toolkit/";

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

  if ( !handle )
  {
    SingletonService singletonService( SingletonService::Get() );
    if ( singletonService )
    {
      Toolkit::StyleManager manager = Toolkit::StyleManager( new Internal::StyleManager() );
      singletonService.Register( typeid( manager ), manager );
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE( Toolkit::StyleManager, Dali::BaseHandle, Create, true )
DALI_TYPE_REGISTRATION_END()

} // namespace

Toolkit::StyleManager StyleManager::Get()
{
  Toolkit::StyleManager manager;

  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    // Check whether the style manager is already created
    Dali::BaseHandle handle = singletonService.GetSingleton( typeid( Toolkit::StyleManager ) );
    if( handle )
    {
      // If so, downcast the handle of singleton
      manager = Toolkit::StyleManager( dynamic_cast< StyleManager* >( handle.GetObjectPtr() ) );
    }
  }

  return manager;
}

StyleManager::StyleManager()
: mDefaultFontSize( -1 ),
  mDefaultFontFamily(""),
  mFeedbackStyle( NULL )
{
  // Add theme builder constants
  mThemeBuilderConstants[ PACKAGE_PATH_KEY ] = DEFAULT_PACKAGE_PATH;
  mThemeBuilderConstants[ APPLICATION_RESOURCE_PATH_KEY ] = Application::GetResourcePath();

  mStyleMonitor = StyleMonitor::Get();
  if( mStyleMonitor )
  {
    mStyleMonitor.StyleChangeSignal().Connect( this, &StyleManager::StyleMonitorChange );
    mDefaultFontSize = mStyleMonitor.GetDefaultFontSize();
  }

  // Sound & haptic style
  mFeedbackStyle = new FeedbackStyle();
}

StyleManager::~StyleManager()
{
  delete mFeedbackStyle;
}

void StyleManager::ApplyTheme( const std::string& themeFile )
{
  SetTheme( themeFile );
}

void StyleManager::ApplyDefaultTheme()
{
  std::string empty;
  SetTheme( empty );
}

const std::string& StyleManager::GetDefaultFontFamily() const
{
  return mDefaultFontFamily;
}

void StyleManager::SetStyleConstant( const std::string& key, const Property::Value& value )
{
  mStyleBuilderConstants[ key ] = value;
}

bool StyleManager::GetStyleConstant( const std::string& key, Property::Value& valueOut )
{
  Property::Value* value = mStyleBuilderConstants.Find( key );
  if( value )
  {
    valueOut = *value;
    return true;
  }

  return false;
}

void StyleManager::ApplyThemeStyle( Toolkit::Control control )
{
  if( !mThemeBuilder )
  {
    ApplyDefaultTheme();
  }

  if( mThemeBuilder )
  {
    ApplyStyle( mThemeBuilder, control );
  }
}

void StyleManager::ApplyThemeStyleAtInit( Toolkit::Control control )
{
  ApplyThemeStyle( control );

  if(mFeedbackStyle)
  {
    mFeedbackStyle->ObjectCreated( control );
  }
}

void StyleManager::ApplyStyle( Toolkit::Control control, const std::string& jsonFileName, const std::string& styleName )
{
  bool builderReady = false;

  // First look in the cache
  Toolkit::Builder builder = FindCachedBuilder( jsonFileName );
  if( builder )
  {
    builderReady = true;
  }
  else
  {
    // Merge theme and style constants
    Property::Map constants( mThemeBuilderConstants );
    constants.Merge( mStyleBuilderConstants );

    // Create it
    builder = CreateBuilder( constants );

    if( LoadJSON( builder, jsonFileName ) )
    {
      CacheBuilder( builder, jsonFileName );
      builderReady = true;
    }
  }

  // Apply the style to the control
  if( builderReady )
  {
    builder.ApplyStyle( styleName, control );
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

void StyleManager::SetTheme( const std::string& themeFile )
{
  bool themeLoaded = false;

  mThemeBuilder = CreateBuilder( mThemeBuilderConstants );

  // Always load the default theme first, then merge in the custom theme if present
  themeLoaded = LoadJSON( mThemeBuilder, DEFAULT_THEME );

  if( ! themeFile.empty() )
  {
    mThemeFile = themeFile;
    themeLoaded = LoadJSON( mThemeBuilder, mThemeFile );
  }

  if( themeLoaded )
  {
    if(mFeedbackStyle)
    {
      mFeedbackStyle->StyleChanged( mThemeFile, StyleChange::THEME_CHANGE );
    }

    EmitStyleChangeSignals(StyleChange::THEME_CHANGE);
  }
  else
  {
    mThemeBuilder.Reset();
  }
}

bool StyleManager::LoadFile( const std::string& filename, std::string& stringOut )
{
  DALI_ASSERT_DEBUG( 0 != filename.length());

  // as toolkit is platform agnostic, it cannot load files from filesystem
  // ask style monitor to load the style sheet
  if( mStyleMonitor )
  {
    return mStyleMonitor.LoadThemeFile( filename, stringOut );
  }

  return false;
}

Toolkit::Builder StyleManager::CreateBuilder( const Property::Map& constants )
{
  Toolkit::Builder builder = Toolkit::Builder::New();
  builder.AddConstants( constants );

  return builder;
}

bool StyleManager::LoadJSON( Toolkit::Builder builder, const std::string& jsonFilePath )
{
  std::string fileString;
  if( LoadFile( jsonFilePath, fileString ) )
  {
    builder.LoadFromString( fileString );
    return true;
  }
  else
  {
    DALI_LOG_WARNING("Error loading file '%s'\n", jsonFilePath.c_str());
    return false;
  }
}

void StyleManager::CollectQualifiers( StringList& qualifiersOut )
{
  // Append the relevant qualifier for orientation
  int orientation = 0; // Get the orientation from the system
  switch( orientation )
  {
    case 90:
    case 270:
    {
      qualifiersOut.push_back( std::string( LANDSCAPE_QUALIFIER ) );
      break;
    }
    case 180:
    case 0: // fall through
    default:
    {
      qualifiersOut.push_back( std::string( PORTRAIT_QUALIFIER ) );
      break;
    }
  }
}

void StyleManager::BuildQualifiedStyleName( const std::string& styleName, const StringList& qualifiers, std::string& qualifiedStyleOut )
{
  qualifiedStyleOut.append( styleName );

  for( StringList::const_iterator it = qualifiers.begin(), itEnd = qualifiers.end(); it != itEnd; ++it )
  {
    const std::string& str = *it;

    qualifiedStyleOut.append( "-" );
    qualifiedStyleOut.append( str );
  }
}

void StyleManager::ApplyStyle( Toolkit::Builder builder, Toolkit::Control control )
{
  std::string styleName = control.GetStyleName();

  if( styleName.empty() )
  {
    styleName = control.GetTypeName();
  }

  // Apply the style after choosing the correct actual style (e.g. landscape or portrait)
  StringList qualifiers;
  CollectQualifiers( qualifiers );

  while( true )
  {
    std::string qualifiedStyleName;
    BuildQualifiedStyleName( styleName, qualifiers, qualifiedStyleName );

    // Break if style found or we have tried the root style name (qualifiers is empty)
    if( builder.ApplyStyle( qualifiedStyleName, control ) || qualifiers.size() == 0 )
    {
      break;
    }

    // Remove the last qualifier in an attempt to find a style that is valid
    qualifiers.pop_back();
  }

  if( mDefaultFontSize >= 0 )
  {
    // Apply the style for logical font size
    std::stringstream fontSizeQualifier;
    fontSizeQualifier << styleName << FONT_SIZE_QUALIFIER << mDefaultFontSize;
    builder.ApplyStyle( fontSizeQualifier.str(), control );
  }
}

Toolkit::Builder StyleManager::FindCachedBuilder( const std::string& key )
{
  BuilderMap::iterator builderIt = mBuilderCache.find( key );
  if( builderIt != mBuilderCache.end() )
  {
    return builderIt->second;
  }

  return Toolkit::Builder();
}

void StyleManager::CacheBuilder( Toolkit::Builder builder, const std::string& key )
{
  mBuilderCache[ key ] = builder;
}

void StyleManager::StyleMonitorChange( StyleMonitor styleMonitor, StyleChange::Type styleChange )
{
  switch ( styleChange )
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
      SetTheme( styleMonitor.GetTheme() );
      break;
    }
  }
  EmitStyleChangeSignals( styleChange );
}

void StyleManager::EmitStyleChangeSignals( StyleChange::Type styleChange )
{
  Toolkit::StyleManager styleManager = StyleManager::Get();

  // Update Controls first
  mControlStyleChangeSignal.Emit( styleManager, styleChange );

  // Inform application last
  mStyleChangedSignal.Emit( styleManager, styleChange );
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
