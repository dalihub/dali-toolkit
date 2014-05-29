//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <algorithm>
#include <dali-toolkit/internal/controls/selectors/rotating-selector-impl.h>

#include "dali-toolkit/public-api/controls/selectors/rotating-selector.h"

using namespace std;

namespace
{
const float TOUCH_OPACITY_THRESHOLD = 0.1f;
} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
namespace
{
//Type registration
BaseHandle Create()
{
  // empty handle, as RotatingSelector takes children during construction
  return Toolkit::RotatingSelector();
}
TypeRegistration mType( typeid(Toolkit::RotatingSelector), typeid(Toolkit::Control), Create );

const Quaternion ROTATION_ANGLE(0.0f, Vector3(1.0f, 0.0f, 0.0f));
}

Dali::Toolkit::RotatingSelector RotatingSelector::New(Actor& unSelectedActor, Actor& selectedActor)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< RotatingSelector > customCheckActor = new RotatingSelector;

  // Pass ownership to CustomActor
  Dali::Toolkit::RotatingSelector handle( *customCheckActor );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  customCheckActor->Initialize();

  customCheckActor->SetSelectedActor(selectedActor);
  customCheckActor->SetUnSelectedActor(unSelectedActor);

  return handle;
}

void RotatingSelector::OnInitialize()
{
  mContainer = Actor::New();
  mContainer.SetName("Selector Container");
  mUnSelectedActor = Actor::New();
  mSelectedActor = Actor::New();

  mRotateAnimation = Animation::New(0.5f);
  mRotateAnimation.FinishedSignal().Connect(this, &RotatingSelector::AnimationCompleted);

  mUnSelectedActor.SetParentOrigin(ParentOrigin::CENTER);
  mUnSelectedActor.SetAnchorPoint(AnchorPoint::CENTER);

  mSelectedActor.SetParentOrigin(ParentOrigin::CENTER);
  mSelectedActor.SetAnchorPoint(AnchorPoint::CENTER);

  mContainer.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() );
  mSelectedActor.ApplyConstraint(constraint);
  mUnSelectedActor.ApplyConstraint(constraint);
  mContainer.ApplyConstraint(constraint);

  mContainer.Add(mUnSelectedActor);
  mSelectedActor.SetRotation(Radian(Math::PI), Vector3::XAXIS );

  mUnSelectedActor.SetName("RotatingSelector : UnSelectedActor");
  mSelectedActor.SetName("RotatingSelector : SelectedActor");

  Self().Add(mContainer);
  Self().SetLeaveRequired(true);

  mRotateAnimation.RotateBy(mContainer, Radian(Math::PI), Vector3(1.0f, 0.0f, 0.0f));
}

RotatingSelector::RotatingSelector()
: Control( REQUIRES_TOUCH_EVENTS ),
  mSelected(false),
  mSelectable(true),
  mIsAnimating(false)
{
}

RotatingSelector::~RotatingSelector()
{
  mRotateAnimation.Reset();
}

void RotatingSelector::SetSelected( bool toggle )
{
  if(toggle != mSelected)
  {
    if( mSelectable )
    {
      ToggleAndAnimateSelection();
    }
  }
}

void RotatingSelector::SetSelectedActor( Actor& selectedActor )
{
  unsigned int numChildren = mSelectedActor.GetChildCount();
  for( unsigned int i=0; i<numChildren; ++i )
  {
    Actor actor = mSelectedActor.GetChildAt(i);
    mSelectedActor.Remove(actor);
  }

  mSelectedActor.Add(selectedActor);
}

Actor RotatingSelector::GetSelectedActor()
{
  return mSelectedActor.GetChildAt(0);
}

void RotatingSelector::SetUnSelectedActor( Actor& unSelectedActor )
{
  unsigned int numChildren = mUnSelectedActor.GetChildCount();

  for(unsigned int i=0; i<numChildren; ++i)
  {
    mUnSelectedActor.Remove(mUnSelectedActor.GetChildAt(0));
  }

  mUnSelectedActor.Add(unSelectedActor);
}

Actor RotatingSelector::GetUnSelectedActor()
{
  return mUnSelectedActor.GetChildAt(0);
}

void RotatingSelector::SetSelectable( bool selectable )
{
  mSelectable = selectable;
}

Toolkit::RotatingSelector::SelectedSignalV2& RotatingSelector::SelectedSignal()
{
  return mCheckedSignalV2;
}

bool RotatingSelector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::RotatingSelector selector = Toolkit::RotatingSelector::DownCast(handle);

  if( Toolkit::RotatingSelector::SIGNAL_CHECKED == signalName )
  {
    selector.SelectedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

bool RotatingSelector::OnTouchEvent(const TouchEvent& event)
{
  if( 1 == event.GetPointCount() )
  {
    switch( event.GetPoint(0).state )
    {
      case TouchPoint::Down:
        if(Self().GetCurrentOpacity() > TOUCH_OPACITY_THRESHOLD)
        {
          mPressed = true;
        }
        break;
      case TouchPoint::Leave:
        mPressed = false;
        break;
      case TouchPoint::Up:
      {
        if(mSelectable && mPressed)
        {
          ToggleAndAnimateSelection();
        }
        mPressed = false;
        break;
      }
      default:
        break;
    }
  }

 return false; // dont consume
}

void RotatingSelector::ToggleAndAnimateSelection()
{
  if(!mIsAnimating)
  {
    mSelected = !mSelected;
    if(mSelected)
    {
      //The checked image (i.e mSelectedActor should be in front)
      mSelectedActor.SetPosition(0.0f, 0.0f, -1.0f);
      mContainer.Add(mSelectedActor);
    }
    else
    {
      //The un checked image (i.e mUnSelectedActor should be in front)
      mUnSelectedActor.SetPosition(0.0f, 0.0f, 1.0f);
      mContainer.Add(mUnSelectedActor);
    }

    mIsAnimating = true;
    mRotateAnimation.Play();
  }
}

void RotatingSelector::AnimationCompleted( Animation& animation )
{
  if(mSelected)
  {
    //The checked image (i.e mSelectedActor should be in front)
    mSelectedActor.SetPosition(0.0f, 0.0f, 0.0f);
    mContainer.Remove(mUnSelectedActor);

  }
  else
  {
    //The un checked image (i.e mUnSelectedActor should be in front)
    mContainer.Remove(mSelectedActor);
    mUnSelectedActor.SetPosition(0.0f, 0.0f, 0.0f);
  }

  mIsAnimating = false;

  //Emit signal.
  Dali::Toolkit::RotatingSelector handle( GetOwner() );
  mCheckedSignalV2.Emit( handle, mSelected );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
