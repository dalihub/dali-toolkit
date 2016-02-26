#ifndef DALI_TOOLKIT_TEST_TEST_BUTTON_H
#define DALI_TOOLKIT_TEST_TEST_BUTTON_H

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

#include <dali-toolkit/dali-toolkit.h>
#include <test-animation-data.h>

namespace Test
{
namespace Impl
{
class TestButton;
}

class TestButton : public Dali::Toolkit::Control
{
public:
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000
  };
  struct Property
  {
    enum
    {
      PRESS_TRANSITION = PROPERTY_START_INDEX,
      RELEASE_TRANSITION,
      DISABLED_TRANSITION,
      ENABLED_TRANSITION
    };
  };
  TestButton();
  TestButton(const TestButton& button);
  TestButton(Impl::TestButton& impl);
  TestButton(Dali::Internal::CustomActor* internal);
  TestButton& operator=( const TestButton& button);
  ~TestButton();
  static TestButton New();
  static TestButton DownCast( Dali::BaseHandle handle );
};

namespace Impl
{

class TestButton : public Dali::Toolkit::Internal::Control
{
public:
  static Test::TestButton New();

  static void SetProperty( Dali::BaseObject* object,
                           Dali::Property::Index index,
                           const Dali::Property::Value& value );

  static Dali::Property::Value GetProperty( Dali::BaseObject* object,
                                            Dali::Property::Index propertyIndex );

protected:
  TestButton();
  virtual ~TestButton();

public:
  Test::TestAnimationData mPressTransitionData;
  Test::TestAnimationData mReleaseTransitionData;
  Test::TestAnimationData mDisabledTransitionData;
  Test::TestAnimationData mEnabledTransitionData;
};

inline TestButton& GetImpl( Test::TestButton& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<TestButton&>( object );
}

inline const TestButton& GetImpl( const Test::TestButton& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const TestButton&>( object );
}

} // Impl
} // Test



#endif // DALI_TOOLKIT_TEST_TEST_BUTTON_H
