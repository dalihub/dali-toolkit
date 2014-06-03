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

#include <dali-toolkit/internal/controls/popup/popup-impl.h>

#include <dali-toolkit/public-api/controls/buttons/button.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

#include <dali-toolkit/internal/controls/relayout-helper.h>
#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>

#include <dali-toolkit/public-api/focus-manager/focus-manager.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace std;

namespace
{
const float CONTENT_DEPTH = 1.0f;                                 ///< 3D Effect of buttons/title etc. appearing off the popup.
const float POPUP_ANIMATION_DURATION = 0.5f;                      ///< Duration of hide/show animations
const float BACKING_DEPTH = -1.0f;                                ///< Depth of backing (positioned just behind dialog, so dialog catches hit events first)

const float POPUP_WIDTH = 720.0f;                             ///< Width of Popup
const float POPUP_OUT_MARGIN_WIDTH = 16.f;                    ///< Space between the screen edge and the popup edge in the horizontal dimension.
const float POPUP_OUT_MARGIN_HEIGHT = 36.f;                   ///< Space between the screen edge and the popup edge in the vertical dimension.
const float POPUP_TITLE_WIDTH = 648.0f;                       ///<Width of Popup Title
const float POPUP_BUTTON_BG_HEIGHT = 96.f;                    ///< Height of Button Background.
const Vector3 DEFAULT_DIALOG_SIZE = Vector3(POPUP_TITLE_WIDTH/POPUP_WIDTH, 0.5f, 0.0f);
const Vector3 DEFAULT_BOTTOM_SIZE = Vector3(1.0f, 0.2f, 0.0f);

const char* const PROPERTY_TITLE = "title";
const char* const PROPERTY_STATE = "state";

// Constraints ///////////////////////////////////////////////////////////////////////////

/**
 * BackgroundSizeConstraint
 *
 * The background size should be at least as big as the Dialog.
 * In some cases a background may have graphics which are visible
 * outside of the Dialog, e.g. A Shadow. For this we need to alter
 * the size of Background.
 */
struct BackgroundSizeConstraint
{
  /**
   * Constraint that sets size to parent's size plus a border.
   *
   * @param[in] outerBorder The border to extend beyond parent's Size.
   */
  BackgroundSizeConstraint( Vector4 outerBorder )
  : mOuterBorder( outerBorder )
  {
  }

  /**
   * (render thread code)
   * @param[in] current The current size.
   * @param[in] parentSizeProperty The parent's size
   */
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& parentSizeProperty )
  {
    Vector3 size = parentSizeProperty.GetVector3();

    size.width += mOuterBorder.x + mOuterBorder.y;
    size.height += mOuterBorder.z + mOuterBorder.w;

    return size;
  }

  const Vector4 mOuterBorder;  ///< The size of the outer-border (Set to 0.0, 0.0f, 0.0f, 0.0f if doesn't exist).
};

struct ButtonAreaSizeConstraint
{
  /**
   * Constraint that sets size to parent's size plus a border.
   *
   * @param[in] outerBorder The border to extend beyond parent's Size.
   */
  ButtonAreaSizeConstraint( Vector4 outerBorder )
  : mOuterBorder( outerBorder )
  {
  }

  /**
   * (render thread code)
   * @param[in] current The current size.
   * @param[in] parentSizeProperty The parent's size
   */
  Vector3 operator()( const Vector3& current,
                      const PropertyInput& parentSizeProperty )
  {
    Vector3 size = parentSizeProperty.GetVector3();

    size.width += mOuterBorder.x + mOuterBorder.y;
    size.width -= (POPUP_OUT_MARGIN_WIDTH + POPUP_OUT_MARGIN_WIDTH);
    size.height = POPUP_BUTTON_BG_HEIGHT;

    return size;
  }

  const Vector4 mOuterBorder;  ///< The size of the outer-border (Set to 0.0, 0.0f, 0.0f, 0.0f if doesn't exist).
};

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::Popup::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::Popup), typeid(Toolkit::Control), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::Popup::SIGNAL_TOUCHED_OUTSIDE, &Popup::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, Toolkit::Popup::SIGNAL_HIDDEN, &Popup::DoConnectSignal );


}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Popup
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::Popup Popup::New()
{
  PopupStylePtr style = PopupStyleDefault::New();

  // Create the implementation
  PopupPtr popup(new Popup(*style));

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::Popup handle(*popup);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  popup->Initialize();

  return handle;
}

Popup::Popup(PopupStyle& style)
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mShowing(false),
  mState(Toolkit::Popup::POPUP_NONE), // Initially, the popup state should not be set, it's set in OnInitialize
  mAlterAddedChild(false),
  mPopupStyle(PopupStylePtr(&style))
{
  SetKeyboardNavigationSupport( true );
}

void Popup::OnInitialize()
{
  Actor self = Self();
  self.SetSensitive(false);

  // Create Layer
  mLayer = Layer::New();
  mLayer.SetParentOrigin(ParentOrigin::CENTER);
  mLayer.SetAnchorPoint(AnchorPoint::CENTER);
  mLayer.RaiseToTop();
  mLayer.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  self.Add(mLayer);

  mPopupBg = Actor::New();
  mPopupBg.SetParentOrigin(ParentOrigin::CENTER);
  mPopupBg.SetAnchorPoint(AnchorPoint::CENTER);
  mPopupBg.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mLayer.Add(mPopupBg);

  // Any content after this point which is added to Self() will be reparented to
  // mContent.
  mAlterAddedChild = true;

  // Add Backing (Dim effect)
  CreateBacking();

  // Add Dialog ( background image, title, content container, button container and tail )
  CreateDialog();

  // Default content.
  ShowTail(ParentOrigin::BOTTOM_CENTER);

  // Hide content by default.
  SetState( Toolkit::Popup::POPUP_HIDE, 0.0f );

  mPropertyTitle = self.RegisterProperty( PROPERTY_TITLE, "", Property::READ_WRITE );
  mPropertyState = self.RegisterProperty( PROPERTY_STATE, "POPUP_HIDE", Property::READ_WRITE );

  // Make self as keyboard focusable and focus group
  self.SetKeyboardFocusable(true);
  SetAsKeyboardFocusGroup(true);
}

void Popup::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  if( index == mPropertyTitle )
  {
    SetTitle(propertyValue.Get<std::string>());
  }
  else if ( index == mPropertyState )
  {
    std::string value( propertyValue.Get<std::string>() );
    if(value == "POPUP_SHOW")
    {
      SetState( Toolkit::Popup::POPUP_SHOW, 0.0f );
    }
    else if( value == "POPUP_HIDE")
    {
      SetState( Toolkit::Popup::POPUP_HIDE, 0.0f );
    }
  }
}

Popup::~Popup()
{
}

size_t Popup::GetButtonCount() const
{
  return mButtons.size();
}

void Popup::SetBackgroundImage( Actor image )
{
  // Removes any previous background.
  if( mBackgroundImage && mPopupBg )
  {
    mPopupBg.Remove( mBackgroundImage );
  }

  // Adds new background to the dialog.
  mBackgroundImage = image;

  // OnDialogTouched only consume the event. It prevents the touch event to be caught by the backing.
  mBackgroundImage.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  mPopupBg.Add( mBackgroundImage );
}

void Popup::SetButtonAreaImage( Actor image )
{
  // Removes any previous area image.
  if( mButtonAreaImage && mPopupBg )
  {
    mPopupBg.Remove( mButtonAreaImage );
  }

  // Adds new area image to the dialog.
  mButtonAreaImage = image;

  // OnDialogTouched only consume the event. It prevents the touch event to be caught by the backing.
  mButtonAreaImage.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  mPopupBg.Add( mButtonAreaImage );
}

void Popup::SetTitle( const std::string& text )
{
  Toolkit::TextView titleActor = Toolkit::TextView::New();
  titleActor.SetText( text );
  titleActor.SetColor( Color::BLACK );
  titleActor.SetMultilinePolicy( Toolkit::TextView::SplitByWord );
  titleActor.SetWidthExceedPolicy( Toolkit::TextView::Split );
  titleActor.SetLineJustification( Toolkit::TextView::Center );

  SetTitle( titleActor );
}

void Popup::SetTitle( Toolkit::TextView titleActor )
{
  // Replaces the current title actor.
  if( mTitle && mPopupBg )
  {
    mPopupBg.Remove( mTitle );
  }
  mTitle = titleActor;

  mPopupBg.Add( mTitle );

  RelayoutRequest();
}

Toolkit::TextView Popup::GetTitle() const
{
  return mTitle;
}

void Popup::AddButton( Toolkit::Button button )
{
  mButtons.push_back( button );
  mBottomBg.Add( button );

  RelayoutRequest();
}

void Popup::SetState( Toolkit::Popup::PopupState state )
{
  SetState( state, POPUP_ANIMATION_DURATION );
}

void Popup::SetState( Toolkit::Popup::PopupState state, float duration )
{
  // default animation behaviour.
  HandleStateChange(state, duration);
}

Toolkit::Popup::PopupState Popup::GetState() const
{
  return mState;
}

void Popup::ShowTail(const Vector3& position)
{
  // Replaces the tail actor.
  if(mTailImage && mTailImage.GetParent())
  {
    mTailImage.GetParent().Remove( mTailImage );
    mTailImage.Reset();
  }

  std::string image = "";

  // depending on position of tail around ParentOrigin, a different tail image is used...
  if(position.y < Math::MACHINE_EPSILON_1)
  {
    image = mPopupStyle->tailUpImage;
  }
  else if(position.y > 1.0f - Math::MACHINE_EPSILON_1)
  {
    image = mPopupStyle->tailDownImage;
  }
  else if(position.x < Math::MACHINE_EPSILON_1)
  {
    image = mPopupStyle->tailLeftImage;
  }
  else if(position.x > 1.0f - Math::MACHINE_EPSILON_1)
  {
    image = mPopupStyle->tailRightImage;
  }

  if(image != "")
  {
    Image tail = Image::New( image );
    mTailImage = ImageActor::New(tail);
    const Vector3 anchorPoint = AnchorPoint::FRONT_BOTTOM_RIGHT - position;

    mTailImage.SetParentOrigin(position);
    mTailImage.SetAnchorPoint(anchorPoint);

    mBottomBg.Add(mTailImage);
  }
}

void Popup::HideTail()
{
  ShowTail(ParentOrigin::CENTER);
}

void Popup::SetStyle(PopupStyle& style)
{
  mPopupStyle = PopupStylePtr(&style);
  // update //
}

PopupStylePtr Popup::GetStyle() const
{
  return mPopupStyle;
}

void Popup::SetDefaultBackgroundImage()
{
  Image bg = Image::New( mPopupStyle->backgroundImage );
  ImageActor bgImage = ImageActor::New( bg );
  bgImage.SetStyle( ImageActor::STYLE_NINE_PATCH );
  bgImage.SetNinePatchBorder( mPopupStyle->backgroundScale9Border );

  Image buttonBg = Image::New( mPopupStyle->buttonAreaImage );
  ImageActor buttonBgImage = ImageActor::New( buttonBg );
  buttonBgImage.SetStyle( ImageActor::STYLE_NINE_PATCH );
  buttonBgImage.SetNinePatchBorder( mPopupStyle->buttonArea9PatchBorder );

  SetBackgroundImage( bgImage );
  SetButtonAreaImage( buttonBgImage );
}

void Popup::CreateBacking()
{
  mBacking = Dali::Toolkit::CreateSolidColorActor( mPopupStyle->backingColor );

  mBacking.SetPositionInheritanceMode(DONT_INHERIT_POSITION);
  mBacking.SetSensitive(true);

  mLayer.Add(mBacking);
  mBacking.SetOpacity(0.0f);
  mBacking.SetPosition(0.0f, 0.0f, BACKING_DEPTH);
  mBacking.TouchedSignal().Connect( this, &Popup::OnBackingTouched );
  mBacking.MouseWheelEventSignal().Connect(this, &Popup::OnBackingMouseWheelEvent);
}

void Popup::CreateDialog()
{
  // Adds default background image.
  SetDefaultBackgroundImage();

  // Adds bottom background
  mBottomBg = Actor::New();
  mPopupBg.Add( mBottomBg );
}

void Popup::HandleStateChange( Toolkit::Popup::PopupState state, float duration )
{
  const Vector2& stageSize( Stage::GetCurrent().GetSize() );

  Vector3 targetSize;
  float targetBackingAlpha;
  Vector3 targetBackingSize;

  if(mState == state)
  {
    return;
  }
  mState = state;
  switch(state)
  {
    case Toolkit::Popup::POPUP_HIDE:
    {
      targetSize = Vector3(0.0f, 0.0f, 1.0f);
      targetBackingAlpha = 0.0f;
      targetBackingSize = Vector3(0.0f, 0.0f, 1.0f);
      mShowing = false;
      ClearKeyInputFocus();

      // Retore the keyboard focus when popup is hidden
      if(mPreviousFocusedActor && mPreviousFocusedActor.IsKeyboardFocusable() )
      {
        Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
        if( keyboardFocusManager )
        {
          keyboardFocusManager.SetCurrentFocusActor(mPreviousFocusedActor);
        }
      }

      break;
    }

    case Toolkit::Popup::POPUP_SHOW:
    default:
    {
      targetSize = Vector3(1.0f, 1.0f, 1.0f);
      targetBackingAlpha = 1.0f;
      float length = (stageSize.width > stageSize.height) ? stageSize.width : stageSize.height;
      targetBackingSize = Vector3( length, length, 1.0f );
      mShowing = true;

      // Add contents to stage for showing.
      if( !mLayer.GetParent() )
      {
        mAlterAddedChild = false;
        Self().Add(mLayer);
        mAlterAddedChild = true;
      }
      Self().SetSensitive(true);
      SetKeyInputFocus();

      // Handle the keyboard focus when popup is shown
      Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
      if( keyboardFocusManager )
      {
        mPreviousFocusedActor = keyboardFocusManager.GetCurrentFocusActor();

        if( mContent && mContent.IsKeyboardFocusable() )
        {
          // If content is focusable, move the focus to content
          keyboardFocusManager.SetCurrentFocusActor(mContent);
        }
        else if( !mButtons.empty() )
        {
          // Otherwise, movethe focus to the first button
          keyboardFocusManager.SetCurrentFocusActor(mButtons[0]);
        }
        else
        {
          DALI_LOG_WARNING("There is no focusable in popup\n");
        }
      }
      break;
    }
  }

  mBacking.SetSize( targetBackingSize );

  if(duration > Math::MACHINE_EPSILON_1)
  {
    if ( mAnimation )
    {
      mAnimation.Stop();
      mAnimation.Clear();
      mAnimation.Reset();
    }
    mAnimation = Animation::New(duration);

    if(mShowing)
    {
      mAnimation.AnimateTo( Property(mBacking, Actor::COLOR_ALPHA), targetBackingAlpha, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
      mAnimation.AnimateTo( Property(mPopupBg, Actor::SCALE), targetSize, AlphaFunctions::EaseInOut, TimePeriod(duration * 0.5f, duration * 0.5f) );
    }
    else
    {
      mAnimation.AnimateTo( Property(mBacking, Actor::COLOR_ALPHA), targetBackingAlpha, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
      mAnimation.AnimateTo( Property(mPopupBg, Actor::SCALE), targetSize, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
    }
    mAnimation.Play();
    mAnimation.FinishedSignal().Connect(this, &Popup::OnStateAnimationFinished);
  }
  else
  {
    mBacking.SetOpacity( targetBackingAlpha );
    mPopupBg.SetScale( targetSize );

    HandleStateChangeComplete();
  }
}

void Popup::HandleStateChangeComplete()
{
  // Remove contents from stage if completely hidden.
  if( (mState == Toolkit::Popup::POPUP_HIDE) && (mLayer.GetParent()) )
  {
    Self().Remove(mLayer);
    Self().SetSensitive( false );

    // Guard against destruction during signal emission
    Toolkit::Popup handle( GetOwner() );
    mHiddenSignalV2.Emit();
  }
}

Toolkit::Popup::TouchedOutsideSignalV2& Popup::OutsideTouchedSignal()
{
  return mTouchedOutsideSignalV2;
}

Toolkit::Popup::HiddenSignalV2& Popup::HiddenSignal()
{
  return mHiddenSignalV2;
}

bool Popup::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Popup popup = Toolkit::Popup::DownCast(handle);

  if( Dali::Toolkit::Popup::SIGNAL_TOUCHED_OUTSIDE == signalName )
  {
    popup.OutsideTouchedSignal().Connect( tracker, functor );
  }
  else if( Dali::Toolkit::Popup::SIGNAL_HIDDEN == signalName )
  {
    popup.HiddenSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Popup::OnStateAnimationFinished( Animation& source )
{
  HandleStateChangeComplete();
}

bool Popup::OnBackingTouched(Actor actor, const TouchEvent& event)
{
  if(event.GetPointCount()>0)
  {
    const TouchPoint& point = event.GetPoint(0);

    if(point.state == TouchPoint::Down)
    {
      // Guard against destruction during signal emission
      Toolkit::Popup handle( GetOwner() );

      mTouchedOutsideSignalV2.Emit();
    }
  }

  return true;
}

bool Popup::OnBackingMouseWheelEvent(Actor actor, const MouseWheelEvent& event)
{
  // consume mouse wheel event in dimmed backing actor
  return true;
}

bool Popup::OnDialogTouched(Actor actor, const TouchEvent& event)
{
  // consume event (stops backing actor receiving touch events)
  return true;
}

void Popup::OnControlChildAdd( Actor& child )
{
  // reparent any children added by user to the body layer.
  if( mAlterAddedChild )
  {
    // Removes previously added content.
    if( mContent )
    {
      mPopupBg.Remove( mContent );
    }

    // Reparent new content.
    Self().Remove( child );

    // keep a handle to the new content.
    mContent = child;

    mPopupBg.Add( mContent );
  }
}

void Popup::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  // Set the popup size
  Vector2 popupSize;
  popupSize.width  = size.width - 2.f * ( POPUP_OUT_MARGIN_WIDTH + mPopupStyle->margin );
  popupSize.height = size.height - 2.f * ( POPUP_OUT_MARGIN_WIDTH + mPopupStyle->margin );

  // Update sizes of all popup's components.

  // Relayout background image.
  // Adjust background position and size relative to parent to cater to outer Border.
  // Some backgrounds are intended to over-spill. That is some content
  // should appear outside the Dialog on all sides i.e. Shadows, glow effects.
  const Vector4 outerBorder = mPopupStyle->backgroundOuterBorder;

  if( mBackgroundImage )
  {
    Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                      ParentSource( Actor::SIZE ),
                                                      BackgroundSizeConstraint(outerBorder) );

    mBackgroundImage.RemoveConstraints();
    mBackgroundImage.ApplyConstraint( constraint );

    mBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mBackgroundImage.SetPosition( -outerBorder.x, -outerBorder.y, 0.0f );
  }

  if( mPopupBg && mButtonAreaImage )
  {
    // If there are no buttons, button background is also removed.
    if ( mButtons.size() == 0 )
    {
      mPopupBg.Remove( mButtonAreaImage );
    }
    else
    {
      Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                      ParentSource( Actor::SIZE ),
                                                      ButtonAreaSizeConstraint(outerBorder) );

      mButtonAreaImage.RemoveConstraints();
      mButtonAreaImage.ApplyConstraint( constraint );

      mButtonAreaImage.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      mButtonAreaImage.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      mButtonAreaImage.SetY( -outerBorder.z - POPUP_OUT_MARGIN_HEIGHT );

      mPopupBg.Add( mButtonAreaImage );
    }
  }

  // Relayout title
  Vector3 positionOffset( 0.0f, mPopupStyle->margin + POPUP_OUT_MARGIN_WIDTH, CONTENT_DEPTH );
  if( mTitle )
  {
    Vector2 titleSize;
    titleSize.width  = popupSize.width;
    titleSize.height = mTitle.GetHeightForWidth( titleSize.width );

    // As the default size policy for text-view is Fixed & Fixed, a size needs to be set.
    // Otherwise size-negotiation algorithm uses the GetNaturalSize() with doesn't take
    // into account the multiline and exceed policies, giving as result a wrong size.
    mTitle.SetSize( titleSize );
    Relayout( mTitle, titleSize, container );

    mTitle.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mTitle.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mTitle.SetPosition( positionOffset );

    positionOffset.y += titleSize.height + mPopupStyle->margin;
  }

  // Relayout content
  if( mContent )
  {
    // If the content width is greater than popup width then scale it down/wrap text as needed
    Vector2 contentSize( RelayoutHelper::GetNaturalSize( mContent ) );
    if( contentSize.width > popupSize.width )
    {
      contentSize.width = popupSize.width;
      contentSize.height = RelayoutHelper::GetHeightForWidth( mContent, contentSize.width );
    }

    mContent.SetSize( contentSize );
    Relayout( mContent, contentSize, container );

    mContent.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mContent.SetAnchorPoint(AnchorPoint::TOP_CENTER);

    mContent.SetPosition( positionOffset );

    positionOffset.y += contentSize.height + mPopupStyle->margin;
  }

  // Relayout Button Area
  if( mBottomBg )
  {
    mBottomBg.SetSize( popupSize.width, mPopupStyle->bottomSize.height );

    mBottomBg.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mBottomBg.SetAnchorPoint(AnchorPoint::TOP_CENTER);

    mBottomBg.SetPosition( positionOffset );
  }

  // Relayout All buttons
  if ( !mButtons.empty() )
  {
    // All buttons should be the same size and fill the button area. The button spacing needs to be accounted for as well.
    Vector2 buttonSize( ( ( popupSize.width - mPopupStyle->buttonSpacing * ( mButtons.size() - 1 ) ) / mButtons.size() ),
                        mPopupStyle->bottomSize.height - mPopupStyle->margin );

    Vector3 buttonPosition;

    for ( ActorIter iter = mButtons.begin(), endIter = mButtons.end();
          iter != endIter;
          ++iter, buttonPosition.x += mPopupStyle->buttonSpacing + buttonSize.width )
    {
      iter->SetPosition( buttonPosition );

      // If there is only one button, it needs to be laid out on center.
      if ( mButtons.size() == 1 )
      {
        iter->SetAnchorPoint( AnchorPoint::CENTER );
        iter->SetParentOrigin( ParentOrigin::CENTER );
      }
      else
      {
        iter->SetAnchorPoint( AnchorPoint::CENTER_LEFT );
        iter->SetParentOrigin( ParentOrigin::CENTER_LEFT );
      }

      Relayout( *iter, buttonSize, container );
    }
  }

  if( mShowing && mBacking )
  {
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    float length = (stageSize.width > stageSize.height) ? stageSize.width : stageSize.height;
    Vector3 targetBackingSize = Vector3( length, length, 1.0f );

    mBacking.SetSize( targetBackingSize );
  }
}

bool Popup::OnKeyEvent(const KeyEvent& event)
{
  bool consumed = false;

  if(event.state == KeyEvent::Down)
  {
    if (event.keyCode == Dali::DALI_KEY_ESCAPE || event.keyCode == Dali::DALI_KEY_BACK)
    {
      SetState(Toolkit::Popup::POPUP_HIDE);
      consumed = true;
    }
  }

  return consumed;
}

Vector3 Popup::GetNaturalSize()
{
  float margin = 2.0f * ( POPUP_OUT_MARGIN_WIDTH + mPopupStyle->margin );
  const float maxWidth = Stage::GetCurrent().GetSize().width - margin;

  Vector3 naturalSize( 0.0f, 0.0f, 0.0f );

  if ( mTitle )
  {
    Vector3 titleNaturalSize = mTitle.GetImplementation().GetNaturalSize();
    // Buffer to avoid errors. The width of the popup could potentially be the width of the title text.
    // It was observed in this case that text wrapping was then inconsistent when seen on device
    const float titleBuffer = 0.5f;
    titleNaturalSize.width += titleBuffer;

    // As TextView GetNaturalSize does not take wrapping into account, limit the width
    // to that of the stage
    if( titleNaturalSize.width >= maxWidth)
    {
      naturalSize.width = maxWidth;
      naturalSize.height = mTitle.GetImplementation().GetHeightForWidth( naturalSize.width );
    }
    else
    {
      naturalSize += titleNaturalSize;
    }

    naturalSize.height += mPopupStyle->margin;
  }

  if( mContent )
  {
    Vector3 contentSize = RelayoutHelper::GetNaturalSize( mContent );
    // Choose the biggest width
    naturalSize.width = std::max( naturalSize.width, contentSize.width );
    naturalSize.height += contentSize.height + mPopupStyle->margin;
  }

  if( !mButtons.empty() )
  {
    naturalSize.height += mPopupStyle->bottomSize.height;
  }

  // Add the margins
  naturalSize.width += margin;
  naturalSize.height += margin;

  return naturalSize;
}

float Popup::GetHeightForWidth( float width )
{
  float height( 0.0f );
  float popupWidth( width - 2.f * ( POPUP_OUT_MARGIN_WIDTH + mPopupStyle->margin ) );

  if ( mTitle )
  {
    height += mTitle.GetImplementation().GetHeightForWidth( popupWidth );
    height += mPopupStyle->margin;
  }

  if( mContent )
  {
    height += RelayoutHelper::GetHeightForWidth( mContent, popupWidth ) + mPopupStyle->margin;
  }

  if( !mButtons.empty() )
  {
    height += mPopupStyle->bottomSize.height;
  }

  // Add the margins
  float margin( 2.0f * ( POPUP_OUT_MARGIN_WIDTH + mPopupStyle->margin ) );
  height += margin;

  return height;
}

float Popup::GetWidthForHeight( float height )
{
  return GetNaturalSize().width;
}

Actor Popup::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled)
{
  Actor nextFocusableActor( currentFocusedActor );

  // TODO: Needs to be optimised

  if ( !currentFocusedActor || ( currentFocusedActor && KeyboardFocusManager::Get().GetFocusGroup(currentFocusedActor) != Self() ) )
  {
    // The current focused actor is not within popup
    if( mContent && mContent.IsKeyboardFocusable() )
    {
      // If content is focusable, move the focus to content
      nextFocusableActor = mContent;
    }
    else if( !mButtons.empty() )
    {
      // Otherwise, movethe focus to the first button
      nextFocusableActor = mButtons[0];
    }
  }
  else
  {
    // Rebuild the focus chain because button or content can be added or removed dynamically
    ActorContainer focusableActors;
    if( mContent && mContent.IsKeyboardFocusable() )
    {
      focusableActors.push_back(mContent);
    }

    for(unsigned int i = 0; i < mButtons.size(); i++)
    {
      if( mButtons[i] && mButtons[i].IsKeyboardFocusable() )
      {
        focusableActors.push_back(mButtons[i]);
      }
    }

    for ( ActorContainer::iterator iter = focusableActors.begin(), end = focusableActors.end(); iter != end; ++iter )
    {
      if ( currentFocusedActor == *iter )
      {
        switch ( direction )
        {
          case Toolkit::Control::Left:
          {
            if ( iter == focusableActors.begin() )
            {
              nextFocusableActor = *( focusableActors.end() - 1 );
            }
            else
            {
              nextFocusableActor = *( iter - 1 );
            }
            break;
          }
          case Toolkit::Control::Right:
          {
            if ( iter == focusableActors.end() - 1 )
            {
              nextFocusableActor = *( focusableActors.begin() );
            }
            else
            {
              nextFocusableActor = *( iter + 1 );
            }
            break;
          }

          case Toolkit::Control::Up:
          {
            if ( *iter == mContent )
            {
              nextFocusableActor = *( focusableActors.end() - 1 );
            }
            else
            {
              if ( mContent && mContent.IsKeyboardFocusable() )
              {
                nextFocusableActor = mContent;
              }
              else
              {
                if ( iter == focusableActors.begin() )
                {
                  nextFocusableActor = *( focusableActors.end() - 1 );
                }
                else
                {
                  nextFocusableActor = *( iter - 1 );
                }
              }
            }
            break;
          }

          case Toolkit::Control::Down:
          {
            if ( mContent && mContent.IsKeyboardFocusable() )
            {
              nextFocusableActor = mContent;
            }
            else
            {
              if ( iter == focusableActors.end() - 1 )
              {
                nextFocusableActor = *( focusableActors.begin() );
              }
              else
              {
                nextFocusableActor = *( iter + 1 );
              }
            }

            if ( *iter == mContent && !mButtons.empty() )
            {
              nextFocusableActor = mButtons[0];
            }
            break;
          }
        }

        if(!nextFocusableActor)
        {
          DALI_LOG_WARNING("Can not decide next focusable actor\n");
        }

        break;
      }
    }
  }

  return nextFocusableActor;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
