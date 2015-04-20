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
#include <dali-toolkit/internal/controls/popup/popup-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/adaptor-framework/physical-keyboard.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/button.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>
#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>

using namespace Dali;

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

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Popup, Toolkit::Control, Create )

DALI_SIGNAL_REGISTRATION( Popup, "touched-outside", SIGNAL_TOUCHED_OUTSIDE )
DALI_SIGNAL_REGISTRATION( Popup, "hidden",          SIGNAL_HIDDEN          )

DALI_TYPE_REGISTRATION_END()

// Properties
const char* const PROPERTY_TITLE = "title";
const char* const PROPERTY_STATE = "state";

const float POPUP_ANIMATION_DURATION = 0.45f;                      ///< Duration of hide/show animations

const float POPUP_WIDTH = 720.0f;                             ///< Width of Popup
const float POPUP_OUT_MARGIN_WIDTH = 16.f;                    ///< Space between the screen edge and the popup edge in the horizontal dimension.
const float POPUP_OUT_MARGIN_HEIGHT = 36.f;                   ///< Space between the screen edge and the popup edge in the vertical dimension.
const float POPUP_TITLE_WIDTH = 648.0f;                       ///<Width of Popup Title
const float POPUP_BUTTON_BG_HEIGHT = 96.f;                    ///< Height of Button Background.
const Vector3 DEFAULT_DIALOG_SIZE = Vector3(POPUP_TITLE_WIDTH/POPUP_WIDTH, 0.5f, 0.0f);
const Vector3 DEFAULT_BOTTOM_SIZE = Vector3(1.0f, 0.2f, 0.0f);

} // unnamed namespace

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
  mPopupStyle(PopupStylePtr(&style)),
  mPropertyTitle(Property::INVALID_INDEX),
  mPropertyState(Property::INVALID_INDEX)
{
  SetKeyboardNavigationSupport( true );
}

void Popup::OnInitialize()
{
  Dali::Stage stage = Dali::Stage::GetCurrent();

  Actor self = Self();
  self.SetSensitive(false);
  // Reisize to fit the height of children
  self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

  // Create Layer
  mLayer = Layer::New();
  mLayer.SetName( "POPUP_LAYER" );
  mLayer.SetParentOrigin(ParentOrigin::CENTER);
  mLayer.SetAnchorPoint(AnchorPoint::CENTER);
  mLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mLayer.SetDrawMode( DrawMode::OVERLAY );

  // Any content after this point which is added to Self() will be reparented to
  // mContent.
  mAlterAddedChild = true;
  // Add Backing (Dim effect)
  CreateBacking();
  mAlterAddedChild = false;

  // Add Dialog ( background image, title, content container, button container and tail )
  CreateDialog();

  mLayer.Add( self );

  mPopupLayout = Toolkit::TableView::New( 3, 1 );
  mPopupLayout.SetName( "POPUP_LAYOUT_TABLE" );
  mPopupLayout.SetParentOrigin(ParentOrigin::CENTER);
  mPopupLayout.SetAnchorPoint(AnchorPoint::CENTER);
  mPopupLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mPopupLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mPopupLayout.SetFitHeight( 0 );   // Set row to fit
  mPopupLayout.SetFitHeight( 1 );   // Set row to fit
  self.Add( mPopupLayout );

  // Any content after this point which is added to Self() will be reparented to
  // mContent.
  mAlterAddedChild = true;

  // Default content.
//  ShowTail(ParentOrigin::BOTTOM_CENTER);

  // Hide content by default.
  SetState( Toolkit::Popup::POPUP_HIDE, 0.0f );

  mPropertyTitle = self.RegisterProperty( PROPERTY_TITLE, "", Property::READ_WRITE );
  mPropertyState = self.RegisterProperty( PROPERTY_STATE, "POPUP_HIDE", Property::READ_WRITE );

  // Make self as keyboard focusable and focus group
  self.SetKeyboardFocusable(true);
  SetAsKeyboardFocusGroup(true);
}

void Popup::MarkDirtyForRelayout()
{
  // Flag all the popup controls for relayout as it is about to be hidden and miss the main flagging pass
  mLayer.RelayoutRequestTree();
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
  mLayer.Unparent();
}

size_t Popup::GetButtonCount() const
{
  return mButtons.size();
}

void Popup::SetBackgroundImage( Actor image )
{
  // Removes any previous background.
  if( mBackgroundImage && mPopupLayout )
  {
    mPopupLayout.Remove( mBackgroundImage );
  }

  // Adds new background to the dialog.
  mBackgroundImage = image;

  mBackgroundImage.SetName( "POPUP_BACKGROUND_IMAGE" );

  // OnDialogTouched only consume the event. It prevents the touch event to be caught by the backing.
  mBackgroundImage.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  mBackgroundImage.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  mBackgroundImage.SetAnchorPoint( AnchorPoint::CENTER );
  mBackgroundImage.SetParentOrigin( ParentOrigin::CENTER );

  Vector3 border( mPopupStyle->backgroundOuterBorder.x, mPopupStyle->backgroundOuterBorder.z, 0.0f );
  mBackgroundImage.SetSizeModeFactor( border );

  const bool prevAlter = mAlterAddedChild;
  mAlterAddedChild = false;
  Self().Add( mBackgroundImage );
  mAlterAddedChild = prevAlter;
}

void Popup::SetButtonAreaImage( Actor image )
{
  // Removes any previous area image.
  if( mButtonAreaImage && mPopupLayout )
  {
    mPopupLayout.Remove( mButtonAreaImage );
  }

  // Adds new area image to the dialog.
  mButtonAreaImage = image;

  // OnDialogTouched only consume the event. It prevents the touch event to be caught by the backing.
  mButtonAreaImage.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  mButtonAreaImage.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mButtonAreaImage.SetAnchorPoint( AnchorPoint::CENTER );
  mButtonAreaImage.SetParentOrigin( ParentOrigin::CENTER );

  if( GetButtonCount() > 0 )
  {
    mBottomBg.Add( mButtonAreaImage );
  }
}

void Popup::SetTitle( const std::string& text )
{
  // Replaces the current title actor.
  if( mPopupLayout )
  {
    mPopupLayout.RemoveChildAt( Toolkit::TableView::CellPosition( 0, 0 ) );
  }

  mTitle = Toolkit::TextLabel::New( text );
  mTitle.SetName( "POPUP_TITLE" );
  mTitle.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
  mTitle.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );

  if( mPopupLayout )
  {
    mTitle.SetPadding( Padding( 0.0f, 0.0f, mPopupStyle->margin, mPopupStyle->margin ) );
    mTitle.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mTitle.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
    mPopupLayout.AddChild( mTitle, Toolkit::TableView::CellPosition( 0, 0 ) );
  }

  RelayoutRequest();
}

std::string Popup::GetTitle() const
{
  if( mTitle )
  {
    return mTitle.GetProperty<std::string>( Toolkit::TextLabel::Property::TEXT );
  }

  return std::string();
}

void Popup::CreateFooter()
{
  if( !mBottomBg )
  {
    // Adds bottom background
    mBottomBg = Actor::New();
    mBottomBg.SetName( "POPUP_BOTTOM_BG" );
    mBottomBg.SetRelayoutEnabled( true );
    mBottomBg.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    mPopupLayout.SetFixedHeight( 2, mPopupStyle->bottomSize.height );   // Buttons
    mPopupLayout.AddChild( mBottomBg, Toolkit::TableView::CellPosition( 2, 0 ) );
  }
}

void Popup::AddButton( Toolkit::Button button )
{
  mButtons.push_back( button );
  button.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::ALL_DIMENSIONS );    // Size will be assigned to it

  // If this is the first button added
  if( mButtons.size() == 1 )
  {
    CreateFooter();

    if( mButtonAreaImage )
    {
      mBottomBg.Add( mButtonAreaImage );
    }
  }

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
    Image tail = ResourceImage::New( image );
    mTailImage = ImageActor::New(tail);
    const Vector3 anchorPoint = AnchorPoint::BOTTOM_RIGHT - position;

    mTailImage.SetParentOrigin(position);
    mTailImage.SetAnchorPoint(anchorPoint);

    CreateFooter();

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
  Image buttonBg = ResourceImage::New( mPopupStyle->buttonAreaImage );
  ImageActor buttonBgImage = ImageActor::New( buttonBg );
  buttonBgImage.SetStyle( ImageActor::STYLE_NINE_PATCH );
  buttonBgImage.SetNinePatchBorder( mPopupStyle->buttonArea9PatchBorder );

  SetBackgroundImage( ImageActor::New( ResourceImage::New( mPopupStyle->backgroundImage ) ) );
  SetButtonAreaImage( buttonBgImage );
}

void Popup::CreateBacking()
{
  mBacking = Dali::Toolkit::CreateSolidColorActor( mPopupStyle->backingColor );
  mBacking.SetName( "POPUP_BACKING" );

  mBacking.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  mBacking.SetSensitive(true);

  mLayer.Add( mBacking );
  mBacking.SetOpacity(0.0f);
  mBacking.TouchedSignal().Connect( this, &Popup::OnBackingTouched );
  mBacking.MouseWheelEventSignal().Connect(this, &Popup::OnBackingMouseWheelEvent);
}

void Popup::CreateDialog()
{
  // Adds default background image.
  SetDefaultBackgroundImage();
}

void Popup::HandleStateChange( Toolkit::Popup::PopupState state, float duration )
{
  Vector3 targetSize;
  float targetBackingAlpha;

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
      mShowing = true;

      // Add contents to stage for showing.
      if( !mLayer.GetParent() )
      {
        Dali::Stage stage = Dali::Stage::GetCurrent();
        stage.Add( mLayer );
        mLayer.RaiseToTop();
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

  Actor self = Self();
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
      mAnimation.AnimateTo( Property(mBacking, Actor::Property::COLOR_ALPHA), targetBackingAlpha, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
      mAnimation.AnimateTo( Property(self, Actor::Property::SCALE), targetSize, AlphaFunctions::EaseInOut, TimePeriod(duration * 0.5f, duration * 0.5f) );
    }
    else
    {
      mAnimation.AnimateTo( Property(mBacking, Actor::Property::COLOR_ALPHA), targetBackingAlpha, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
      mAnimation.AnimateTo( Property(self, Actor::Property::SCALE), targetSize, AlphaFunctions::EaseInOut, TimePeriod(0.0f, duration * 0.5f) );
    }
    mAnimation.Play();
    mAnimation.FinishedSignal().Connect(this, &Popup::OnStateAnimationFinished);
  }
  else
  {
    mBacking.SetOpacity( targetBackingAlpha );
    self.SetScale( targetSize );

    HandleStateChangeComplete();
  }
}

void Popup::HandleStateChangeComplete()
{
  // Remove contents from stage if completely hidden.
  if( ( mState == Toolkit::Popup::POPUP_HIDE ) && mLayer.GetParent() )
  {
    mLayer.Unparent();
    Self().SetSensitive( false );

    // Guard against destruction during signal emission
    Toolkit::Popup handle( GetOwner() );
    mHiddenSignal.Emit();
  }
}

Toolkit::Popup::TouchedOutsideSignalType& Popup::OutsideTouchedSignal()
{
  return mTouchedOutsideSignal;
}

Toolkit::Popup::HiddenSignalType& Popup::HiddenSignal()
{
  return mHiddenSignal;
}

bool Popup::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::Popup popup = Toolkit::Popup::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TOUCHED_OUTSIDE ) )
  {
    popup.OutsideTouchedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_HIDDEN ) )
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

      mTouchedOutsideSignal.Emit();
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
      mPopupLayout.RemoveChildAt( Toolkit::TableView::CellPosition( 1, 0 ) );
    }

    // keep a handle to the new content.
    mContent = child;

    mPopupLayout.AddChild( mContent, Toolkit::TableView::CellPosition( 1, 0 ) );
  }
}

void Popup::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  // Hide the background image
  mBackgroundImage.SetVisible( !( mButtons.empty() && mPopupLayout.GetChildCount() == 0 ) );

  // Relayout All buttons
  if ( !mButtons.empty() )
  {
    // All buttons should be the same size and fill the button area. The button spacing needs to be accounted for as well.
    Vector2 buttonSize( ( ( size.width - mPopupStyle->buttonSpacing * ( mButtons.size() + 1 ) ) / mButtons.size() ),
                        mPopupStyle->bottomSize.height - mPopupStyle->margin );

    Vector3 buttonPosition( mPopupStyle->buttonSpacing, 0.0f, 0.0f );

    for ( ActorIter iter = mButtons.begin(), endIter = mButtons.end();
          iter != endIter;
          ++iter, buttonPosition.x += mPopupStyle->buttonSpacing + buttonSize.width )
    {
      Actor button = *iter;

      // If there is only one button, it needs to be laid out on center.
      if ( mButtons.size() == 1 )
      {
        buttonPosition.x = 0.0f;
        button.SetAnchorPoint( AnchorPoint::CENTER );
        button.SetParentOrigin( ParentOrigin::CENTER );
      }
      else
      {
        button.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
        button.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      }

      button.SetPosition( buttonPosition );

      button.PropagateRelayoutFlags();    // Reset relayout flags for relayout
      container.Add( button, buttonSize );
    }
  }
}

void Popup::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
  if( mPopupLayout )
  {
    if( policy == ResizePolicy::FIT_TO_CHILDREN )
    {
      mPopupLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, dimension );
      if( dimension & Dimension::HEIGHT )
      {
        mPopupLayout.SetFitHeight( 1 );
      }
    }
    else
    {
      mPopupLayout.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, dimension );
      // Make the content cell fill the whole of the available space
      if( dimension & Dimension::HEIGHT )
      {
        mPopupLayout.SetRelativeHeight( 1, 1.0f );
      }
    }
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

    // As TextLabel GetNaturalSize does not take wrapping into account, limit the width
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
    Vector3 contentSize = mContent.GetNaturalSize();
    // Choose the biggest width
    naturalSize.width = std::max( naturalSize.width, contentSize.width );
    if( naturalSize.width > maxWidth )
    {
      naturalSize.width = maxWidth;
      contentSize.height = mContent.GetHeightForWidth( maxWidth );
    }
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
    height += mContent.GetHeightForWidth( popupWidth ) + mPopupStyle->margin;
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
