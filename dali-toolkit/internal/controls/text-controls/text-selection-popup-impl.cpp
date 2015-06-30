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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-toolkit/public-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <libintl.h>
#include <cfloat>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
// todo Move this to adaptor??
#define GET_LOCALE_TEXT(string) dgettext("elementary", string)

const Dali::Vector4 DEFAULT_POPUP_DIVIDER_COLOR( Dali::Vector4( 0.23f, 0.72f, 0.8f, 0.11f ) );
const Dali::Vector4 DEFAULT_OPTION_ICON( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_PRESSED_COLOR( Dali::Vector4( 0.24f, 0.72f, 0.8f, 0.11f ) );

const std::string DEFAULT_POPUP_BACKGROUND_IMAGE( DALI_IMAGE_DIR "selection-popup-bg#.png" );
const std::string OPTION_ICON_CLIPBOARD( DALI_IMAGE_DIR "copy_paste_icon_clipboard.png" );
const std::string OPTION_ICON_COPY( DALI_IMAGE_DIR "copy_paste_icon_copy.png" );
const std::string OPTION_ICON_CUT( DALI_IMAGE_DIR "copy_paste_icon_cut.png" );
const std::string OPTION_ICON_PASTE( DALI_IMAGE_DIR "copy_paste_icon_paste.png" );
const std::string OPTION_ICON_SELECT( DALI_IMAGE_DIR "copy_paste_icon_select.png" );
const std::string OPTION_ICON_SELECT_ALL( DALI_IMAGE_DIR "copy_paste_icon_select_all.png" );

const float OPTION_MARGIN_WIDTH( 10.f );          ///< The margin between the right or lefts edge and the text or icon.

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
const char* const OPTION_CUT("option-cut");                                        // "Cut" popup option.
const char* const OPTION_COPY("option-copy");                                      // "Copy" popup option.
const char* const OPTION_PASTE("option-paste");                                    // "Paste" popup option.
const char* const OPTION_CLIPBOARD("option-clipboard");                            // "Clipboard" popup option.

BaseHandle Create()
{
  return Toolkit::TextSelectionPopup::New( Toolkit::TextSelectionPopup::NONE, NULL );
}

// Setup properties, signals and actions using the type-registry.

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextSelectionPopup, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-max-size", VECTOR2,   POPUP_MAX_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-min-size", VECTOR2,   POPUP_MIN_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "option-max-size", VECTOR2,   OPTION_MAX_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "option-min-size", VECTOR2,   OPTION_MIN_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "option-divider-size", VECTOR2,   OPTION_DIVIDER_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-clipboard-button-image", STRING, POPUP_CLIPBOARD_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-cut-button-image", STRING, POPUP_CUT_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-copy-button-image", STRING, POPUP_COPY_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-paste-button-image", STRING, POPUP_PASTE_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-select-button-image", STRING, POPUP_SELECT_BUTTON_ICON_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionPopup, "popup-select-all-button-image", STRING, POPUP_SELECT_ALL_BUTTON_ICON_IMAGE )

DALI_TYPE_REGISTRATION_END()

} // namespace


Dali::Toolkit::TextSelectionPopup TextSelectionPopup::New( Toolkit::TextSelectionPopup::Buttons buttonsToEnable,
                                                           TextSelectionPopupCallbackInterface* callbackInterface )
{
   // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextSelectionPopup > impl = new TextSelectionPopup( callbackInterface );

  // Pass ownership to CustomActor handle
  Dali::Toolkit::TextSelectionPopup handle( *impl );

  impl->mEnabledButtons = buttonsToEnable;

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
      case Toolkit::TextSelectionPopup::Property::POPUP_MIN_SIZE:
      {
        impl.SetDimensionToCustomise( POPUP_MINIMUM_SIZE, value.Get< Vector2 >() );
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
    } // switch
  }
  return value;
}

void TextSelectionPopup::RaiseAbove( Layer target )
{
  if( mToolbar )
  {
    mToolbar.RaiseAbove( target );
  }
}

void TextSelectionPopup::OnInitialize()
{
  CreatePopup();
}

void TextSelectionPopup::OnStageConnection( int depth )
{
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // TextSelectionToolbar::OnStageConnection() will set the depths of all the popup's components.
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
      Actor self = Self();
      mMaxSize = dimension;
      if ( mToolbar )
      {
        mToolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, mMaxSize );
      }
      break;
    }
    case POPUP_MINIMUM_SIZE :
    {
      Actor self = Self();
      mMinSize = dimension;
      // Option can not be smaller than this if only one.
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
      return mMaxSize;
    }
    case POPUP_MINIMUM_SIZE :
    {
      return mMinSize;
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

 void TextSelectionPopup::CreateOrderedListOfPopupOptions()
 {
   mOrderListOfButtons.clear();
   mOrderListOfButtons.reserve( 8u );

   // Create button for each possible option using Option priority
   if ( !mCutIconImage )
   {
     mCutIconImage = ResourceImage::New( OPTION_ICON_CUT );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::CUT, mCutOptionPriority, OPTION_CUT, POPUP_CUT_STRING , mCutIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::CUT)  ) );

   if ( !mCopyIconImage )
   {
     mCopyIconImage = ResourceImage::New( OPTION_ICON_COPY );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::COPY, mCopyOptionPriority, OPTION_COPY, POPUP_COPY_STRING, mCopyIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::COPY)  ) );

   if ( !mPasteIconImage )
   {
     mPasteIconImage = ResourceImage::New( OPTION_ICON_PASTE );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::PASTE, mPasteOptionPriority, OPTION_PASTE, POPUP_PASTE_STRING, mPasteIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::PASTE)  ) );

   if ( !mSelectIconImage )
   mSelectIconImage = ResourceImage::New( OPTION_ICON_SELECT );
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::SELECT, mSelectOptionPriority, OPTION_SELECT_WORD, POPUP_SELECT_STRING, mSelectIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::SELECT)  ) );

   if ( !mSelectAllIconImage )
   {
    mSelectAllIconImage = ResourceImage::New( OPTION_ICON_SELECT_ALL );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::SELECT_ALL, mSelectAllOptionPriority, OPTION_SELECT_ALL, POPUP_SELECT_ALL_STRING, mSelectAllIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::SELECT_ALL)  ) );

   if ( !mClipboardIconImage )
   {
     mClipboardIconImage = ResourceImage::New( OPTION_ICON_CLIPBOARD );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( Toolkit::TextSelectionPopup::CLIPBOARD, mClipboardOptionPriority, OPTION_CLIPBOARD, POPUP_CLIPBOARD_STRING, mClipboardIconImage, ( mEnabledButtons & Toolkit::TextSelectionPopup::CLIPBOARD)  ) );

   // Sort the buttons according their priorities.
   std::sort( mOrderListOfButtons.begin(), mOrderListOfButtons.end(), TextSelectionPopup::ButtonPriorityCompare() );
 }

 void TextSelectionPopup::AddOption( const ButtonRequirement& button, bool showDivider, bool showIcons, bool showCaption  )
 {
   const std::string& name = button.name;
   const std::string& caption = button.caption;
   Image iconImage = button.icon;

   // 1. Create the backgrounds for the popup option both normal and pressed.
   // Both containers will be added to a button.

   Toolkit::TableView optionContainer = Toolkit::TableView::New( (showIcons&showCaption)?2:1 , 1 );
   optionContainer.SetFitHeight( 0 );
   optionContainer.SetFitWidth( 0 );

   Toolkit::TableView  optionPressedContainer = Toolkit::TableView::New( (showIcons&showCaption)?2:1 , 1 );
   optionPressedContainer.SetFitHeight( 0 );
   optionPressedContainer.SetFitWidth( 0 );
   optionPressedContainer.SetBackgroundColor( mPressedColor );

#ifdef DECORATOR_DEBUG
   optionContainer.SetName("optionContainer");
   optionPressedContainer.SetName("optionPressedContainer");
#endif
   // 2. Add text.

   if ( showCaption )
   {
     Toolkit::TextLabel captionTextLabel = Toolkit::TextLabel::New();
     captionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
     captionTextLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

     Toolkit::TextLabel pressedCaptionTextLabel = Toolkit::TextLabel::New();
     pressedCaptionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
     pressedCaptionTextLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

     Padding padding;
     padding.left = 24.0f;
     padding.right = 24.0f;
     padding.top = 14.0f;
     padding.bottom = 14.0f;
     captionTextLabel.SetPadding( padding );
     pressedCaptionTextLabel.SetPadding( padding );

     optionContainer.AddChild( captionTextLabel, Toolkit::TableView::CellPosition(( showIcons&showCaption)?1:0, 0 )  );
     optionPressedContainer.AddChild( pressedCaptionTextLabel, Toolkit::TableView::CellPosition(( showIcons&showCaption)?1:0, 0 ) );
   }

   int depth = Self().GetHierarchyDepth();
   // 3. Create the icons
   if ( showIcons )
   {
     ImageActor pressedIcon = ImageActor::New(  iconImage );
     ImageActor icon = ImageActor::New(  iconImage );
     icon.SetSortModifier( DECORATION_DEPTH_INDEX + depth - 1 );
     pressedIcon.SetSortModifier( DECORATION_DEPTH_INDEX + depth - 1 );

     icon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
     pressedIcon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
     icon.SetColor( mIconColor );

     if ( showCaption & showIcons )
     {
       optionContainer.SetFitHeight( 1 );
       optionContainer.SetFitWidth( 1 );
       optionPressedContainer.SetFitHeight( 1 );
       optionPressedContainer.SetFitWidth( 1 );
     }

     optionContainer.AddChild( icon, Toolkit::TableView::CellPosition( 0, 0 )  );
     optionPressedContainer.AddChild( pressedIcon, Toolkit::TableView::CellPosition( 0, 0 )  );

     icon.SetPadding( Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
     pressedIcon.SetPadding( Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
   }

   // 4. Create a option.
   Toolkit::PushButton option = Toolkit::PushButton::New();
   option.SetName( name );
   option.SetAnimationTime( 0.0f );
   option.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

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

   // 5. Set the normal option image.
   option.SetButtonImage( optionContainer );

   // 6. Set the pressed option image
   option.SetSelectedImage( optionPressedContainer );

   // 7 Add option to tool bar
   mToolbar.AddOption( option );

   // 8. Add the divider
   if( showDivider )
   {
     const Size size( mOptionDividerSize.width, 0.0f ); // Height FILL_TO_PARENT

     ImageActor divider = Toolkit::CreateSolidColorActor( Color::WHITE );
#ifdef DECORATOR_DEBUG
     divider.SetName("Text's popup divider");
#endif
     divider.SetSize( size );
     divider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
     divider.SetColor( mDividerColor );
     divider.SetSortModifier( DECORATION_DEPTH_INDEX + depth );
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

 void TextSelectionPopup::CreatePopup()
 {
   Actor self = Self();
   CreateOrderedListOfPopupOptions();  //todo Currently causes all options to be shown
   self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
   SetBackgroundImage( NinePatchImage::New( DEFAULT_POPUP_BACKGROUND_IMAGE ) );

   if( !mToolbar )
   {
     mToolbar = Toolkit::TextSelectionToolbar::New();
     mToolbar.SetParentOrigin( ParentOrigin::CENTER );
     mToolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, mMaxSize );
     self.Add( mToolbar );
     AddPopupOptionsToToolbar( mShowIcons, mShowCaptions );
   }
 }

TextSelectionPopup::TextSelectionPopup( TextSelectionPopupCallbackInterface* callbackInterface )
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mToolbar(),
  mMaxSize(),
  mMinSize(),
  mOptionDividerSize( Size( 2.0f, 0.0f) ),
  mEnabledButtons( Toolkit::TextSelectionPopup::NONE ),
  mCallbackInterface( callbackInterface ),
  mDividerColor( DEFAULT_POPUP_DIVIDER_COLOR ),
  mIconColor( DEFAULT_OPTION_ICON ),
  mPressedColor( DEFAULT_OPTION_PRESSED_COLOR ),
  mSelectOptionPriority( 1 ),
  mSelectAllOptionPriority ( 2 ),
  mCutOptionPriority ( 4 ),
  mCopyOptionPriority ( 3 ),
  mPasteOptionPriority ( 5 ),
  mClipboardOptionPriority( 6 ),
  mShowIcons( false ),
  mShowCaptions( true )
{
}

TextSelectionPopup::~TextSelectionPopup()
{
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali


