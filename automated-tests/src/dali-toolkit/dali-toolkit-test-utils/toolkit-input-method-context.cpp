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

// CLASS HEADER
#include "toolkit-input-method-context.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class RenderSurface;


class InputMethodContext : public Dali::BaseObject
{
public:
  typedef Dali::InputMethodContext::ActivatedSignalType ActivatedSignalType;
  typedef Dali::InputMethodContext::KeyboardEventSignalType KeyboardEventSignalType;
  typedef Dali::InputMethodContext::StatusSignalType StatusSignalType;
  typedef Dali::InputMethodContext::VoidSignalType VoidSignalType;

public:
  static Dali::InputMethodContext New();

  InputMethodContext( /* Ecore_X_Window ecoreXwin */ );
  void Finalize();
  void ConnectCallbacks();
  void DisconnectCallbacks();
  void Activate();
  void Deactivate();
  void Reset();

  bool RestoreAfterFocusLost() const;
  void SetRestoreAfterFocusLost( bool toggle );
  void NotifyCursorPosition();
  void SetCursorPosition( unsigned int cursorPosition );
  unsigned int GetCursorPosition() const;
  void SetSurroundingText( const std::string& text );
  const std::string& GetSurroundingText() const;
  void ApplyOptions( const InputMethodOptions& options );
  bool FilterEventKey( const Dali::KeyEvent& keyEvent );
  void SetPreeditStyle( Dali::InputMethodContext::PreeditStyle type );
  void GetPreeditStyle( Dali::InputMethodContext::PreEditAttributeDataContainer& attrs ) const;

public:  // Signals
  ActivatedSignalType& ActivatedSignal() { return mActivatedSignal; }
  KeyboardEventSignalType& EventReceivedSignal() { return mEventSignal; }
  KeyboardEventSignalType& KeyboardEventReceivedSignal() { return mKeyboardEventSignal; }
  StatusSignalType& StatusChangedSignal() { return mKeyboardStatusSignal; }
  VoidSignalType& ResizedSignal() { return mKeyboardResizeSignal; }
  VoidSignalType& LanguageChangedSignal() { return mKeyboardLanguageChangedSignal; }

protected:
  virtual ~InputMethodContext();

private:
  void CreateContext( /*Ecore_X_Window ecoreXwin*/ );
  void DeleteContext();

private:
  // Undefined
  InputMethodContext( const InputMethodContext& );
  InputMethodContext& operator=( InputMethodContext& );

private:
  int mIMFCursorPosition;
  std::string mSurroundingText;
  bool mRestoreAfterFocusLost:1;             ///< Whether the keyboard needs to be restored (activated ) after focus regained.
  bool mIdleCallbackConnected:1;             ///< Whether the idle callback is already connected.
  InputMethodOptions        mOptions;
  Dali::InputMethodContext::PreEditAttributeDataContainer mPreeditAttrs; ///< Stores preedit attribute data

  ActivatedSignalType      mActivatedSignal;
  KeyboardEventSignalType  mEventSignal;
  KeyboardEventSignalType  mKeyboardEventSignal;
  StatusSignalType         mKeyboardStatusSignal;
  VoidSignalType           mKeyboardResizeSignal;
  VoidSignalType           mKeyboardLanguageChangedSignal;

  static Dali::InputMethodContext mToolkitInputMethodContext;

public:

inline static Internal::Adaptor::InputMethodContext& GetImplementation(Dali::InputMethodContext& inputMethodContext)
{
  BaseObject& handle = inputMethodContext.GetBaseObject();
  return static_cast<Internal::Adaptor::InputMethodContext&>(handle);
}

inline static const  Internal::Adaptor::InputMethodContext& GetImplementation(const Dali::InputMethodContext& inputMethodContext)
{
  const BaseObject& handle = inputMethodContext.GetBaseObject();
  return static_cast<const Internal::Adaptor::InputMethodContext&>(handle);
}

};

Dali::InputMethodContext Dali::Internal::Adaptor::InputMethodContext::mToolkitInputMethodContext;

Dali::InputMethodContext InputMethodContext::New()
{
  if( ! mToolkitInputMethodContext )
  {
    mToolkitInputMethodContext = Dali::InputMethodContext( new Dali::Internal::Adaptor::InputMethodContext() );
  }
  return mToolkitInputMethodContext;
}

InputMethodContext::InputMethodContext( /*Ecore_X_Window ecoreXwin*/ )
: mIMFCursorPosition( 0 ),
  mSurroundingText(),
  mRestoreAfterFocusLost( false ),
  mIdleCallbackConnected( false )
{
  CreateContext( /*ecoreXwin*/ );
  ConnectCallbacks();
}

InputMethodContext::~InputMethodContext()
{
  DisconnectCallbacks();
  DeleteContext();
}

void InputMethodContext::Finalize()
{
}

void InputMethodContext::CreateContext( /*Ecore_X_Window ecoreXwin*/ )
{
}

void InputMethodContext::DeleteContext()
{
}

// Callbacks for predicitive text support.
void InputMethodContext::ConnectCallbacks()
{
}

void InputMethodContext::DisconnectCallbacks()
{
}

void InputMethodContext::Activate()
{
}

void InputMethodContext::Deactivate()
{
}

void InputMethodContext::Reset()
{
}

bool InputMethodContext::RestoreAfterFocusLost() const
{
  return mRestoreAfterFocusLost;
}

void InputMethodContext::SetRestoreAfterFocusLost( bool toggle )
{
  mRestoreAfterFocusLost = toggle;
}

void InputMethodContext::NotifyCursorPosition()
{
}

void InputMethodContext::SetCursorPosition( unsigned int cursorPosition )
{
  mIMFCursorPosition = static_cast< int >( cursorPosition );
}

unsigned int InputMethodContext::GetCursorPosition() const
{
  return static_cast<unsigned int>( mIMFCursorPosition );
}

void InputMethodContext::SetSurroundingText( const std::string& text )
{
  mSurroundingText = text;
}

const std::string& InputMethodContext::GetSurroundingText() const
{
  return mSurroundingText;
}

void InputMethodContext::ApplyOptions( const InputMethodOptions& options )
{
}

bool InputMethodContext::FilterEventKey( const Dali::KeyEvent& keyEvent )
{
  return false;
}

void InputMethodContext::SetPreeditStyle( Dali::InputMethodContext::PreeditStyle type )
{
  Dali::InputMethodContext::PreeditAttributeData data;
  data.preeditType = type;
  mPreeditAttrs.PushBack( data );
}

void InputMethodContext::GetPreeditStyle( Dali::InputMethodContext::PreEditAttributeDataContainer& attrs ) const
{
  attrs = mPreeditAttrs;
}
} // Adaptor

} // Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

InputMethodContext::InputMethodContext()
{
}

InputMethodContext::~InputMethodContext()
{
}

InputMethodContext InputMethodContext::New()
{
  return InputMethodContext::New( Actor() );
}

InputMethodContext InputMethodContext::New( Actor actor )
{
  return Internal::Adaptor::InputMethodContext::New();
}

void InputMethodContext::Finalize()
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).Finalize();
}

void InputMethodContext::Activate()
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).Activate();
}

void InputMethodContext::Deactivate()
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).Deactivate();
}

bool InputMethodContext::RestoreAfterFocusLost() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).RestoreAfterFocusLost();
}

void InputMethodContext::SetRestoreAfterFocusLost( bool toggle )
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetRestoreAfterFocusLost( toggle );
}

void InputMethodContext::Reset()
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).Reset();
}

void InputMethodContext::NotifyCursorPosition()
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).NotifyCursorPosition();
}

void InputMethodContext::SetCursorPosition( unsigned int SetCursorPosition )
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetCursorPosition( SetCursorPosition );
}

unsigned int InputMethodContext::GetCursorPosition() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetCursorPosition();
}

void InputMethodContext::SetSurroundingText( const std::string& text )
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetSurroundingText( text );
}

const std::string& InputMethodContext::GetSurroundingText() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetSurroundingText();
}

void InputMethodContext::NotifyTextInputMultiLine( bool multiLine )
{
}

void InputMethodContext::ApplyOptions( const InputMethodOptions& options )
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).ApplyOptions( options );
}

bool InputMethodContext::FilterEventKey( const Dali::KeyEvent& keyEvent )
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).FilterEventKey( keyEvent );
}

void InputMethodContext::SetPreeditStyle( Dali::InputMethodContext::PreeditStyle type )
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetPreeditStyle( type );
}

void InputMethodContext::GetPreeditStyle( Dali::InputMethodContext::PreEditAttributeDataContainer& attrs ) const
{
  Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetPreeditStyle( attrs );
}

// Signals
InputMethodContext::ActivatedSignalType& InputMethodContext::ActivatedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).ActivatedSignal();
}

InputMethodContext::KeyboardEventSignalType& InputMethodContext::EventReceivedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).EventReceivedSignal();
}

InputMethodContext::KeyboardEventSignalType& InputMethodContext::KeyboardEventReceivedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).KeyboardEventReceivedSignal();
}

InputMethodContext::StatusSignalType& InputMethodContext::StatusChangedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).StatusChangedSignal();
}

InputMethodContext::VoidSignalType& InputMethodContext::ResizedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).ResizedSignal();
}

InputMethodContext::VoidSignalType& InputMethodContext::LanguageChangedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).LanguageChangedSignal();
}

InputMethodContext::InputMethodContext(Internal::Adaptor::InputMethodContext *impl)
  : BaseHandle(impl)
{
}

} // namespace Dali
