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
#include <dali-toolkit/internal/text/decorator/text-decorator.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/images/nine-patch-image.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <libintl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>

#ifdef DEBUG_ENABLED
#define DECORATOR_DEBUG
#endif

// todo Move this to adaptor??
#define GET_LOCALE_TEXT(string) dgettext("elementary", string)

// Local Data
namespace
{

const char* DEFAULT_GRAB_HANDLE_IMAGE( DALI_IMAGE_DIR "insertpoint-icon.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );
//const char* DEFAULT_SELECTION_HANDLE_ONE_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-left-press.png" );
//const char* DEFAULT_SELECTION_HANDLE_TWO_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-right-press.png" );
const char* DEFAULT_CURSOR_IMAGE( DALI_IMAGE_DIR "decorator-cursor.png");

const Dali::Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.5f, 2.0f, 1.0f );
const Dali::Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.5f, 1.5f, 1.0f );

const std::size_t CURSOR_BLINK_INTERVAL = 500; // Cursor blink interval
const std::size_t MILLISECONDS = 1000;

const std::string POPUP_BACKGROUND( DALI_IMAGE_DIR "popup_bubble_bg.#.png" );
const std::string POPUP_BACKGROUND_EFFECT( DALI_IMAGE_DIR "popup_bubble_bg_ef.#.png" );
const std::string POPUP_BACKGROUND_LINE( DALI_IMAGE_DIR "popup_bubble_bg_line.#.png" );

const Dali::Vector4 DEFAULT_POPUP_BACKGROUND( Dali::Vector4( .20f, 0.29f, 0.44f, 1.0f ) );
const Dali::Vector4 DEFAULT_POPUP_BACKGROUND_PRESSED( Dali::Vector4( 0.07f, 0.10f, 0.17f, 1.0f ) );
const Dali::Vector4 DEFAULT_POPUP_LINE_COLOR( Dali::Vector4( 0.36f, 0.45f, 0.59f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_ICON( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_ICON_PRESSED( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Dali::Vector4 DEFAULT_OPTION_TEXT_PRESSED( Dali::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

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

enum Buttons
{
  ButtonsCut,
  ButtonsCopy,
  ButtonsPaste,
  ButtonsSelect,
  ButtonsSelectAll,
  ButtonsClipboard,
  ButtonsEnumEnd
};

struct ButtonRequirement
{
  ButtonRequirement()
  : id( ButtonsEnumEnd ),
    priority( 0u ),
    name(),
    caption(),
    icon(),
    enabled( false )
  {}

  ButtonRequirement( Buttons buttonId,
                     std::size_t buttonPriority,
                     const std::string& buttonName,
                     const std::string& buttonCaption,
                     Dali::ResourceImage buttonIcon,
                     bool buttonEnabled )
  : id( buttonId ),
    priority( buttonPriority ),
    name( buttonName ),
    caption( buttonCaption ),
    icon( buttonIcon ),
    enabled( buttonEnabled )
  {}

  Buttons id;
  std::size_t priority;
  std::string name;
  std::string caption;
  Dali::ResourceImage icon;
  bool enabled;
};

// Comparison function for ButtonRequirement Priority
bool PriorityCompare( ButtonRequirement const& a, ButtonRequirement const& b)
{
  return a.priority < b.priority;
}

} // end of namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Decorator::Impl : public ConnectionTracker
{
  struct CursorImpl
  {
    CursorImpl()
    : x(0.0f),
      y(0.0f),
      height(0.0f),
      color(Dali::Color::WHITE)
    {
    }

    float x;
    float y;
    float height;

    Vector4 color;
  };

  struct SelectionHandleImpl
  {
    SelectionHandleImpl()
    : x(0.0f),
      y(0.0f),
      cursorHeight(0.0f),
      flipped(false)
    {
    }

    float x;
    float y;
    float cursorHeight; ///< Not the handle height
    bool flipped;

    ImageActor actor;
    Actor grabArea;

    Image pressedImage;
    Image releasedImage;
  };

  struct PopupImpl
  {
    PopupImpl()
    : mVisiblePopUpSize( Vector2( 100.0f, 100.0f ) ),
      mRequiredPopUpSize( Vector2( 100.0f, 100.0f ) ),
      mMaxWidth( DEFAULT_POPUP_MAX_SIZE.width ),
      mBackgroundColor( DEFAULT_POPUP_BACKGROUND ),
      mBackgroundPressedColor( DEFAULT_POPUP_BACKGROUND_PRESSED ),
      mLineColor( DEFAULT_POPUP_LINE_COLOR ),
      mIconColor( DEFAULT_OPTION_ICON ),
      mIconPressedColor( DEFAULT_OPTION_ICON_PRESSED ),
      mTextColor( DEFAULT_OPTION_TEXT ),
      mTextPressedColor( DEFAULT_OPTION_TEXT_PRESSED )
      {
      }

      Layer mRoot;                                        // The actor which all popup content is added to (i.e. panel and buttons)
      Actor mButtons;                                     // Actor which holds all the buttons, sensitivity can be set on all buttons via this actor
      ImageActor mBackground;                             // The background popup panel
      ImageActor mTail;                                   // The tail for the popup
      ImageActor mTailEffect;                             // the tail effect
      ImageActor mTailLine;                               // The border/outline around the tail

      Size mVisiblePopUpSize;                             // Visible Size of popup excluding content that needs scrolling.
      Size mRequiredPopUpSize;                            // Total size of popup including any invisible margin
      float mMaxWidth;                                    // Max width of the Popup

      Vector4 mNinePatchMargins;                          // Margins between the edge of the cropped image and the nine patch rect (left, right, top, bottom).

      Size mContentSize;                                  // Size of Content (i.e. Buttons)
      //Animation mAnimation;                               // Popup Hide/Show animation.

      std::vector<ButtonRequirement> mOrderListOfButtons; // List of buttons in the order to be displayed and a flag to indicate if needed.

      Vector4 mBackgroundColor;             // Color of the background of the text input popup
      Vector4 mBackgroundPressedColor;      // Color of the option background.
      Vector4 mLineColor;                   // Color of the line around the text input popup
      Vector4 mIconColor;                   // Color of the popup icon.
      Vector4 mIconPressedColor;            // Color of the popup icon when pressed.
      Vector4 mTextColor;                   // Color of the popup text.
      Vector4 mTextPressedColor;            // Color of the popup text when pressed.

      // Priority of Options/Buttons in the Cut and Paste pop-up, higher priority buttons are displayed first, left to right.
      std::size_t mSelectOptionPriority;    // Position of Select Button
      std::size_t mSelectAllOptionPriority; // Position of Select All button
      std::size_t mCutOptionPriority;       // Position of Cut button
      std::size_t mCopyOptionPriority;      // Position of Copy button
      std::size_t mPasteOptionPriority;     // Position of Paste button
      std::size_t mClipboardOptionPriority; // Position of Clipboard button

      bool mShowIcons; // Flag to show icons
  };


  Impl( Dali::Toolkit::Internal::Control& parent, Observer& observer )
  : mParent(parent),
    mObserver(observer),
    mActiveCursor(ACTIVE_CURSOR_NONE),
    mActiveGrabHandle(false),
    mActiveSelection( false ),
    mActiveCopyPastePopup( false ),
    mCursorBlinkInterval( CURSOR_BLINK_INTERVAL ),
    mCursorBlinkDuration( 0.0f ),
    mCursorBlinkStatus( true ),
    mGrabDisplacementX( 0.0f ),
    mGrabDisplacementY( 0.0f ),
    mBoundingBox( Rect<int>() )
  {
  }

  /**
   * Relayout of the decorations owned by the decorator.
   * @param[in] size The Size of the UI control the decorater is adding it's decorations to.
   */
  void Relayout( const Vector2& size )
  {
    SetCursors();

    // Show or hide the grab handle
    if( mActiveGrabHandle )
    {
      SetupTouchEvents();

      CreateActiveLayer();
      CreateGrabHandle();

      mGrabHandle.SetPosition( mCursor[PRIMARY_CURSOR].x, mCursor[PRIMARY_CURSOR].y + mCursor[PRIMARY_CURSOR].height );
    }
    else if( mGrabHandle )
    {
      UnparentAndReset( mGrabHandle );
    }

    // Show or hide the selection handles/highlight
    if( mActiveSelection )
    {
      SetupTouchEvents();

      CreateActiveLayer();
      CreateSelectionHandles();

      SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
      primary.actor.SetPosition( primary.x, primary.y + primary.cursorHeight );

      SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
      secondary.actor.SetPosition( secondary.x, secondary.y + secondary.cursorHeight );

      //CreateHighlight(); TODO
    }
    else
    {
      UnparentAndReset( mSelectionHandle[ PRIMARY_SELECTION_HANDLE ].actor );
      UnparentAndReset( mSelectionHandle[ SECONDARY_SELECTION_HANDLE ].actor );
    }

    if ( mActiveCopyPastePopup )
    {
      CreatePopup();

      mCopyPastePopup.mRoot.SetPosition( Vector3( 180.0f, -100.0f, 0.0f ) ); //todo grabhandle or selection handle postions to be used
    }
    else
    {
      DestroyPopup();
    }
  }

  void CreateCursor( ImageActor& cursor )
  {
    if ( !mCursorImage )
    {
      mCursorImage = ResourceImage::New( DEFAULT_CURSOR_IMAGE );
    }
    cursor = ImageActor::New( mCursorImage );
    cursor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  }

  // Add or Remove cursor(s) from parent
  void SetCursors()
  {
    Actor parent = mParent.Self();
    /* Create Primary and or Secondary Cursor(s) if active and add to parent */
    if ( mActiveCursor == ACTIVE_CURSOR_PRIMARY )
    {
      if ( !mPrimaryCursor )
      {
        CreateCursor( mPrimaryCursor );
#ifdef DECORATOR_DEBUG
        mPrimaryCursor.SetName( "PrimaryCursorActor" );
#endif
        parent.Add( mPrimaryCursor);
      }

      mPrimaryCursor.SetPosition( mCursor[PRIMARY_CURSOR].x, mCursor[PRIMARY_CURSOR].y );
    }
    else if ( mActiveCursor == ACTIVE_CURSOR_BOTH )
    {
      if ( !mSecondaryCursor )
      {
        CreateCursor( mSecondaryCursor );
#ifdef DECORATOR_DEBUG
        mSecondaryCursor.SetName( "SecondaryCursorActor" );
#endif
        parent.Add( mSecondaryCursor);
      }
    }
    else
    {
      /* ACTIVE_CURSOR_NONE so unparent cursors*/
      if ( mPrimaryCursor )
      {
        UnparentAndReset( mPrimaryCursor );
      }

      if ( mSecondaryCursor )
      {
        UnparentAndReset( mSecondaryCursor );
      }
    }
  }

  bool OnCursorBlinkTimerTick()
  {
    // Cursor blinking
    if ( ACTIVE_CURSOR_PRIMARY )
    {
      mPrimaryCursor.SetVisible( mCursorBlinkStatus );
    }
    else if ( ACTIVE_CURSOR_BOTH )
    {
      mPrimaryCursor.SetVisible( mCursorBlinkStatus );
      mSecondaryCursor.SetVisible( mCursorBlinkStatus );
    }

    mCursorBlinkStatus = !mCursorBlinkStatus;

    return true;
  }

  void SetupTouchEvents()
  {
    if ( !mTapDetector )
    {
      mTapDetector = TapGestureDetector::New();
      mTapDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnTap );
    }

    if ( !mPanGestureDetector )
    {
      mPanGestureDetector = PanGestureDetector::New();
      mPanGestureDetector.DetectedSignal().Connect( this, &Decorator::Impl::OnPan );
    }
  }

  void CreateActiveLayer()
  {
    if( !mActiveLayer )
    {
      Actor parent = mParent.Self();

      mActiveLayer = Layer::New();
#ifdef DECORATOR_DEBUG
      mActiveLayer.SetName ( "ActiveLayerActor" );
#endif

      mActiveLayer.SetAnchorPoint( AnchorPoint::CENTER);
      mActiveLayer.SetParentOrigin( ParentOrigin::CENTER);
      mActiveLayer.SetSizeMode( SIZE_EQUAL_TO_PARENT );
      mActiveLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );

      parent.Add( mActiveLayer );
    }

    mActiveLayer.RaiseToTop();
  }

  void CreateGrabHandle()
  {
    if( !mGrabHandle )
    {
      if ( !mGrabHandleImage )
      {
        mGrabHandleImage = ResourceImage::New( DEFAULT_GRAB_HANDLE_IMAGE );
      }

      mGrabHandle = ImageActor::New( mGrabHandleImage );
#ifdef DECORATOR_DEBUG
      mGrabHandle.SetName( "GrabHandleActor" );
#endif
      mGrabHandle.SetParentOrigin( ParentOrigin::TOP_LEFT );
      mGrabHandle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      mGrabHandle.SetDrawMode( DrawMode::OVERLAY );

      mGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      mGrabArea.SetName( "GrabArea" );
#endif
      mGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
      mGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      mGrabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
      mGrabHandle.Add(mGrabArea);

      mTapDetector.Attach( mGrabArea );
      mPanGestureDetector.Attach( mGrabArea );

      mActiveLayer.Add(mGrabHandle);
    }
  }

  void CreateSelectionHandles()
  {
    SelectionHandleImpl& primary = mSelectionHandle[ PRIMARY_SELECTION_HANDLE ];
    if ( !primary.actor )
    {
      if ( !primary.releasedImage )
      {
        primary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE );
      }

      primary.actor = ImageActor::New( primary.releasedImage );
#ifdef DECORATOR_DEBUG
      primary.actor.SetName("SelectionHandleOne");
#endif
      primary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      primary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      primary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      primary.flipped = false;

      primary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      primary.grabArea.SetName("SelectionHandleOneGrabArea");
#endif
      primary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      primary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      primary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( primary.grabArea );
      mPanGestureDetector.Attach( primary.grabArea );
      primary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleOneTouched );

      primary.actor.Add( primary.grabArea );
      mActiveLayer.Add( primary.actor );
    }

    SelectionHandleImpl& secondary = mSelectionHandle[ SECONDARY_SELECTION_HANDLE ];
    if ( !secondary.actor )
    {
      if ( !secondary.releasedImage )
      {
        secondary.releasedImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO );
      }

      secondary.actor = ImageActor::New( secondary.releasedImage );
#ifdef DECORATOR_DEBUG
      secondary.actor.SetName("SelectionHandleTwo");
#endif
      secondary.actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
      secondary.actor.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
      secondary.actor.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text
      secondary.flipped = false;

      secondary.grabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
#ifdef DECORATOR_DEBUG
      secondary.grabArea.SetName("SelectionHandleTwoGrabArea");
#endif
      secondary.grabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
      secondary.grabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
      secondary.grabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

      mTapDetector.Attach( secondary.grabArea );
      mPanGestureDetector.Attach( secondary.grabArea );
      secondary.grabArea.TouchedSignal().Connect( this, &Decorator::Impl::OnHandleTwoTouched );

      secondary.actor.Add( secondary.grabArea );
      mActiveLayer.Add( secondary.actor );
    }

    //SetUpHandlePropertyNotifications(); TODO
  }

  void OnTap( Actor actor, const TapGesture& tap )
  {
    if( actor == mGrabHandle )
    {
      // TODO
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    if( actor == mGrabArea )
    {
      if( Gesture::Started == gesture.state )
      {
        mGrabDisplacementX = mGrabDisplacementY = 0;
      }

      mGrabDisplacementX += gesture.displacement.x;
      mGrabDisplacementY += gesture.displacement.y;

      float x = mCursor[PRIMARY_CURSOR].x + mGrabDisplacementX;
      float y = mCursor[PRIMARY_CURSOR].y + mCursor[PRIMARY_CURSOR].height*0.5f + mGrabDisplacementY;

      if( Gesture::Started    == gesture.state ||
          Gesture::Continuing == gesture.state )
      {
        mObserver.GrabHandleEvent( GRAB_HANDLE_PRESSED, x, y );
      }
      else if( Gesture::Finished  == gesture.state ||
               Gesture::Cancelled == gesture.state )
      {
        mObserver.GrabHandleEvent( GRAB_HANDLE_RELEASED, x, y );
      }
    }
  }

  bool OnHandleOneTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }

  bool OnHandleTwoTouched( Actor actor, const TouchEvent& touch )
  {
    // TODO
    return false;
  }

  /**
   * Popup
   */

  void CreateOrderedListOfPopupOptions()
  {
    mCopyPastePopup.mOrderListOfButtons.clear();

    // Create button for each possible option using Option priority
    ResourceImage cutIcon = ResourceImage::New( OPTION_ICON_CUT );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsCut, mCopyPastePopup.mCutOptionPriority, OPTION_CUT, GET_LOCALE_TEXT("IDS_COM_BODY_CUT"), cutIcon, true ) );

    ResourceImage copyIcon = ResourceImage::New( OPTION_ICON_COPY );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsCopy, mCopyPastePopup.mCopyOptionPriority, OPTION_COPY, GET_LOCALE_TEXT("IDS_COM_BODY_COPY"), copyIcon, true ) );

    ResourceImage pasteIcon = ResourceImage::New( OPTION_ICON_PASTE );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsPaste, mCopyPastePopup.mPasteOptionPriority, OPTION_PASTE, GET_LOCALE_TEXT("IDS_COM_BODY_PASTE"), pasteIcon, true ) );

    ResourceImage selectIcon = ResourceImage::New( OPTION_ICON_SELECT );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsSelect, mCopyPastePopup.mSelectOptionPriority, OPTION_SELECT_WORD, GET_LOCALE_TEXT("IDS_COM_SK_SELECT"), selectIcon, true ) );

    ResourceImage selectAllIcon = ResourceImage::New( OPTION_ICON_SELECT_ALL );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsSelectAll, mCopyPastePopup.mSelectAllOptionPriority, OPTION_SELECT_ALL, GET_LOCALE_TEXT("IDS_COM_BODY_SELECT_ALL"), selectAllIcon, true ) );

    ResourceImage clipboardIcon = ResourceImage::New( OPTION_ICON_CLIPBOARD );
    mCopyPastePopup.mOrderListOfButtons.push_back( ButtonRequirement( ButtonsClipboard, mCopyPastePopup.mClipboardOptionPriority, OPTION_CLIPBOARD, GET_LOCALE_TEXT("IDS_COM_BODY_CLIPBOARD"), clipboardIcon, true ) );

    // Sort the buttons according their priorities.
    std::sort( mCopyPastePopup.mOrderListOfButtons.begin(), mCopyPastePopup.mOrderListOfButtons.end(), PriorityCompare );
  }

  void CreateBackground( PopupImpl& popup )
  {
    // Create background-panel if not already created (required if we have at least one option)
    if ( !popup.mBackground )
    {
      ResourceImage bgImg = ResourceImage::New( POPUP_BACKGROUND );
      popup.mBackground = ImageActor::New( bgImg );
      popup.mBackground.SetColor( popup.mBackgroundColor );
      popup.mBackground.SetParentOrigin( ParentOrigin::CENTER );

      NinePatchImage backgroundImageNinePatch = NinePatchImage::DownCast( bgImg );
      if( backgroundImageNinePatch )
      {
        const Size ninePatchImageSize = Size( static_cast<float>( bgImg.GetWidth() ), static_cast<float>( bgImg.GetHeight() ) );
        Rect<int> childRect = backgroundImageNinePatch.GetChildRectangle();

        // -1u because of the cropping.
        popup.mNinePatchMargins.x = childRect.x - 1u;
        popup.mNinePatchMargins.y = ninePatchImageSize.width - ( childRect.x + childRect.width ) - 1u;
        popup.mNinePatchMargins.z = childRect.y - 1u;
        popup.mNinePatchMargins.w = ninePatchImageSize.height - ( childRect.y + childRect.height ) - 1u;
      }

      ResourceImage::Image bgEffectImg = ResourceImage::New( POPUP_BACKGROUND_EFFECT );
      ImageActor backgroundEffect = ImageActor::New( bgEffectImg );
      backgroundEffect.SetParentOrigin( ParentOrigin::CENTER );

      ResourceImage::Image bgLine = ResourceImage::New( POPUP_BACKGROUND_LINE );
      ImageActor backgroundLine = ImageActor::New( bgLine );
      backgroundLine.SetParentOrigin( ParentOrigin::CENTER );
      backgroundLine.SetColor( popup.mLineColor );

      popup.mBackground.Add( backgroundEffect );
      popup.mBackground.Add( backgroundLine );
    }
  }

  void AddOption( Actor& parent, const std::string& name, const std::string& caption, const Image iconImage, bool finalOption, bool showIcons )
  {
    // 1. Create the backgrounds for the popup option both normal and pressed.
    // Both containers will be added to a button.
    Actor optionContainer = Actor::New();
    optionContainer.SetDrawMode( DrawMode::OVERLAY );
    //optionContainer.SetParentOrigin( ParentOrigin::CENTER );
    optionContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    ImageActor optionPressedContainer = Toolkit::CreateSolidColorActor( mCopyPastePopup.mBackgroundPressedColor );
    optionPressedContainer.SetDrawMode( DrawMode::OVERLAY );
    //optionPressedContainer.SetParentOrigin( ParentOrigin::CENTER );
    optionPressedContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    // 2. Add text.
    Toolkit::TextLabel captionTextLabel = Toolkit::TextLabel::New();
    captionTextLabel.SetProperty( TextLabel::Property::TEXT, caption );
    optionContainer.Add( captionTextLabel );

    Toolkit::TextLabel pressedCaptionTextLabel = Toolkit::TextLabel::New();
    pressedCaptionTextLabel.SetProperty( TextLabel::Property::TEXT, caption );
    optionPressedContainer.Add( pressedCaptionTextLabel );

    // Calculates the icon/text position.
    float iconTextOffsetY = 0.0f;

    if ( showIcons )
    {
      // 3. Create the icons
      ImageActor pressedIcon = ImageActor::New(  iconImage );
      ImageActor icon = ImageActor::New(  iconImage );

      optionContainer.Add( icon );
      optionPressedContainer.Add( pressedIcon );

      iconTextOffsetY = 0.5f * ( ( DEFAULT_POPUP_MAX_SIZE.height - mCopyPastePopup.mNinePatchMargins.z - mCopyPastePopup.mNinePatchMargins.w ) - ( OPTION_ICON_SIZE.height + OPTION_GAP_ICON_TEXT + OPTION_TEXT_LINE_HEIGHT ) );

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
    captionTextLabel.SetSize( textSize );
    pressedCaptionTextLabel.SetSize( textSize );

    // 4. Calculate the size of option.

    // The width is the max size of the text or the icon plus the margins clamped between the option min and max size.
    // The height is the whole popup height minus the ninepatch margins.
    const Vector2 optionSize( std::min( OPTION_MAX_WIDTH, std::max( OPTION_MIN_WIDTH, std::max( textSize.width, OPTION_ICON_SIZE.width ) + 2.f * OPTION_MARGIN_WIDTH ) ),
                              DEFAULT_POPUP_MAX_SIZE.height - mCopyPastePopup.mNinePatchMargins.z - mCopyPastePopup.mNinePatchMargins.w );

    optionContainer.SetSize( optionSize );
    optionPressedContainer.SetSize( optionSize );

    // 5. Create a option.
    Toolkit::PushButton option = Toolkit::PushButton::New();
    option.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
    option.SetSize( optionSize );
    option.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    option.SetX( mCopyPastePopup.mContentSize.width );
    option.SetName( name );
    option.SetAnimationTime( 0.0f );
    //option.ClickedSignal().Connect( this, &TextInputPopup::OnButtonPressed );

    parent.Add( option );

    // 6. Set the normal option image.
    option.SetButtonImage( optionContainer );

    // 7. Set the pressed option image
    option.SetSelectedImage( optionPressedContainer );

    // 8. Update the content size.
    mCopyPastePopup.mContentSize.width += optionSize.width;
    mCopyPastePopup.mContentSize.height = std::max ( optionSize.height, mCopyPastePopup.mContentSize.height );

    // 9. Add the divider
    if( !finalOption )
    {
      const Size size( POPUP_DIVIDER_WIDTH, mCopyPastePopup.mContentSize.height );

      ImageActor divider =  Toolkit::CreateSolidColorActor( Color::WHITE );
      divider.SetSize (size);
      divider.SetParentOrigin( ParentOrigin::TOP_LEFT );
      divider.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      divider.SetPosition( mCopyPastePopup.mContentSize.width - POPUP_DIVIDER_WIDTH, 0.0f );
      parent.Add( divider );
    }
  }

  void SetUpPopup( Actor& parent, Size& size )
  {
    // Create Layer and Stencil.
    parent = Layer::New();
    Layer layer = Layer::New();
    ImageActor stencil = CreateSolidColorActor( Color::RED );
    stencil.SetDrawMode( DrawMode::STENCIL );
    stencil.SetVisible( true );
    Actor scrollview = Actor::New();

    //todo Use Size negotiation
    layer.SetSize( size ); // matches stencil size
    parent.SetSize( size ); // matches stencil size
    stencil.SetSize( size );
    scrollview.SetSize( size );
    mCopyPastePopup.mButtons.SetSize( size );

    layer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    scrollview.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    //mCopyPastePopup.mBackground.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    mCopyPastePopup.mButtons.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    parent.Add( mCopyPastePopup.mBackground );
    parent.Add( layer );
    layer.Add( stencil );
    layer.Add( scrollview );
    scrollview.Add( mCopyPastePopup.mButtons );
  }

  void AddPopupOptions( bool createTail, bool showIcons )
  {
    mCopyPastePopup.mShowIcons = showIcons;

    mCopyPastePopup.mContentSize = Vector2::ZERO;

    mCopyPastePopup.mButtons = Actor::New();

    // Add the options into the buttons container.

    // 1. Determine how many buttons are active and should be added to container.
    std::size_t numberOfOptions = 0u;
    for( std::vector<ButtonRequirement>::const_iterator it = mCopyPastePopup.mOrderListOfButtons.begin(), endIt = mCopyPastePopup.mOrderListOfButtons.end(); ( it != endIt ); ++it )
    {
      const ButtonRequirement& button( *it );
      if( button.enabled )
      {
        ++numberOfOptions;
      }
    }

    // 2. Iterate list of buttons and add active ones.
    std::size_t optionsAdded = 0u;
    for( std::vector<ButtonRequirement>::const_iterator it = mCopyPastePopup.mOrderListOfButtons.begin(), endIt = mCopyPastePopup.mOrderListOfButtons.end(); ( it != endIt ); ++it )
    {
      const ButtonRequirement& button( *it );
      if ( button.enabled )
      {
        ++optionsAdded;
        AddOption( mCopyPastePopup.mButtons, button.name, button.caption, button.icon, optionsAdded == numberOfOptions, mCopyPastePopup.mShowIcons );
      }
    }

    // Calculate the size of the whole popup which may not be all visible.
    mCopyPastePopup.mRequiredPopUpSize = Size( std::min( mCopyPastePopup.mMaxWidth, mCopyPastePopup.mContentSize.width + mCopyPastePopup.mNinePatchMargins.x + mCopyPastePopup.mNinePatchMargins.y ), DEFAULT_POPUP_MAX_SIZE. height );

    // Set the size of the background, background line and background effect.
    mCopyPastePopup.mBackground.SetSize( mCopyPastePopup.mRequiredPopUpSize);
    for( std::size_t index = 0u, childCount = mCopyPastePopup.mBackground.GetChildCount(); index < childCount; ++index )
    {
      mCopyPastePopup.mBackground.GetChildAt( index ).SetSize( mCopyPastePopup.mRequiredPopUpSize );
    }

    // Size of the contents within the popup
    mCopyPastePopup.mVisiblePopUpSize = Size( mCopyPastePopup.mRequiredPopUpSize.width - mCopyPastePopup.mNinePatchMargins.x - mCopyPastePopup.mNinePatchMargins.y, mCopyPastePopup.mRequiredPopUpSize.height - mCopyPastePopup.mNinePatchMargins.z - mCopyPastePopup.mNinePatchMargins.w );

  }

  void CreatePopup()
  {
    if ( !mCopyPastePopup.mRoot )
    {
      mActiveCopyPastePopup = true;
      CreateOrderedListOfPopupOptions();  //todo Currently causes all options to be shown
      CreateBackground( mCopyPastePopup );
      AddPopupOptions( true, true );
      SetUpPopup( mCopyPastePopup.mRoot, mCopyPastePopup.mVisiblePopUpSize );
      Actor parent = mParent.Self();
      parent.Add( mCopyPastePopup.mRoot );
    }
  }

  void DestroyPopup()
  {
    if ( mCopyPastePopup.mRoot )
    {
      mActiveCopyPastePopup = false;
      UnparentAndReset( mCopyPastePopup.mButtons );
      UnparentAndReset( mCopyPastePopup.mRoot );
    }
  }

  Internal::Control& mParent;
  Observer& mObserver;

  Layer mActiveLayer; // Layer for active handles and alike that ensures they are above all else.

  unsigned int mActiveCursor;
  bool         mActiveGrabHandle;
  bool         mActiveSelection;
  bool         mActiveCopyPastePopup;

  CursorImpl mCursor[CURSOR_COUNT];

  Timer mCursorBlinkTimer; // Timer to signal cursor to blink
  unsigned int mCursorBlinkInterval;
  float mCursorBlinkDuration;
  bool mCursorBlinkStatus; // Flag to switch between blink on and blink off

  ImageActor mPrimaryCursor;
  ImageActor mSecondaryCursor;

  ImageActor mGrabHandle;
  Actor mGrabArea;
  float mGrabDisplacementX;
  float mGrabDisplacementY;

  SelectionHandleImpl mSelectionHandle[SELECTION_HANDLE_COUNT];

  PopupImpl mCopyPastePopup;

  Image mCursorImage;
  Image mGrabHandleImage;

  TapGestureDetector mTapDetector;
  PanGestureDetector mPanGestureDetector;

  Rect<int> mBoundingBox;
};

DecoratorPtr Decorator::New( Internal::Control& parent, Observer& observer )
{
  return DecoratorPtr( new Decorator(parent, observer) );
}

void Decorator::SetBoundingBox( const Rect<int>& boundingBox )
{
  mImpl->mBoundingBox = boundingBox;
}

const Rect<int>& Decorator::GetBoundingBox() const
{
  return mImpl->mBoundingBox;
}

void Decorator::Relayout( const Vector2& size )
{
  mImpl->Relayout( size );
}

/** Cursor **/

void Decorator::SetActiveCursor( ActiveCursor activeCursor )
{
  mImpl->mActiveCursor = activeCursor;
}

unsigned int Decorator::GetActiveCursor() const
{
  return mImpl->mActiveCursor;
}

void Decorator::SetPosition( Cursor cursor, float x, float y, float height )
{
  // Adjust grab handle displacement
  mImpl->mGrabDisplacementX -= x - mImpl->mCursor[cursor].x;
  mImpl->mGrabDisplacementY -= y - mImpl->mCursor[cursor].y;

  mImpl->mCursor[cursor].x = x;
  mImpl->mCursor[cursor].y = y;
  mImpl->mCursor[cursor].height = height;
}

void Decorator::GetPosition( Cursor cursor, float& x, float& y, float& height ) const
{
  x = mImpl->mCursor[cursor].x;
  y = mImpl->mCursor[cursor].y;
  height = mImpl->mCursor[cursor].height;
}

void Decorator::SetCursorImage( Dali::Image image )
{
  mImpl->mCursorImage = image;
}

Dali::Image Decorator::GetCursorImage() const
{
  return mImpl->mCursorImage;
}

void Decorator::SetColor( Cursor cursor, const Dali::Vector4& color )
{
  mImpl->mCursor[cursor].color = color;
}

const Dali::Vector4& Decorator::GetColor( Cursor cursor ) const
{
  return mImpl->mCursor[cursor].color;
}

void Decorator::StartCursorBlink()
{
  if ( !mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer = Timer::New( mImpl->mCursorBlinkInterval );
    mImpl->mCursorBlinkTimer.TickSignal().Connect( mImpl, &Decorator::Impl::OnCursorBlinkTimerTick );
  }

  if ( !mImpl->mCursorBlinkTimer.IsRunning() )
  {
    mImpl->mCursorBlinkTimer.Start();
  }
}

void Decorator::StopCursorBlink()
{
  if ( mImpl->mCursorBlinkTimer )
  {
    mImpl->mCursorBlinkTimer.Stop();
  }
}

void Decorator::SetCursorBlinkInterval( float seconds )
{
  mImpl->mCursorBlinkInterval = seconds*MILLISECONDS; // Convert to milliseconds
}

float Decorator::GetCursorBlinkInterval() const
{
  return mImpl->mCursorBlinkInterval;
}

void Decorator::SetCursorBlinkDuration( float seconds )
{
  mImpl->mCursorBlinkDuration = seconds;
}

float Decorator::GetCursorBlinkDuration() const
{
  return mImpl->mCursorBlinkDuration;
}

/** GrabHandle **/

void Decorator::SetGrabHandleActive( bool active )
{
  mImpl->mActiveGrabHandle = active;
}

bool Decorator::IsGrabHandleActive() const
{
  return mImpl->mActiveGrabHandle;
}

void Decorator::SetGrabHandleImage( Dali::Image image )
{
  mImpl->mGrabHandleImage = image;
}

Dali::Image Decorator::GetGrabHandleImage() const
{
  return mImpl->mGrabHandleImage;
}

/** Selection **/

void Decorator::SetSelectionActive( bool active )
{
  mImpl->mActiveSelection = active;
}

bool Decorator::IsSelectionActive() const
{
  return mImpl->mActiveSelection;
}

void Decorator::SetPosition( SelectionHandle handle, float x, float y, float height )
{
  mImpl->mSelectionHandle[handle].x = x;
  mImpl->mSelectionHandle[handle].y = y;
  mImpl->mSelectionHandle[handle].cursorHeight = height;
}

void Decorator::GetPosition( SelectionHandle handle, float& x, float& y, float& height ) const
{
  x = mImpl->mSelectionHandle[handle].x;
  y = mImpl->mSelectionHandle[handle].y;
  height = mImpl->mSelectionHandle[handle].cursorHeight;
}

void Decorator::SetImage( SelectionHandle handle, SelectionHandleState state, Dali::Image image )
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    mImpl->mSelectionHandle[handle].pressedImage = image;
  }
  else
  {
    mImpl->mSelectionHandle[handle].releasedImage = image;
  }
}

Dali::Image Decorator::GetImage( SelectionHandle handle, SelectionHandleState state ) const
{
  if( SELECTION_HANDLE_PRESSED == state )
  {
    return mImpl->mSelectionHandle[handle].pressedImage;
  }

  return mImpl->mSelectionHandle[handle].releasedImage;
}

void Decorator::ShowPopup()
{
  if ( !mImpl->mCopyPastePopup.mRoot )
  {
    mImpl->CreatePopup();
  }
}

void Decorator::HidePopup()
{
  if ( mImpl->mCopyPastePopup.mRoot )
  {
    mImpl->DestroyPopup();
  }
}

Decorator::~Decorator()
{
  delete mImpl;
}

Decorator::Decorator( Dali::Toolkit::Internal::Control& parent, Observer& observer )
: mImpl( NULL )
{
  mImpl = new Decorator::Impl( parent, observer );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
