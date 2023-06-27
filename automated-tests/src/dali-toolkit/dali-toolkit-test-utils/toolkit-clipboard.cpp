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

#include "toolkit-clipboard.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/text-clipboard-event-notifier.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Implementation of the Clip Board
 */

class TextClipboard :  public Dali::BaseObject
{
public:

  /**
   * @copydoc Dali::TextClipboardEventNotifier::Get()
   */
  static Dali::TextClipboard Get();

  /**
   * Constructor
   * @param[in] ecoreXwin, The window is created by application.
   */
  TextClipboard(/*Ecore_X_Window ecoreXwin*/);
  virtual ~TextClipboard();

  /**
   * @copydoc Dali::TextClipboard::IsAvailable()
   */
  static bool IsAvailable();

  /**
   * @copydoc Dali::TextClipboard::SetItem()
   */
  bool SetItem(const std::string &itemData);

  /**
   * @copydoc Dali::TextClipboard::RequestItem()
   */
  void RequestItem();

  /**
   * @copydoc Dali::TextClipboard::NumberOfClipboardItems()
   */
  unsigned int NumberOfItems();

  /**
   * @copydoc Dali::TextClipboard::ShowClipboard()
   */
  void ShowClipboard();

  /**
   * @copydoc Dali::TextClipboard::HideClipboard()
   */
  void HideClipboard();

  /**
  * @copydoc Dali::TextClipboard::IsVisible()
  */
  bool IsVisible() const;

private:
  TextClipboard( const TextClipboard& );
  TextClipboard& operator=( TextClipboard& );

  static Dali::TextClipboard mToolkitClipboard;
  bool mVisible;
  std::string mItem;
  int mCount;
}; // class TextClipboard


Dali::TextClipboard Dali::Internal::Adaptor::TextClipboard::mToolkitClipboard;


TextClipboard::TextClipboard()
{
  mVisible = false;
  mCount = 0;
}

TextClipboard::~TextClipboard()
{
}

Dali::TextClipboard TextClipboard::Get()
{
  if( ! mToolkitClipboard )
  {
    mToolkitClipboard = Dali::TextClipboard( new Dali::Internal::Adaptor::TextClipboard() );
  }
  return mToolkitClipboard;
}

bool TextClipboard::IsAvailable()
{
  if(mToolkitClipboard)
  {
    return true;
  }
  return false;
}

bool TextClipboard::SetItem(const std::string &itemData )
{
  mItem = itemData;
  mCount = 1;
  return true;
}

void TextClipboard::RequestItem()
{
  Dali::TextClipboardEventNotifier clipboardEventNotifier(Dali::TextClipboardEventNotifier::Get());
  if ( clipboardEventNotifier )
  {
    clipboardEventNotifier.SetContent( mItem );
    clipboardEventNotifier.EmitContentSelectedSignal();
  }
}

unsigned int TextClipboard::NumberOfItems()
{
  return mCount;
}

void TextClipboard::ShowClipboard()
{
  mVisible = true;
}

void TextClipboard::HideClipboard()
{
  mVisible = false;
}

bool TextClipboard::IsVisible() const
{
  return mVisible;
}

} // namespace Adaptor

} // namespace Internal


inline static Internal::Adaptor::TextClipboard& GetImplementation(Dali::TextClipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::TextClipboard theClipboard = Dali::TextClipboard::Get();
  BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<Internal::Adaptor::TextClipboard&>(object);
}

inline static const  Internal::Adaptor::TextClipboard& GetImplementation(const Dali::TextClipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::TextClipboard theClipboard = Dali::TextClipboard::Get();
  const BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<const Internal::Adaptor::TextClipboard&>(object);
}


TextClipboard::TextClipboard()
{
}
TextClipboard::~TextClipboard()
{
}
TextClipboard::TextClipboard(Internal::Adaptor::TextClipboard *impl)
  : BaseHandle(impl)
{
}

TextClipboard TextClipboard::Get()
{
  return Internal::Adaptor::TextClipboard::Get();
}
bool TextClipboard::SetItem( const std::string &itemData)
{
  return GetImplementation(*this).SetItem( itemData );
}

void TextClipboard::RequestItem()
{
  GetImplementation(*this).RequestItem();
}

unsigned int TextClipboard::NumberOfItems()
{
  return GetImplementation(*this).NumberOfItems();
}

void TextClipboard::ShowClipboard()
{
  GetImplementation(*this).ShowClipboard();
}

void TextClipboard::HideClipboard()
{
  GetImplementation(*this).HideClipboard();
}

bool TextClipboard::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

} // namespace Dali
