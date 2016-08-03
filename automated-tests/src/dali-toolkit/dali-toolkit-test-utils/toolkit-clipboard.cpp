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
#include <dali/devel-api/adaptor-framework/clipboard-event-notifier.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Implementation of the Clip Board
 */

class Clipboard :  public Dali::BaseObject
{
public:

  /**
   * @copydoc Dali::ClipboardEventNotifier::Get()
   */
  static Dali::Clipboard Get();

  /**
   * Constructor
   * @param[in] ecoreXwin, The window is created by application.
   */
  Clipboard(/*Ecore_X_Window ecoreXwin*/);
  virtual ~Clipboard();

  /**
   * @copydoc Dali::Clipboard::SetItem()
   */
  bool SetItem(const std::string &itemData);

  /**
   * @copydoc Dali::Clipboard::RequestItem()
   */
  void RequestItem();

  /**
   * @copydoc Dali::Clipboard::NumberOfClipboardItems()
   */
  unsigned int NumberOfItems();

  /**
   * @copydoc Dali::Clipboard::ShowClipboard()
   */
  void ShowClipboard();

  /**
   * @copydoc Dali::Clipboard::HideClipboard()
   */
  void HideClipboard();

  /**
  * @copydoc Dali::Clipboard::IsVisible()
  */
  bool IsVisible() const;

private:
  Clipboard( const Clipboard& );
  Clipboard& operator=( Clipboard& );

  static Dali::Clipboard mToolkitClipboard;
  bool mVisible;
  std::string mItem;
  int mCount;
}; // class clipboard


Dali::Clipboard Dali::Internal::Adaptor::Clipboard::mToolkitClipboard;


Clipboard::Clipboard()
{
  mVisible = false;
  mCount = 0;
}

Clipboard::~Clipboard()
{
}

Dali::Clipboard Clipboard::Get()
{
  if( ! mToolkitClipboard )
  {
    mToolkitClipboard = Dali::Clipboard( new Dali::Internal::Adaptor::Clipboard() );
  }
  return mToolkitClipboard;
}

bool Clipboard::SetItem(const std::string &itemData )
{
  mItem = itemData;
  mCount = 1;
  return true;
}

void Clipboard::RequestItem()
{
  Dali::ClipboardEventNotifier clipboardEventNotifier(Dali::ClipboardEventNotifier::Get());
  if ( clipboardEventNotifier )
  {
    clipboardEventNotifier.SetContent( mItem );
    clipboardEventNotifier.EmitContentSelectedSignal();
  }
}

unsigned int Clipboard::NumberOfItems()
{
  return mCount;
}

void Clipboard::ShowClipboard()
{
  mVisible = true;
}

void Clipboard::HideClipboard()
{
  mVisible = false;
}

bool Clipboard::IsVisible() const
{
  return mVisible;
}

} // namespace Adaptor

} // namespace Internal


inline static Internal::Adaptor::Clipboard& GetImplementation(Dali::Clipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::Clipboard theClipboard = Dali::Clipboard::Get();
  BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<Internal::Adaptor::Clipboard&>(object);
}

inline static const  Internal::Adaptor::Clipboard& GetImplementation(const Dali::Clipboard& clipboard)
{
  // Bypass any passed in clipboard handle - it probably won't be initialized
  Dali::Clipboard theClipboard = Dali::Clipboard::Get();
  const BaseObject& object = theClipboard.GetBaseObject();
  return static_cast<const Internal::Adaptor::Clipboard&>(object);
}


Clipboard::Clipboard()
{
}
Clipboard::~Clipboard()
{
}
Clipboard::Clipboard(Internal::Adaptor::Clipboard *impl)
  : BaseHandle(impl)
{
}

Clipboard Clipboard::Get()
{
  return Internal::Adaptor::Clipboard::Get();
}
bool Clipboard::SetItem( const std::string &itemData)
{
  return GetImplementation(*this).SetItem( itemData );
}

void Clipboard::RequestItem()
{
  GetImplementation(*this).RequestItem();
}

unsigned int Clipboard::NumberOfItems()
{
  return GetImplementation(*this).NumberOfItems();
}

void Clipboard::ShowClipboard()
{
  GetImplementation(*this).ShowClipboard();
}

void Clipboard::HideClipboard()
{
  GetImplementation(*this).HideClipboard();
}

bool Clipboard::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

} // namespace Dali
