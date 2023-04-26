/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include "dummy-control.h"

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

namespace Dali
{

namespace Toolkit
{

DummyControl::DummyControl()
{
}

DummyControl::DummyControl(const DummyControl& control)
: Control( control )
{
}

DummyControl::~DummyControl()
{
}

DummyControl DummyControl::DownCast( BaseHandle handle )
{
  return Control::DownCast<DummyControl, DummyControlImpl>(handle);
}

DummyControl& DummyControl::operator=(const DummyControl& control)
{
  Control::operator=( control );
  return *this;
}

// Used to test signal connections
void DummyControlImpl::CustomSlot1( Actor actor )
{
  mCustomSlot1Called = true;
}

namespace {

BaseHandle Create()
{
  return DummyControlImpl::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::DummyControl, Toolkit::Control, Create );
DALI_TYPE_REGISTRATION_END()

Dali::PropertyRegistration dummyControlVisualProperty01(
  typeRegistration, "testVisual", Dali::Toolkit::DummyControl::Property::TEST_VISUAL, Dali::Property::MAP, &Dali::Toolkit::DummyControlImpl::SetProperty, &Dali::Toolkit::DummyControlImpl::GetProperty );

Dali::PropertyRegistration dummyControlVisualProperty02(
  typeRegistration, "testVisual2", Dali::Toolkit::DummyControl::Property::TEST_VISUAL2, Dali::Property::MAP, &Dali::Toolkit::DummyControlImpl::SetProperty, &Dali::Toolkit::DummyControlImpl::GetProperty );

Dali::PropertyRegistration dummyControlVisualProperty03(
  typeRegistration, "foregroundVisual", Dali::Toolkit::DummyControl::Property::FOREGROUND_VISUAL, Dali::Property::MAP, &Dali::Toolkit::DummyControlImpl::SetProperty, &Dali::Toolkit::DummyControlImpl::GetProperty );

Dali::PropertyRegistration dummyControlVisualProperty04(
  typeRegistration, "focusVisual", Dali::Toolkit::DummyControl::Property::FOCUS_VISUAL, Dali::Property::MAP, &Dali::Toolkit::DummyControlImpl::SetProperty, &Dali::Toolkit::DummyControlImpl::GetProperty );

Dali::PropertyRegistration dummyControlVisualProperty05(
  typeRegistration, "labelVisual", Dali::Toolkit::DummyControl::Property::LABEL_VISUAL, Dali::Property::MAP, &Dali::Toolkit::DummyControlImpl::SetProperty, &Dali::Toolkit::DummyControlImpl::GetProperty );

}

DummyControl DummyControlImpl::New()
{
  IntrusivePtr< DummyControlImpl > impl = new DummyControlImpl;
  DummyControl control( *impl );
  impl->Initialize();
  return control;
}

DummyControlImpl::DummyControlImpl()
: Control( ControlBehaviour() ),
  mCustomSlot1Called(false)
{
}

DummyControlImpl::~DummyControlImpl()
{
}

void DummyControlImpl::RegisterVisual( Property::Index index, Toolkit::Visual::Base visual )
{
  DevelControl::RegisterVisual( *this, index, visual );

  VisualIndices::iterator iter = std::find( mRegisteredVisualIndices.begin(), mRegisteredVisualIndices.end(), index );
  if( iter == mRegisteredVisualIndices.end() )
  {
    mRegisteredVisualIndices.push_back(index);
  }
}

void DummyControlImpl::RegisterVisual( Property::Index index, Toolkit::Visual::Base visual, bool enabled )
{
  DevelControl::RegisterVisual( *this, index, visual, enabled );

  VisualIndices::iterator iter = std::find( mRegisteredVisualIndices.begin(), mRegisteredVisualIndices.end(), index );
  if( iter == mRegisteredVisualIndices.end() )
  {
    mRegisteredVisualIndices.push_back(index);
  }
}

void DummyControlImpl::UnregisterVisual( Property::Index index )
{
  DevelControl::UnregisterVisual( *this, index );

  VisualIndices::iterator iter = std::find( mRegisteredVisualIndices.begin(), mRegisteredVisualIndices.end(), index );
  if( iter != mRegisteredVisualIndices.end() )
  {
    mRegisteredVisualIndices.erase(iter);
  }
}

Toolkit::Visual::Base DummyControlImpl::GetVisual( Property::Index index )
{
  return DevelControl::GetVisual( *this, index );
}

void DummyControlImpl::EnableVisual( Property::Index index, bool enabled )
{
  DevelControl::EnableVisual( *this, index, enabled );
}

bool DummyControlImpl::IsVisualEnabled( Property::Index index )
{
  return DevelControl::IsVisualEnabled( *this, index );
}

Animation DummyControlImpl::CreateTransition( const Toolkit::TransitionData& transition )
{
  return DevelControl::CreateTransition( *this, transition );
}

void DummyControlImpl::DoAction( Dali::Property::Index index, Dali::Property::Index action, const Dali::Property::Value attributes )
{
  DummyControl control( *this );
  DevelControl::DoAction(  control, index, action, attributes);
}

void DummyControlImpl::SetProperty( BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value )
{
  Toolkit::DummyControl control = Toolkit::DummyControl::DownCast( Dali::BaseHandle( object ) );
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(control.GetImplementation());

  switch(index)
  {
    case Toolkit::DummyControl::Property::TEST_VISUAL:
    case Toolkit::DummyControl::Property::TEST_VISUAL2:
    case Toolkit::DummyControl::Property::FOREGROUND_VISUAL:
    case Toolkit::DummyControl::Property::FOCUS_VISUAL:
    case Toolkit::DummyControl::Property::LABEL_VISUAL:
    {
      const Property::Map* map = value.GetMap();
      if( map != NULL )
      {
        VisualFactory visualFactory = VisualFactory::Get();
        Visual::Base visual = visualFactory.CreateVisual(*map);
        dummyImpl.RegisterVisual(index, visual);
      }
      break;
    }
  }
}

Property::Value DummyControlImpl::GetProperty( BaseObject* object, Dali::Property::Index propertyIndex )
{
  Toolkit::DummyControl control = Toolkit::DummyControl::DownCast( Dali::BaseHandle( object ) );
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>( control.GetImplementation() );

  Visual::Base visual = dummyImpl.GetVisual( propertyIndex );
  Property::Map map;
  if( visual )
  {
    visual.CreatePropertyMap( map );
  }
  Dali::Property::Value value = map;

  return value;
}


Toolkit::DummyControl Impl::DummyControl::New()
{
  IntrusivePtr< Toolkit::Impl::DummyControl > impl = new Toolkit::Impl::DummyControl;
  Toolkit::DummyControl control( *impl );
  impl->Initialize();
  return control;
}

int Impl::DummyControl::constructorCount;
int Impl::DummyControl::destructorCount;

Impl::DummyControl::DummyControl()
: DummyControlImpl(),
  initializeCalled(false),
  activatedCalled(false),
  onAccValueChangeCalled(false),
  themeChangeCalled(false),
  fontChangeCalled(false),
  pinchCalled(false),
  panCalled(false),
  tapCalled(false),
  longPressCalled(false),
  stageConnectionCalled(false),
  stageDisconnectionCalled(false),
  childAddCalled(false),
  childRemoveCalled(false),
  sizeSetCalled(false),
  sizeAnimationCalled(false),
  hoverEventCalled(false),
  wheelEventCalled(false),
  keyEventCalled(false),
  keyInputFocusGained(false),
  keyInputFocusLost(false)
{
  ++constructorCount;
}

Impl::DummyControl::~DummyControl()
{
  ++destructorCount;
}

void Impl::DummyControl::OnInitialize() { initializeCalled = true; }
bool Impl::DummyControl::OnAccessibilityActivated() { activatedCalled = true; return true; }
bool Impl::DummyControl::OnAccessibilityValueChange( bool isIncrease )
{
  onAccValueChangeCalled = true; return true;
}

void Impl::DummyControl::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  themeChangeCalled = change == StyleChange::THEME_CHANGE;
  fontChangeCalled = change == StyleChange::DEFAULT_FONT_SIZE_CHANGE;
}
void Impl::DummyControl::OnPinch(const PinchGesture& pinch) { pinchCalled = true; }
void Impl::DummyControl::OnPan(const PanGesture& pan) { panCalled = true; }
void Impl::DummyControl::OnTap(const TapGesture& tap) { tapCalled = true; }
void Impl::DummyControl::OnLongPress(const LongPressGesture& longPress) { longPressCalled = true; }
void Impl::DummyControl::OnSceneConnection( int depth ) { Control::OnSceneConnection( depth ); stageConnectionCalled = true; }
void Impl::DummyControl::OnSceneDisconnection() { stageDisconnectionCalled = true; Control::OnSceneDisconnection(); }
void Impl::DummyControl::OnChildAdd(Actor& child) { childAddCalled = true; }
void Impl::DummyControl::OnChildRemove(Actor& child) { childRemoveCalled = true; }
void Impl::DummyControl::OnSizeSet(const Vector3& targetSize) { Control::OnSizeSet( targetSize ); sizeSetCalled = true; }
void Impl::DummyControl::OnSizeAnimation(Animation& animation, const Vector3& targetSize) { Control::OnSizeAnimation( animation, targetSize ); sizeAnimationCalled = true; }
bool Impl::DummyControl::OnKeyEvent(const KeyEvent& event) { keyEventCalled = true; return false;}
void Impl::DummyControl::OnKeyInputFocusGained()
{
  if(this->HasKeyInputFocus())
  {
    keyInputFocusGained = true;
  }
}
void Impl::DummyControl::OnKeyInputFocusLost()
{
  if(!this->HasKeyInputFocus())
  {
    keyInputFocusLost = true;
  }
}

void Impl::DummyControl::SetLayout( Property::Index visualIndex, Property::Map& map )
{
  Property::Value value( map );
  mLayouts[visualIndex] = value;
}

void Impl::DummyControl::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  if ( mRelayoutCallback )
  {
    mRelayoutCallback( size );  // Execute callback if set
  }

  Property::Map emptyMap;

  for( VisualIndices::iterator iter = mRegisteredVisualIndices.begin(); iter != mRegisteredVisualIndices.end() ; ++iter )
  {
    Visual::Base visual = GetVisual(*iter);
    Property::Value value = mLayouts[*iter];
    Property::Map* map = NULL;

    if( value.GetType() != Property::NONE )
    {
      map = value.GetMap();
    }
    if( map == NULL )
    {
      map = &emptyMap;
    }

    visual.SetTransformAndSize( *map, size );
  }
}

void Impl::DummyControl::SetRelayoutCallback( RelayoutCallbackFunc callback  )
{
  mRelayoutCallback = callback;
}

Vector3 Impl::DummyControl::GetNaturalSize()
{
  Vector2 currentSize;

  for( auto elem : mRegisteredVisualIndices )
  {
    Vector2 naturalSize;
    Visual::Base visual = GetVisual(elem);
    visual.GetNaturalSize( naturalSize );
    currentSize.width = std::max( naturalSize.width, currentSize.width );
    currentSize.height = std::max( naturalSize.height, currentSize.height );
  }

  return Vector3( currentSize );
}



DummyControl DummyControl::New( bool override )
{
  DummyControl control;

  if (override)
  {
    control = Impl::DummyControl::New();
  }
  else
  {
    control = DummyControlImpl::New();
  }

  return control;
}

DummyControl::DummyControl( DummyControlImpl& implementation )
: Control( implementation )
{
}

DummyControl::DummyControl( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<DummyControlImpl>(internal);
}

} // namespace Toolkit

} // namespace Dali
