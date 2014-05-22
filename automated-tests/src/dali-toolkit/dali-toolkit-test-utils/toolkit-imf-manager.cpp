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
#include "toolkit-imf-manager.h"

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <Ecore_IMF.h>
#include <Ecore_X.h>

#include <dali/dali.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/key-event-integ.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class RenderSurface;


class ImfManager : public Dali::BaseObject
{
public:
  typedef Dali::ImfManager::ImfManagerSignalV2 ImfManagerSignalV2;
  typedef Dali::ImfManager::ImfEventSignalV2 ImfEventSignalV2;

public:
  static Dali::ImfManager Get();

  ImfManager( /* Ecore_X_Window ecoreXwin */ );
  void ConnectCallbacks();
  void DisconnectCallbacks();
  void Activate();
  void Deactivate();
  void Reset();

  Ecore_IMF_Context* GetContext();
  bool RestoreAfterFocusLost() const;
  void SetRestoreAferFocusLost( bool toggle );
  void PreEditChanged( void *data, Ecore_IMF_Context *imfContext, void *event_info );
  void CommitReceived( void *data, Ecore_IMF_Context *imfContext, void *event_info );
  Eina_Bool RetrieveSurrounding( void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition );
  void DeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info );
  void NotifyCursorPosition();
  int GetCursorPosition();
  void SetCursorPosition( unsigned int cursorPosition );
  void SetSurroundingText( std::string text );
  std::string GetSurroundingText();

public:  // Signals
  ImfManagerSignalV2& ActivatedSignal() { return mActivatedSignalV2; }
  ImfEventSignalV2& EventReceivedSignal() { return mEventSignalV2; }

protected:
  virtual ~ImfManager();

private:
  void CreateContext( /*Ecore_X_Window ecoreXwin*/ );
  void DeleteContext();

private:
  // Undefined
  ImfManager( const ImfManager& );
  ImfManager& operator=( ImfManager& );

private:
  Ecore_IMF_Context* mIMFContext;
  int mIMFCursorPosition;
  std::string mSurroundingText;
  bool mRestoreAfterFocusLost:1;             ///< Whether the keyboard needs to be restored (activated ) after focus regained.
  bool mIdleCallbackConnected:1;             ///< Whether the idle callback is already connected.

  std::vector<Dali::Integration::KeyEvent> mKeyEvents; ///< Stores key events to be sent from idle call-back.
  ImfManagerSignalV2      mActivatedSignalV2;
  ImfEventSignalV2        mEventSignalV2;


  static Dali::ImfManager mToolkitImfManager;

public:

inline static Internal::Adaptor::ImfManager& GetImplementation(Dali::ImfManager& imfManager)
{
  Dali::ImfManager actualImfManager = ImfManager::Get();

  BaseObject& handle = actualImfManager.GetBaseObject();
  return static_cast<Internal::Adaptor::ImfManager&>(handle);
}

inline static const  Internal::Adaptor::ImfManager& GetImplementation(const Dali::ImfManager& imfManager)
{
  Dali::ImfManager actualImfManager = ImfManager::Get();

  const BaseObject& handle = imfManager.GetBaseObject();
  return static_cast<const Internal::Adaptor::ImfManager&>(handle);
}

};


namespace
{

// Currently this code is internal to dali/dali/internal/event/text/utf8.h but should be made Public and used from there instead.
size_t Utf8SequenceLength(const unsigned char leadByte)
{
  size_t length = 0;

  if ((leadByte & 0x80) == 0 )          //ASCII character (lead bit zero)
  {
    length = 1;
  }
  else if (( leadByte & 0xe0 ) == 0xc0 ) //110x xxxx
  {
    length = 2;
  }
  else if (( leadByte & 0xf0 ) == 0xe0 ) //1110 xxxx
  {
    length = 3;
  }
  else if (( leadByte & 0xf8 ) == 0xf0 ) //1111 0xxx
  {
    length = 4;
  }

  return length;
}

// Static function calls used by ecore 'c' style callback registration
void Commit( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->CommitReceived( data, imfContext, event_info );
  }
}

void PreEdit( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->PreEditChanged( data, imfContext, event_info );
  }
}

Eina_Bool ImfRetrieveSurrounding(void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    return imfManager->RetrieveSurrounding( data, imfContext, text, cursorPosition );
  }
  else
  {
    return false;
  }
}

/**
 * Called when an IMF delete surrounding event is received.
 * Here we tell the application that it should delete a certain range.
 */
void ImfDeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->DeleteSurrounding( data, imfContext, event_info );
  }
}

} // unnamed namespace

Dali::ImfManager Dali::Internal::Adaptor::ImfManager::mToolkitImfManager;


Dali::ImfManager ImfManager::Get()
{
  Dali::ImfManager manager;

  if( ! mToolkitImfManager )
  {
    mToolkitImfManager = Dali::ImfManager( new Dali::Internal::Adaptor::ImfManager() );
  }
  return mToolkitImfManager;
}

ImfManager::ImfManager( /*Ecore_X_Window ecoreXwin*/ )
: mIMFContext(),
  mIMFCursorPosition( 0 ),
  mSurroundingText(""),
  mRestoreAfterFocusLost( false ),
  mIdleCallbackConnected( false ),
  mKeyEvents()
{
  //ecore_imf_init();
  CreateContext( /*ecoreXwin*/ );
  ConnectCallbacks();
  //VirtualKeyboard::ConnectCallbacks( mIMFContext );
}

ImfManager::~ImfManager()
{
  //VirtualKeyboard::DisconnectCallbacks( mIMFContext );
  DisconnectCallbacks();
  DeleteContext();
  //ecore_imf_shutdown();
}

void ImfManager::CreateContext( /*Ecore_X_Window ecoreXwin*/ )
{
}

void ImfManager::DeleteContext()
{
}

// Callbacks for predicitive text support.
void ImfManager::ConnectCallbacks()
{
  //if ( mIMFContext )  {
  //ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_PREEDIT_CHANGED,    PreEdit,    this );
  //ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_COMMIT,             Commit,     this );
  //ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_DELETE_SURROUNDING, ImfDeleteSurrounding, this );
  //}
}

void ImfManager::DisconnectCallbacks()
{
  // if ( mIMFContext )
  // {
  //   ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_PREEDIT_CHANGED,    PreEdit );
  //   ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_COMMIT,             Commit );
  //   ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_DELETE_SURROUNDING, ImfDeleteSurrounding );
  // }
}

void ImfManager::Activate()
{
  // // Reset mIdleCallbackConnected
  // mIdleCallbackConnected = false;

  // if ( mIMFContext )
  // {
  //   ecore_imf_context_focus_in( mIMFContext );
  //   // emit keyboard activated signal
  //   Dali::ImfManager handle( this );
  //   mActivatedSignalV2.Emit( handle );
  // }
}

void ImfManager::Deactivate()
{
  // if( mIMFContext )
  // {
  //   Reset();
  //   ecore_imf_context_focus_out( mIMFContext );
  // }
  // // Reset mIdleCallbackConnected
  // mIdleCallbackConnected = false;
}

void ImfManager::Reset()
{
  // if ( mIMFContext )
  // {
  //   ecore_imf_context_reset( mIMFContext );
  // }
}

Ecore_IMF_Context* ImfManager::GetContext()
{
  //return mIMFContext;
  return NULL;
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return mRestoreAfterFocusLost;
}

void ImfManager::SetRestoreAferFocusLost( bool toggle )
{
  mRestoreAfterFocusLost = toggle;
}

void ImfManager::PreEditChanged( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
  // char *preEditString( NULL );
  // int cursorPosition( 0 );
  // Eina_List *attrs = NULL;
  // Eina_List *l = NULL;

  // Ecore_IMF_Preedit_Attr *attr;

  // // Retrieves attributes as well as the string the cursor position offset from start of pre-edit string.
  // // the attributes (attrs) is used in languages that use the soft arrows keys to insert characters into a current pre-edit string.
  // ecore_imf_context_preedit_string_with_attributes_get( imfContext, &preEditString, &attrs, &cursorPosition );
  // if ( attrs )
  // {
  //   // iterate through the list of attributes getting the type, start and end position.
  //   for ( l = attrs, (attr =  (Ecore_IMF_Preedit_Attr*)eina_list_data_get(l) ); l; l = eina_list_next(l), ( attr = (Ecore_IMF_Preedit_Attr*)eina_list_data_get(l) ))
  //   {
  //     if ( attr->preedit_type == ECORE_IMF_PREEDIT_TYPE_SUB4 ) // (Ecore_IMF)
  //     {
  //       // check first byte so know how many bytes a character is represented by as keyboard returns cursor position in bytes. Which is different for some languages.
  //       size_t visualCharacterIndex = 0;
  //       size_t byteIndex = 0;
  //       // iterate through null terminated string checking each character's position against the given byte position ( attr->end_index ).
  //       while ( preEditString[byteIndex] != '\0' )
  //       {
  //         // attr->end_index is provided as a byte position not character and we need to know the character position.
  //         size_t currentSequenceLength = Utf8SequenceLength(preEditString[byteIndex]); // returns number of bytes used to represent character.
  //         if ( byteIndex == attr->end_index )
  //         {
  //           cursorPosition = visualCharacterIndex;
  //           break;
  //           // end loop as found cursor position that matches byte position
  //         }
  //         else
  //         {
  //           byteIndex += currentSequenceLength; // jump to next character
  //           visualCharacterIndex++;  // increment character count so we know our position for when we get a match
  //         }
  //         DALI_ASSERT_DEBUG( visualCharacterIndex < strlen( preEditString ));
  //       }
  //     }
  //   }
  // }
  // if ( Dali::Adaptor::IsAvailable() )
  // {
  //   std::string keyString ( preEditString );
  //   int numberOfChars( 0 );
  //   Dali::ImfManager handle( this );
  //   Dali::ImfManager::ImfEventData imfEventData ( Dali::ImfManager::PREEDIT, keyString, cursorPosition, numberOfChars );
  //   Dali::ImfManager::ImfCallbackData callbackData = mEventSignalV2.Emit( handle, imfEventData );
  //   if ( callbackData.update )
  //   {
  //     SetCursorPosition( callbackData.cursorPosition );
  //     SetSurroundingText( callbackData.currentText );
  //     NotifyCursorPosition();
  //   }
  //   if ( callbackData.preeditResetRequired )
  //   {
  //     Reset();
  //   }
  // }
  // free( preEditString );
}

void ImfManager::CommitReceived( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
  // if ( Dali::Adaptor::IsAvailable() )
  // {
  //   const std::string keyString( (char *)event_info );
  //   const int cursorOffset( 0 );
  //   const int numberOfChars( 0 );

  //   Dali::ImfManager handle( this );
  //   Dali::ImfManager::ImfEventData imfEventData ( Dali::ImfManager::COMMIT, keyString, cursorOffset, numberOfChars );
  //   Dali::ImfManager::ImfCallbackData callbackData = mEventSignalV2.Emit( handle, imfEventData );

  //   if ( callbackData.update )
  //   {
  //     SetCursorPosition( callbackData.cursorPosition );
  //     SetSurroundingText( callbackData.currentText );

  //     NotifyCursorPosition();
  //   }
  // }
}

/**
 * Called when an IMF retrieve surround event is received.
 * Here the IMF module wishes to know the string we are working with and where within the string the cursor is
 * We need to signal the application to tell us this information.
 */
Eina_Bool ImfManager::RetrieveSurrounding( void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition )
{
  // std::string keyString ( "" );
  // int cursorOffset( 0 );
  // int numberOfChars( 0 );
  // Dali::ImfManager::ImfEventData imfData ( Dali::ImfManager::GETSURROUNDING , keyString, cursorOffset, numberOfChars );
  // Dali::ImfManager handle( this );
  // mEventSignalV2.Emit( handle, imfData );
  // if ( text )
  // {
  //   std::string surroundingText( GetSurroundingText() );
  //   if ( !surroundingText.empty() )
  //   {
  //     *text = strdup( surroundingText.c_str() );
  //   }
  //   else
  //   {
  //     *text = strdup( "" );
  //   }
  // }
  // if ( cursorPosition )
  // {
  //   *cursorPosition = GetCursorPosition();
  // }
  return EINA_TRUE;
}

/**
 * Called when an IMF delete surrounding event is received.
 * Here we tell the application that it should delete a certain range.
 */
void ImfManager::DeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  // if ( Dali::Adaptor::IsAvailable() )
  // {
  //   Ecore_IMF_Event_Delete_Surrounding* deleteSurroundingEvent = (Ecore_IMF_Event_Delete_Surrounding*) event_info;
  //   const std::string keyString( "" );
  //   const int cursorOffset( deleteSurroundingEvent->offset );
  //   const int numberOfChars( deleteSurroundingEvent->n_chars );
  //   Dali::ImfManager::ImfEventData imfData ( Dali::ImfManager::DELETESURROUNDING , keyString, cursorOffset, numberOfChars );
  //   Dali::ImfManager handle( this );
  //   mEventSignalV2.Emit( handle, imfData );
  // }
}

void ImfManager::NotifyCursorPosition()
{
  // if ( mIMFContext )
  // {
  //   ecore_imf_context_cursor_position_set( mIMFContext, mIMFCursorPosition );
  // }
}

int ImfManager::GetCursorPosition()
{
  return mIMFCursorPosition;
}

void ImfManager::SetCursorPosition( unsigned int cursorPosition )
{
  mIMFCursorPosition = ( int )cursorPosition;
}

void ImfManager::SetSurroundingText( std::string text )
{
  mSurroundingText = text;
}

std::string ImfManager::GetSurroundingText()
{
  return mSurroundingText;
}

} // Adaptor

} // Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

ImfManager::ImfManager()
{
}

ImfManager::~ImfManager()
{
}

ImfManager ImfManager::Get()
{
  return Internal::Adaptor::ImfManager::Get();
}

ImfContext ImfManager::GetContext()
{
  return reinterpret_cast<ImfContext>( Internal::Adaptor::ImfManager::GetImplementation(*this).GetContext() );
}

void ImfManager::Activate()
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).Activate();
}

void ImfManager::Deactivate()
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).Deactivate();
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return Internal::Adaptor::ImfManager::GetImplementation(*this).RestoreAfterFocusLost();
}

void ImfManager::SetRestoreAferFocusLost( bool toggle )
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).SetRestoreAferFocusLost( toggle );
}

void ImfManager::Reset()
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).Reset();
}

void ImfManager::NotifyCursorPosition()
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).NotifyCursorPosition();
}

void ImfManager::SetCursorPosition( unsigned int SetCursorPosition )
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).SetCursorPosition( SetCursorPosition );
}

int ImfManager::GetCursorPosition()
{
  return Internal::Adaptor::ImfManager::GetImplementation(*this).GetCursorPosition();
}

void ImfManager::SetSurroundingText( std::string text )
{
  Internal::Adaptor::ImfManager::GetImplementation(*this).SetSurroundingText( text );
}

std::string ImfManager::GetSurroundingText()
{
  return Internal::Adaptor::ImfManager::GetImplementation(*this).GetSurroundingText();
}

ImfManager::ImfManagerSignalV2& ImfManager::ActivatedSignal()
{
  return Internal::Adaptor::ImfManager::GetImplementation(*this).ActivatedSignal();
}

ImfManager::ImfEventSignalV2& ImfManager::EventReceivedSignal()
{
  return Internal::Adaptor::ImfManager::GetImplementation(*this).EventReceivedSignal();
}

ImfManager::ImfManager(Internal::Adaptor::ImfManager *impl)
  : BaseHandle(impl)
{
}

} // namespace Dali
