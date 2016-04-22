/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-controls/text-selection-popup-impl.h>

// EXTERNAL INCLUDES
#include <libintl.h>
#include <cfloat>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// todo Move this to adaptor??
#define GET_LOCALE_TEXT(string) dgettext("dali-toolkit", string)

const std::string TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME( "textselectionpopupbutton" );
const Dali::Vector4 DEFAULT_OPTION_PRESSED_COLOR( Dali::Vector4( 0.24f, 0.72f, 0.8f, 1.0f ) );

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

#ifdef DGETTEXT_ENABLED

#define POPUP_CUT_STRING GET_LOCALE_TEXT("IDS_COM_BODY_CUT")
#define POPUP_COPY_STRING GET_LOCALE_TEXT("IDS_COM_BODY_COPY")
#define POPUP_PASTE_STRING GET_LOCALE_TEXT("IDS_COM_BODY_PASTE")
#define POPUP_SELECT_STRING GET_LOCALE_TEXT("IDS_COM_SK_SELECT")
#define POPUP_SELECT_ALL_STRING GET_LOCALE_TEXT("IDS_COM_BODY_SELECT_ALL")
#define POPUP_CLIPBOARD_STRING GET_LOCALE_TEXT("IDS_COM_BODY_CLIPBOARD")

#else

#define POPUP_CUT_STRING  "Cut"
#define POPUP_COPY_STRING  "Copy"
#define POPUP_PASTE_STRING  "Paste"
#define POPUP_SELECT_STRING  "Select"
#define POPUP_SELECT_ALL_STRING  "Select All"
#define POPUP_CLIPBOARD_STRING  "Clipboard"

#endif

const char* const OPTION_SELECT_WORD = "option-select_word";                       // "Select Word" popup option.
const char* const OPTION_SELECT_ALL("option-select_all");                          // "Select All" popup option.
const char* const OPTION_CUT("optionCut");                                        // "Cut" popup option.
const char* const OPTION_COPY("optionCopy");                                      // "Copy" popup option.
const char* const OPTION_PASTE("optionPaste");                                    // "Paste" popup option.
const char* const OPTION_CLIPBOARD("optionClipboard");                            // "Clipboard" popup option.

BaseHandle Create()
{
  return Toolkit::TextSelectionPopup::New( NULL );
}

// Setup properties, signals and actions using the type-registry.

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextSelectionPopup, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupMaxSize", VECTOR2,   POPUP_MAX_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupMinSize", VECTOR2,   POPUP_MIN_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "optionMaxSize", VECTOR2,   OPTION_MAX_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "optionMinSize", VECTOR2,   OPTION_MIN_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "optionDividerSize", VECTOR2,   OPTION_DIVIDER_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupClipboardButtonImage", STRING, POPUP_CLIPBOARD_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupCutButtonImage", STRING, POPUP_CUT_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupCopyButtonImage", STRING, POPUP_COPY_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupPasteButtonImage", STRING, POPUP_PASTE_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupSelectButtonImage", STRING, POPUP_SELECT_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupSelectAllButtonImage", STRING, POPUP_SELECT_ALL_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupDividerColor", VECTOR4, POPUP_DIVIDER_COLOR )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupIconColor", VECTOR4, POPUP_ICON_COLOR )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupPressedColor", VECTOR4, POPUP_PRESSED_COLOR )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupPressedImage", STRING, POPUP_PRESSED_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupFadeInDuration", FLOAT, POPUP_FADE_IN_DURATION )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popupFadeOutDuration", FLOAT, POPUP_FADE_OUT_DURATION )

DALI_TYPE_REGISTRATION_END()

} // namespace


Dali::Toolkit::TextSelectionPopup TextSelectionPopup::New( TextSelectionPopupCallbackInterface* callbackInterface )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextSelectionPopup::New\n" );

   // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextSelectionPopup > impl = new TextSelectionPopup( callbackInterface );

  // Pass ownership to CustomActor handle
  Dali::Toolkit::TextSelectionPopup handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextSelectionPopup::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextSelectionPopup selectionPopup = Toolkit::TextSelectionPopup::DownCast( Dali::BaseHandle( object ) );

  if( selectionPopup )
  {
    TextSelectionPopup& impl( GetImpl( selectionPopup ) );

    switch( index )
    {
      case Toolkit::TextSelectionPopup::Property::POPUP_MAX_SIZE:
      {
       impl.SetDimensionToCustomise( POPUP_MAXIMUM_SIZE, value.Get< Vector2 >() );
       break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_MAX_SIZE:
      {
        impl.SetDimensionToCustomise( OPTION_MAXIMUM_SIZE, value.Get< Vector2 >() );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_MIN_SIZE:
      {
        impl.SetDimensionToCustomise( OPTION_MINIMUM_SIZE, value.Get< Vector2>() );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
      {
        impl.SetDimensionToCustomise( OPTION_DIVIDER_SIZE, value.Get< Vector2>() );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::CLIPBOARD, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::CUT, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::COPY, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::PASTE, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::SELECT, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetButtonImage( Toolkit::TextSelectionPopup::SELECT_ALL, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_DIVIDER_COLOR:
      {
        impl.mDividerColor = value.Get< Vector4 >();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_ICON_COLOR:
      {
        impl.mIconColor = value.Get< Vector4 >();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_COLOR:
      {
        impl.mPressedColor = value.Get< Vector4 >();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
      {
        impl.SetPressedImage( value.Get< std::string >() );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
      {
        impl.mFadeInDuration = value.Get < float >();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
      {
        impl.mFadeOutDuration = value.Get < float >();
        break;
      }
    } // switch
  } // TextSelectionPopup
}

Property::Value TextSelectionPopup::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextSelectionPopup selectionPopup = Toolkit::TextSelectionPopup::DownCast( Dali::BaseHandle( object ) );

  if( selectionPopup )
  {
    TextSelectionPopup& impl( GetImpl( selectionPopup ) );

    switch( index )
    {
      case Toolkit::TextSelectionPopup::Property::POPUP_MAX_SIZE:
      {
        value = impl.GetDimensionToCustomise( POPUP_MAXIMUM_SIZE );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_MAX_SIZE:
      {
        value = impl.GetDimensionToCustomise( OPTION_MAXIMUM_SIZE );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_MIN_SIZE:
      {
        value = impl.GetDimensionToCustomise( OPTION_MINIMUM_SIZE );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::OPTION_DIVIDER_SIZE:
      {
        value = impl.GetDimensionToCustomise( OPTION_DIVIDER_SIZE );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::CLIPBOARD ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::CUT ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::COPY ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::PASTE ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::SELECT ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetButtonImage( Toolkit::TextSelectionPopup::SELECT_ALL ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PRESSED_IMAGE:
      {
        value = impl.GetPressedImage();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_FADE_IN_DURATION:
      {
        value = impl.mFadeInDuration;
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_FADE_OUT_DURATION:
      {
        value = impl.mFadeOutDuration;
        break;
      }
    } // switch
  }
  return value;
}

void TextSelectionPopup::EnableButtons( Toolkit::TextSelectionPopup::Buttons buttonsToEnable )
{
  mEnabledButtons = buttonsToEnable;
  mButtonsChanged = true;
}

void TextSelectionPopup::RaiseAbove( Layer target )
{
  if( mToolbar )
  {
    mToolbar.RaiseAbove( target );
  }
}

void TextSelectionPopup::ShowPopup()
{
  if( ( !mPopupShowing || mButtonsChanged ) &&
      ( Toolkit::TextSelectionPopup::NONE != mEnabledButtons ) )
  {
    Actor self = Self();
    AddPopupOptionsToToolbar( mShowIcons, mShowCaptions );

    Animation animation = Animation::New( mFadeInDuration );
    animation.AnimateTo( Property(self, Actor::Property::COLOR_ALPHA), 1.0f  );
    animation.Play();
    mPopupShowing = true;
  }
}

void TextSelectionPopup::HidePopup()
{
  if ( mPopupShowing )
  {
    mPopupShowing = false;
    Actor self = Self();
    Animation animation = Animation::New( mFadeOutDuration );
    animation.AnimateTo( Property(self, Actor::Property::COLOR_ALPHA), 0.0f  );
    animation.FinishedSignal().Connect( this, &TextSelectionPopup::HideAnimationFinished );
    animation.Play();
  }
}

void TextSelectionPopup::OnInitialize()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextSelectionPopup::OnInitialize\n" );
  Actor self = Self();
  self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  self.SetProperty( Actor::Property::COLOR_ALPHA, 0.0f );
}

void TextSelectionPopup::OnStageConnection( int depth )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextSelectionPopup::OnStageConnection\n" );
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // TextSelectionToolbar::OnStageConnection() will set the depths of all the popup's components.
}

void TextSelectionPopup::HideAnimationFinished( Animation& animation )
{
  Actor self = Self();
  if ( !mPopupShowing ) // During the Hide/Fade animation there could be a call to Show the Popup again, mPopupShowing will be true in this case.
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextSelectionPopup::HideAnimationFinished\n" );
    UnparentAndReset( mToolbar );
  }
}

bool TextSelectionPopup::OnCutButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Toolkit::TextSelectionPopup::CUT );
  }

  return true;
}

bool TextSelectionPopup::OnCopyButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::COPY );
  }

  return true;
}

bool TextSelectionPopup::OnPasteButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::PASTE );
  }

  return true;
}

bool TextSelectionPopup::OnSelectButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::SELECT );
  }

  return true;
}

bool TextSelectionPopup::OnSelectAllButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::SELECT_ALL );
  }

  return true;
}

bool TextSelectionPopup::OnClipboardButtonPressed( Toolkit::Button button )
{
  if( mCallbackInterface )
  {
    mCallbackInterface->TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::CLIPBOARD );
  }

  return true;
}

void TextSelectionPopup::SetDimensionToCustomise( const PopupCustomisations& settingToCustomise, const Size& dimension )
{
  switch( settingToCustomise )
  {
    case POPUP_MAXIMUM_SIZE :
    {
      mPopupMaxSize = dimension;
      if ( mToolbar )
      {
        mToolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, dimension );
      }
      break;
    }
    case OPTION_MAXIMUM_SIZE :
    {
      mOptionMaxSize = dimension;
      // Option max size not currently currently supported
      break;
    }
    case OPTION_MINIMUM_SIZE :
    {
      mOptionMinSize = dimension;
      // Option min size not currently currently supported
      break;
    }
    case OPTION_DIVIDER_SIZE :
    {
      mOptionDividerSize = dimension;
      if ( mToolbar )
      {
        // Resize Dividers not currently supported
      }
      break;
    }
  } // switch
}

Size TextSelectionPopup::GetDimensionToCustomise( const PopupCustomisations& settingToCustomise )
{
  switch( settingToCustomise )
  {
    case POPUP_MAXIMUM_SIZE :
    {
      if ( mToolbar )
      {
        return mToolbar.GetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE ).Get< Vector2 >();
      }
      else
      {
        return mPopupMaxSize;
      }
    }
    case OPTION_MAXIMUM_SIZE :
    {
      return mOptionMaxSize;
    }
    case OPTION_MINIMUM_SIZE :
    {
      return mOptionMinSize;
    }
    case OPTION_DIVIDER_SIZE :
    {
      return mOptionDividerSize;
    }
  } // switch

  return Size::ZERO;
}

void TextSelectionPopup::SetButtonImage( Toolkit::TextSelectionPopup::Buttons button, Dali::Image image )
{
   switch ( button )
   {
   break;
   case Toolkit::TextSelectionPopup::CLIPBOARD:
   {
     mClipboardIconImage  = image;
   }
   break;
   case Toolkit::TextSelectionPopup::CUT :
   {
     mCutIconImage = image;
   }
   break;
   case Toolkit::TextSelectionPopup::COPY :
   {
     mCopyIconImage = image;
   }
   break;
   case Toolkit::TextSelectionPopup::PASTE :
   {
     mPasteIconImage = image;
   }
   break;
   case Toolkit::TextSelectionPopup::SELECT :
   {
     mSelectIconImage = image;
   }
   break;
   case Toolkit::TextSelectionPopup::SELECT_ALL :
   {
     mSelectAllIconImage = image;
   }
   break;
   default :
   {
     DALI_ASSERT_DEBUG( "TextSelectionPopup SetPopupImage Unknown Button" );
   }
   } // switch
}

Dali::Image TextSelectionPopup::GetButtonImage( Toolkit::TextSelectionPopup::Buttons button )
{
  switch ( button )
  {
  case Toolkit::TextSelectionPopup::CLIPBOARD :
  {
    return mClipboardIconImage;
  }
  break;
  case Toolkit::TextSelectionPopup::CUT :
  {
    return mCutIconImage;
  }
  break;
  case Toolkit::TextSelectionPopup::COPY :
  {
    return mCopyIconImage;
  }
  break;
  case Toolkit::TextSelectionPopup::PASTE :
  {
    return mPasteIconImage;
  }
  break;
  case Toolkit::TextSelectionPopup::SELECT :
  {
    return mSelectIconImage;
  }
  break;
  case Toolkit::TextSelectionPopup::SELECT_ALL :
  {
    return mSelectAllIconImage;
  }
  break;
  default :
  {
    DALI_ASSERT_DEBUG( "TextSelectionPopup GetPopupImage Unknown Button" );
  }
  } // switch

  return Dali::Image();
}

void TextSelectionPopup::SetPressedImage( const std::string& filename )
{
  mPressedImage = filename;
}

std::string TextSelectionPopup::GetPressedImage() const
{
  return mPressedImage;
}

 void TextSelectionPopup::CreateOrderedListOfPopupOptions()
 {
   mOrderListOfButtons.clear();
   mOrderListOfButtons.reserve( 8u );

   // Create button for each possible option using Option priority
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::CUT, mCutOptionPriority, OPTION_CUT, POPUP_CUT_STRING , mCutIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::CUT)  ) );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::COPY, mCopyOptionPriority, OPTION_COPY, POPUP_COPY_STRING, mCopyIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::COPY)  ) );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::PASTE, mPasteOptionPriority, OPTION_PASTE, POPUP_PASTE_STRING, mPasteIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::PASTE)  ) );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::SELECT, mSelectOptionPriority, OPTION_SELECT_WORD, POPUP_SELECT_STRING, mSelectIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::SELECT)  ) );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::SELECT_ALL, mSelectAllOptionPriority, OPTION_SELECT_ALL, POPUP_SELECT_ALL_STRING, mSelectAllIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::SELECT_ALL)  ) );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::CLIPBOARD, mClipboardOptionPriority, OPTION_CLIPBOARD, POPUP_CLIPBOARD_STRING, mClipboardIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::CLIPBOARD)  ) );

   // Sort the buttons according their priorities.
   std::sort( mOrderListOfButtons.begin(), mOrderListOfButtons.end(), TextSelectionPopup::ButtonPriorityCompare() );
 }

 void TextSelectionPopup::AddOption( const ButtonRequirement& button, bool showDivider, bool showIcons, bool showCaption  )
 {
   // 1. Create a option.
   DALI_LOG_INFO( gLogFilter, Debug::General, "TextSelectionPopup::AddOption\n" );

   Toolkit::PushButton option = Toolkit::PushButton::New();
   option.SetName( button.name );
   option.SetAnimationTime( 0.0f );
   option.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

   switch( button.id )
   {
     case Toolkit::TextSelectionPopup::CUT:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnCutButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::COPY:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnCopyButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::PASTE:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnPasteButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::SELECT:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnSelectButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::SELECT_ALL:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnSelectAllButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::CLIPBOARD:
     {
       option.ClickedSignal().Connect( this, &TextSelectionPopup::OnClipboardButtonPressed );
       break;
     }
     case Toolkit::TextSelectionPopup::NONE:
     {
       // Nothing to do:
       break;
     }
   }

   // 2. Set the options contents.
   if( showCaption )
   {
     // PushButton layout properties.
     option.SetProperty( Toolkit::PushButton::Property::LABEL_PADDING, Vector4( 24.0f, 24.0f, 14.0f, 14.0f ) );

     // Label properties.
     Property::Map buttonLabelProperties;
     buttonLabelProperties.Insert( "text", button.caption );
     option.SetProperty( Toolkit::Button::Property::LABEL, buttonLabelProperties );
   }
   if( showIcons )
   {
     option.SetProperty( Toolkit::PushButton::Property::ICON_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
     option.SetProperty( Toolkit::PushButton::Property::ICON_ALIGNMENT, "TOP" );

     // TODO: This is temporarily disabled until the text-selection-popup image API is changed to strings.
     //option.SetProperty( Toolkit::PushButton::Property::SELECTED_ICON, button.icon );
     //option.SetProperty( Toolkit::PushButton::Property::UNSELECTED_ICON, button.icon );
   }

   // 3. Set the normal option image (blank / Transparent).
   option.SetUnselectedImage( "" );

   // 4. Set the pressed option image.
   // The image can be blank, the color can be used regardless.
   option.SetSelectedImage( mPressedImage );
   option.SetProperty( Toolkit::Button::Property::SELECTED_COLOR, mPressedColor );
   option.SetProperty( Toolkit::Control::Property::STYLE_NAME, TEXT_SELECTION_POPUP_BUTTON_STYLE_NAME );

   // 5 Add option to tool bar
   mToolbar.AddOption( option );

   // 6. Add the divider
   if( showDivider )
   {
     const Size size( mOptionDividerSize.width, 0.0f ); // Height FILL_TO_PARENT

     Toolkit::Control divider = Toolkit::Control::New();
#ifdef DECORATOR_DEBUG
     divider.SetName("Text's popup divider");
#endif
     divider.SetSize( size );
     divider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
     divider.SetBackgroundColor( mDividerColor  );
     mToolbar.AddDivider( divider );
   }
 }

 std::size_t TextSelectionPopup::GetNumberOfEnabledOptions()
 {
   std::size_t numberOfOptions = 0u;
   for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
   {
     const ButtonRequirement& button( *it );
     if( button.enabled )
     {
       ++numberOfOptions;
     }
   }

   return numberOfOptions;
 }

 void TextSelectionPopup::AddPopupOptionsToToolbar( bool showIcons, bool showCaptions )
 {
   DALI_LOG_INFO( gLogFilter, Debug::General, "TextSelectionPopup::AddPopupOptionsToToolbar\n" );

   CreateOrderedListOfPopupOptions();

   mButtonsChanged = false;
   UnparentAndReset( mToolbar);

   if( !mToolbar )
   {
     Actor self = Self();
     mToolbar = Toolkit::TextSelectionToolbar::New();
     if ( mPopupMaxSize != Vector2::ZERO ) // If PopupMaxSize property set then apply to Toolbar. Toolbar currently is not retriving this from json
     {
       mToolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, mPopupMaxSize );
     }
     mToolbar.SetParentOrigin( ParentOrigin::CENTER );
#ifdef DECORATOR_DEBUG
     mToolbar.SetName("TextSelectionToolbar");
#endif
     self.Add( mToolbar );
   }

   // Iterate list of buttons and add active ones to Toolbar
   std::size_t numberOfOptionsRequired =  GetNumberOfEnabledOptions();
   std::size_t numberOfOptionsAdded = 0u;
   for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
   {
     const ButtonRequirement& button( *it );
     if ( button.enabled )
     {
       numberOfOptionsAdded++;
       AddOption(  button, ( numberOfOptionsAdded < numberOfOptionsRequired ) , showIcons, showCaptions );
     }
   }
 }

TextSelectionPopup::TextSelectionPopup( TextSelectionPopupCallbackInterface* callbackInterface )
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mToolbar(),
  mPopupMaxSize(),
  mOptionMaxSize(),
  mOptionMinSize(),
  mOptionDividerSize(),
  mEnabledButtons( Toolkit::TextSelectionPopup::NONE ),
  mCallbackInterface( callbackInterface ),
  mPressedColor( DEFAULT_OPTION_PRESSED_COLOR ),
  mDividerColor( Color::WHITE ),
  mIconColor( Color::WHITE ),
  mSelectOptionPriority( 1 ),
  mSelectAllOptionPriority ( 2 ),
  mCutOptionPriority ( 4 ),
  mCopyOptionPriority ( 3 ),
  mPasteOptionPriority ( 5 ),
  mClipboardOptionPriority( 6 ),
  mFadeInDuration(0.0f),
  mFadeOutDuration(0.0f),
  mShowIcons( false ),
  mShowCaptions( true ),
  mPopupShowing( false ),
  mButtonsChanged( false )
{
}

TextSelectionPopup::~TextSelectionPopup()
{
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali


