/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <limits>
#include <stack>
#include <typeinfo>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

/**
 * @brief Replace the background visual if it's a color visual with the renderIfTransparent property set as required.
 * @param[in] controlImpl The control implementation
 * @param[in] renderIfTransaparent Whether we should render if the color is transparent
 */
void ChangeBackgroundColorVisual( Control& controlImpl, bool renderIfTransparent )
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );

  Toolkit::Visual::Base backgroundVisual = controlDataImpl.GetVisual( Toolkit::Control::Property::BACKGROUND );
  if( backgroundVisual )
  {
    Property::Map map;
    backgroundVisual.CreatePropertyMap( map );
    Property::Value* typeValue = map.Find( Toolkit::Visual::Property::TYPE );
    if( typeValue && typeValue->Get< int >() == Toolkit::Visual::COLOR )
    {
      // Only change it if it's a color visual
      map[ Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT ] = renderIfTransparent;
      controlImpl.SetBackground( map );
    }
  }
}

/**
 * @brief Creates a clipping renderer if required.
 * (EG. If no renders exist and clipping is enabled).
 * @param[in] controlImpl The control implementation.
 */
void CreateClippingRenderer( Control& controlImpl )
{
  // We want to add a transparent background if we do not have one for clipping.
  Actor self( controlImpl.Self() );
  int clippingMode = ClippingMode::DISABLED;
  if( self.GetProperty( Actor::Property::CLIPPING_MODE ).Get( clippingMode ) )
  {
    switch( clippingMode )
    {
      case ClippingMode::CLIP_CHILDREN:
      {
        if( self.GetRendererCount() == 0u )
        {
          Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
          if( controlDataImpl.mVisuals.Empty() )
          {
            controlImpl.SetBackgroundColor( Color::TRANSPARENT );
          }
          else
          {
            // We have visuals, check if we've set the background and re-create it to
            // render even if transparent (only if it's a color visual)
            ChangeBackgroundColorVisual( controlImpl, true );
          }
        }
        break;
      }

      case ClippingMode::DISABLED:
      case ClippingMode::CLIP_TO_BOUNDING_BOX:
      {
        // If we have a background visual, check if it's a color visual and remove the render if transparent flag
        ChangeBackgroundColorVisual( controlImpl, false );
        break;
      }
    }
  }
}

/**
 * @brief Sets Control::Property::BACKGROUND visual
 * @param[in] controlImpl The control implementation
 * @param[in] visual The control background visual
 * @param[in] size The current size
 */
void SetBackgroundVisual( Control::Impl& controlImpl, Toolkit::Visual::Base& visual, const Vector2& size )
{
  Property::Map transformMap = Property::Map();

  Vector2 newSize( 0.f, 0.f );
  newSize.width = size.width + ( controlImpl.mPadding.start + controlImpl.mPadding.end );
  newSize.height = size.height + ( controlImpl.mPadding.top + controlImpl.mPadding.bottom );

  if( ( controlImpl.mMargin.start != 0 ) ||
      ( controlImpl.mMargin.end != 0 ) ||
      ( controlImpl.mMargin.top != 0 ) ||
      ( controlImpl.mMargin.bottom != 0 ) )
  {
    transformMap.Add( Toolkit::Visual::Transform::Property::SIZE, newSize )
                .Add( Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ) )
                .Add( Toolkit::Visual::Transform::Property::OFFSET, Vector2( controlImpl.mMargin.start, controlImpl.mMargin.top ) )
                .Add( Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ) )
                .Add( Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN )
                .Add( Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN );
  }
  else if( ( controlImpl.mPadding.start != 0 ) ||
           ( controlImpl.mPadding.end != 0 ) ||
           ( controlImpl.mPadding.top != 0 ) ||
           ( controlImpl.mPadding.bottom != 0 ) )
  {
    transformMap.Add( Toolkit::Visual::Transform::Property::SIZE, newSize )
                .Add( Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ) )
                .Add( Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN )
                .Add( Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN );
  }

  visual.SetTransformAndSize( transformMap, newSize ); // Send an empty map as we do not want to modify the visual's set transform
}

} // unnamed namespace


Toolkit::Control Control::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Control> controlImpl = new Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) );

  // Pass ownership to handle
  Toolkit::Control handle( *controlImpl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  controlImpl->Initialize();

  return handle;
}

void Control::SetStyleName( const std::string& styleName )
{
  if( styleName != mImpl->mStyleName )
  {
    mImpl->mStyleName = styleName;

    // Apply new style, if stylemanager is available
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if( styleManager )
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
    }
  }
}

const std::string& Control::GetStyleName() const
{
  return mImpl->mStyleName;
}

void Control::SetBackgroundColor( const Vector4& color )
{
  mImpl->mBackgroundColor = color;
  Property::Map map;
  map[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::COLOR;
  map[ Toolkit::ColorVisual::Property::MIX_COLOR ] = color;

  int clippingMode = ClippingMode::DISABLED;
  if( ( Self().GetProperty( Actor::Property::CLIPPING_MODE ).Get( clippingMode ) ) &&
      ( clippingMode == ClippingMode::CLIP_CHILDREN ) )
  {
    // If clipping-mode is set to CLIP_CHILDREN, then force visual to add the render even if transparent
    map[ Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT ] = true;
  }

  SetBackground( map );
}

Vector4 Control::GetBackgroundColor() const
{
  return mImpl->mBackgroundColor;
}

void Control::SetBackground( const Property::Map& map )
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual( map );
  visual.SetName("background");
  if( visual )
  {
    mImpl->RegisterVisual( Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND );

    // Trigger a size negotiation request that may be needed by the new visual to relayout its contents.
    RelayoutRequest();
  }
}

void Control::SetBackgroundImage( Image image )
{
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual( image );
  if( visual )
  {
    mImpl->RegisterVisual( Toolkit::Control::Property::BACKGROUND, visual, DepthIndex::BACKGROUND );
  }
}

void Control::ClearBackground()
{
   mImpl->UnregisterVisual( Toolkit::Control::Property::BACKGROUND );
   mImpl->mBackgroundColor = Color::TRANSPARENT;

   // Trigger a size negotiation request that may be needed when unregistering a visual.
   RelayoutRequest();
}

void Control::EnableGestureDetection(Gesture::Type type)
{
  if ( (type & Gesture::Pinch) && !mImpl->mPinchGestureDetector )
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::Pan) && !mImpl->mPanGestureDetector )
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Impl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::Tap) && !mImpl->mTapGestureDetector )
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Impl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if ( (type & Gesture::LongPress) && !mImpl->mLongPressGestureDetector )
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Impl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void Control::DisableGestureDetection(Gesture::Type type)
{
  if ( (type & Gesture::Pinch) && mImpl->mPinchGestureDetector )
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if ( (type & Gesture::Pan) && mImpl->mPanGestureDetector )
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if ( (type & Gesture::Tap) && mImpl->mTapGestureDetector )
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if ( (type & Gesture::LongPress) && mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector.Detach(Self());
    mImpl->mLongPressGestureDetector.Reset();
  }
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void Control::SetKeyboardNavigationSupport(bool isSupported)
{
  mImpl->mIsKeyboardNavigationSupported = isSupported;
}

bool Control::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void Control::SetKeyInputFocus()
{
  if( Self().OnStage() )
  {
    Toolkit::KeyInputFocusManager::Get().SetFocus(Toolkit::Control::DownCast(Self()));
  }
}

bool Control::HasKeyInputFocus()
{
  bool result = false;
  if( Self().OnStage() )
  {
    Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
    if( Self() == control )
    {
      result = true;
    }
  }
  return result;
}

void Control::ClearKeyInputFocus()
{
  if( Self().OnStage() )
  {
    Toolkit::KeyInputFocusManager::Get().RemoveFocus(Toolkit::Control::DownCast(Self()));
  }
}

void Control::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  mImpl->mIsKeyboardFocusGroup = isFocusGroup;

  // The following line will be removed when the deprecated API in KeyboardFocusManager is deleted
  Toolkit::KeyboardFocusManager::Get().SetAsFocusGroup(Self(), isFocusGroup);
}

bool Control::IsKeyboardFocusGroup()
{
  return Toolkit::KeyboardFocusManager::Get().IsFocusGroup(Self());
}

void Control::AccessibilityActivate()
{
  // Inform deriving classes
  OnAccessibilityActivated();
}

void Control::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool Control::OnAccessibilityActivated()
{
  return false; // Accessibility activation is not handled by default
}

bool Control::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
}

bool Control::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool Control::OnAccessibilityTouch(const TouchEvent& touchEvent)
{
  return false; // Accessibility touch event is not handled by default
}

bool Control::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool Control::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

Actor Control::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled)
{
  return Actor();
}

void Control::OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor)
{
}

Toolkit::Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return mImpl->mKeyInputFocusGainedSignal;
}

Toolkit::Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return mImpl->mKeyInputFocusLostSignal;
}

bool Control::EmitKeyEventSignal( const KeyEvent& event )
{
  // Guard against destruction during signal emission
  Dali::Toolkit::Control handle( GetOwner() );

  bool consumed = false;

  consumed = mImpl->FilterKeyEvent( event );

  // signals are allocated dynamically when someone connects
  if ( !consumed && !mImpl->mKeyEventSignal.Empty() )
  {
    consumed = mImpl->mKeyEventSignal.Emit( handle, event );
  }

  if ( !consumed )
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Control::Control( ControlBehaviour behaviourFlags )
: CustomActorImpl( static_cast< ActorFlags >( behaviourFlags ) ),
  mImpl(new Impl(*this))
{
  mImpl->mFlags = behaviourFlags;
}

Control::~Control()
{
  delete mImpl;
}

void Control::Initialize()
{
  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if( (mImpl->mFlags & REQUIRES_STYLE_CHANGE_SIGNALS) ||
      !(mImpl->mFlags & DISABLE_STYLE_CHANGE_SIGNALS) )
  {
    Toolkit::StyleManager styleManager = StyleManager::Get();

    // if stylemanager is available
    if( styleManager )
    {
      StyleManager& styleManagerImpl = GetImpl( styleManager );

      // Register for style changes
      styleManagerImpl.ControlStyleChangeSignal().Connect( this, &Control::OnStyleChange );

      // Apply the current style
      styleManagerImpl.ApplyThemeStyleAtInit( Toolkit::Control( GetOwner() ) );
    }
  }

  if( mImpl->mFlags & REQUIRES_KEYBOARD_NAVIGATION_SUPPORT )
  {
    SetKeyboardNavigationSupport( true );
  }
}

void Control::OnInitialize()
{
}

void Control::OnControlChildAdd( Actor& child )
{
}

void Control::OnControlChildRemove( Actor& child )
{
}

void Control::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  // By default the control is only interested in theme (not font) changes
  if( styleManager && change == StyleChange::THEME_CHANGE )
  {
    GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
    RelayoutRequest();
  }
}

void Control::OnPinch(const PinchGesture& pinch)
{
  if( !( mImpl->mStartingPinchScale ) )
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if( pinch.state == Gesture::Started )
  {
    *( mImpl->mStartingPinchScale ) = Self().GetCurrentScale();
  }

  Self().SetScale( *( mImpl->mStartingPinchScale ) * pinch.scale );
}

void Control::OnPan( const PanGesture& pan )
{
}

void Control::OnTap(const TapGesture& tap)
{
}

void Control::OnLongPress( const LongPressGesture& longPress )
{
}

void Control::EmitKeyInputFocusSignal( bool focusGained )
{
  Dali::Toolkit::Control handle( GetOwner() );

  if ( focusGained )
  {
    // signals are allocated dynamically when someone connects
    if ( !mImpl->mKeyInputFocusGainedSignal.Empty() )
    {
      mImpl->mKeyInputFocusGainedSignal.Emit( handle );
    }
  }
  else
  {
    // signals are allocated dynamically when someone connects
    if ( !mImpl->mKeyInputFocusLostSignal.Empty() )
    {
      mImpl->mKeyInputFocusLostSignal.Emit( handle );
    }
  }
}

void Control::OnStageConnection( int depth )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::OnStageConnection number of registered visuals(%d)\n",  mImpl->mVisuals.Size() );

  Actor self( Self() );

  for(RegisteredVisualContainer::Iterator iter = mImpl->mVisuals.Begin(); iter!= mImpl->mVisuals.End(); iter++)
  {
    // Check whether the visual is empty and enabled
    if( (*iter)->visual && (*iter)->enabled )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Control::OnStageConnection Setting visual(%d) on stage\n", (*iter)->index );
      Toolkit::GetImplementation((*iter)->visual).SetOnStage( self );
    }
  }

  // The clipping renderer is only created if required.
  CreateClippingRenderer( *this );
}

void Control::OnStageDisconnection()
{
  mImpl->OnStageDisconnection();
}

void Control::OnKeyInputFocusGained()
{
  EmitKeyInputFocusSignal( true );
}

void Control::OnKeyInputFocusLost()
{
  EmitKeyInputFocusSignal( false );
}

void Control::OnChildAdd(Actor& child)
{
  // Notify derived classes.
  OnControlChildAdd( child );
}

void Control::OnChildRemove(Actor& child)
{
  // Notify derived classes.
  OnControlChildRemove( child );
}

void Control::OnPropertySet( Property::Index index, Property::Value propertyValue )
{
  // If the clipping mode has been set, we may need to create a renderer.
  // Only do this if we are already on-stage as the OnStageConnection will handle the off-stage clipping controls.
  if( ( index == Actor::Property::CLIPPING_MODE ) && Self().OnStage() )
  {
    // Note: This method will handle whether creation of the renderer is required.
    CreateClippingRenderer( *this );
  }
}

void Control::OnSizeSet(const Vector3& targetSize)
{
  Toolkit::Visual::Base visual = mImpl->GetVisual( Toolkit::Control::Property::BACKGROUND );
  if( visual )
  {
    Vector2 size( targetSize );
    SetBackgroundVisual( *mImpl, visual, size );

  }
}

void Control::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

bool Control::OnTouchEvent(const TouchEvent& event)
{
  return false; // Do not consume
}

bool Control::OnHoverEvent(const HoverEvent& event)
{
  return false; // Do not consume
}

bool Control::OnKeyEvent(const KeyEvent& event)
{
  return false; // Do not consume
}

bool Control::OnWheelEvent(const WheelEvent& event)
{
  return false; // Do not consume
}

void Control::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  for( unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i )
  {
    Actor child = Self().GetChildAt( i );
    Vector2 newChildSize( size );

    // When set the padding or margin on the control, child should be resized and repositioned.
    if( ( mImpl->mMargin.start != 0 ) || ( mImpl->mMargin.end != 0 ) || ( mImpl->mMargin.top != 0 ) || ( mImpl->mMargin.bottom != 0 ) )
    {
      // Cannot use childs Position property as it can already have margin applied on it,
      // so we end up cumulatively applying them over and over again.
      Toolkit::Control childControl = Toolkit::Control::DownCast( child );

      // If control not a LayoutItem layout then must be the old Relayout algorithm hence account
      // for margins.
      if ( childControl && ! Toolkit::DevelControl::GetLayout( childControl ) )
      {
        Vector2 childOffset( 0.f, 0.f );
        childOffset.x += mImpl->mMargin.start;
        childOffset.y += mImpl->mMargin.top;
        child.SetPosition( childOffset.x, childOffset.y );
      }
    }

    container.Add( child, size );
  }

  Toolkit::Visual::Base visual = mImpl->GetVisual( Toolkit::Control::Property::BACKGROUND );
  if( visual )
  {
    SetBackgroundVisual( *mImpl, visual, size );
  }
}

void Control::OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
{
}

Vector3 Control::GetNaturalSize()
{
  Toolkit::Visual::Base visual = mImpl->GetVisual( Toolkit::Control::Property::BACKGROUND );
  if( visual )
  {
    Vector2 naturalSize;
    visual.GetNaturalSize( naturalSize );
    naturalSize.width += ( mImpl->mPadding.start + mImpl->mPadding.end );
    naturalSize.height += ( mImpl->mPadding.top + mImpl->mPadding.bottom );
    return Vector3( naturalSize );
  }
  return Vector3::ZERO;
}

float Control::CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
{
  return CalculateChildSizeBase( child, dimension );
}

float Control::GetHeightForWidth( float width )
{
  return GetHeightForWidthBase( width );
}

float Control::GetWidthForHeight( float height )
{
  return GetWidthForHeightBase( height );
}

bool Control::RelayoutDependentOnChildren( Dimension::Type dimension )
{
  return RelayoutDependentOnChildrenBase( dimension );
}

void Control::OnCalculateRelayoutSize( Dimension::Type dimension )
{
}

void Control::OnLayoutNegotiated( float size, Dimension::Type dimension )
{
}

void Control::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalConnected( slotObserver, callback );
}

void Control::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  mImpl->SignalDisconnected( slotObserver, callback );
}

Control& GetImplementation( Dali::Toolkit::Control& handle )
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  Control& impl = dynamic_cast< Internal::Control& >( customInterface );
  return impl;
}

const Control& GetImplementation( const Dali::Toolkit::Control& handle )
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const Control& impl = dynamic_cast< const Internal::Control& >( customInterface );
  return impl;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
