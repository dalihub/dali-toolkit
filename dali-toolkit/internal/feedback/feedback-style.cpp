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

// CLASS HEADER
#include <dali-toolkit/internal/feedback/feedback-style.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/devel-api/adaptor-framework/style-monitor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/internal/feedback/feedback-ids.h>

using std::string;

namespace // unnamed namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::General, false, "LOG_FEEDBACK");
#endif

const char* DEFAULT_FEEDBACK_THEME_PATH = DALI_STYLE_DIR"default-feedback-theme.json";

// Sets bool and string if the node has a child "name"
void GetIfString(const Dali::Toolkit::TreeNode& node, const std::string& name, bool& exists, std::string& str)
{
  const Dali::Toolkit::TreeNode* child = node.GetChild(name);
  if( child &&
      Dali::Toolkit::TreeNode::STRING == child->GetType() )
  {
    exists = true;
    str = child->GetString();
  }
}

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

struct SignalFeedbackInfo
{
  /**
   * Default constructor.
   */
  SignalFeedbackInfo()
  :mHasHapticFeedbackInfo(false),
   mHasSoundFeedbackInfo(false)
  {
  }

  bool mHasHapticFeedbackInfo;
  bool mHasSoundFeedbackInfo;
  string mSignalName;
  string mHapticFeedbackPattern;
  string mSoundFeedbackPattern;
  string mHapticFeedbackFile;
  string mSoundFeedbackFile;
};

typedef std::vector<SignalFeedbackInfo> SignalFeedbackInfoContainer;
typedef SignalFeedbackInfoContainer::const_iterator SignalFeedbackInfoConstIter;

struct FeedbackStyleInfo
{
  /**
   * Default constructor.
   */
  FeedbackStyleInfo()
  {
  }

  string mTypeName;

  SignalFeedbackInfoContainer mSignalFeedbackInfoList;
};

static const FeedbackStyleInfo DEFAULT_FEEDBACK_STYLE_INFO;

FeedbackStyle::FeedbackStyle()
: mConnections( this )
{
  mFeedback = Dali::FeedbackPlayer::Get();

  string defaultTheme;

  if( mFeedback && mFeedback.LoadFile( DEFAULT_FEEDBACK_THEME_PATH, defaultTheme ) )
  {
    LoadTheme( defaultTheme );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "ResourceLoader::LoadTheme(%s) - loaded %d bytes\n",
                   DEFAULT_FEEDBACK_THEME_PATH, defaultTheme.size() );
  }
  else
  {
    DALI_LOG_ERROR("ResourceLoader::LoadTheme(%s) - failed to load\n", DEFAULT_FEEDBACK_THEME_PATH);
  }

}

FeedbackStyle::~FeedbackStyle()
{
}

struct PlayFeedbackFromSignal
{
  PlayFeedbackFromSignal( FeedbackStyle& controller, const string& typeName, const string& signalName )
  : mController( controller ),
    mTypeName( typeName ),
    mSignalName( signalName )
  {
  }

  void operator()()
  {
    mController.PlayFeedback( mTypeName, mSignalName );
  }

  FeedbackStyle& mController;
  string mTypeName;
  string mSignalName;
};


void FeedbackStyle::ObjectCreated( BaseHandle handle )
{
  std::string typeName = handle.GetTypeName();

  if( handle )
  {
    string type = handle.GetTypeName();

    const FeedbackStyleInfo styleInfo = GetStyleInfo( type );

    for( SignalFeedbackInfoConstIter iter = styleInfo.mSignalFeedbackInfoList.begin(); iter != styleInfo.mSignalFeedbackInfoList.end(); ++iter )
    {
      const SignalFeedbackInfo& info = *iter;

      if( info.mHasHapticFeedbackInfo || info.mHasSoundFeedbackInfo )
      {
        if( !info.mHapticFeedbackPattern.empty() || !info.mHapticFeedbackFile.empty() ||
            !info.mSoundFeedbackPattern.empty()  || !info.mSoundFeedbackFile.empty() )
        {
          handle.ConnectSignal( this,
                                info.mSignalName,
                                PlayFeedbackFromSignal( *this, type, info.mSignalName ) );

          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "FeedbackStyle::Set found Haptic pattern %s for Object type: %s, Signal Type: %s\n",
                         info.mHapticFeedbackPattern.c_str(), type.c_str(), info.mSignalName.c_str() );
        }
        else
        {
          DALI_LOG_ERROR("FeedbackStyle::Set() Warning Inconsistent data in theme file!\n");
        }
      }
    }
  }
}

const FeedbackStyleInfo& FeedbackStyle::GetStyleInfo( const string& type ) const
{
  std::map<const string, FeedbackStyleInfo>::const_iterator iter( mStyleInfoLut.find( type ) );
  if( iter != mStyleInfoLut.end() )
  {
    return iter->second;
  }
  else
  {
    return DEFAULT_FEEDBACK_STYLE_INFO;
  }
}

void FeedbackStyle::StyleChanged( const std::string& userDefinedThemePath, Dali::StyleChange::Type styleChange )
{
  if( StyleChange::THEME_CHANGE )
  {
    string userDefinedTheme;

    if( mFeedback && mFeedback.LoadFile( userDefinedThemePath, userDefinedTheme ) )
    {
      if( !LoadTheme( userDefinedTheme ) )
      {
        DALI_LOG_ERROR("FeedbackStyle::StyleChanged() User defined theme failed to load! \n");

        //If there is any problem is using the user defined theme, then fall back to default theme
        if( !LoadTheme( DEFAULT_FEEDBACK_THEME_PATH ) )
        {
          //If the default theme fails, Then No luck!
          DALI_LOG_ERROR("FeedbackStyle::StyleChanged() Default theme failed to load! \n");
        }
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "ResourceLoader::LoadTheme(%s) - loaded %d bytes\n",
                       userDefinedThemePath.c_str(), userDefinedTheme.size() );
      }
    }
    else
    {
      DALI_LOG_ERROR("ResourceLoader::LoadTheme(%s) - failed to load\n", userDefinedThemePath.c_str());
    }
  }
}

bool FeedbackStyle::LoadTheme( const string& data )
{
  bool result = false;

  try
  {
    LoadFromString( data );

    result = true;
  }
  catch(...)
  {
    //Problem in user set theme, So fallback to use default theme.
    DALI_LOG_ERROR( "FeedbackStyle::LoadTheme() Failed to load theme\n" );
  }

  return result;
}

void FeedbackStyle::LoadFromString( const string& data )
{
  Toolkit::JsonParser parser = Toolkit::JsonParser::New();
  const Toolkit::TreeNode* root = NULL;

  if( !parser.Parse( data ) )
  {
    DALI_LOG_WARNING( "JSON Parse Error:'%s'\n", parser.GetErrorDescription().c_str() );
    DALI_LOG_WARNING( "JSON Parse Line :'%d (%d)'\n",
                      parser.GetErrorLineNumber(),
                      parser.GetErrorColumn() );
  }
  else
  {
    root = parser.GetRoot();
  }

  if(root)
  {
    // Clear previously loaded style
    mStyleInfoLut.clear();

    // Parse style
    if( const TreeNode* node = root->GetChild("style") )
    {
      Toolkit::TreeNode::ConstIterator iter = node->CBegin();
      Toolkit::TreeNode::ConstIterator end = node->CEnd();
      for( ; iter != end; ++iter )
      {
        const char* key = (*iter).first;
        FeedbackStyleInfo themeInfo;
        themeInfo.mTypeName = key;

        if( const TreeNode* signals = (*iter).second.GetChild("signals") )
        {
          TreeNode::ConstIterator signalIter = signals->CBegin();
          TreeNode::ConstIterator signalEnd = signals->CEnd();
          for( ; signalIter != signalEnd; ++signalIter )
          {
            SignalFeedbackInfo signalFeedbackInfo;

            const TreeNode* type = (*signalIter).second.GetChild("type");
            DALI_ASSERT_ALWAYS(type && TreeNode::STRING == type->GetType() && "Signal must have a type");
            signalFeedbackInfo.mSignalName = type->GetString();

            GetIfString( (*signalIter).second, "hapticFeedbackPattern",
                         signalFeedbackInfo.mHasHapticFeedbackInfo,
                         signalFeedbackInfo.mHapticFeedbackPattern );

            GetIfString( (*signalIter).second, "hapticFeedbackFile",
                         signalFeedbackInfo.mHasHapticFeedbackInfo,
                         signalFeedbackInfo.mHapticFeedbackFile );

            GetIfString( (*signalIter).second, "soundFeedbackPattern",
                         signalFeedbackInfo.mHasSoundFeedbackInfo,
                         signalFeedbackInfo.mSoundFeedbackPattern );

            GetIfString( (*signalIter).second, "hapticFeedbackFile",
                         signalFeedbackInfo.mHasSoundFeedbackInfo,
                         signalFeedbackInfo.mSoundFeedbackFile );

            if( signalFeedbackInfo.mHasHapticFeedbackInfo || signalFeedbackInfo.mHasSoundFeedbackInfo )
            {
              AddSignalInfo( themeInfo, signalFeedbackInfo );
            }
          }
        }

        mStyleInfoLut[key] = themeInfo;

      } // for styles
    } // if(style)
  } // if(root)

} // LoadFromString()

void FeedbackStyle::AddSignalInfo( FeedbackStyleInfo& styleInfo, SignalFeedbackInfo signalInfo )
{
  bool updated = false;
  std::vector<SignalFeedbackInfo>::iterator iter;

  // If info exists for the signal then update it, else add new
  for( iter = styleInfo.mSignalFeedbackInfoList.begin(); iter != styleInfo.mSignalFeedbackInfoList.end(); ++iter )
  {
    if( (*iter).mSignalName == signalInfo.mSignalName )
    {
      (*iter).mHasHapticFeedbackInfo = signalInfo.mHasHapticFeedbackInfo;
      (*iter).mHapticFeedbackPattern = signalInfo.mHapticFeedbackPattern;
      (*iter).mHapticFeedbackFile    = signalInfo.mHapticFeedbackFile;
      (*iter).mHasSoundFeedbackInfo  = signalInfo.mHasSoundFeedbackInfo;
      (*iter).mSoundFeedbackPattern  = signalInfo.mSoundFeedbackPattern;
      (*iter).mSoundFeedbackFile     = signalInfo.mSoundFeedbackFile;

      updated = true;
      break;
    }
  }

  if( !updated )
  {
    styleInfo.mSignalFeedbackInfoList.push_back( signalInfo );
  }
}

void FeedbackStyle::PlayFeedback(const string& type, const string& signalName)
{
  const FeedbackStyleInfo styleInfo = GetStyleInfo(type);
  SignalFeedbackInfoConstIter iter;

  for(iter = styleInfo.mSignalFeedbackInfoList.begin(); iter != styleInfo.mSignalFeedbackInfoList.end(); ++iter)
  {
    const SignalFeedbackInfo& info = *iter;

    if(info.mSignalName == signalName)
    {
      if(info.mHasHapticFeedbackInfo)
      {
        if(!info.mHapticFeedbackPattern.empty())
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "FeedbackStyle::PlayFeedback Playing Haptic effect: Object type: %s, Signal type: %s, pattern type: %s\n",
              type.c_str(), signalName.c_str(), info.mHapticFeedbackPattern.c_str());

          mFeedback.PlayFeedbackPattern( FEEDBACK_TYPE_VIBRATION, GetFeedbackPattern(info.mHapticFeedbackPattern) );
        }
        else if(!info.mHapticFeedbackFile.empty())
        {
          mFeedback.PlayFile( info.mHapticFeedbackFile );
        }
      }

      if(info.mHasSoundFeedbackInfo)
      {
        if(!info.mSoundFeedbackPattern.empty())
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "FeedbackStyle::PlayFeedback Playing Sound effect: Object type: %s, Signal type: %s, pattern type: %s\n",
              type.c_str(), signalName.c_str(), info.mHapticFeedbackPattern.c_str());

          mFeedback.PlayFeedbackPattern( FEEDBACK_TYPE_SOUND, GetFeedbackPattern(info.mSoundFeedbackPattern) );
        }
        else if(!info.mSoundFeedbackFile.empty())
        {
          mFeedback.PlaySound( info.mSoundFeedbackFile );
        }
      }

      break;
    }
  }
}

FeedbackPattern FeedbackStyle::GetFeedbackPattern( const string &pattern )
{
  if( 0 == mFeedbackPatternLut.size() )
  {
    mFeedbackPatternLut["FEEDBACK_PATTERN_NONE"]                = Dali::FEEDBACK_PATTERN_NONE;
    mFeedbackPatternLut["FEEDBACK_PATTERN_TAP"]                 = Dali::FEEDBACK_PATTERN_TAP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SIP"]                 = Dali::FEEDBACK_PATTERN_SIP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SIP_BACKSPACE"]       = Dali::FEEDBACK_PATTERN_SIP_BACKSPACE;
    mFeedbackPatternLut["FEEDBACK_PATTERN_MAX_CHARACTER"]       = Dali::FEEDBACK_PATTERN_MAX_CHARACTER;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY0"]                = Dali::FEEDBACK_PATTERN_KEY0;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY1"]                = Dali::FEEDBACK_PATTERN_KEY1;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY2"]                = Dali::FEEDBACK_PATTERN_KEY2;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY3"]                = Dali::FEEDBACK_PATTERN_KEY3;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY4"]                = Dali::FEEDBACK_PATTERN_KEY4;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY5"]                = Dali::FEEDBACK_PATTERN_KEY5;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY6"]                = Dali::FEEDBACK_PATTERN_KEY6;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY7"]                = Dali::FEEDBACK_PATTERN_KEY7;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY8"]                = Dali::FEEDBACK_PATTERN_KEY8;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY9"]                = Dali::FEEDBACK_PATTERN_KEY9;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY_STAR"]            = Dali::FEEDBACK_PATTERN_KEY_STAR;
    mFeedbackPatternLut["FEEDBACK_PATTERN_KEY_SHARP"]           = Dali::FEEDBACK_PATTERN_KEY_SHARP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_HOLD"]                = Dali::FEEDBACK_PATTERN_HOLD;
    mFeedbackPatternLut["FEEDBACK_PATTERN_MULTI_TAP"]           = Dali::FEEDBACK_PATTERN_MULTI_TAP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_HW_TAP"]              = Dali::FEEDBACK_PATTERN_HW_TAP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_HW_HOLD"]             = Dali::FEEDBACK_PATTERN_HW_HOLD;
    mFeedbackPatternLut["FEEDBACK_PATTERN_MESSAGE"]             = Dali::FEEDBACK_PATTERN_MESSAGE;
    mFeedbackPatternLut["FEEDBACK_PATTERN_MESSAGE_ON_CALL"]     = Dali::FEEDBACK_PATTERN_MESSAGE_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_EMAIL"]               = Dali::FEEDBACK_PATTERN_EMAIL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_EMAIL_ON_CALL"]       = Dali::FEEDBACK_PATTERN_EMAIL_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_WAKEUP"]              = Dali::FEEDBACK_PATTERN_WAKEUP;
    mFeedbackPatternLut["FEEDBACK_PATTERN_WAKEUP_ON_CALL"]      = Dali::FEEDBACK_PATTERN_WAKEUP_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SCHEDULE"]            = Dali::FEEDBACK_PATTERN_SCHEDULE;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SCHEDULE_ON_CALL"]    = Dali::FEEDBACK_PATTERN_SCHEDULE_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_TIMER"]               = Dali::FEEDBACK_PATTERN_TIMER;
    mFeedbackPatternLut["FEEDBACK_PATTERN_TIMER_ON_CALL"]       = Dali::FEEDBACK_PATTERN_TIMER_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_GENERAL"]             = Dali::FEEDBACK_PATTERN_GENERAL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_GENERAL_ON_CALL"]     = Dali::FEEDBACK_PATTERN_GENERAL_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_POWER_ON"]            = Dali::FEEDBACK_PATTERN_POWER_ON;
    mFeedbackPatternLut["FEEDBACK_PATTERN_POWER_OFF"]           = Dali::FEEDBACK_PATTERN_POWER_OFF;
    mFeedbackPatternLut["FEEDBACK_PATTERN_CHARGERCONN"]         = Dali::FEEDBACK_PATTERN_CHARGERCONN;
    mFeedbackPatternLut["FEEDBACK_PATTERN_CHARGERCONN_ON_CALL"] = Dali::FEEDBACK_PATTERN_CHARGERCONN_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_FULLCHARGED"]         = Dali::FEEDBACK_PATTERN_FULLCHARGED;
    mFeedbackPatternLut["FEEDBACK_PATTERN_FULLCHARGED_ON_CALL"] = Dali::FEEDBACK_PATTERN_FULLCHARGED_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_LOWBATT"]             = Dali::FEEDBACK_PATTERN_LOWBATT;
    mFeedbackPatternLut["FEEDBACK_PATTERN_LOWBATT_ON_CALL"]     = Dali::FEEDBACK_PATTERN_LOWBATT_ON_CALL;
    mFeedbackPatternLut["FEEDBACK_PATTERN_LOCK"]                = Dali::FEEDBACK_PATTERN_LOCK;
    mFeedbackPatternLut["FEEDBACK_PATTERN_UNLOCK"]              = Dali::FEEDBACK_PATTERN_UNLOCK;
    mFeedbackPatternLut["FEEDBACK_PATTERN_CALLCONNECT"]         = Dali::FEEDBACK_PATTERN_CALLCONNECT;
    mFeedbackPatternLut["FEEDBACK_PATTERN_DISCALLCONNECT"]      = Dali::FEEDBACK_PATTERN_DISCALLCONNECT;
    mFeedbackPatternLut["FEEDBACK_PATTERN_MINUTEMINDER"]        = Dali::FEEDBACK_PATTERN_MINUTEMINDER;
    mFeedbackPatternLut["FEEDBACK_PATTERN_VIBRATION"]           = Dali::FEEDBACK_PATTERN_VIBRATION;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SHUTTER"]             = Dali::FEEDBACK_PATTERN_SHUTTER;
    mFeedbackPatternLut["FEEDBACK_PATTERN_LIST_REORDER"]        = Dali::FEEDBACK_PATTERN_LIST_REORDER;
    mFeedbackPatternLut["FEEDBACK_PATTERN_SLIDER_SWEEP"]        = Dali::FEEDBACK_PATTERN_SLIDER_SWEEP;
  }

  std::map<const string, FeedbackPattern>::const_iterator iter( mFeedbackPatternLut.find( pattern ) );

  if( iter != mFeedbackPatternLut.end() )
  {
    return iter->second;
  }
  else
  {
    DALI_LOG_ERROR( "Unknown feedback pattern type: %s, So Defaulting to FEEDBACK_PATTERN_NONE!\n" );
    return Dali::FEEDBACK_PATTERN_NONE;
  }
}

} // namespace Toolkit

} // namespace Internal

} // namespace Dali
