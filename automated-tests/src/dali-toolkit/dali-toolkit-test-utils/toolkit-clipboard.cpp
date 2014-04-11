//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "toolkit-clipboard.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>


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
   * @copydoc Dali::Clipboard::GetItem()
   */
  std::string GetItem( unsigned int index );

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


private:
  Clipboard( const Clipboard& );
  Clipboard& operator=( Clipboard& );

  static Dali::Clipboard mToolkitClipboard;
}; // class clipboard


Dali::Clipboard Dali::Internal::Adaptor::Clipboard::mToolkitClipboard;


Clipboard::Clipboard( /*Ecore_X_Window ecoreXwin*/)
{
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
  return true;
}

std::string Clipboard::GetItem( unsigned int index )
{
  return "";
}

unsigned int Clipboard::NumberOfItems()
{
  return 0;
}

void Clipboard::ShowClipboard()
{
}

void Clipboard::HideClipboard()
{
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

std::string Clipboard::GetItem( unsigned int index )
{
  return GetImplementation(*this).GetItem( index );
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

} // namespace Dali
