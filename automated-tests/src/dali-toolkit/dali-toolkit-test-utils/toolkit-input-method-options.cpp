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
 * This is a copy of dali-adaptor/devel-api/adaptor-framework/input-method-options.cpp.
 */

// CLASS HEADER
#include <dali/devel-api/adaptor-framework/input-method-options.h>

using namespace Dali::InputMethod;
using namespace Dali::InputMethod::Category;

namespace Dali
{

#define TOKEN_STRING(x) #x

struct InputMethodOptions::Impl
{
  Impl()
  {
    mPanelLayout = PanelLayout::NORMAL;
    mAutoCapital = AutoCapital::SENTENCE;
    mButtonAction = ButtonAction::DEFAULT;
    mVariation = NormalLayout::NORMAL;
  }

  PanelLayout::Type mPanelLayout;
  AutoCapital::Type mAutoCapital;
  ButtonAction::Type mButtonAction;
  int mVariation:4;
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

void InputMethodOptions::ApplyProperty( const Property::Map& settings )
{
  for ( unsigned int i = 0, count = settings.Count(); i < count; ++i )
  {
    Property::Key key = settings.GetKeyAt( i );
    if( key.type == Property::Key::INDEX )
    {
      continue;
    }

    Property::Value item = settings.GetValue(i);

    if( key == TOKEN_STRING( PANEL_LAYOUT ) )
    {
      if( item.GetType() == Property::INTEGER )
      {
        int value = item.Get< int >();
        mImpl->mPanelLayout = static_cast<InputMethod::PanelLayout::Type>(value);
      }
    }
    else if ( key == TOKEN_STRING( BUTTON_ACTION ) )
    {
      if ( item.GetType() == Property::INTEGER )
      {
        int value = item.Get< int >();
        mImpl->mButtonAction = static_cast<InputMethod::ButtonAction::Type>(value);
      }
    }
    else if ( key == TOKEN_STRING( AUTO_CAPITALIZE ) )
    {
      if ( item.GetType() == Property::INTEGER )
      {
        int value = item.Get< int >();
        mImpl->mAutoCapital = static_cast<InputMethod::AutoCapital::Type>(value);
      }
    }
    else if( key == TOKEN_STRING( VARIATION ) )
    {
      if( item.GetType() == Property::INTEGER )
      {
        int value = item.Get< int >();
        mImpl->mVariation = value;
      }
    }
    else
    {
    }
  }
}

void InputMethodOptions::RetrieveProperty( Property::Map& settings )
{
  settings[TOKEN_STRING( PANEL_LAYOUT )] = mImpl->mPanelLayout;
  settings[TOKEN_STRING( BUTTON_ACTION )] = mImpl->mButtonAction;
  settings[TOKEN_STRING( AUTO_CAPITALIZE )] = mImpl->mAutoCapital;
  settings[TOKEN_STRING( VARIATION )] = mImpl->mVariation;
}

bool InputMethodOptions::CompareAndSet( InputMethod::Category::Type type, const InputMethodOptions& options, int& index)
{
  return true;
  bool updated = false;

  switch (type)
  {
    case PANEL_LAYOUT:
    {
      if ( options.mImpl->mPanelLayout != mImpl->mPanelLayout )
      {
        mImpl->mPanelLayout = options.mImpl->mPanelLayout;
        index = static_cast<int>(mImpl->mPanelLayout);
        updated = true;
      }
      break;
    }
    case BUTTON_ACTION:
    {
      if ( options.mImpl->mButtonAction != mImpl->mButtonAction )
      {
        mImpl->mButtonAction = options.mImpl->mButtonAction;
        index = static_cast<int>(mImpl->mButtonAction);
        updated = true;
      }
      break;
    }
    case AUTO_CAPITALIZE:
    {
      if ( options.mImpl->mAutoCapital != mImpl->mAutoCapital )
      {
        mImpl->mAutoCapital = options.mImpl->mAutoCapital;
        index = static_cast<int>(mImpl->mAutoCapital);
        updated = true;
      }
      break;
    }
    case VARIATION:
    {
      if ( options.mImpl->mVariation != mImpl->mVariation )
      {
        mImpl->mVariation = options.mImpl->mVariation;
        index = static_cast<int>(mImpl->mVariation);
        updated = true;
      }
      break;
    }
  }
  return updated;
}

} // namespace Dali
