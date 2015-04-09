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
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <libintl.h>

// todo Move this to adaptor??
#define GET_LOCALE_TEXT(string) dgettext("elementary", string)

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const Dali::Vector4 DEFAULT_POPUP_BACKGROUND( Dali::Vector4( .20f, 0.29f, 0.44f, 1.0f ) );
const Dali::Vector4 DEFAULT_POPUP_BACKGROUND_PRESSED( Dali::Vector4( 0.07f, 0.10f, 0.17f, 1.0f ) );
const Dali::Vector4 DEFAULT_POPUP_LINE_COLOR( Dali::Vector4( 0.36f, 0.45f, 0.59f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_ICON( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_ICON_PRESSED( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT_PRESSED( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

const std::string DEFAULT_POPUP_BACKGROUND_IMAGE( DALI_IMAGE_DIR "popup_bubble_bg.#.png" );
const std::string OPTION_ICON_CLIPBOARD( DALI_IMAGE_DIR "copy_paste_icon_clipboard.png" );
const std::string OPTION_ICON_COPY( DALI_IMAGE_DIR "copy_paste_icon_copy.png" );
const std::string OPTION_ICON_CUT( DALI_IMAGE_DIR "copy_paste_icon_cut.png" );
const std::string OPTION_ICON_PASTE( DALI_IMAGE_DIR "copy_paste_icon_paste.png" );
const std::string OPTION_ICON_SELECT( DALI_IMAGE_DIR "copy_paste_icon_select.png" );
const std::string OPTION_ICON_SELECT_ALL( DALI_IMAGE_DIR "copy_paste_icon_select_all.png" );

const Dali::Vector2 DEFAULT_POPUP_MAX_SIZE( 470.0f, 120.0f ); ///< The maximum size of the popup.

const float OPTION_TEXT_LINE_HEIGHT( 32.0f );     ///< The line height of the text.
const Dali::Vector2 OPTION_ICON_SIZE( 0.f, 0.f );       ///< The size of the icon.
const float OPTION_GAP_ICON_TEXT( 6.f );          ///< The gap between the icon and the text
const float OPTION_MARGIN_WIDTH( 10.f );          ///< The margin between the right or lefts edge and the text or icon.
const float OPTION_MAX_WIDTH( DEFAULT_POPUP_MAX_SIZE.width / 6 ); ///< The maximum width of the option (currently set to the max)
const float OPTION_MIN_WIDTH( 86.0f );           ///< The minimum width of the option.

const float POPUP_DIVIDER_WIDTH( 1.f );        ///< The size of the divider.

const Dali::Vector2 POPUP_TAIL_SIZE( 20.0f, 16.0f ); ///< The size of the tail.
const float POPUP_TAIL_Y_OFFSET( 5.f );        ///< The y offset of the tail (when its position is on the bottom).
const float POPUP_TAIL_TOP_Y_OFFSET( 3.f );    ///< The y offset of the tail (when its position is on the top).

const float HIDE_POPUP_ANIMATION_DURATION( 0.2f ); ///< Duration of popup hide animation in seconds.
const float SHOW_POPUP_ANIMATION_DURATION( 0.2f ); ///< Duration of popup show animation in seconds.

const char* const OPTION_SELECT_WORD = "option-select_word";                       // "Select Word" popup option.
const char* const OPTION_SELECT_ALL("option-select_all");                          // "Select All" popup option.
const char* const OPTION_CUT("option-cut");                                        // "Cut" popup option.
const char* const OPTION_COPY("option-copy");                                      // "Copy" popup option.
const char* const OPTION_PASTE("option-paste");                                    // "Paste" popup option.
const char* const OPTION_CLIPBOARD("option-clipboard");                            // "Clipboard" popup option.

} // namespace

//// Comparison function for ButtonRequirement Priority
//bool TextSelectionPopup::PriorityCompare( ButtonRequirement const& a, ButtonRequirement const& b )
//{
//  return a.priority < b.priority;
//}


Dali::Toolkit::TextSelectionPopup TextSelectionPopup::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextSelectionPopup > impl = new TextSelectionPopup();

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
       impl.SetPopupMaxSize( value.Get< Vector2 >() );
       break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_BACKGROUND_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_BACKGROUND, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_CLIPBOARD_BUTTON, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_CUT_BUTTON_ICON, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_COPY_BUTTON_ICON, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_PASTE_BUTTON_ICON, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_SELECT_BUTTON_ICON, image );
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        impl.SetPopupImage( POPUP_SELECT_ALL_BUTTON_ICON, image );
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
        value = impl.GetPopupMaxSize();
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_BACKGROUND_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_BACKGROUND ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CLIPBOARD_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_CLIPBOARD_BUTTON ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_CUT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_CUT_BUTTON_ICON ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_COPY_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_COPY_BUTTON_ICON ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_PASTE_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_PASTE_BUTTON_ICON ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_SELECT_BUTTON_ICON ) );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::TextSelectionPopup::Property::POPUP_SELECT_ALL_BUTTON_ICON_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( impl.GetPopupImage( POPUP_SELECT_ALL_BUTTON_ICON ) );
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

void TextSelectionPopup::OnInitialize()
{
  CreatePopup();
}

void TextSelectionPopup::OnRelayout( const Vector2& size, RelayoutContainer& container )
{

}

void TextSelectionPopup::SetPopupMaxSize( const Size& maxSize )
{
  mMaxSize = maxSize;
}

const Dali::Vector2& TextSelectionPopup::GetPopupMaxSize() const
{
  return mMaxSize;
}

void TextSelectionPopup::SetPopupImage( PopupParts part, Dali::Image image )
{
   switch ( part )
   {
   case POPUP_BACKGROUND :
   {
     mBackgroundImage = image;
   }
   break;
   case POPUP_CLIPBOARD_BUTTON :
   {
     mClipboardIconImage  = image;
   }
   break;
   case POPUP_CUT_BUTTON_ICON :
   {
     mCutIconImage = image;
   }
   break;
   case POPUP_COPY_BUTTON_ICON :
   {
     mCopyIconImage = image;
   }
   break;
   case POPUP_PASTE_BUTTON_ICON :
   {
     mPasteIconImage = image;
   }
   break;
   case POPUP_SELECT_BUTTON_ICON :
   {
     mSelectIconImage = image;
   }
   break;
   case POPUP_SELECT_ALL_BUTTON_ICON :
   {
     mSelectAllIconImage = image;
   }
   break;

   } // switch
}

Dali::Image TextSelectionPopup::GetPopupImage( PopupParts part )
{
  switch ( part )
  {
  case POPUP_BACKGROUND :
  {
    return mBackgroundImage;
  }
  break;
  case POPUP_CLIPBOARD_BUTTON :
  {
    return mClipboardIconImage;
  }
  break;
  case POPUP_CUT_BUTTON_ICON :
  {
    return mCutIconImage;
  }
  break;
  case POPUP_COPY_BUTTON_ICON :
  {
    return mCopyIconImage;
  }
  break;
  case POPUP_PASTE_BUTTON_ICON :
  {
    return mPasteIconImage;
  }
  break;
  case POPUP_SELECT_BUTTON_ICON :
  {
    return mSelectIconImage;
  }
  break;
  case POPUP_SELECT_ALL_BUTTON_ICON :
  {
    return mSelectAllIconImage;
  }
  break;
  default :
  {
    DALI_ASSERT_DEBUG( "Unknown Popup Part" );
  }
  } // switch

  return Dali::Image();
}

 void TextSelectionPopup::CreateOrderedListOfPopupOptions()
 {
   mOrderListOfButtons.clear();

   // Create button for each possible option using Option priority
   if ( !mCutIconImage )
   {
     mCutIconImage = ResourceImage::New( OPTION_ICON_CUT );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsCut, mCutOptionPriority, OPTION_CUT, GET_LOCALE_TEXT("IDS_COM_BODY_CUT"), mCutIconImage, true ) );

   if ( !mCopyIconImage )
   {
     mCopyIconImage = ResourceImage::New( OPTION_ICON_COPY );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsCopy, mCopyOptionPriority, OPTION_COPY, GET_LOCALE_TEXT("IDS_COM_BODY_COPY"), mCopyIconImage, true ) );

   if ( !mPasteIconImage )
   {
     mPasteIconImage = ResourceImage::New( OPTION_ICON_PASTE );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsPaste, mPasteOptionPriority, OPTION_PASTE, GET_LOCALE_TEXT("IDS_COM_BODY_PASTE"), mPasteIconImage, true ) );

   if ( !mSelectIconImage )
   mSelectIconImage = ResourceImage::New( OPTION_ICON_SELECT );
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsSelect, mSelectOptionPriority, OPTION_SELECT_WORD, GET_LOCALE_TEXT("IDS_COM_SK_SELECT"), mSelectIconImage, true ) );

   if ( !mSelectAllIconImage )
   {
    mSelectAllIconImage = ResourceImage::New( OPTION_ICON_SELECT_ALL );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsSelectAll, mSelectAllOptionPriority, OPTION_SELECT_ALL, GET_LOCALE_TEXT("IDS_COM_BODY_SELECT_ALL"), mSelectAllIconImage, true ) );

   if ( !mClipboardIconImage )
   {
     mClipboardIconImage = ResourceImage::New( OPTION_ICON_CLIPBOARD );
   }
   mOrderListOfButtons.push_back( ButtonRequirement( ButtonsClipboard, mClipboardOptionPriority, OPTION_CLIPBOARD, GET_LOCALE_TEXT("IDS_COM_BODY_CLIPBOARD"), mClipboardIconImage, true ) );

   // Sort the buttons according their priorities.
   std::sort( mOrderListOfButtons.begin(), mOrderListOfButtons.end(), TextSelectionPopup::ButtonPriorityCompare() );
 }

 void TextSelectionPopup::CreateBackground()
 {
   if ( !mBackgroundImage )
   {
     mBackgroundImage = ResourceImage::New( DEFAULT_POPUP_BACKGROUND_IMAGE );
   }

   NinePatchImage backgroundImageNinePatch = NinePatchImage::DownCast( mBackgroundImage );
   if( backgroundImageNinePatch )
   {
     const Size ninePatchImageSize = Size( static_cast<float>( mBackgroundImage.GetWidth() ), static_cast<float>( mBackgroundImage.GetHeight() ) );
     Rect<int> childRect = backgroundImageNinePatch.GetChildRectangle();

     // -1u because of the cropping.
     mNinePatchMargins.x = childRect.x - 1u;
     mNinePatchMargins.y = ninePatchImageSize.width - ( childRect.x + childRect.width ) - 1u;
     mNinePatchMargins.z = childRect.y - 1u;
     mNinePatchMargins.w = ninePatchImageSize.height - ( childRect.y + childRect.height ) - 1u;
   }

   SetBackgroundImage( mBackgroundImage );
   SetBackgroundColor( mBackgroundColor );
 }

 void TextSelectionPopup::AddOption( Actor& parent, const std::string& name, const std::string& caption, const Image iconImage, bool finalOption, bool showIcons )
 {
   // 1. Create the backgrounds for the popup option both normal and pressed.
   // Both containers will be added to a button.
   Actor optionContainer = Actor::New();
   optionContainer.SetRelayoutEnabled( true );
   optionContainer.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   optionContainer.SetDrawMode( DrawMode::OVERLAY );
   optionContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

   ImageActor optionPressedContainer = Toolkit::CreateSolidColorActor( mBackgroundPressedColor );
   optionPressedContainer.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   optionPressedContainer.SetDrawMode( DrawMode::OVERLAY );
   optionPressedContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

   // 2. Add text.
   Toolkit::TextLabel captionTextLabel = Toolkit::TextLabel::New();
   captionTextLabel.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   captionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
   // optionContainer.Add( captionTextLabel ); Temporary removed.

   Toolkit::TextLabel pressedCaptionTextLabel = Toolkit::TextLabel::New();
   pressedCaptionTextLabel.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   pressedCaptionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
   // optionPressedContainer.Add( pressedCaptionTextLabel ); Temporary removed.

   // Calculates the icon/text position.
   float iconTextOffsetY = 0.0f;

   if ( showIcons )
   {
     // 3. Create the icons
     ImageActor pressedIcon = ImageActor::New(  iconImage );
     ImageActor icon = ImageActor::New(  iconImage );

     optionContainer.Add( icon );
     optionPressedContainer.Add( pressedIcon );

     iconTextOffsetY = 0.5f * ( ( DEFAULT_POPUP_MAX_SIZE.height - mNinePatchMargins.z - mNinePatchMargins.w ) - ( OPTION_ICON_SIZE.height + OPTION_GAP_ICON_TEXT + OPTION_TEXT_LINE_HEIGHT ) );

     icon.SetParentOrigin( ParentOrigin::TOP_CENTER );
     icon.SetAnchorPoint( AnchorPoint::TOP_CENTER );
     icon.SetY( iconTextOffsetY );

     pressedIcon.SetParentOrigin( ParentOrigin::TOP_CENTER );
     pressedIcon.SetAnchorPoint( AnchorPoint::TOP_CENTER );
     pressedIcon.SetY( iconTextOffsetY );

     // Layout icon + gap + text
     captionTextLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
     pressedCaptionTextLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
     pressedCaptionTextLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
     captionTextLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
     pressedCaptionTextLabel.SetY( -iconTextOffsetY );
     captionTextLabel.SetY( -iconTextOffsetY );
   }
   else
   {
     // Centre option text
     captionTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
     captionTextLabel.SetParentOrigin( ParentOrigin::CENTER );
     pressedCaptionTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
     pressedCaptionTextLabel.SetParentOrigin( ParentOrigin::CENTER );
   }

   // Calculate the size of the text.
   Vector3 textSize = captionTextLabel.GetNaturalSize();
   textSize.width = std::min( textSize.width, OPTION_MAX_WIDTH - 2.f * OPTION_MARGIN_WIDTH );

   // Set the size to the text. Text will be ellipsized if exceeds the max width.
   captionTextLabel.SetSize( textSize.GetVectorXY() );
   pressedCaptionTextLabel.SetSize( textSize.GetVectorXY() );

   // 4. Calculate the size of option.

   // The width is the max size of the text or the icon plus the margins clamped between the option min and max size.
   // The height is the whole popup height minus the ninepatch margins.
   const Vector2 optionSize( std::min( OPTION_MAX_WIDTH, std::max( OPTION_MIN_WIDTH, std::max( textSize.width, OPTION_ICON_SIZE.width ) + 2.f * OPTION_MARGIN_WIDTH ) ),
                             DEFAULT_POPUP_MAX_SIZE.height - mNinePatchMargins.z - mNinePatchMargins.w );

   optionContainer.SetSize( optionSize );
   optionPressedContainer.SetSize( optionSize );

   // 5. Create a option.
   Toolkit::PushButton option = Toolkit::PushButton::New();
   option.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   option.SetSize( optionSize );
   option.SetAnchorPoint( AnchorPoint::TOP_LEFT );
   option.SetX( mContentSize.width );
   option.SetName( name );
   option.SetAnimationTime( 0.0f );
   //option.ClickedSignal().Connect( this, &TextInputPopup::OnButtonPressed );

   parent.Add( option );

   // 6. Set the normal option image.
   option.SetButtonImage( optionContainer );

   // 7. Set the pressed option image
   option.SetSelectedImage( optionPressedContainer );

   // 8. Update the content size.
   mContentSize.width += optionSize.width;
   mContentSize.height = std::max ( optionSize.height, mContentSize.height );

   // 9. Add the divider
   if( !finalOption )
   {
     const Size size( POPUP_DIVIDER_WIDTH, mContentSize.height );

     ImageActor divider =  Toolkit::CreateSolidColorActor( Color::WHITE );
     divider.SetResizePolicy( FIXED, ALL_DIMENSIONS );
     divider.SetSize( size );
     divider.SetParentOrigin( ParentOrigin::TOP_LEFT );
     divider.SetAnchorPoint( AnchorPoint::TOP_LEFT );
     divider.SetPosition( mContentSize.width - POPUP_DIVIDER_WIDTH, 0.0f );
     parent.Add( divider );
   }
 }

 void TextSelectionPopup::SetUpPopup( Size& size )
 {
   Actor self = Self();

   // Create Layer and Stencil.
   mStencilLayer = Layer::New();
   ImageActor stencil = CreateSolidColorActor( Color::RED );
   stencil.SetDrawMode( DrawMode::STENCIL );
   stencil.SetVisible( true );
   Actor scrollview = Actor::New(); //todo make a scrollview
   stencil.SetRelayoutEnabled( true );

   self.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   self.SetSize( mRequiredPopUpSize ); // control matches stencil size

   mStencilLayer.SetResizePolicy( FIXED, ALL_DIMENSIONS );
   mStencilLayer.SetSize( size ); // matches stencil size

   stencil.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
   scrollview.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
   mButtons.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );

   mStencilLayer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
   scrollview.SetAnchorPoint(AnchorPoint::TOP_LEFT);
   mButtons.SetAnchorPoint( AnchorPoint::TOP_LEFT );

   mStencilLayer.SetPosition( mNinePatchMargins.x,  mNinePatchMargins.y );

   self.Add( mStencilLayer );
   mStencilLayer.Add( stencil );
   mStencilLayer.Add( scrollview );
   scrollview.Add( mButtons );
 }

 void TextSelectionPopup::AddPopupOptions( bool createTail, bool showIcons )
 {
   mShowIcons = showIcons;

   mContentSize = Vector2::ZERO;

   mButtons = Actor::New();
   mButtons.SetRelayoutEnabled( true );

   // Add the options into the buttons container.

   // 1. Determine how many buttons are active and should be added to container.
   std::size_t numberOfOptions = 0u;
   for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
   {
     const ButtonRequirement& button( *it );
     if( button.enabled )
     {
       ++numberOfOptions;
     }
   }

   // 2. Iterate list of buttons and add active ones.
   std::size_t optionsAdded = 0u;
   for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
   {
     const ButtonRequirement& button( *it );
     if ( button.enabled )
     {
       ++optionsAdded;
       AddOption( mButtons, button.name, button.caption, button.icon, optionsAdded == numberOfOptions, mShowIcons );
     }
   }

   // Calculate the size of the whole popup which may not be all visible.
   mRequiredPopUpSize = Size( std::min( mMaxSize.width, mContentSize.width + mNinePatchMargins.x + mNinePatchMargins.y ), DEFAULT_POPUP_MAX_SIZE.height );

   // Size of the contents within the popup
   mVisiblePopUpSize = Size( mRequiredPopUpSize.width - mNinePatchMargins.x - mNinePatchMargins.y, mRequiredPopUpSize.height - mNinePatchMargins.z - mNinePatchMargins.w );
 }

 void TextSelectionPopup::CreatePopup()
 {
   if ( !mStencilLayer )
   {
     CreateOrderedListOfPopupOptions();  //todo Currently causes all options to be shown
     CreateBackground();
     AddPopupOptions( true, true );
     SetUpPopup( mVisiblePopUpSize );
   }

   mStencilLayer.RaiseToTop();
 }

TextSelectionPopup::TextSelectionPopup()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_NONE ) ),
  mMaxSize ( DEFAULT_POPUP_MAX_SIZE ),
  mVisiblePopUpSize( DEFAULT_POPUP_MAX_SIZE ),
  mRequiredPopUpSize( DEFAULT_POPUP_MAX_SIZE ),
  mBackgroundColor( DEFAULT_POPUP_BACKGROUND ),
  mBackgroundPressedColor( DEFAULT_POPUP_BACKGROUND_PRESSED ),
  mLineColor( DEFAULT_POPUP_LINE_COLOR ),
  mIconColor( DEFAULT_OPTION_ICON ),
  mIconPressedColor( DEFAULT_OPTION_ICON_PRESSED ),
  mTextColor( DEFAULT_OPTION_TEXT ),
  mTextPressedColor( DEFAULT_OPTION_TEXT_PRESSED ),
  mSelectOptionPriority( 1 ),
  mSelectAllOptionPriority ( 2 ),
  mCutOptionPriority ( 3 ),
  mCopyOptionPriority ( 4 ),
  mPasteOptionPriority ( 5 ),
  mClipboardOptionPriority( 6 ),
  mShowIcons( true )
{
}

TextSelectionPopup::~TextSelectionPopup()
{
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
