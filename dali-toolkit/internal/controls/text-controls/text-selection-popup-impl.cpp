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
#include <dali-toolkit/public-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <libintl.h>
#include <cfloat>

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
const Dali::Vector4 DEFAULT_OPTION_ICON_PRESSED( Dali::Vector4( 0.5f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT_PRESSED( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

const std::string DEFAULT_POPUP_BACKGROUND_IMAGE( DALI_IMAGE_DIR "popup_bubble_bg.#.png" );
const std::string OPTION_ICON_CLIPBOARD( DALI_IMAGE_DIR "copy_paste_icon_clipboard.png" );
const std::string OPTION_ICON_COPY( DALI_IMAGE_DIR "copy_paste_icon_copy.png" );
const std::string OPTION_ICON_CUT( DALI_IMAGE_DIR "copy_paste_icon_cut.png" );
const std::string OPTION_ICON_PASTE( DALI_IMAGE_DIR "copy_paste_icon_paste.png" );
const std::string OPTION_ICON_SELECT( DALI_IMAGE_DIR "copy_paste_icon_select.png" );
const std::string OPTION_ICON_SELECT_ALL( DALI_IMAGE_DIR "copy_paste_icon_select_all.png" );

const Dali::Vector2 DEFAULT_POPUP_MAX_SIZE( 450.0f, 100.0f ); ///< The maximum size of the popup.

const Dali::Vector2 OPTION_ICON_SIZE( 65.0f, 65.0f );       ///< The size of the icon.
const float OPTION_MARGIN_WIDTH( 10.f );          ///< The margin between the right or lefts edge and the text or icon.
const float OPTION_MAX_WIDTH( 110.0f );          ///< The maximum width of the option   //todo Make Property
const float OPTION_MIN_WIDTH( 86.0f );           ///< The minimum width of the option. //todo Make Property
const float POPUP_DIVIDER_WIDTH( 3.f );        ///< The size of the divider.

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
   if ( mBackgroundImage )
   {
     SetBackgroundImage (  mBackgroundImage );
   }

   SetBackgroundColor( mBackgroundColor );
 }

 void TextSelectionPopup::AddOption( Dali::Toolkit::TableView& parent, const std::string& name, const std::string& caption, const Image iconImage, bool finalOption, bool showIcons, bool showCaption, std::size_t& indexInTable )
 {
   // 1. Create the backgrounds for the popup option both normal and pressed.
   // Both containers will be added to a button.

   Toolkit::TableView optionContainer = Toolkit::TableView::New( (showIcons)?2:1 , 1 );
   optionContainer.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
   optionContainer.SetMinimumSize( Vector2( OPTION_MIN_WIDTH, 0 ) );
   optionContainer.SetFitWidth( 0 );

   Toolkit::TableView  optionPressedContainer = Toolkit::TableView::New( (showIcons)?2:1 , 1 );
   optionPressedContainer.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
   optionPressedContainer.SetMinimumSize( Vector2( OPTION_MIN_WIDTH, 0 ) );
   optionPressedContainer.SetFitWidth( 0 );
#ifdef DECORATOR_DEBUG
   optionContainer.SetName("optionContainer");
   optionPressedContainer.SetName("optionPressedContainer");
#endif
   // 2. Add text.

   if ( showCaption )
   {
   Toolkit::TextLabel captionTextLabel = Toolkit::TextLabel::New();
   captionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
   optionContainer.SetFitHeight( 0 );

   Toolkit::TextLabel pressedCaptionTextLabel = Toolkit::TextLabel::New();
   pressedCaptionTextLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, caption );
   optionPressedContainer.SetFitHeight( 0 );

   captionTextLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
   captionTextLabel.SetMaximumSize( Vector2( OPTION_MAX_WIDTH - 2.f * OPTION_MARGIN_WIDTH , FLT_MAX ) ); //todo FLT_MAX Size negotiation feature needed

   pressedCaptionTextLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
   pressedCaptionTextLabel.SetMaximumSize( Vector2( OPTION_MAX_WIDTH - 2.f * OPTION_MARGIN_WIDTH , FLT_MAX) ); //todo FLT_MAX Size negotiation feature needed

   optionContainer.AddChild( captionTextLabel, Toolkit::TableView::CellPosition( 1, 0 )  ); // todo Labels need ellipsis or similar
   optionPressedContainer.AddChild( pressedCaptionTextLabel, Toolkit::TableView::CellPosition( 1, 0 )  ); // todo Labels need ellipsis or similar
   }

   if ( showIcons )
   {
     // 3. Create the icons
     ImageActor pressedIcon = ImageActor::New(  iconImage );
     ImageActor icon = ImageActor::New(  iconImage );
     icon.SetDepthIndex( DECORATION_DEPTH_INDEX - 1 );
     pressedIcon.SetDepthIndex( DECORATION_DEPTH_INDEX - 1 );
     icon.SetName("image-icon-2014");
     icon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
     pressedIcon.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
     pressedIcon.SetColor( mIconPressedColor );
     optionContainer.SetFitHeight( 0 );
     optionPressedContainer.SetFitHeight( 0 );
     optionContainer.AddChild( icon, Toolkit::TableView::CellPosition( 0, 0 )  );
     optionPressedContainer.AddChild( pressedIcon, Toolkit::TableView::CellPosition( 0, 0 )  );
     icon.SetPadding( Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
     pressedIcon.SetPadding( Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
   }

   // 5. Create a option.
   Toolkit::PushButton option = Toolkit::PushButton::New();
   option.SetName( name );
   option.SetAnimationTime( 0.0f );
   option.SetSize( OPTION_ICON_SIZE );
   //option.ClickedSignal().Connect( this, &TextInputPopup::OnButtonPressed );

   // 6. Set the normal option image.
   option.SetButtonImage( optionContainer );

   // 7. Set the pressed option image
   option.SetSelectedImage( optionPressedContainer );

   // 9 Add option to table view
   parent.SetFitWidth( indexInTable );
   parent.AddChild( option, Toolkit::TableView::CellPosition( 0, indexInTable )  );
   indexInTable++;

   // 10. Add the divider
   if( !finalOption )
   {
     const Size size( POPUP_DIVIDER_WIDTH, 0.0f ); // Height FILL_TO_PARENT

     ImageActor divider = Toolkit::CreateSolidColorActor( Color::WHITE );

     divider.SetSize( size );
     divider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
     parent.SetFitWidth( indexInTable );
     parent.AddChild( divider, Toolkit::TableView::CellPosition( 0, indexInTable )  );
     indexInTable++;
   }
 }

 void TextSelectionPopup::SetUpPopup()
 {
   Actor self = Self();
   self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

   // Create Layer and Stencil.
   mStencilLayer = Layer::New();
   mStencilLayer.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
   mStencilLayer.SetParentOrigin( ParentOrigin::CENTER );

   ImageActor stencil = CreateSolidColorActor( Color::RED );
   stencil.SetDrawMode( DrawMode::STENCIL );
   stencil.SetVisible( true );
   stencil.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
   stencil.SetParentOrigin( ParentOrigin::CENTER );

   Actor scrollview = Actor::New(); //todo make a scrollview
   scrollview.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
   scrollview.SetParentOrigin( ParentOrigin::CENTER );

   mTableOfButtons.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
   mTableOfButtons.SetFitHeight( 0 );
   mTableOfButtons.SetParentOrigin( ParentOrigin::CENTER );

   mStencilLayer.Add( stencil );
   mStencilLayer.Add( scrollview );
   scrollview.Add( mTableOfButtons );
   self.Add( mStencilLayer );
   //self.Add ( mTableOfButtons );
 }

 void TextSelectionPopup::AddPopupOptions( bool createTail, bool showIcons, bool showCaptions )
 {
   mContentSize = Vector2::ZERO;

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

   numberOfOptions = ( numberOfOptions*2 ) - 1 ; // Last Option does not get a divider so -1 or if only one option then also no divider

   mTableOfButtons = Dali::Toolkit::TableView::New( 1, numberOfOptions );

   for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
   {
     const ButtonRequirement& button( *it );
     if ( button.enabled )
     {
       AddOption( mTableOfButtons, button.name, button.caption, button.icon, optionsAdded == numberOfOptions - 1, showIcons, showCaptions, optionsAdded ); // -1 to ignore the last divider
     }
   }
 }

 void TextSelectionPopup::CreatePopup()
 {
   if ( !mStencilLayer )
   {
     CreateOrderedListOfPopupOptions();  //todo Currently causes all options to be shown
     CreateBackground();
     AddPopupOptions( true, true, false );  // todo false so not to show Labels until ellipses or similar possible.
     SetUpPopup();
   }

   mStencilLayer.RaiseToTop();
 }

TextSelectionPopup::TextSelectionPopup()
: Control( ControlBehaviour( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ) ),
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
  mShowIcons( true ),
  mShowCaptions( false )
{
}

TextSelectionPopup::~TextSelectionPopup()
{
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
