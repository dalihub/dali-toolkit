/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

/**
 * This is a copy of dali-adaptor/integration-api/adaptor-framework/input-method-options.cpp.
 */

// CLASS HEADER
#include <dali/integration-api/adaptor-framework/input-method-options.h>

using namespace Dali::Integration::InputMethod::Category;

namespace Dali
{
namespace Integration
{

namespace
{

Dali::InputMethod::PanelLayout NormalizePanelLayout(int value)
{
  using namespace Dali::InputMethod;

  switch(value)
  {
    case 0:
      return PanelLayout::NORMAL;
    case 1:
      return PanelLayout::NUMBER;
    case 2:
      return PanelLayout::EMAIL;
    case 3:
      return PanelLayout::URL;
    case 4:
      return PanelLayout::PHONENUMBER;
    case 5:
      return PanelLayout::IP;
    case 6:
      return PanelLayout::MONTH;
    case 7:
      return PanelLayout::NUMBER_ONLY;
    case 8: // Legacy HEX from TizenFX/DALi property maps.
      return PanelLayout::NUMBER_ONLY;
    case 9: // Legacy TERMINAL from TizenFX/DALi property maps.
      return PanelLayout::NORMAL;
    case 10:
      return PanelLayout::PASSWORD;
    case 11:
      return PanelLayout::DATE_TIME;
    case 12:
      return PanelLayout::EMOTICON;
    case 13:
    {
      return PanelLayout::VOICE;
    }
  }

  return PanelLayout::NORMAL;
}

int ToLegacyPanelLayoutValue(Dali::InputMethod::PanelLayout layout)
{
  using namespace Dali::InputMethod;

  switch(layout)
  {
    case PanelLayout::NORMAL:
      return 0;
    case PanelLayout::NUMBER:
      return 1;
    case PanelLayout::EMAIL:
      return 2;
    case PanelLayout::URL:
      return 3;
    case PanelLayout::PHONENUMBER:
      return 4;
    case PanelLayout::IP:
      return 5;
    case PanelLayout::MONTH:
      return 6;
    case PanelLayout::NUMBER_ONLY:
      return 7;
    case PanelLayout::PASSWORD:
      return 10;
    case PanelLayout::DATE_TIME:
      return 11;
    case PanelLayout::EMOTICON:
      return 12;
    case PanelLayout::VOICE:
      return 13;
    default:
      return 0;
  }
}

Dali::InputMethod::ReturnKeyType NormalizeReturnKeyType(int value)
{
  using namespace Dali::InputMethod;

  switch(value)
  {
    case static_cast<int>(ReturnKeyType::DEFAULT):
      return ReturnKeyType::DEFAULT;
    case static_cast<int>(ReturnKeyType::DONE):
      return ReturnKeyType::DONE;
    case static_cast<int>(ReturnKeyType::GO):
      return ReturnKeyType::GO;
    case static_cast<int>(ReturnKeyType::JOIN):
      return ReturnKeyType::JOIN;
    case static_cast<int>(ReturnKeyType::LOGIN):
      return ReturnKeyType::LOGIN;
    case static_cast<int>(ReturnKeyType::NEXT):
      return ReturnKeyType::NEXT;
    case static_cast<int>(ReturnKeyType::SEARCH):
      return ReturnKeyType::SEARCH;
    case static_cast<int>(ReturnKeyType::SEND):
      return ReturnKeyType::SEND;
    case static_cast<int>(ReturnKeyType::SIGNIN):
      return ReturnKeyType::SIGNIN;
    default:
      return ReturnKeyType::DEFAULT;
  }
}

Dali::InputMethod::AutoCapitalType NormalizeAutoCapitalType(int value)
{
  using namespace Dali::InputMethod;

  switch(value)
  {
    case static_cast<int>(AutoCapitalType::NONE):
      return AutoCapitalType::NONE;
    case static_cast<int>(AutoCapitalType::WORD):
      return AutoCapitalType::WORD;
    case static_cast<int>(AutoCapitalType::SENTENCE):
      return AutoCapitalType::SENTENCE;
    case static_cast<int>(AutoCapitalType::ALL_CHARACTER):
      return AutoCapitalType::ALL_CHARACTER;
    default:
      return AutoCapitalType::SENTENCE;
  }
}

Dali::InputMethod::PanelLayoutVariation ToPanelLayoutVariation(
  Dali::InputMethod::PanelLayout layout,
  int rawVariation)
{
  using namespace Dali::InputMethod;

  switch(layout)
  {
    case PanelLayout::NORMAL:
    {
      switch(rawVariation)
      {
        case 1:
          return PanelLayoutVariation::NORMAL_WITH_FILENAME;
        case 2:
          return PanelLayoutVariation::NORMAL_WITH_PERSON_NAME;
        default:
          return PanelLayoutVariation::NORMAL_NORMAL;
      }
    }

    case PanelLayout::NUMBER_ONLY:
    {
      switch(rawVariation)
      {
        case 1:
          return PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED;
        case 2:
          return PanelLayoutVariation::NUMBER_ONLY_WITH_DECIMAL;
        case 3:
          return PanelLayoutVariation::NUMBER_ONLY_WITH_SIGNED_AND_DECIMAL;
        default:
          return PanelLayoutVariation::NUMBER_ONLY_NORMAL;
      }
    }

    case PanelLayout::PASSWORD:
    {
      switch(rawVariation)
      {
        case 1:
          return PanelLayoutVariation::PASSWORD_WITH_NUMBER_ONLY;
        default:
          return PanelLayoutVariation::PASSWORD_NORMAL;
      }
    }

    default:
    {
      return PanelLayoutVariation::NORMAL_NORMAL;
    }
  }
}

} // unnamed namespace

#define TOKEN_STRING(x) #x

struct InputMethodOptions::Impl
{
  Impl()
  {
    mPanelLayout          = Dali::InputMethod::PanelLayout::NORMAL;
    mAutoCapital          = Dali::InputMethod::AutoCapitalType::SENTENCE;
    mButtonAction         = Dali::InputMethod::ReturnKeyType::DEFAULT;
    mPanelLayoutVariation = Dali::InputMethod::PanelLayoutVariation::NORMAL_NORMAL;
    mVariation            = 0;
  }

  Dali::InputMethod::PanelLayout          mPanelLayout;
  Dali::InputMethod::AutoCapitalType      mAutoCapital;
  Dali::InputMethod::ReturnKeyType        mButtonAction;
  Dali::InputMethod::PanelLayoutVariation mPanelLayoutVariation;
  int                                     mVariation : 4;
};

InputMethodOptions::InputMethodOptions()
{
  mImpl.reset(new Impl());
}

InputMethodOptions::~InputMethodOptions()
{
  // destructor cannot be inlined and must be in a unit
  // for unique_ptr to work with forward declaration
}

bool InputMethodOptions::IsPassword() const
{
  return (mImpl->mPanelLayout == Dali::InputMethod::PanelLayout::PASSWORD);
}

void InputMethodOptions::ApplyProperty(const Property::Map& settings)
{
  for(unsigned int i = 0, count = settings.Count(); i < count; ++i)
  {
    Property::Key key = settings.GetKeyAt(i);
    if(key.type == Property::Key::INDEX)
    {
      continue;
    }

    Property::Value item = settings.GetValue(i);

    if(key == TOKEN_STRING(PANEL_LAYOUT))
    {
      if(item.GetType() == Property::INTEGER)
      {
        int value           = item.Get<int>();
        mImpl->mPanelLayout = NormalizePanelLayout(value);
      }
    }
    else if(key == TOKEN_STRING(BUTTON_ACTION))
    {
      if(item.GetType() == Property::INTEGER)
      {
        int value            = item.Get<int>();
        mImpl->mButtonAction = NormalizeReturnKeyType(value);
      }
    }
    else if(key == TOKEN_STRING(AUTO_CAPITALIZE))
    {
      if(item.GetType() == Property::INTEGER)
      {
        int value           = item.Get<int>();
        mImpl->mAutoCapital = NormalizeAutoCapitalType(value);
      }
    }
    else if(key == TOKEN_STRING(VARIATION))
    {
      if(item.GetType() == Property::INTEGER)
      {
        int value         = item.Get<int>();
        mImpl->mVariation = value;
      }
    }
    else
    {
    }
  }

  // Synchronize the stored public enum from the raw variation and panel layout.
  mImpl->mPanelLayoutVariation = ToPanelLayoutVariation(mImpl->mPanelLayout, mImpl->mVariation);
}

void InputMethodOptions::RetrieveProperty(Property::Map& settings)
{
  settings[TOKEN_STRING(PANEL_LAYOUT)]    = ToLegacyPanelLayoutValue(mImpl->mPanelLayout);
  settings[TOKEN_STRING(BUTTON_ACTION)]   = static_cast<int>(mImpl->mButtonAction);
  settings[TOKEN_STRING(AUTO_CAPITALIZE)] = static_cast<int>(mImpl->mAutoCapital);
  settings[TOKEN_STRING(VARIATION)]       = mImpl->mVariation;
}

bool InputMethodOptions::CompareAndSet(InputMethod::Category::Type type, const InputMethodOptions& options, int& index)
{
  bool updated = false;

  switch(type)
  {
    case PANEL_LAYOUT:
    {
      if(options.mImpl->mPanelLayout != mImpl->mPanelLayout)
      {
        mImpl->mPanelLayout          = options.mImpl->mPanelLayout;
        mImpl->mPanelLayoutVariation = options.mImpl->mPanelLayoutVariation;
        index                        = static_cast<int>(mImpl->mPanelLayout);
        updated                      = true;
      }
      break;
    }
    case BUTTON_ACTION:
    {
      if(options.mImpl->mButtonAction != mImpl->mButtonAction)
      {
        mImpl->mButtonAction = options.mImpl->mButtonAction;
        index                = static_cast<int>(mImpl->mButtonAction);
        updated              = true;
      }
      break;
    }
    case AUTO_CAPITALIZE:
    {
      if(options.mImpl->mAutoCapital != mImpl->mAutoCapital)
      {
        mImpl->mAutoCapital = options.mImpl->mAutoCapital;
        index               = static_cast<int>(mImpl->mAutoCapital);
        updated             = true;
      }
      break;
    }
    case VARIATION:
    {
      if(options.mImpl->mVariation != mImpl->mVariation)
      {
        mImpl->mVariation            = options.mImpl->mVariation;
        mImpl->mPanelLayoutVariation = options.mImpl->mPanelLayoutVariation;
        index                        = static_cast<int>(mImpl->mVariation);
        updated                      = true;
      }
      break;
    }
  }

  return updated;
}

} // namespace Integration
} // namespace Dali
