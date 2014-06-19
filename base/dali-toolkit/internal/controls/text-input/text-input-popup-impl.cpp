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

#include <dali/integration-api/debug.h>

#include <dali-toolkit/internal/controls/text-input/text-input-popup-impl.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

#include <libintl.h>

using namespace std;
using namespace Dali;

#define GET_LOCALE_TEXT(string) dgettext("sys_string", string)

namespace
{
const Vector2 DEFAULT_POPUP_INDICATOR_OFFSET(0.0f, 60.0f);


// TODO: This should be based on the content for example:
// 1. For selection: should be above top of highlighted selection, or below bottom of highlighted selection + end handle.
// 2. For cursor: should be above top of cursor, or below bottom of cursor + grab handle.
const std::string POPUP_ALTERNATIVE_OFFSET("popup-alternative-offset");       ///< Alternative offset property for confinenment constraint.
const std::string POPUP_REQUESTED_POSITION("popup-requested-position");       ///< Position the Popup was requested to be, for confinenment constraint.

/**
 * Image resource paths
 */
const std::string POPUP_BACKGROUND( DALI_IMAGE_DIR "popup_bubble_bg.#.png" );
const std::string POPUP_BACKGROUND_EFFECT( DALI_IMAGE_DIR "popup_bubble_bg_ef.#.png" );
const std::string POPUP_BACKGROUND_LINE( DALI_IMAGE_DIR "popup_bubble_bg_line.#.png" );
const std::string POPUP_TAIL_BOTTOM( DALI_IMAGE_DIR "popup_bubble_tail_bottom.png" );
const std::string POPUP_TAIL_BOTTOM_EFFECT( DALI_IMAGE_DIR "popup_bubble_tail_bottom_ef.png" );
const std::string POPUP_TAIL_BOTTOM_LINE( DALI_IMAGE_DIR "popup_bubble_tail_bottom_line.png" );
const std::string OPTION_ICON_CLIPBOARD( DALI_IMAGE_DIR "copy_paste_icon_clipboard.png" );
const std::string OPTION_ICON_COPY( DALI_IMAGE_DIR "copy_paste_icon_copy.png" );
const std::string OPTION_ICON_CUT( DALI_IMAGE_DIR "copy_paste_icon_cut.png" );
const std::string OPTION_ICON_PASTE( DALI_IMAGE_DIR "copy_paste_icon_paste.png" );
const std::string OPTION_ICON_SELECT( DALI_IMAGE_DIR "copy_paste_icon_select.png" );
const std::string OPTION_ICON_SELECT_ALL( DALI_IMAGE_DIR "copy_paste_icon_select_all.png" );

/**
 * Constant values for building the GUI
 */
const Vector4 POPUP_BORDER( 14.0f, 14.0f, 14.0f, 14.0f );  ///< The margin of the popup.
const Vector2 POPUP_MIN_SIZE( 0.0f, 126.0f );  ///< The minimum size of the popup.
const Vector2 POPUP_MAX_SIZE( 720.0f, 126.0f );  ///< The maximum size of the popup.
const float POPUP_TAIL_Y_OFFSET( -2.25f );  ///< The y offset of the tail.
const Vector2 POPUP_TAIL_SIZE( 36.0f, 36.0f );  ///< The size of the tail.
const Vector2 POPUP_DIVIDER_SIZE( 1.0f, 126.0f );  ///< The size of the divider.

const Vector4 OPTION_MARGIN( 0.0f, 0.0f, 24.0f, 19.0f );  ///< The margin of the icon. the order is left, right, top and bottom
const Vector2 OPTION_MAX_SIZE( 360.0f, 126.0f );  ///< The maximum size of the option.
const Vector2 OPTION_MIN_SIZE( 128.0f, 126.0f );  ///< The minimum size of the option.
const Vector2 OPTION_ICON_SIZE( 45.0f, 45.0f );  ///< The size of the icon.
const Vector2 OPTION_TEXT_MIN_SIZE( 128.0f, 30.0f );  ///< The minimum size of the text.
const float OPTION_GAP_ICON_TEXT( 8.0f );  ///< The gap between the icon and the text

const float HIDE_POPUP_ANIMATION_DURATION( 0.2f );                            ///< Duration of popup hide animation in seconds.
const float SHOW_POPUP_ANIMATION_DURATION( 0.2f );                            ///< Duration of popup show animation in seconds.

/**
 * Default Colors
 */
const Vector4 DEFAULT_POPUP_BACKGROUND( Vector4( .20f, 0.29f, 0.44f, 1.0f ) );
const Vector4 DEFAULT_POPUP_BACKGROUND_PRESSED( Vector4( 0.07f, 0.10f, 0.17f, 1.0f ) );
const Vector4 DEFAULT_POPUP_LINE_COLOR( Vector4( 0.36f, 0.45f, 0.59f, 1.0f ) );
const Vector4 DEFAULT_OPTION_ICON( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Vector4 DEFAULT_OPTION_ICON_PRESSED( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Vector4 DEFAULT_OPTION_TEXT( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
const Vector4 DEFAULT_OPTION_TEXT_PRESSED( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );


/**
 * Confine Actor to boundaries of reference actor (e.g. Parent)
 * Actor bounds (top-left position + size) are confined to reference Actor's
 * bounds.
 */
struct ConfinementConstraint
{
  /**
   * Confinement constraint constructor.
   * @param[in] topLeftMargin (optional) Top-Left margins (defaults to 0.0f, 0.0f)
   * @param[in] bottomRightMargin (optional) Bottom-Right margins (defaults to 0.0f, 0.0f)
   * @paran[in[ flipHorizontal (optional) whether to flip Actor to other side if near edge
   * @param[in] flipVertical (optional) whether to flip Actor to the other side if near edge
   * @param[in] boundingRect Rectangle to bound Popup to.
   *
   */
  ConfinementConstraint(Vector2 topLeftMargin = Vector2::ZERO, Vector2 bottomRightMargin = Vector2::ZERO, bool flipHorizontal = false, bool flipVertical = false, Rect<float> boundingRect = Rect<float>(0.0f, 0.0f, 0.0f, 0.0f) )
  : mMinIndent(topLeftMargin),
    mMaxIndent(bottomRightMargin),
    mFlipHorizontal(flipHorizontal),
    mFlipVertical(flipVertical),
    mBoundingRect( boundingRect )
  {
  }

  Vector3 operator()(const Vector3&    constPosition,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentOriginProperty,
                     const PropertyInput& anchorPointProperty,
                     const PropertyInput& referenceSizeProperty,
                     const PropertyInput& alternativeOffsetProperty,
                     const PropertyInput& requestedPositionProperty )
  {
    const Vector3& size = sizeProperty.GetVector3();
    const Vector3& origin = parentOriginProperty.GetVector3();
    const Vector3& anchor = anchorPointProperty.GetVector3();
    const Vector3& referenceSize = referenceSizeProperty.GetVector3();
    const Vector2& alternativeOffset = alternativeOffsetProperty.GetVector2();
    const Vector3& requestedPosition = requestedPositionProperty.GetVector3();

    Vector3 newPosition( requestedPosition );

    // Get actual position of Actor relative to parent's Top-Left.
    Vector3 position(constPosition + origin * referenceSize);

    // if top-left corner is outside of Top-Left bounds, then push back in screen.

    Vector3 corner(position - size * anchor - mMinIndent);

    if ( mFlipHorizontal )
    {
      if( corner.x < mBoundingRect.x )
      {
        // Snap Popup to left hand boundary so stays visible
        corner.x = mBoundingRect.x - ( origin.x * referenceSize.x ) + ( size.x * anchor.x );
        newPosition.x = corner.x;
      }
      else if ( ( corner.x + size.x ) > ( mBoundingRect.x + mBoundingRect.width ))
      {
        // Calculate offset from left boundary Popup must be placed at so it does not exceed right side boundary.
        float requiredOffSetFromLeftBoundaryToFit = mBoundingRect.width - size.x;
        corner.x = mBoundingRect.x + requiredOffSetFromLeftBoundaryToFit - ( origin.x * referenceSize.x ) + ( size.x * anchor.x );
        newPosition.x = corner.x;
       }
    }

    if(mFlipVertical && corner.y < 0.0f)
    {
      corner.y = 0.0f;
      newPosition.y += size.height + alternativeOffset.height;
    }

    newPosition.y -= std::min(corner.y, 0.0f);

    // if bottom-right corner is outside of Bottom-Right bounds, then push back in screen.
    corner += size - referenceSize + mMinIndent + mMaxIndent;

    if(mFlipVertical && corner.y > 0.0f)
    {
      corner.y = 0.0f;
      newPosition.y -= size.height + alternativeOffset.height;
    }

    return newPosition;
  }

  Vector3 mMinIndent;                                   ///< Top-Left Margin
  Vector3 mMaxIndent;                                   ///< Bottom-Right Margin.
  bool mFlipHorizontal;                                 ///< Whether to flip actor's position if exceeds horizontal screen bounds
  bool mFlipVertical;                                   ///< Whether to flip actor's position if exceeds vertical screen bounds
  Rect<float> mBoundingRect;                            ///< Bounding Rect Popup must stay within
};

/**
 * Confine actor to the x axis boundaries of reference actor (e.g. Parent)
 */
struct ParentXAxisConstraint
{
  /**
   * Confinement constraint constructor.
   */
  ParentXAxisConstraint( float handlesMidPoint = 0.0f )
  : mHandlesMidPoint( handlesMidPoint )
  {
  }

  float operator()(  const float          constXPosition,
                     const PropertyInput& localSizeWidthProperty,
                     const PropertyInput& parentWidthProperty,
                     const PropertyInput& anchorPointXProperty )
  {
    const float localSizeProperty = localSizeWidthProperty.GetFloat();
    const float size = parentWidthProperty.GetFloat();

    float newPosition = std::max( mHandlesMidPoint, -size/2 + localSizeProperty );
    newPosition = std::min( newPosition, size/2 - localSizeProperty );
    return newPosition;
  }

  float mHandlesMidPoint;
};


} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

const char* const TextInputPopup::SIGNAL_PRESSED = "pressed";
const char* const TextInputPopup::SIGNAL_HIDE_FINISHED = "hide-finished";
const char* const TextInputPopup::SIGNAL_SHOW_FINISHED = "show-finished";

const char* const TextInputPopup::OPTION_SELECT_WORD = "option-select_word";                       // "Select Word" popup option.
const char* const TextInputPopup::OPTION_SELECT_ALL("option-select_all");                          // "Select All" popup option.
const char* const TextInputPopup::OPTION_CUT("option-cut");                                        // "Cut" popup option.
const char* const TextInputPopup::OPTION_COPY("option-copy");                                      // "Copy" popup option.
const char* const TextInputPopup::OPTION_PASTE("option-paste");                                    // "Paste" popup option.
const char* const TextInputPopup::OPTION_CLIPBOARD("option-clipboard");                            // "Clipboard" popup option.

TextInputPopup::TextInputPopup()
: mState(StateHidden),
  mRoot(Layer::New()),
  mPopupTailXPosition( 0.0f ),
  mContentSize( POPUP_MIN_SIZE ),
  mBackgroundColor( DEFAULT_POPUP_BACKGROUND ),
  mBackgroundPressedColor( DEFAULT_POPUP_BACKGROUND_PRESSED ),
  mLineColor( DEFAULT_POPUP_LINE_COLOR ),
  mIconColor( DEFAULT_OPTION_ICON ),
  mIconPressedColor( DEFAULT_OPTION_ICON_PRESSED ),
  mTextColor( DEFAULT_OPTION_TEXT ),
  mTextPressedColor( DEFAULT_OPTION_TEXT_PRESSED ),
  mSelectOptionPriority(1),
  mSelectAllOptionPriority(2),
  mCutOptionPriority(3),
  mCopyOptionPriority(4),
  mPasteOptionPriority(5),
  mClipboardOptionPriority(6),
  mPressedSignal(),
  mHideFinishedSignal(),
  mShowFinishedSignal()
{
  mAlternativeOffsetProperty = mRoot.RegisterProperty( POPUP_ALTERNATIVE_OFFSET, Vector2::ZERO );
  mRequestionPositionProperty = mRoot.RegisterProperty( POPUP_REQUESTED_POSITION, Vector3::ZERO );
  mRoot.SetParentOrigin( ParentOrigin::CENTER );
  mRoot.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  // constrain popup to size of parent.
}

void TextInputPopup::AddToStage()
{
  // TODO: Confinement constraint borders should be defined by the application.
  // It should also not use the stage directly, instead it should add to parent container.
  Stage::GetCurrent().Add(mRoot);

  ApplyConfinementConstraint();
}

void TextInputPopup::ApplyConfinementConstraint()
{
  mRoot.RemoveConstraints();
  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    LocalSource( Actor::SIZE ),
                                                    LocalSource( Actor::PARENT_ORIGIN ),
                                                    LocalSource( Actor::ANCHOR_POINT ),
                                                    ParentSource( Actor::SIZE ),
                                                    LocalSource( mAlternativeOffsetProperty ),
                                                    LocalSource( mRequestionPositionProperty),
                                                    ConfinementConstraint( DEFAULT_POPUP_INDICATOR_OFFSET,
                                                                           Vector2::ZERO,
                                                                           true,
                                                                           true, mBoundingRect ) );
  mRoot.ApplyConstraint(constraint);
}

void TextInputPopup::ApplyTailConstraint()
{
  mTail.RemoveConstraints();
  Constraint constraint = Constraint::New<float>( Actor::POSITION_X,
                                                  LocalSource( Actor::SIZE_WIDTH ),
                                                  ParentSource( Actor::SIZE_WIDTH ),
                                                  LocalSource( Actor::PARENT_ORIGIN_X ),
                                                  ParentXAxisConstraint( mPopupTailXPosition ));
  mTail.ApplyConstraint( constraint );
}

void TextInputPopup::CreateStencil( const Vector2& size)
{
  mStencil = CreateSolidColorActor( Color::BLUE );
  mStencil.SetParentOrigin( ParentOrigin::CENTER );
  mStencil.SetAnchorPoint( AnchorPoint::CENTER );
  mStencil.SetDrawMode( DrawMode::STENCIL );
  mStencil.SetVisible( true );
  mStencil.SetName( "popup-stencil" );
  mStencil.SetSize( size );
}

void TextInputPopup::OnScrollStarted( const Vector3& position )
{
  mBackground.SetSensitive( false );
}

void TextInputPopup::OnScrollCompleted( const Vector3& position )
{
  mBackground.SetSensitive( true );
}

void TextInputPopup::CreateScrollView()
{
  mScrollView = Toolkit::ScrollView::New();
  mScrollView.SetName("popup-scroll-view");
  mScrollView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mScrollView.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mScrollView.SetPosition( POPUP_BORDER.x, POPUP_BORDER.y );
  mScrollView.SetScrollingDirection( PanGestureDetector::DIRECTION_HORIZONTAL, Degree( 40.0f ) );
  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollStartedSignal().Connect( this, &TextInputPopup::OnScrollStarted );
  mScrollView.ScrollCompletedSignal().Connect( this, &TextInputPopup::OnScrollCompleted );
}

void TextInputPopup::UpdateScrollViewProperty( const Vector2& visibleSize )
{
  mScrollView.SetSize( visibleSize.x, visibleSize.y );

  RulerPtr rulerX = new DefaultRuler();  // IntrusivePtr which is unreferenced when ScrollView is destroyed.
  RulerPtr rulerY = new DefaultRuler();  // IntrusivePtr which is unreferenced when ScrollView is destroyed.
  rulerY->Disable();
  rulerX->SetDomain( RulerDomain( 0, mContentSize.width, true ) );
  mScrollView.SetRulerX(rulerX);
  mScrollView.SetRulerY(rulerY);
}

void TextInputPopup::RemoveFromStage()
{
  Stage::GetCurrent().Remove( mRoot );
}

void TextInputPopup::Clear()
{
  if ( mBackground )
  {
    UnparentAndReset( mTail );
    UnparentAndReset( mStencil );
    UnparentAndReset( mBackground );
    UnparentAndReset( mScrollView );
    mButtonContainer.clear();
    mDividerContainer.clear();

    RemoveFromStage();
    mRoot.RemoveConstraints();

    mState = StateHidden;
  }
}

ImageActor TextInputPopup::CreateOptionIcon( Image iconImage, const Vector4& color )
{
  ImageActor icon = ImageActor::New( iconImage );
  icon.SetSize( OPTION_ICON_SIZE );
  icon.SetParentOrigin( ParentOrigin::TOP_CENTER );
  icon.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  icon.SetColor( color );
  icon.SetY( OPTION_MARGIN.z );
  return icon;
}

Toolkit::TextView TextInputPopup::CreateOptionCaption( const std::string& caption, const Vector4& color )
{
  TextStyle style;
  style.SetTextColor( color );

  PointSize pointSize( Font::PixelsToPoints( OPTION_TEXT_MIN_SIZE.y ) );
  style.SetFontPointSize( pointSize );

  MarkupProcessor::StyledTextArray styledCaption;
  styledCaption.push_back( MarkupProcessor::StyledText( Text( caption ), style ) );

  Toolkit::TextView textView = Toolkit::TextView::New( styledCaption );
  textView.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
  textView.SetWidthExceedPolicy( Toolkit::TextView::ShrinkToFit );
  textView.SetHeightExceedPolicy( Toolkit::TextView::ShrinkToFit );
  textView.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  textView.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  textView.SetY( -OPTION_MARGIN.w );

  const float textWidth = textView.GetWidthForHeight( OPTION_TEXT_MIN_SIZE.y );
  textView.SetSize( textWidth, OPTION_TEXT_MIN_SIZE.y );

  return textView;
}

void TextInputPopup::CreateBackground()
{
  // Create background-panel if not already created (required if we have at least one option)
  if ( !mBackground )
  {
    Image bgImg = Image::New( POPUP_BACKGROUND );
    mBackground = ImageActor::New( bgImg );
    mBackground.SetAnchorPoint( AnchorPoint::CENTER );
    mBackground.SetParentOrigin( ParentOrigin::CENTER );
    mBackground.SetName( "text-input-popup-background" );
    mBackground.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mBackground.SetColor( mBackgroundColor );

    Image bgEffectImg = Image::New( POPUP_BACKGROUND_EFFECT );
    mBackgroundEffect = ImageActor::New( bgEffectImg );
    mBackgroundEffect.SetAnchorPoint( AnchorPoint::CENTER );
    mBackgroundEffect.SetParentOrigin( ParentOrigin::CENTER );
    mBackgroundEffect.SetName( "text-input-popup-background-effect" );
    mBackgroundEffect.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mBackgroundEffect.SetZ( 1.0f );
    mBackground.Add( mBackgroundEffect );

    Image bgLine = Image::New( POPUP_BACKGROUND_LINE );
    mBackgroundLine = ImageActor::New( bgLine );
    mBackgroundLine.SetAnchorPoint( AnchorPoint::CENTER);
    mBackgroundLine.SetParentOrigin( ParentOrigin::CENTER );
    mBackgroundLine.SetName( "text-input-popup-background-effect" );
    mBackgroundLine.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mBackgroundLine.SetColor( mLineColor );
    mBackgroundLine.SetZ( 0.1f );
    mBackgroundEffect.Add( mBackgroundLine );

    Hide(false);
    AddToStage();
  }
}

void TextInputPopup::CreateTail()
{
  if ( !mTail )
  {
    Image tail = Image::New( POPUP_TAIL_BOTTOM );
    mTail = ImageActor::New( tail );
    mTail.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mTail.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mTail.SetName( "text-input-popup-tail" );
    mTail.SetPosition( 0.0f, POPUP_TAIL_Y_OFFSET - POPUP_BORDER.w, 1.2f );
    mTail.SetColor( mBackgroundColor );

    Image tailEffect = Image::New( POPUP_TAIL_BOTTOM_EFFECT );
    mTailEffect = ImageActor::New( tailEffect );
    mTailEffect.SetParentOrigin( ParentOrigin::CENTER );
    mTailEffect.SetAnchorPoint( AnchorPoint::CENTER );
    mTailEffect.SetName( "text-input-popup-tail-effect" );
    mTailEffect.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mTailEffect.SetZ( 0.1f );
    mTail.Add( mTailEffect );

    Image tailLine = Image::New( POPUP_TAIL_BOTTOM_LINE );
    mTailLine = ImageActor::New( tailLine );
    mTailLine.SetParentOrigin( ParentOrigin::CENTER );
    mTailLine.SetAnchorPoint( AnchorPoint::CENTER );
    mTailLine.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    mTailLine.SetName( "text-input-popup-tail-line" );
    mTailLine.SetColor( mLineColor );
    mTailLine.SetZ( 0.1f );
    mTailEffect.Add( mTailLine );
  }
}

ImageActor TextInputPopup::CreateDivider()
{
    ImageActor divider = Toolkit::CreateSolidColorActor( mLineColor );
    divider.SetParentOrigin( ParentOrigin::TOP_LEFT );
    divider.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    divider.SetSize( POPUP_DIVIDER_SIZE );
    divider.SetPosition( mContentSize.width - POPUP_DIVIDER_SIZE.width, 0.0f );

    // Keep track of all the dividers. As their height's need to be updated to the max. of all
    // buttons currently added.
    mDividerContainer.push_back( divider );

    return divider;
}

ImageActor TextInputPopup::CreatePressedBackground( const Vector2& requiredSize )
{
  ImageActor pressedBg = Toolkit::CreateSolidColorActor( mBackgroundPressedColor );
  pressedBg.SetDrawMode( DrawMode::OVERLAY );
  pressedBg.SetParentOrigin( ParentOrigin::CENTER );
  pressedBg.SetAnchorPoint( AnchorPoint::CENTER );
  pressedBg.SetSize( requiredSize );
  return pressedBg;
}

TextInputPopup::ButtonRequirement TextInputPopup::CreateRequiredButton( TextInputPopup::Buttons buttonId, std::size_t orderOfPriority,
                                                                        const std::string& name, const std::string& caption, Image iconImage, bool enabled )
{
  TextInputPopup::ButtonRequirement currentButton;
  currentButton.buttonId = buttonId;
  currentButton.orderOfPriority = orderOfPriority;
  currentButton.name = name;
  currentButton.caption = caption;
  currentButton.iconImage = iconImage;
  currentButton.enabled = enabled;

  return currentButton;
}

void TextInputPopup::CreateOrderedListOfOptions()
{
  mOrderListOfButtons.clear();

  for ( std::size_t index= 0; index < ButtonsEnumEnd; index++ )
  {
    TextInputPopup::ButtonRequirement currentButton;

    // Create button for each possible option using Option priority
    switch ( index )
    {
      case ButtonsCut:
      {
        Image cutIcon = Image::New( OPTION_ICON_CUT );
        currentButton = CreateRequiredButton( ButtonsCut, mCutOptionPriority, OPTION_CUT, GET_LOCALE_TEXT("IDS_COM_BODY_CUT"), cutIcon, false );
        break;
      }
      case ButtonsCopy:
      {
        Image copyIcon = Image::New( OPTION_ICON_COPY );
        currentButton = CreateRequiredButton( ButtonsCopy, mCopyOptionPriority, OPTION_COPY, GET_LOCALE_TEXT("IDS_COM_BODY_COPY"), copyIcon, false );
        break;
      }
      case ButtonsPaste:
      {
        Image pasteIcon = Image::New( OPTION_ICON_PASTE );
        currentButton = CreateRequiredButton( ButtonsPaste, mPasteOptionPriority, OPTION_PASTE, GET_LOCALE_TEXT("IDS_COM_BODY_PASTE"), pasteIcon, false );
        break;
      }
      case ButtonsSelect:
      {
        Image selectIcon = Image::New( OPTION_ICON_SELECT );
        currentButton = CreateRequiredButton( ButtonsSelect, mSelectOptionPriority, OPTION_SELECT_WORD, GET_LOCALE_TEXT("IDS_COM_SK_SELECT"), selectIcon, false );
        break;
      }
      case ButtonsSelectAll:
      {
        Image selectAllIcon = Image::New( OPTION_ICON_SELECT_ALL );
        currentButton = CreateRequiredButton( ButtonsSelectAll, mSelectAllOptionPriority, OPTION_SELECT_ALL, GET_LOCALE_TEXT("IDS_COM_BODY_SELECT_ALL"), selectAllIcon, false );
        break;
      }
      case ButtonsClipboard:
      {
        Image clipboardIcon = Image::New( OPTION_ICON_CLIPBOARD );
        currentButton = CreateRequiredButton( ButtonsClipboard, mClipboardOptionPriority, OPTION_CLIPBOARD, GET_LOCALE_TEXT("IDS_COM_BODY_CLIPBOARD"), clipboardIcon, false );
        break;
      }
      case ButtonsEnumEnd:
      {
        DALI_ASSERT_DEBUG( "ButtonsEnumEnd used but an invalid choice");
        currentButton.orderOfPriority = 0;
        break;
      }
    }

    bool match = false;

    // Insert button in list of buttons in order of priority setting.
    for( std::vector<ButtonRequirement>::iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt && !match ); ++it )
    {
      const ButtonRequirement& button( *it );
      if ( currentButton.orderOfPriority < button.orderOfPriority )
      {
        if ( currentButton.orderOfPriority != 0 ) // If order priority 0 then do not add button as not required.
        {
          mOrderListOfButtons.insert( it, currentButton );
        }
        match = true;
      }
    }

    if ( !match)
    {
      mOrderListOfButtons.push_back( currentButton );
    }
  }
}

void TextInputPopup::AddOption(const std::string& name, const std::string& caption, const Image iconImage, bool finalOption)
{
  // 1. Create container for text and icon when not pressed.
  Actor optionContainer = Actor::New();
  optionContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
  optionContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // 2. Add text.
  Toolkit::TextView captionTextView = CreateOptionCaption( caption, mTextColor );
  optionContainer.Add( captionTextView );

  // 3. Add icon.
  ImageActor icon = CreateOptionIcon( iconImage, mIconColor );
  optionContainer.Add( icon );

  // 4. Calculate the size of option.
  const Vector2 textSize = Vector2( captionTextView.GetNaturalSize() );
  const Vector2 optionSize( std::max( textSize.x, OPTION_ICON_SIZE.x ),
                            OPTION_MARGIN.z + OPTION_ICON_SIZE.y + OPTION_GAP_ICON_TEXT + textSize.y + OPTION_MARGIN.w );
  const Vector2 constrainedOptionSize = Min( Max( optionSize, OPTION_MIN_SIZE ), OPTION_MAX_SIZE );

  // 5. Create a option.
  Toolkit::PushButton option = Toolkit::PushButton::New();
  option.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
  option.SetParentOrigin( ParentOrigin::TOP_LEFT );
  option.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  option.SetSize( constrainedOptionSize );
  option.SetX( mContentSize.x );
  option.SetName( name );
  option.ClickedSignal().Connect( this, &TextInputPopup::OnButtonPressed );
  mScrollView.Add( option );

  // 6. Set the normal option image.
  option.SetButtonImage( optionContainer );

  // 7. Update the content size.
  mContentSize.x += constrainedOptionSize.x;

  // 8. Create the pressed container.
  Actor optionPressedContainer = Actor::New();

  // 9. Add option pressed background.
  Vector2 optionPressedBackgroundSize( constrainedOptionSize.x - 1.0f, constrainedOptionSize.y - 2.0f );
  ImageActor optionPressedBackground = CreatePressedBackground( optionPressedBackgroundSize );
  optionPressedContainer.Add( optionPressedBackground );

  // 10. Add pressed text
  Toolkit::TextView pressedCaptionTextView = CreateOptionCaption( caption, mTextPressedColor );
  optionPressedBackground.Add( pressedCaptionTextView );

  // 11. Add pressed icon
  ImageActor pressedIcon = CreateOptionIcon( iconImage, mIconPressedColor );
  optionPressedBackground.Add( pressedIcon );

  // 12. Set the pressed option image
  option.SetPressedImage( optionPressedContainer );

  // 13. Add the divider
  if ( !finalOption )
  {
    ImageActor divider = CreateDivider();
    mScrollView.Add( divider );
  }
}

void TextInputPopup::Hide(bool animate)
{
  if( mRoot )
  {
    if(mAnimation)
    {
      mAnimation.Clear();
      mAnimation.Reset();
    }

    if(animate)
    {
      mAnimation = Animation::New( HIDE_POPUP_ANIMATION_DURATION );
      mAnimation.AnimateTo( Property(mRoot, Actor::SCALE), Vector3::ZERO, AlphaFunctions::EaseOut );
      mAnimation.AnimateTo( Property(mRoot, Actor::COLOR_ALPHA), 0.0f, AlphaFunctions::EaseOut );
      mAnimation.Play();

      mAnimation.FinishedSignal().Connect( this, &TextInputPopup::OnHideFinished );
      mState = StateHiding;
    }
    else
    {
      mRoot.SetProperty(Actor::SCALE, Vector3::ZERO);
      mRoot.SetProperty(Actor::COLOR_ALPHA, 0.0f);
      mState = StateHidden;
    }
  }
}

void TextInputPopup::Show(bool animate)
{
  if( mRoot )
  {
    mRoot.SetSensitive( true );

    if(mAnimation)
    {
      mAnimation.Clear();
      mAnimation.Reset();
    }

    if(animate)
    {
      mAnimation = Animation::New( SHOW_POPUP_ANIMATION_DURATION );
      mAnimation.AnimateTo( Property(mRoot, Actor::SCALE), Vector3::ONE, AlphaFunctions::EaseOut );
      mAnimation.AnimateTo( Property(mRoot, Actor::COLOR_ALPHA), 1.0f, AlphaFunctions::EaseOut );
      mAnimation.Play();

      mAnimation.FinishedSignal().Connect( this, &TextInputPopup::OnShowFinished );
      mState = StateShowing;
    }
    else
    {
      mRoot.SetProperty(Actor::SCALE, Vector3::ONE);
      mRoot.SetProperty(Actor::COLOR_ALPHA, 1.0f);
      mState = StateShown;
    }
  }
}

void TextInputPopup::SetAlternativeOffset(Vector2 offset)
{
  mRoot.SetProperty( mAlternativeOffsetProperty, offset );
  ApplyConfinementConstraint();
}

TextInputPopup::State TextInputPopup::GetState(void) const
{
  return mState;
}

Actor TextInputPopup::GetRootActor() const
{
  return mRoot;
}

// Styling

void TextInputPopup::SetCutPastePopupColor( const Vector4& color )
{
  mBackgroundColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupColor() const
{
  return mBackgroundColor;
}

void TextInputPopup::SetCutPastePopupPressedColor( const Vector4& color )
{
  mBackgroundPressedColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupPressedColor() const
{
  return mBackgroundPressedColor;
}

void TextInputPopup::SetCutPastePopupBorderColor( const Vector4& color )
{
  mLineColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupBorderColor() const
{
  return mLineColor;
}

void TextInputPopup::SetCutPastePopupIconColor( const Vector4& color )
{
  mIconColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupIconColor() const
{
  return mIconColor;
}

void TextInputPopup::SetCutPastePopupIconPressedColor( const Vector4& color )
{
  mIconPressedColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupIconPressedColor()
{
  return mIconPressedColor;
}

void TextInputPopup::SetCutPastePopupTextColor( const Vector4& color )
{
  mTextColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupTextColor()
{
  return mTextColor;
}

void TextInputPopup::SetCutPastePopupTextPressedColor( const Vector4& color )
{
  mTextPressedColor = color;
}

const Vector4& TextInputPopup::GetCutPastePopupTextPressedColor()
{
  return mTextPressedColor;
}

void TextInputPopup::TogglePopupButtonOnOff( TextInputPopup::Buttons requiredButton, bool enable )
{
  bool match ( false );
  for( std::vector<ButtonRequirement>::iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt && !match ); ++it )
   {
     ButtonRequirement& button( *it );
     if ( requiredButton == button.buttonId )
     {
       button.enabled = enable;
       match = true;
     }
   }
}

void TextInputPopup::SetButtonPriorityPosition( TextInputPopup::Buttons button, unsigned int priority )
{
  switch ( button )
  {
    case ButtonsCut:
    {
      mCutOptionPriority = priority;
      break;
    }
    case ButtonsCopy:
    {
      mCopyOptionPriority = priority;
      break;
    }
    case ButtonsPaste:
    {
      mPasteOptionPriority = priority;
      break;
    }
    case ButtonsSelect:
    {
      mSelectOptionPriority = priority;
      break;
    }
    case ButtonsSelectAll:
    {
      mSelectAllOptionPriority = priority;
      break;
    }
    case ButtonsClipboard:
    {
      mClipboardOptionPriority = priority;
      break;
    }
    case ButtonsEnumEnd:
    {
      DALI_ASSERT_DEBUG( "ButtonsEnumEnd used but an invalid choice");
      break;
    }
  }
  CreateOrderedListOfOptions(); // Update list of options as priority changed.
}

unsigned int TextInputPopup::GetButtonPriorityPosition( TextInputPopup::Buttons button ) const
{
  unsigned int priority = 0;

  switch ( button )
  {
    case ButtonsCut:
    {
      priority = mCutOptionPriority;
      break;
    }
    case ButtonsCopy:
    {
      priority = mCopyOptionPriority;
      break;
    }
    case ButtonsPaste:
    {
      priority = mPasteOptionPriority;
      break;
    }
    case ButtonsSelect:
    {
      priority = mSelectOptionPriority;
      break;
    }
    case ButtonsSelectAll:
    {
      priority = mSelectAllOptionPriority;
      break;
    }
    case ButtonsClipboard:
    {
      priority = mClipboardOptionPriority;
      break;
    }
    case ButtonsEnumEnd:
    {
      DALI_ASSERT_DEBUG( "ButtonsEnumEnd used but an invalid choice");
      break;
    }
  }

  return priority;
}

void TextInputPopup::AddPopupOptions()
{
  mContentSize = POPUP_MIN_SIZE;

  // 1. Create the background.
  CreateBackground();
  mRoot.Add(mBackground);

  // 2. Create the tail.
  CreateTail();
  mBackground.Add( mTail );

  // 3. Create the scroll view.
  CreateScrollView();
  mBackground.Add( mScrollView );

  // 4. Create the options and add into the scroll view.
  for( std::vector<ButtonRequirement>::const_iterator it = mOrderListOfButtons.begin(), endIt = mOrderListOfButtons.end(); ( it != endIt ); ++it )
  {
    const ButtonRequirement& button( *it );
    if ( button.enabled )
    {
      AddOption( button.name, button.caption, button.iconImage, it != ( endIt - 1 ) ? false : true );
    }
  }

  // 5. Calcurate a lot of size to make the layout.
  const Vector2 visibleContentSize  = Vector2( std::min( mContentSize.x, POPUP_MAX_SIZE.x - POPUP_BORDER.x - POPUP_BORDER.y ), mContentSize.y );
  const Vector2 popupSize = Vector2( POPUP_BORDER.x + visibleContentSize.x + POPUP_BORDER.y,
                                     POPUP_BORDER.z + visibleContentSize.y + POPUP_BORDER.w );

  // 6. Set the scroll view ruller.
  UpdateScrollViewProperty( visibleContentSize );

  // 8. Create stencil
  const Vector2 stencilSize = Vector2( popupSize.x, popupSize.y + POPUP_TAIL_SIZE.x + POPUP_TAIL_Y_OFFSET );
  CreateStencil( stencilSize );
  mRoot.Add( mStencil );

  // 7. Set the root size.
  mRoot.SetSize( popupSize );   // Make Root Actor reflect the size of its content

}

void TextInputPopup::SetPopupBoundary( const Rect<float>& boundingRectangle )
{
  mBoundingRect =  boundingRectangle;
}

void TextInputPopup::SetTailPosition( const Vector3& position )
{
  mRoot.SetProperty( mRequestionPositionProperty, position );
  mPopupTailXPosition = position.x;
  ApplyConfinementConstraint();
  ApplyTailConstraint();
}

bool TextInputPopup::OnButtonPressed( Toolkit::Button button )
{
  mPressedSignal.Emit( button );
  return false;
}

void TextInputPopup::OnHideFinished(Animation& source)
{
  source.FinishedSignal().Disconnect( this, &TextInputPopup::OnHideFinished );
  Clear();
  mState = StateHidden;
  mHideFinishedSignal.Emit( *this );
}

void TextInputPopup::OnShowFinished(Animation& source)
{
  source.FinishedSignal().Disconnect( this, &TextInputPopup::OnShowFinished );
  mState = StateShown;
  mShowFinishedSignal.Emit( *this );
}

TextInputPopup::PressedSignalV2& TextInputPopup::PressedSignal()
{
  return mPressedSignal;
}

TextInputPopup::HideFinishedSignalV2& TextInputPopup::HideFinishedSignal()
{
  return mHideFinishedSignal;
}

TextInputPopup::ShowFinishedSignalV2& TextInputPopup::ShowFinishedSignal()
{
  return mShowFinishedSignal;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

