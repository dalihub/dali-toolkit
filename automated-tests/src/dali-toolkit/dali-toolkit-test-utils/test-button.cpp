/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <test-button.h>
#include <dali/devel-api/object/type-registry-helper.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
Property::Value ConvertAnimationMap( const Test::TestAnimationData& animationMap )
{
  // We have a data structure. Now convert it back into an array:
  Property::Array animators;
  for( unsigned int i=0; i<animationMap.Size(); ++i )
  {
    Property::Map animator;
    animator.Insert( "target", Property::Value(animationMap.mAnimationDataList[i]->target ));
    animator.Insert( "property", Property::Value(animationMap.mAnimationDataList[i]->property ));
    animator.Insert( "value", Property::Value(animationMap.mAnimationDataList[i]->value ));
    animator.Insert( "alphaFunction", Property::Value(animationMap.mAnimationDataList[i]->alphaFunction ));
    animator.Insert( "timePeriodDelay", Property::Value(animationMap.mAnimationDataList[i]->timePeriodDelay ));
    animator.Insert( "timePeriodDuration", Property::Value(animationMap.mAnimationDataList[i]->timePeriodDuration ));
    animators.PushBack( animator );
  }
  Property::Value animation( animators );
  return animation;
}
}

namespace Test
{
namespace Impl
{

Test::TestButton TestButton::New()
{
  IntrusivePtr<TestButton> internalTestButton = new TestButton();
  Test::TestButton button( *internalTestButton );
  internalTestButton->Initialize();
  return button;
}

TestButton::TestButton()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS|REQUIRES_STYLE_CHANGE_SIGNALS ) )
{
}

TestButton::~TestButton()
{
}

void TestButton::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Test::TestButton button = Test::TestButton::DownCast( Dali::BaseHandle( object ) );

  if ( button )
  {
    TestButton& buttonImpl = GetImpl(button);
    switch ( index )
    {
      case Test::TestButton::Property::PRESS_TRANSITION:
      {
        if( value.GetType() == Property::MAP )
        {
          Property::Map* valueMap = value.GetMap();
          buttonImpl.mPressTransitionData.Clear();
          NewAnimation( *valueMap, buttonImpl.mPressTransitionData );
        }
        else if( value.GetType() == Property::ARRAY )
        {
          Property::Array* valueArray = value.GetArray();
          buttonImpl.mPressTransitionData.Clear();
          NewAnimation( *valueArray, buttonImpl.mPressTransitionData );
        }
        break;
      }
      case Test::TestButton::Property::BACKGROUND_COLOR:
      {
        buttonImpl.mBackgroundColor = value.Get<Vector4>();
        break;
      }

      case Test::TestButton::Property::FOREGROUND_COLOR:
      {
        buttonImpl.mForegroundColor = value.Get<Vector4>();
        break;
      }
    }
  }
}

Property::Value TestButton::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Test::TestButton button = Test::TestButton::DownCast( Dali::BaseHandle( object ) );

  Property::Value value;

  if ( button )
  {
    TestButton& buttonImpl = GetImpl(button);
    switch ( propertyIndex )
    {
      case Test::TestButton::Property::PRESS_TRANSITION:
      {
        return ConvertAnimationMap(buttonImpl.mPressTransitionData);
      }
      case Test::TestButton::Property::RELEASE_TRANSITION:
      {
        return ConvertAnimationMap(buttonImpl.mReleaseTransitionData);
      }
      case Test::TestButton::Property::DISABLED_TRANSITION:
      {
        return ConvertAnimationMap(buttonImpl.mDisabledTransitionData);
      }
      case Test::TestButton::Property::ENABLED_TRANSITION:
      {
        return ConvertAnimationMap(buttonImpl.mEnabledTransitionData);
      }
      case Test::TestButton::Property::BACKGROUND_COLOR:
      {
        return Property::Value(buttonImpl.mBackgroundColor);
      }
      case Test::TestButton::Property::FOREGROUND_COLOR:
      {
        return Property::Value(buttonImpl.mForegroundColor);
      }
    }
  }
  return Property::Value();
}

BaseHandle Create()
{
  return TestButton::New();
}

// Generates typeRegistration static variable.
DALI_TYPE_REGISTRATION_BEGIN( Test::TestButton, Dali::Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( Test, TestButton, "pressTransition", ARRAY, PRESS_TRANSITION )
DALI_PROPERTY_REGISTRATION( Test, TestButton, "releaseTransition", ARRAY, RELEASE_TRANSITION)
DALI_PROPERTY_REGISTRATION( Test, TestButton, "disabledTransition", ARRAY, DISABLED_TRANSITION )
DALI_PROPERTY_REGISTRATION( Test, TestButton, "enabledTransition", ARRAY, ENABLED_TRANSITION )
DALI_PROPERTY_REGISTRATION( Test, TestButton, "backgroundColor", VECTOR4, BACKGROUND_COLOR )
DALI_PROPERTY_REGISTRATION( Test, TestButton, "foregroundColor", VECTOR4, FOREGROUND_COLOR )

DALI_TYPE_REGISTRATION_END()

} // Impl Namespace

TestButton::TestButton()
: Control()
{
}

TestButton::TestButton(const TestButton& button)
: Control( button )
{
}

TestButton::TestButton(Impl::TestButton& impl)
: Control(impl)
{
}

TestButton::TestButton(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Impl::TestButton>(internal);
}

TestButton& TestButton::operator=( const TestButton& button)
{
  if(&button != this)
  {
    Control::operator=(button);
  }
  return *this;
}

TestButton::~TestButton()
{
}

TestButton TestButton::New()
{
  return Impl::TestButton::New();
}

TestButton TestButton::DownCast( BaseHandle handle )
{
  return Control::DownCast<TestButton,Impl::TestButton>(handle);
}

} // namespace Test
