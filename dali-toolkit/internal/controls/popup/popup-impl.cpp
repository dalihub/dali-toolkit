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
#include <dali-toolkit/internal/controls/popup/popup-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/devel-api/adaptor-framework/physical-keyboard.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>
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

/**
 * Creation function for main Popup type.
 * @return Handle to the new popup object.
 */
BaseHandle Create()
{
  return Toolkit::Popup::New();
}

// Toast style defaults.
const int          DEFAULT_TOAST_AUTO_HIDE_DELAY = 3000;                                    ///< Toast will auto-hide after 3000ms (3 seconds)
const float        DEFAULT_TOAST_TRANSITION_TIME = 0.65f;                                   ///< Default time the toast Popup will take to show and hide.
const Vector3      DEFAULT_TOAST_BOTTOM_PARENT_ORIGIN( 0.5f, 0.94f, 0.5f );                 ///< This is similar to BOTTOM_CENTER, but vertically higher up, as a ratio of parent height.
const Vector3      DEFAULT_TOAST_WIDTH_OF_STAGE_RATIO( 0.75f, 0.75f, 0.75f );               ///< Amount of the stage's width that the toast popup will take up.

/**
 * Creation function for named type "popup-toast".
 * @return Handle to the new toast popup object.
 */
BaseHandle CreateToast()
{
  Toolkit::Popup popup = Toolkit::Popup::New();

  // Setup for Toast Popup type.
  popup.SetSizeModeFactor( DEFAULT_TOAST_WIDTH_OF_STAGE_RATIO );
  popup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
  popup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  popup.SetProperty( Toolkit::Popup::Property::CONTEXTUAL_MODE, Toolkit::Popup::NON_CONTEXTUAL );
  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, DEFAULT_TOAST_TRANSITION_TIME );
  popup.SetProperty( Toolkit::Popup::Property::TAIL_VISIBILITY, false );

  // Disable the dimmed backing.
  popup.SetProperty( Toolkit::Popup::Property::BACKING_ENABLED, false );

  // The toast popup should fade in (not zoom).
  popup.SetProperty( Toolkit::Popup::Property::ANIMATION_MODE, Toolkit::Popup::FADE );

  // The toast popup should auto-hide.
  popup.SetProperty( Toolkit::Popup::Property::AUTO_HIDE_DELAY, DEFAULT_TOAST_AUTO_HIDE_DELAY );

  // Align to the bottom of the screen.
  popup.SetParentOrigin( DEFAULT_TOAST_BOTTOM_PARENT_ORIGIN );
  popup.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );

  // Let events pass through the toast popup.
  popup.SetProperty( Toolkit::Popup::Property::TOUCH_TRANSPARENT, true );

  return popup;
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::Popup, Toolkit::Control, Create )

// Main content related properties.
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "title",                             MAP,              TITLE                  )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "content",                           MAP,              CONTENT                )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "footer",                            MAP,              FOOTER                 )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "display-state",                     STRING,           DISPLAY_STATE          )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "touch-transparent",                 BOOLEAN,          TOUCH_TRANSPARENT      )

// Contextual related properties.
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-visibility",                   BOOLEAN,          TAIL_VISIBILITY        )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-position",                     VECTOR3,          TAIL_POSITION          )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "contextual-mode",                   STRING,           CONTEXTUAL_MODE        )

// Animation related properties.
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "animation-duration",                FLOAT,            ANIMATION_DURATION     )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "animation-mode",                    STRING,           ANIMATION_MODE         )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "entry-animation",                   MAP,              ENTRY_ANIMATION        )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "exit-animation",                    MAP,              EXIT_ANIMATION         )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "auto-hide-delay",                   INTEGER,          AUTO_HIDE_DELAY        )

// Style related properties.
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "backing-enabled",                   BOOLEAN,          BACKING_ENABLED        )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "backing-color",                     VECTOR4,          BACKING_COLOR          )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "popup-background-image",            STRING,           POPUP_BACKGROUND_IMAGE )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-up-image",                     STRING,           TAIL_UP_IMAGE          )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-down-image",                   STRING,           TAIL_DOWN_IMAGE        )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-left-image",                   STRING,           TAIL_LEFT_IMAGE        )
DALI_PROPERTY_REGISTRATION( Toolkit, Popup, "tail-right-image",                  STRING,           TAIL_RIGHT_IMAGE       )

// Signals.
DALI_SIGNAL_REGISTRATION(   Toolkit, Popup, "touched-outside",                                     SIGNAL_TOUCHED_OUTSIDE )
DALI_SIGNAL_REGISTRATION(   Toolkit, Popup, "showing",                                             SIGNAL_SHOWING         )
DALI_SIGNAL_REGISTRATION(   Toolkit, Popup, "shown",                                               SIGNAL_SHOWN           )
DALI_SIGNAL_REGISTRATION(   Toolkit, Popup, "hiding",                                              SIGNAL_HIDING          )
DALI_SIGNAL_REGISTRATION(   Toolkit, Popup, "hidden",                                              SIGNAL_HIDDEN          )

DALI_TYPE_REGISTRATION_END()

// Named type registration.

// Toast Popup: Non-modal popup that displays information at the bottom of the screen.
TypeRegistration typeRegistrationToast( "popup-toast", typeid( Toolkit::Popup ), CreateToast );

// Enumeration to / from string conversion tables

const Scripting::StringEnum DisplayStateTable[] = {
  { "SHOWING", Toolkit::Popup::SHOWING },
  { "SHOWN",   Toolkit::Popup::SHOWN   },
  { "HIDING",  Toolkit::Popup::HIDING  },
  { "HIDDEN",  Toolkit::Popup::HIDDEN  },
}; const unsigned int DisplayStateTableCount = sizeof( DisplayStateTable ) / sizeof( DisplayStateTable[0] );

const Scripting::StringEnum AnimationModeTable[] = {
  { "NONE",    Toolkit::Popup::NONE    },
  { "ZOOM",    Toolkit::Popup::ZOOM    },
  { "FADE",    Toolkit::Popup::FADE    },
  { "CUSTOM",  Toolkit::Popup::CUSTOM  },
}; const unsigned int AnimationModeTableCount = sizeof( AnimationModeTable ) / sizeof( AnimationModeTable[0] );

const Scripting::StringEnum ContextualModeTable[] = {
  { "NON_CONTEXTUAL", Toolkit::Popup::NON_CONTEXTUAL },
  { "ABOVE",          Toolkit::Popup::ABOVE          },
  { "RIGHT",          Toolkit::Popup::RIGHT          },
  { "BELOW",          Toolkit::Popup::BELOW          },
  { "LEFT",           Toolkit::Popup::LEFT           },
}; const unsigned int ContextualModeTableCount = sizeof( ContextualModeTable ) / sizeof( ContextualModeTable[0] );

// Popup defaults.
const Vector3 DEFAULT_POPUP_PARENT_RELATIVE_SIZE( 0.75f, 1.0f, 1.0f );                      ///< Default size percentage of parent.
const float   DEFAULT_POPUP_ANIMATION_DURATION =  0.6f;                                     ///< Duration of hide/show animations.
const float   POPUP_OUT_MARGIN_WIDTH =            16.f;                                     ///< Space between the screen edge and the popup edge in the horizontal dimension.
const float   POPUP_OUT_MARGIN_HEIGHT =           36.f;                                     ///< Space between the screen edge and the popup edge in the vertical dimension.
const Vector3 DEFAULT_TAIL_POSITION( 0.5f, 1.0f, 0.0f );                                    ///< Position the tail will be displayed when enabled without setting the position.

// Contextual defaults.
const Vector2 DEFAULT_CONTEXTUAL_ADJACENCY_MARGIN( 10.0f, 10.0f );                          ///< How close the Popup will be to it's contextual parent.
const Vector2 DEFAULT_CONTEXTUAL_STAGE_BORDER( 15.0f, 15.0f );                              ///< How close the Popup can be to the stage edges.

// Popup style defaults.
const char*   DEFAULT_BACKGROUND_IMAGE_PATH =     DALI_IMAGE_DIR "00_popup_bg.9.png";       ///< Background image.
const char*   DEFAULT_TAIL_UP_IMAGE_PATH =        DALI_IMAGE_DIR "popup_tail_up.png";       ///< Tail up image.
const char*   DEFAULT_TAIL_DOWN_IMAGE_PATH =      DALI_IMAGE_DIR "popup_tail_down.png";     ///< Tail down image.
const char*   DEFAULT_TAIL_LEFT_IMAGE_PATH =      DALI_IMAGE_DIR "popup_tail_left.png";     ///< Tail left image.
const char*   DEFAULT_TAIL_RIGHT_IMAGE_PATH =     DALI_IMAGE_DIR "popup_tail_right.png";    ///< Tail right image.

const Vector4 DEFAULT_BACKING_COLOR( 0.0f, 0.0f, 0.0f, 0.5f );                              ///< Color of the dimmed backing.
const Vector3 BACKGROUND_OUTER_BORDER( 40.0f, 30.0f, 0.0f );                                ///< External border.
const Rect<float>  DEFAULT_TITLE_PADDING( 20.0f, 20.0f, 20.0f, 20.0f );                     ///< Title padding used on popups with content and/or controls (from Tizen GUI UX).
const Rect<float>  DEFAULT_TITLE_ONLY_PADDING( 8.0f, 8.0f, 8.0f, 8.0f );                    ///< Title padding used on popups with a title only (like toast popups).
const Vector3 FOOTER_SIZE( 620.0f, 96.0f,0.0f );                                            ///< Default size of the bottom control area.
const float   DEFAULT_RELATIVE_PARENT_WIDTH =     0.75f;                                    ///< If width is not fixed, relative size to parent is used by default.

} // Unnamed namespace

/*
 * Implementation.
 */

Dali::Toolkit::Popup Popup::New()
{
  // Create the implementation
  PopupPtr popup( new Popup() );

  // Pass ownership to CustomActor via derived handle.
  Dali::Toolkit::Popup handle( *popup );

  // Second-phase initialisation of the implementation.
  // This can only be done after the CustomActor connection has been made.
  popup->Initialize();

  return handle;
}

Popup::Popup()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  // Main variables:
  mAlterAddedChild( false ),
  mLayoutDirty( true ),
  mTouchTransparent( false ),

  // Property variables:
  mDisplayState( Toolkit::Popup::HIDDEN ), // Hidden until shown with SetDisplayState()
  mTailVisible( false ),
  mTailPosition( DEFAULT_TAIL_POSITION ),
  mContextualMode( Toolkit::Popup::NON_CONTEXTUAL ),
  mAnimationDuration( DEFAULT_POPUP_ANIMATION_DURATION ),
  mAnimationMode( Toolkit::Popup::FADE ),
  mAutoHideDelay( 0 ),
  mBackingEnabled( true ),
  mBackingColor( DEFAULT_BACKING_COLOR ),
  mTailUpImage( DEFAULT_TAIL_UP_IMAGE_PATH ),
  mTailDownImage( DEFAULT_TAIL_DOWN_IMAGE_PATH ),
  mTailLeftImage( DEFAULT_TAIL_LEFT_IMAGE_PATH ),
  mTailRightImage( DEFAULT_TAIL_RIGHT_IMAGE_PATH )
{
  SetKeyboardNavigationSupport( true );
}

void Popup::OnInitialize()
{
  Actor self = Self();
  self.SetName( "popup" );

  // Apply some default resizing rules.
  self.SetParentOrigin( ParentOrigin::CENTER );
  self.SetAnchorPoint( AnchorPoint::CENTER );

  self.SetSizeModeFactor( DEFAULT_POPUP_PARENT_RELATIVE_SIZE );
  self.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

  // Create a new layer so all Popup components can appear above all other actors.
  mLayer = Layer::New();
  mLayer.SetName( "popup-layer" );

  mLayer.SetParentOrigin( ParentOrigin::CENTER );
  mLayer.SetAnchorPoint( AnchorPoint::CENTER );
  mLayer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  // Important to set as invisible as otherwise, if the popup is parented,
  // but not shown yet it will appear statically on the screen.
  mLayer.SetVisible( false );

  // Add the layer to the hierarchy.
  self.Add( mLayer );

  // Add Backing (Dimmed effect).
  mLayer.Add( CreateBacking() );

  mPopupContainer = Actor::New();
  mPopupContainer.SetName( "popup-container" );
  mPopupContainer.SetParentOrigin( ParentOrigin::CENTER );
  mPopupContainer.SetAnchorPoint( AnchorPoint::CENTER );
  mPopupContainer.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  mLayer.Add( mPopupContainer );

  // Create the Popup layout to contain all main content.
  mPopupLayout = Toolkit::TableView::New( 3, 1 );

  // Adds the default background image.
  SetPopupBackgroundImage( ImageActor::New( ResourceImage::New( DEFAULT_BACKGROUND_IMAGE_PATH ) ) );

  mPopupLayout.SetName( "popup-layout-table" );
  mPopupLayout.SetParentOrigin( ParentOrigin::CENTER );
  mPopupLayout.SetAnchorPoint( AnchorPoint::CENTER );

  mPopupLayout.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::WIDTH );
  mPopupLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mPopupLayout.SetSize( Stage::GetCurrent().GetSize().x * DEFAULT_RELATIVE_PARENT_WIDTH, 0.0f );

  mPopupLayout.SetFitHeight( 0 ); // Set row to fit.
  mPopupLayout.SetFitHeight( 1 ); // Set row to fit.

  mPopupLayout.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  mPopupContainer.Add( mPopupLayout );

  // Any content after this point which is added to Self() will be re-parented to mContent.
  mAlterAddedChild = true;

  // Make self keyboard focusable and a focus group.
  self.SetKeyboardFocusable( true );
  SetAsKeyboardFocusGroup( true );
}

Popup::~Popup()
{
  mEntryAnimationData.Clear();
  mExitAnimationData.Clear();
}

void Popup::LayoutAnimation()
{
  // Perform setup based on the currently selected animation.
  switch( mAnimationMode )
  {
    case Toolkit::Popup::ZOOM:
    {
      // Zoom animations start fully zoomed out.
      mPopupContainer.SetScale( Vector3::ZERO );
      break;
    }

    case Toolkit::Popup::FADE:
    {
      // Fade animations start transparent.
      mPopupContainer.SetOpacity( 0.0f );
      break;
    }

    case Toolkit::Popup::CUSTOM:
    {
      // Initialise the custom animation by playing to the end of it's exit animation instantly.
      // EG. If it was zooming in, then we zoom out fully instantly so the zoom in works.
      StartTransitionAnimation( false, true );
      break;
    }

    case Toolkit::Popup::NONE:
    {
      break;
    }
  }
}

void Popup::StartTransitionAnimation( bool transitionIn, bool instantaneous /* false */ )
{
  // Stop and recreate animation.
  if ( mAnimation )
  {
    mAnimation.Stop();
    mAnimation.Clear();
    mAnimation.Reset();
  }
  float duration = GetAnimationDuration();

  // Setup variables ready to start the animations.
  // If we are performing the animation instantaneously, we do not want to emit a signal.
  if( !instantaneous )
  {
    if( transitionIn )
    {
      // Setup variables and signal that we are starting the transition.
      // Note: We signal even if the transition is instant so signal order is consistent.
      mShowingSignal.Emit();
    }
    else
    {
      mHidingSignal.Emit();
    }
  }

  // Perform chosen animation for the Popup.
  switch( mAnimationMode )
  {
    case Toolkit::Popup::NONE:
    {
      mAnimation = Animation::New( 0.0f );
      break;
    }

    case Toolkit::Popup::ZOOM:
    {
      mAnimation = Animation::New( duration );
      if( duration > Math::MACHINE_EPSILON_0 )
      {
        if( transitionIn )
        {
          mAnimation.AnimateTo( Property( mPopupContainer, Actor::Property::SCALE ), Vector3::ONE, AlphaFunction::EASE_IN_OUT, TimePeriod( duration * 0.25f, duration * 0.75f ) );
        }
        else
        {
          // Zoom out animation is twice the speed. Modify the duration variable so the backing animation speed is modified also.
          duration /= 2.0f;
          mAnimation.SetDuration( duration );
          mAnimation.AnimateTo( Property( mPopupContainer, Actor::Property::SCALE ), Vector3::ZERO, AlphaFunction::EASE_IN_OUT, TimePeriod( 0.0f, duration ) );
        }
      }
      else
      {
        mPopupContainer.SetScale( transitionIn ? Vector3::ONE : Vector3::ZERO );
      }
      break;
    }

    case Toolkit::Popup::FADE:
    {
      mAnimation = Animation::New( duration );
      if( duration > Math::MACHINE_EPSILON_0 )
      {
        if( transitionIn )
        {
          mAnimation.AnimateTo( Property( mPopupContainer, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN_OUT, TimePeriod( 0.30f, duration * 0.70f ) );
        }
        else
        {
          mAnimation.AnimateTo( Property( mPopupContainer, Actor::Property::COLOR_ALPHA ), 0.0f, AlphaFunction::EASE_IN_OUT, TimePeriod( 0.0f, duration * 0.70f ) );
        }
      }
      else
      {
        mPopupContainer.SetOpacity( transitionIn ? 1.0f : 0.0f );
      }
      break;
    }

    case Toolkit::Popup::CUSTOM:
    {
      // Use a user specified animation for in and out.
      // Read the correct animation depending on entry or exit.
      // Attempt to use animation data defined from script data.
      Dali::AnimationData* animationData = transitionIn ? &mEntryAnimationData : &mExitAnimationData;

      // Create a new animation from the pre-defined data in the AnimationData class.
      // If there is no data, mAnimation is invalidated.
      mAnimation = animationData->CreateAnimation( mPopupContainer, duration );

      // If we don't have a valid animation, provide a blank one so play() can still function generically.
      if( !mAnimation )
      {
        // No animation was configured (even though custom mode was specified). Create a dummy animation to avoid an exception.
        mAnimation = Animation::New( 0.0f );
      }

      break;
    }
  }

  // Animate the backing, if enabled.
  // This is set up last so that different animation modes can have an effect on the backing animation speed.
  if( mBackingEnabled )
  {
    if( duration > Math::MACHINE_EPSILON_0 )
    {
      if( transitionIn )
      {
        mAnimation.AnimateTo( Property( mBacking, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN_OUT, TimePeriod( 0.0f, duration * 0.70f ) );
      }
      else
      {
        mAnimation.AnimateTo( Property( mBacking, Actor::Property::COLOR_ALPHA ), 0.0f, AlphaFunction::EASE_IN_OUT, TimePeriod( 0.30f, duration * 0.70f ) );
      }
    }
    else
    {
      mBacking.SetOpacity( transitionIn ? 1.0f : 0.0f );
    }
  }

  // If we are performing the animation instantaneously, jump to the position directly and do not signal.
  if( instantaneous )
  {
    mAnimation.SetCurrentProgress( 1.0f );
    mAnimation.Play();
  }
  else if( duration > Math::MACHINE_EPSILON_0 )
  {
    // Run the animation.
    mAnimation.FinishedSignal().Connect( this, &Popup::OnDisplayChangeAnimationFinished );
    mAnimation.Play();
  }
  else
  {
    // We did not use an animation to achive the transition.
    // Trigger the state change directly.
    DisplayStateChangeComplete();
  }
}

void Popup::OnDisplayChangeAnimationFinished( Animation& source )
{
  DisplayStateChangeComplete();
}

void Popup::DisplayStateChangeComplete()
{
  // Remove contents from stage if completely hidden.
  if( mDisplayState == Toolkit::Popup::HIDING )
  {
    mDisplayState = Toolkit::Popup::HIDDEN;

    mLayer.SetVisible( false );
    mPopupLayout.SetSensitive( false );

    // Guard against destruction during signal emission.
    Toolkit::Popup handle( GetOwner() );
    mHiddenSignal.Emit();
  }
  else if( mDisplayState == Toolkit::Popup::SHOWING )
  {
    mDisplayState = Toolkit::Popup::SHOWN;
    Toolkit::Popup handle( GetOwner() );
    mShownSignal.Emit();

    // Start a timer to auto-hide if enabled.
    if( mAutoHideDelay > 0u )
    {
      mAutoHideTimer = Timer::New( mAutoHideDelay );
      mAutoHideTimer.TickSignal().Connect( this, &Popup::OnAutoHideTimeReached );
      mAutoHideTimer.Start();
    }
  }
}

bool Popup::OnAutoHideTimeReached()
{
  // Display timer has expired, auto hide the popup exactly as if the user had clicked outside.
  SetDisplayState( Toolkit::Popup::HIDDEN );

  if( mAutoHideTimer )
  {
    mAutoHideTimer.Stop();
    mAutoHideTimer.TickSignal().Disconnect( this, &Popup::OnAutoHideTimeReached );
    mAutoHideTimer.Reset();
  }
  return true;
}

void Popup::SetPopupBackgroundImage( Actor image )
{
  // Removes any previous background.
  if( mPopupBackgroundImage )
  {
    mPopupContainer.Remove( mPopupBackgroundImage );
  }

  // Adds new background to the dialog.
  mPopupBackgroundImage = image;
  mPopupBackgroundImage.SetName( "popup-background-image" );
  mPopupBackgroundImage.SetAnchorPoint( AnchorPoint::CENTER );
  mPopupBackgroundImage.SetParentOrigin( ParentOrigin::CENTER );

  // OnDialogTouched only consumes the event. It prevents the touch event to be caught by the backing.
  mPopupBackgroundImage.TouchedSignal().Connect( this, &Popup::OnDialogTouched );

  // Set the popup border to be slightly larger than the layout contents.
  mPopupBackgroundImage.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  mPopupBackgroundImage.SetSizeModeFactor( BACKGROUND_OUTER_BORDER );

  const bool prevAlter = mAlterAddedChild;
  mAlterAddedChild = false;
  mPopupContainer.Add( mPopupBackgroundImage );
  mAlterAddedChild = prevAlter;

  mLayoutDirty = true;
}

Actor Popup::GetPopupBackgroundImage() const
{
  return mPopupBackgroundImage;
}

void Popup::SetTitle( Actor titleActor )
{
  // Replaces the current title actor.
  if( !mPopupLayout )
  {
    return;
  }

  if( mTitle )
  {
    mPopupLayout.RemoveChildAt( Toolkit::TableView::CellPosition( 0, 0) );
  }
  mTitle = titleActor;

  if( mTitle )
  {
    // Set up padding to give sensible default behaviour
    // (an application developer can later override this if they wish).
    mTitle.SetPadding( DEFAULT_TITLE_PADDING );

    mPopupLayout.AddChild( mTitle, Toolkit::TableView::CellPosition( 0, 0 ) );
  }

  mLayoutDirty = true;
  RelayoutRequest();
}

Actor Popup::GetTitle() const
{
  return mTitle;
}

void Popup::SetContent( Actor content )
{
  // Remove previous content actor.
  if( mPopupLayout )
  {
    mPopupLayout.RemoveChildAt( Toolkit::TableView::CellPosition( 1, 0 ) );
  }
   // Keep a handle to the new content.
  mContent = content;

  if( mContent )
  {
    mContent.SetName( "popup-content" );

    mPopupLayout.AddChild( mContent, Toolkit::TableView::CellPosition( 1, 0 ) );
  }

  mLayoutDirty = true;
  RelayoutRequest();
}

Actor Popup::GetContent() const
{
  return mContent;
}

void Popup::SetFooter( Actor footer )
{
  // Remove previous content actor.
  if( mPopupLayout )
  {
    mPopupLayout.RemoveChildAt( Toolkit::TableView::CellPosition( 2, 0 ) );
  }

  // Keep a handle to the new content.
  mFooter = footer;

  if( mFooter )
  {
    mFooter.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

    // The control container has a fixed height.
    mPopupLayout.SetFitHeight( 2u );
    mPopupLayout.AddChild( footer, Toolkit::TableView::CellPosition( 2, 0 ) );
  }

  mLayoutDirty = true;
  RelayoutRequest();
}

Actor Popup::GetFooter() const
{
  return mFooter;
}

void Popup::SetDisplayState( Toolkit::Popup::DisplayState displayState )
{
  // Convert the 4-way state to a bool, true for show, false for hide.
  bool display = ( displayState == Toolkit::Popup::SHOWING ) || ( displayState == Toolkit::Popup::SHOWN );

  // Ignore if we are already at the target display state.
  if( display == ( ( mDisplayState == Toolkit::Popup::SHOWING ) || ( mDisplayState == Toolkit::Popup::SHOWN ) ) )
  {
    return;
  }

  // Convert the bool state to the actual display state to use.
  mDisplayState = display ? Toolkit::Popup::SHOWING : Toolkit::Popup::HIDING;

  if ( display )
  {
    // Update the state to indicate the current intent.
    mDisplayState = Toolkit::Popup::SHOWING;

    // We are displaying so bring the popup layer to the front, and set it visible so it is rendered.
    mLayer.RaiseToTop();
    mLayer.SetVisible( true );

    // Set up the layout if this is the first display or the layout has become dirty.
    if( mLayoutDirty )
    {
      // Bake-in any style and layout options to create the Popup layout.
      LayoutPopup();
    }

    // Allow the popup to catch events.
    mPopupLayout.SetSensitive( true );
    SetKeyInputFocus();

    // Handle the keyboard focus when popup is shown.
    Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
    if( keyboardFocusManager )
    {
      mPreviousFocusedActor = keyboardFocusManager.GetCurrentFocusActor();

      if( mContent && mContent.IsKeyboardFocusable() )
      {
        // If content is focusable, move the focus to content.
        keyboardFocusManager.SetCurrentFocusActor( mContent );
      }
      else
      {
        DALI_LOG_WARNING( "There is no focusable in popup\n" );
      }
    }
  }
  else // Not visible.
  {
    mDisplayState = Toolkit::Popup::HIDING;
    ClearKeyInputFocus();

    // Restore the keyboard focus when popup is hidden.
    if( mPreviousFocusedActor && mPreviousFocusedActor.IsKeyboardFocusable() )
    {
      Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
      if( keyboardFocusManager )
      {
        keyboardFocusManager.SetCurrentFocusActor( mPreviousFocusedActor );
      }
    }
  }

  // Perform animation.
  StartTransitionAnimation( display );
}

Toolkit::Popup::DisplayState Popup::GetDisplayState() const
{
  return mDisplayState;
}

void Popup::LayoutPopup()
{
  mLayoutDirty = false;

  /* When animating in, we want to respect the origin applied to Self().
   * For example, if zooming, not only will the final result be anchored to the
   * selected point, but the zoom will originate from this point also.
   *
   * EG: ParentOrigin::TOP_LEFT, AnchorPoint::TOP_LEFT :
   *
   *       --------                --------
   *       |X|                     |XXX|
   *       |``        Animates     |XXX|
   *       |             to:       |XXX|
   *       |                       |````
   *       |                       |
   */
  mPopupContainer.SetParentOrigin( Self().GetCurrentParentOrigin() );
  mPopupContainer.SetAnchorPoint( Self().GetCurrentAnchorPoint() );

  // If there is only a title, use less padding.
  if( mTitle )
  {
    if( !mContent && !mFooter )
    {
      mTitle.SetPadding( DEFAULT_TITLE_ONLY_PADDING );
    }
    else
    {
      mTitle.SetPadding( DEFAULT_TITLE_PADDING );
    }
  }

  // Allow derived classes to perform any layout they may need to do.
  OnLayoutSetup();

  // Update background visibility.
  mPopupContainer.SetVisible( !( !mFooter && mPopupLayout.GetChildCount() == 0 ) );

  // Create / destroy / position the tail as needed.
  LayoutTail();

  // Setup any layout and initialisation required for the selected animation.
  LayoutAnimation();

  RelayoutRequest();
}

void Popup::LayoutTail()
{
  // Removes the tail actor.
  if( mTailImage && mTailImage.GetParent() )
  {
    mTailImage.GetParent().Remove( mTailImage );
    mTailImage.Reset();
  }

  if( !mTailVisible )
  {
    return;
  }

  const Vector3& position = GetTailPosition();
  std::string image;

  // depending on position of tail around ParentOrigin, a different tail image is used...
  if( position.y < Math::MACHINE_EPSILON_1 )
  {
    image = mTailUpImage;
  }
  else if( position.y > 1.0f - Math::MACHINE_EPSILON_1 )
  {
    image = mTailDownImage;
  }
  else if( position.x < Math::MACHINE_EPSILON_1 )
  {
    image = mTailLeftImage;
  }
  else if( position.x > 1.0f - Math::MACHINE_EPSILON_1 )
  {
    image = mTailRightImage;
  }

  if( !image.empty() )
  {
    // Adds the tail actor.
    Image tail = ResourceImage::New( image );
    mTailImage = ImageActor::New( tail );
    mTailImage.SetName( "tail-image" );
    const Vector3 anchorPoint = AnchorPoint::BOTTOM_RIGHT - position;
    mTailImage.SetParentOrigin( position );
    mTailImage.SetAnchorPoint( anchorPoint );

    mLayer.Add( mTailImage );
  }
}

void Popup::SetContextualMode( Toolkit::Popup::ContextualMode mode )
{
  mContextualMode = mode;
  mLayoutDirty = true;
}

Toolkit::Popup::ContextualMode Popup::GetContextualMode() const
{
  return mContextualMode;
}

ImageActor Popup::CreateBacking()
{
  mBacking = Dali::Toolkit::CreateSolidColorActor( mBackingColor );
  mBacking.SetName( "popup-backing" );

  // Must always be positioned top-left of stage, regardless of parent.
  mBacking.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
  mBacking.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  // Always the full size of the stage.
  mBacking.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  mBacking.SetSize( Stage::GetCurrent().GetSize() );

  // Catch events.
  mBacking.SetSensitive( true );

  // Default to being transparent.
  mBacking.SetOpacity( 0.0f );
  mBacking.TouchedSignal().Connect( this, &Popup::OnBackingTouched );
  mBacking.WheelEventSignal().Connect( this, &Popup::OnBackingWheelEvent );
  return mBacking;
}

Toolkit::Popup::TouchedOutsideSignalType& Popup::OutsideTouchedSignal()
{
  return mTouchedOutsideSignal;
}

Toolkit::Popup::DisplayStateChangeSignalType& Popup::ShowingSignal()
{
  return mShowingSignal;
}

Toolkit::Popup::DisplayStateChangeSignalType& Popup::ShownSignal()
{
  return mShownSignal;
}

Toolkit::Popup::DisplayStateChangeSignalType& Popup::HidingSignal()
{
  return mHidingSignal;
}

Toolkit::Popup::DisplayStateChangeSignalType& Popup::HiddenSignal()
{
  return mHiddenSignal;
}

void Popup::SetTailVisibility( bool visible )
{
  mTailVisible = visible;
  mLayoutDirty = true;
}

const bool Popup::IsTailVisible() const
{
  return mTailVisible;
}

void Popup::SetTailPosition( Vector3 position )
{
  mTailPosition = position;
  mLayoutDirty = true;
}

const Vector3& Popup::GetTailPosition() const
{
  return mTailPosition;
}

void Popup::SetAnimationDuration( float duration )
{
  mAnimationDuration = duration;
  mLayoutDirty = true;
}

float Popup::GetAnimationDuration() const
{
  return mAnimationDuration;
}

void Popup::SetAnimationMode( Toolkit::Popup::AnimationMode animationMode )
{
  mAnimationMode = animationMode;
  mLayoutDirty = true;
}

Toolkit::Popup::AnimationMode Popup::GetAnimationMode() const
{
  return mAnimationMode;
}

void Popup::SetEntryAnimationData( const Property::Map& map )
{
  mEntryAnimationData.Clear();
  Scripting::NewAnimation( map, mEntryAnimationData );
}

void Popup::SetExitAnimationData( const Property::Map& map )
{
  mExitAnimationData.Clear();
  Scripting::NewAnimation( map, mExitAnimationData );
}

void Popup::SetAutoHideDelay( int delay )
{
  mAutoHideDelay = delay;
}

int Popup::GetAutoHideDelay() const
{
  return mAutoHideDelay;
}

void Popup::SetBackingEnabled( bool enabled )
{
  mBackingEnabled = enabled;
  mLayoutDirty = true;
}

const bool Popup::IsBackingEnabled() const
{
  return mBackingEnabled;
}

void Popup::SetBackingColor( Vector4 color )
{
  mBackingColor = color;
  mLayoutDirty = true;
}

const Vector4& Popup::GetBackingColor() const
{
  return mBackingColor;
}

void Popup::SetTailUpImage( std::string image )
{
  mTailUpImage = image;
  mLayoutDirty = true;
}

const std::string& Popup::GetTailUpImage() const
{
  return mTailUpImage;
}

void Popup::SetTailDownImage( std::string image )
{
  mTailDownImage = image;
  mLayoutDirty = true;
}

const std::string& Popup::GetTailDownImage() const
{
  return mTailDownImage;
}

void Popup::SetTailLeftImage( std::string image )
{
  mTailLeftImage = image;
  mLayoutDirty = true;
}

const std::string& Popup::GetTailLeftImage() const
{
  return mTailLeftImage;
}

void Popup::SetTailRightImage( std::string image )
{
  mTailRightImage = image;
  mLayoutDirty = true;
}

const std::string& Popup::GetTailRightImage() const
{
  return mTailRightImage;
}

void Popup::SetTouchTransparent( bool enabled )
{
  mTouchTransparent = enabled;
}

const bool Popup::IsTouchTransparent() const
{
  return mTouchTransparent;
}

void Popup::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::Popup popup = Toolkit::Popup::DownCast( Dali::BaseHandle( object ) );

  if ( popup )
  {
    Popup& popupImpl( GetImpl( popup ) );

    switch ( propertyIndex )
    {
      case Toolkit::Popup::Property::TITLE:
      {
        Property::Map valueMap;
        if( value.Get( valueMap ) )
        {
          popupImpl.SetTitle( Scripting::NewActor( valueMap ) );
        }
        break;
      }
      case Toolkit::Popup::Property::CONTENT:
      {
        Property::Map valueMap;
        if( value.Get( valueMap ) )
        {
          popupImpl.SetContent( Scripting::NewActor( valueMap ) );
        }
        break;
      }
      case Toolkit::Popup::Property::FOOTER:
      {
        Property::Map valueMap;
        if( value.Get( valueMap ) )
        {
          popupImpl.SetFooter( Scripting::NewActor( valueMap ) );
        }
        break;
      }
      case Toolkit::Popup::Property::DISPLAY_STATE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          Toolkit::Popup::DisplayState displayState( Toolkit::Popup::HIDDEN );
          if( Scripting::GetEnumeration< Toolkit::Popup::DisplayState >( valueString.c_str(), DisplayStateTable, DisplayStateTableCount, displayState ) )
          {
            popupImpl.SetDisplayState( displayState );
          }
        }
        break;
      }
      case Toolkit::Popup::Property::TOUCH_TRANSPARENT:
      {
        bool valueBool;
        if( value.Get( valueBool ) )
        {
          popupImpl.SetTouchTransparent( valueBool );
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_VISIBILITY:
      {
        bool valueBool;
        if( value.Get( valueBool ) )
        {
          popupImpl.SetTailVisibility( valueBool );
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_POSITION:
      {
        Vector3 valueVector3;
        if( value.Get( valueVector3 ) )
        {
          popupImpl.SetTailPosition( valueVector3 );
        }
        break;
      }
      case Toolkit::Popup::Property::CONTEXTUAL_MODE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          Toolkit::Popup::ContextualMode contextualMode( Toolkit::Popup::BELOW );
          if( Scripting::GetEnumeration< Toolkit::Popup::ContextualMode >( valueString.c_str(), ContextualModeTable, ContextualModeTableCount, contextualMode ) )
          {
            popupImpl.SetContextualMode( contextualMode );
          }
        }
        break;
      }
      case Toolkit::Popup::Property::ANIMATION_DURATION:
      {
        float valueFloat;
        if( value.Get( valueFloat ) )
        {
          popupImpl.SetAnimationDuration( valueFloat );
        }
        break;
      }
      case Toolkit::Popup::Property::ANIMATION_MODE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          Toolkit::Popup::AnimationMode animationMode( Toolkit::Popup::FADE );
          if( Scripting::GetEnumeration< Toolkit::Popup::AnimationMode >( valueString.c_str(), AnimationModeTable, AnimationModeTableCount, animationMode ) )
          {
            popupImpl.SetAnimationMode( animationMode );
          }
        }
        break;
      }
      case Toolkit::Popup::Property::ENTRY_ANIMATION:
      {
        Property::Map valueMap;
        if( value.Get( valueMap ) )
        {
          popupImpl.SetEntryAnimationData( valueMap );
        }
        break;
      }
      case Toolkit::Popup::Property::EXIT_ANIMATION:
      {
        Property::Map valueMap;
        if( value.Get( valueMap ) )
        {
          popupImpl.SetExitAnimationData( valueMap );
        }
        break;
      }
      case Toolkit::Popup::Property::AUTO_HIDE_DELAY:
      {
        int valueInt;
        if( value.Get( valueInt ) )
        {
          popupImpl.SetAutoHideDelay( valueInt );
        }
        break;
      }
      case Toolkit::Popup::Property::BACKING_ENABLED:
      {
        bool valueBool;
        if( value.Get( valueBool ) )
        {
          popupImpl.SetBackingEnabled( valueBool );
        }
        break;
      }
      case Toolkit::Popup::Property::BACKING_COLOR:
      {
        Vector4 valueVector4;
        if( value.Get( valueVector4 ) )
        {
          popupImpl.SetBackingColor( valueVector4 );
        }
        break;
      }
      case Toolkit::Popup::Property::POPUP_BACKGROUND_IMAGE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          Image image = ResourceImage::New( valueString );
          if( image )
          {
            ImageActor actor = ImageActor::New( image );
            popupImpl.SetPopupBackgroundImage( actor );
          }
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_UP_IMAGE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          popupImpl.SetTailUpImage( valueString );
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_DOWN_IMAGE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          popupImpl.SetTailDownImage( valueString );
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_LEFT_IMAGE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          popupImpl.SetTailLeftImage( valueString );
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_RIGHT_IMAGE:
      {
        std::string valueString;
        if( value.Get( valueString ) )
        {
          popupImpl.SetTailRightImage( valueString );
        }
        break;
      }
    }
  }
}

Property::Value Popup::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::Popup popup = Toolkit::Popup::DownCast( Dali::BaseHandle( object ) );

  if ( popup )
  {
    Popup& popupImpl( GetImpl( popup ) );

    switch ( propertyIndex )
    {
      case Toolkit::Popup::Property::TITLE:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( popupImpl.GetTitle(), map );
        value = map;
        break;
      }
      case Toolkit::Popup::Property::CONTENT:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( popupImpl.GetContent(), map );
        value = map;
        break;
      }
      case Toolkit::Popup::Property::FOOTER:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( popupImpl.GetFooter(), map );
        value = map;
        break;
      }
      case Toolkit::Popup::Property::DISPLAY_STATE:
      {
        value = Scripting::GetLinearEnumerationName< Toolkit::Popup::DisplayState >( popupImpl.GetDisplayState(), DisplayStateTable, DisplayStateTableCount );
        break;
      }
      case Toolkit::Popup::Property::TOUCH_TRANSPARENT:
      {
        value = popupImpl.IsTouchTransparent();
        break;
      }
      case Toolkit::Popup::Property::TAIL_VISIBILITY:
      {
        value = popupImpl.IsTailVisible();
        break;
      }
      case Toolkit::Popup::Property::TAIL_POSITION:
      {
        value = popupImpl.GetTailPosition();
        break;
      }
      case Toolkit::Popup::Property::CONTEXTUAL_MODE:
      {
        value = Scripting::GetLinearEnumerationName< Toolkit::Popup::ContextualMode >( popupImpl.GetContextualMode(), ContextualModeTable, ContextualModeTableCount );
        break;
      }
      case Toolkit::Popup::Property::ANIMATION_DURATION:
      {
        value = popupImpl.GetAnimationDuration();
        break;
      }
      case Toolkit::Popup::Property::ANIMATION_MODE:
      {
        value = Scripting::GetLinearEnumerationName< Toolkit::Popup::AnimationMode >( popupImpl.GetAnimationMode(), AnimationModeTable, AnimationModeTableCount );
        break;
      }
      case Toolkit::Popup::Property::ENTRY_ANIMATION:
      {
        // Note: Cannot retrieve property map from animation.
        Property::Map map;
        value = map;
        break;
      }
      case Toolkit::Popup::Property::EXIT_ANIMATION:
      {
        // Note: Cannot retrieve property map from animation.
        Property::Map map;
        value = map;
        break;
      }
      case Toolkit::Popup::Property::AUTO_HIDE_DELAY:
      {
        value = popupImpl.GetAutoHideDelay();
        break;
      }
      case Toolkit::Popup::Property::BACKING_ENABLED:
      {
        value = popupImpl.IsBackingEnabled();
        break;
      }
      case Toolkit::Popup::Property::BACKING_COLOR:
      {
        value = popupImpl.GetBackingColor();
        break;
      }
      case Toolkit::Popup::Property::POPUP_BACKGROUND_IMAGE:
      {
        ResourceImage image = ResourceImage::DownCast( popupImpl.GetPopupBackgroundImage() );
        if( image )
        {
          value = image.GetUrl();
        }
        break;
      }
      case Toolkit::Popup::Property::TAIL_UP_IMAGE:
      {
        value = popupImpl.GetTailUpImage();
        break;
      }
      case Toolkit::Popup::Property::TAIL_DOWN_IMAGE:
      {
        value = popupImpl.GetTailDownImage();
        break;
      }
      case Toolkit::Popup::Property::TAIL_LEFT_IMAGE:
      {
        value = popupImpl.GetTailLeftImage();
        break;
      }
      case Toolkit::Popup::Property::TAIL_RIGHT_IMAGE:
      {
        value = popupImpl.GetTailRightImage();
        break;
      }
    }
  }

  return value;
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
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_SHOWING ) )
  {
    popup.ShowingSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_SHOWN ) )
  {
    popup.ShownSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_HIDING ) )
  {
    popup.HidingSignal().Connect( tracker, functor );
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

bool Popup::OnBackingTouched( Actor actor, const TouchEvent& event )
{
  // Allow events to pass through if touch transparency is enabled.
  if( mTouchTransparent )
  {
    return false;
  }

  if( event.GetPointCount() > 0 )
  {
    const TouchPoint& point = event.GetPoint( 0 );

    if( point.state == TouchPoint::Down )
    {
      // Guard against destruction during signal emission.
      Toolkit::Popup handle( GetOwner() );

      mTouchedOutsideSignal.Emit();
    }
  }

  // Block anything behind backing becoming touched.
  mLayer.SetTouchConsumed( true );
  return true;
}

bool Popup::OnBackingWheelEvent( Actor actor, const WheelEvent& event )
{
  // Allow events to pass through if touch transparency is enabled.
  if( mTouchTransparent )
  {
    return false;
  }

  // Consume wheel event in dimmed backing actor.
  mLayer.SetTouchConsumed( true );
  return true;
}

bool Popup::OnDialogTouched(Actor actor, const TouchEvent& event)
{
  // Allow events to pass through if touch transparency is enabled.
  if( mTouchTransparent )
  {
    return false;
  }

  // Consume event (stops backing actor receiving touch events)
  mLayer.SetTouchConsumed( true );
  return true;
}

void Popup::OnControlStageConnection()
{
  mLayoutDirty = true;
  RelayoutRequest();
}

void Popup::OnControlChildAdd( Actor& child )
{
  // Re-parent any children added by user to the body layer.
  if( mAlterAddedChild )
  {
    SetContent( child );
  }
  else
  {
    mLayoutDirty = true;
    RelayoutRequest();
  }
}

void Popup::LayoutContext( const Vector2& size )
{
  // Do nothing if not in a contextual mode (or there is no parent context).
  Actor self = Self();
  Actor parent = self.GetParent();
  if( ( mContextualMode == Toolkit::Popup::NON_CONTEXTUAL ) || !parent )
  {
    return;
  }

  mPopupContainer.SetParentOrigin( ParentOrigin::CENTER );
  // We always anchor to the CENTER, rather than a different anchor point for each contextual
  // mode to allow code-reuse of the bound checking code (for maintainability).
  mPopupContainer.SetAnchorPoint( AnchorPoint::CENTER );

  // Setup with some pre-calculations for speed.
  Vector3 halfStageSize( Stage().GetCurrent().GetSize() / 2.0f );
  Vector3 parentPosition( parent.GetCurrentPosition() );
  Vector2 halfSize( size / 2.0f );
  Vector2 halfParentSize( parent.GetRelayoutSize( Dimension::WIDTH ) / 2.0f, parent.GetRelayoutSize( Dimension::HEIGHT ) / 2.0f );
  Vector3 newPosition( Vector3::ZERO );

  // Perform different positioning based on the specified contextual layout mode.
  switch( mContextualMode )
  {
    case Toolkit::Popup::BELOW:
    {
      newPosition.x += halfSize.x - halfParentSize.x;
      newPosition.y += halfSize.y + halfParentSize.y + DEFAULT_CONTEXTUAL_ADJACENCY_MARGIN.y;
      break;
    }
    case Toolkit::Popup::ABOVE:
    {
      newPosition.x += halfSize.x - halfParentSize.x;
      newPosition.y -= halfSize.y + halfParentSize.y + DEFAULT_CONTEXTUAL_ADJACENCY_MARGIN.y;
      break;
    }
    case Toolkit::Popup::RIGHT:
    {
      newPosition.x += halfSize.x + halfParentSize.x + DEFAULT_CONTEXTUAL_ADJACENCY_MARGIN.x;
      newPosition.y += halfSize.y - halfParentSize.y;
      break;
    }
    case Toolkit::Popup::LEFT:
    {
      newPosition.x -= halfSize.x + halfParentSize.x + DEFAULT_CONTEXTUAL_ADJACENCY_MARGIN.x;
      newPosition.y += halfSize.y - halfParentSize.y;
      break;
    }
    case Toolkit::Popup::NON_CONTEXTUAL:
    {
      // Code won't reach here (caught earlier).
      break;
    }
  }

  // On-screen position checking.
  // Check new position is not too far right. If so, correct it.
  // Note: Check for right rather than left first, so if popup is too wide, the left check overrides
  // the right check and we at least see the left portion of the popup (as this is more useful).
  if( newPosition.x >= ( halfStageSize.x - parentPosition.x - halfSize.x - DEFAULT_CONTEXTUAL_STAGE_BORDER.x ) )
  {
    newPosition.x = halfStageSize.x - parentPosition.x - halfSize.x - DEFAULT_CONTEXTUAL_STAGE_BORDER.x;
  }
  // Check new position is not too far left. If so, correct it.
  if( newPosition.x < halfSize.x - ( parentPosition.x + halfStageSize.x ) + DEFAULT_CONTEXTUAL_STAGE_BORDER.x )
  {
    newPosition.x = halfSize.x - ( parentPosition.x + halfStageSize.x ) + DEFAULT_CONTEXTUAL_STAGE_BORDER.x;// - parentSize.x;
  }
  // Check new position is not too far down. If so, correct it.
  if( newPosition.y >= ( halfStageSize.y - parentPosition.y - halfSize.y - DEFAULT_CONTEXTUAL_STAGE_BORDER.y ) )
  {
    newPosition.y = halfStageSize.y - parentPosition.y - halfSize.y - DEFAULT_CONTEXTUAL_STAGE_BORDER.y;
  }
  // Check new position is not too far up. If so, correct it.
  if( newPosition.y < halfSize.y - ( parentPosition.y + halfStageSize.y ) + DEFAULT_CONTEXTUAL_STAGE_BORDER.y )
  {
    newPosition.y = halfSize.y - ( parentPosition.y + halfStageSize.y ) + DEFAULT_CONTEXTUAL_STAGE_BORDER.y;
  }

  // Set the final position.
  mPopupContainer.SetPosition( newPosition );
}

void Popup::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  Vector2 useSize( size );

  // Use the Popup layouts size, unless requested to use a fixed size.
  // In which case take the size set for the Popup itself.
  ResizePolicy::Type widthPolicy = Self().GetResizePolicy( Dimension::WIDTH );
  ResizePolicy::Type heightPolicy = Self().GetResizePolicy( Dimension::HEIGHT );

  // Width calculations:
  if( widthPolicy == ResizePolicy::USE_NATURAL_SIZE || widthPolicy == ResizePolicy::FIT_TO_CHILDREN )
  {
    // If we using a child-based policy, take the size from the popup layout.
    mPopupLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH );
    useSize.width = mPopupLayout.GetRelayoutSize( Dimension::WIDTH );

    mPopupLayout.SetFitWidth( 0u );
  }
  else
  {
    // If we using a parent-based policy, take the size from the popup object itself (self).
    mPopupLayout.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::WIDTH );

    mPopupLayout.SetFixedWidth( 0u, useSize.width );
  }

  // Height calculations:
  // Title: Let the title be as high as it needs to be.
  mPopupLayout.SetFitHeight( 0u );

  // Footer: Convert the footer's resize policy to a TableView row policy.
  if( mFooter )
  {
    ResizePolicy::Type footerHeightPolicy = mFooter.GetResizePolicy( Dimension::HEIGHT );
    if( ( footerHeightPolicy == ResizePolicy::USE_NATURAL_SIZE ) ||
        ( footerHeightPolicy == ResizePolicy::FIT_TO_CHILDREN ) )
    {
      mPopupLayout.SetFitHeight( 2u );
    }
    else if( footerHeightPolicy == ResizePolicy::FIXED )
    {
      mPopupLayout.SetFixedHeight( 2u, mFooter.GetRelayoutSize( Dimension::HEIGHT) );
    }
    else
    {
      mPopupLayout.SetRelativeHeight( 2u, 1.0f );
    }
  }
  else
  {
    mPopupLayout.SetFixedHeight( 2u, 0.0f );
  }

  // Popup contents: Adjust the tableview's policies based on the popup's policies.
  if( heightPolicy == ResizePolicy::USE_NATURAL_SIZE || heightPolicy == ResizePolicy::FIT_TO_CHILDREN )
  {
    mPopupLayout.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );

    // Let both the contents expand as necessary.
    mPopupLayout.SetFitHeight( 1u );
    useSize.height = mPopupLayout.GetRelayoutSize( Dimension::HEIGHT );
  }
  else
  {
    mPopupLayout.SetResizePolicy( heightPolicy, Dimension::HEIGHT );

    // Let the content expand to fill the remaining space.
    mPopupLayout.SetRelativeHeight( 1u, 1.0f );
    mPopupLayout.SetResizePolicy( ResizePolicy::USE_ASSIGNED_SIZE, Dimension::HEIGHT );
  }

  // Relayout the popup-layout to give it it's new size this frame.
  container.Add( mPopupLayout, useSize );

  if( mContent )
  {
    container.Add( mContent, Vector2( mContent.GetRelayoutSize( Dimension::WIDTH ), mContent.GetRelayoutSize( Dimension::HEIGHT ) ) );
  }

  // Perform contextual layout setup if required.
  // This is done each time in case the parent moves.
  // This will have no effect if no contextual mode is selected.
  LayoutContext( useSize );
}

void Popup::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
  // To get the popup to emulate fit-to-children, we need to actually set use-natural-size.
  if( ( dimension & Dimension::HEIGHT ) && ( policy == ResizePolicy::FIT_TO_CHILDREN ) )
  {
    Self().SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  }

  mLayoutDirty = true;
  return;
}

Vector3 Popup::GetNaturalSize()
{
  return mPopupLayout.GetNaturalSize();
}

float Popup::GetHeightForWidth( float width )
{
  return mPopupLayout.GetHeightForWidth( width );
}

float Popup::GetWidthForHeight( float height )
{
  return mPopupLayout.GetWidthForHeight( height );
}

bool Popup::OnKeyEvent( const KeyEvent& event )
{
  // Allow events to pass through if touch transparency is enabled.
  if( mTouchTransparent )
  {
    return false;
  }

  bool consumed = false;

  if( event.state == KeyEvent::Down )
  {
    if (event.keyCode == Dali::DALI_KEY_ESCAPE || event.keyCode == Dali::DALI_KEY_BACK)
    {
      SetDisplayState( Toolkit::Popup::HIDDEN );
      consumed = true;
    }
  }

  return consumed;
}

Actor Popup::GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled )
{
  Actor nextFocusableActor( currentFocusedActor );

  // TODO: Needs to be optimised
  if( !currentFocusedActor || ( currentFocusedActor && KeyboardFocusManager::Get().GetFocusGroup( currentFocusedActor ) != Self() ) )
  {
    // The current focused actor is not within popup
    if( mContent && mContent.IsKeyboardFocusable() )
    {
      // If content is focusable, move the focus to content
      nextFocusableActor = mContent;
    }
  }
  else
  {
    // Rebuild the focus chain because controls or content can be added or removed dynamically
    std::vector< Actor > focusableActors;
    if( mContent && mContent.IsKeyboardFocusable() )
    {
      focusableActors.push_back( mContent );
    }

    for( std::vector< Actor >::iterator iter = focusableActors.begin(), end = focusableActors.end(); iter != end; ++iter )
    {
      if( currentFocusedActor == *iter )
      {
        switch( direction )
        {
          case Toolkit::Control::KeyboardFocus::LEFT:
          {
            if( iter == focusableActors.begin() )
            {
              nextFocusableActor = *( focusableActors.end() - 1 );
            }
            else
            {
              nextFocusableActor = *( iter - 1 );
            }
            break;
          }
          case Toolkit::Control::KeyboardFocus::RIGHT:
          {
            if( iter == focusableActors.end() - 1 )
            {
              nextFocusableActor = *( focusableActors.begin() );
            }
            else
            {
              nextFocusableActor = *( iter + 1 );
            }
            break;
          }

          case Toolkit::Control::KeyboardFocus::UP:
          {
            if( mContent && *iter == mContent )
            {
              nextFocusableActor = *( focusableActors.end() - 1 );
            }
            else
            {
              if( mContent && mContent.IsKeyboardFocusable() )
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

          case Toolkit::Control::KeyboardFocus::DOWN:
          {
            if( mContent && mContent.IsKeyboardFocusable() )
            {
              nextFocusableActor = mContent;
            }
            else
            {
              if( iter == focusableActors.end() - 1 )
              {
                nextFocusableActor = *( focusableActors.begin() );
              }
              else
              {
                nextFocusableActor = *( iter + 1 );
              }
            }
            break;
          }
        }

        if( !nextFocusableActor )
        {
          DALI_LOG_WARNING( "Can not decide next focusable actor\n" );
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
