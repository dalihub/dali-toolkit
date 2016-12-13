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
 *
 */

#include <iostream>
#include <stdlib.h>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-wrapper.h>
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_control_wrapper_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_control_wrapper_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
bool gOnRelayout = false;
} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Impl
{
struct TestCustomControl : public Toolkit::Internal::ControlWrapper
{
  /**
  * Constructor
  */
  TestCustomControl()  : Toolkit::Internal::ControlWrapper( CustomControlBehaviour( Toolkit::Internal::ControlWrapper::DISABLE_STYLE_CHANGE_SIGNALS |
          Toolkit::Internal::ControlWrapper::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT )) ,
          mDaliProperty( Property::INVALID_INDEX ),
          mSizeSet( Vector3::ZERO ),
          mTargetSize( Vector3::ZERO ),
          mNego( false ),
          mDepth( 0u )
  {
  }

  TestCustomControl(bool nego)  : Toolkit::Internal::ControlWrapper( CustomControlBehaviour( Toolkit::Internal::ControlWrapper::DISABLE_STYLE_CHANGE_SIGNALS |
          Toolkit::Internal::ControlWrapper::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT ) ),
          mDaliProperty( Property::INVALID_INDEX ),
          mSizeSet( Vector3::ZERO ),
          mTargetSize( Vector3::ZERO ),
          mNego( nego )
  {
  }
  /**
   * Destructor
   */
  virtual ~TestCustomControl()
  {
  }

  void Initialize( const char* name = NULL )
  {
    mDaliProperty = Self().RegisterProperty( "Dali", std::string("no"), Property::READ_WRITE );

    OnInitialize( name );
  }

  virtual void OnInitialize( const char* name ) {}

  // From Toolkit::Internal::ControlWrapper
  virtual void OnStageConnection( int depth )
  {
    mDepth = depth;
  }
  virtual void OnStageDisconnection()
  {
  }
  virtual void OnChildAdd( Actor& child )
  {
  }
  virtual void OnChildRemove( Actor& child )
  {
  }
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue )
  {
  }
  virtual void OnSizeSet( const Vector3& targetSize )
  {
    mSizeSet = targetSize;
  }
  virtual void OnSizeAnimation( Animation& animation, const Vector3& targetSize )
  {
    mTargetSize = targetSize;
  }
  virtual bool OnTouchEvent( const TouchEvent& event )
  {
    return true;
  }
  virtual bool OnHoverEvent( const HoverEvent& event )
  {
    return true;
  }
  virtual bool OnWheelEvent( const WheelEvent& event )
  {
    return true;
  }
  virtual bool OnKeyEvent( const KeyEvent& event )
  {
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }
  virtual Vector3 GetNaturalSize()
  {
    return Vector3( 0.0f, 0.0f, 0.0f );
  }

  virtual float GetHeightForWidth( float width )
  {
    return 0.0f;
  }

  virtual float GetWidthForHeight( float height )
  {
    return 0.0f;
  }

  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container )
  {
    gOnRelayout = true;
  }

  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension )
  {
  }

  virtual void OnCalculateRelayoutSize( Dimension::Type dimension )
  {
  }

  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension )
  {
    return 0.0f;
  }

  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension )
  {
  }

  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS )
  {
    return false;
  }

  void SetDaliProperty(std::string s)
  {
    Self().SetProperty(mDaliProperty, s);
  }
  void TestRelayoutRequest()
  {
    RelayoutRequest();
  }

  float TestGetHeightForWidthBase( float width )
  {
    return GetHeightForWidthBase( width );
  }

  float TestGetWidthForHeightBase( float height )
  {
    return GetWidthForHeightBase( height );
  }

  float TestCalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
  {
    return CalculateChildSizeBase( child, dimension );
  }

  bool TestRelayoutDependentOnChildrenBase( Dimension::Type dimension )
  {
    return RelayoutDependentOnChildrenBase( dimension );
  }

  Property::Index mDaliProperty;
  Vector3 mSizeSet;
  Vector3 mTargetSize;
  bool mNego;
  unsigned int mDepth;
};
}

int UtcDaliControlWrapperConstructor(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Toolkit::Internal::ControlWrapper* controlWrapperImpl = new Toolkit::Internal::ControlWrapper( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper;

  DALI_TEST_CHECK( !ControlWrapper::DownCast( controlWrapper ) );

  controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  DALI_TEST_CHECK( ControlWrapper::DownCast( controlWrapper ) );
  END_TEST;
}

int UtcDaliControlWrapperDestructor(void)
{
  TestApplication application;

  ControlWrapper control = ControlWrapper::New( *( new Toolkit::Internal::ControlWrapper( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT ) ) );

  ControlWrapper control2( control );

  DALI_TEST_CHECK( control );
  control.Reset();
  DALI_TEST_CHECK( !control );

  DALI_TEST_CHECK( control2 );
  control2.Reset();
  DALI_TEST_CHECK( !control2 );

  END_TEST;
}

int UtcDaliControlWrapperRelayoutRequest(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( gOnRelayout, false, TEST_LOCATION );

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  Stage::GetCurrent().Add( controlWrapper );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gOnRelayout, true, TEST_LOCATION );
  gOnRelayout = false;

  controlWrapperImpl->TestRelayoutRequest();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gOnRelayout, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperImplGetHeightForWidthBase(void)
{
  TestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  float width = 300.0f;
  float v = 0.0f;

  application.SendNotification();
  application.Render();

  v = controlWrapperImpl->TestGetHeightForWidthBase( width );

  DALI_TEST_EQUALS( width, v, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperGetWidthForHeightBase(void)
{
  TestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  float height = 300.0f;
  float v = 0.0f;

  application.SendNotification();
  application.Render();

  v = controlWrapperImpl->TestGetWidthForHeightBase( height );

  DALI_TEST_EQUALS( height, v, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperCalculateChildSizeBase(void)
{
  TestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  Actor child = Actor::New();
  child.SetResizePolicy( Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS );
  child.SetSize(150, 150);

  application.SendNotification();
  application.Render();

  float v = 9.99f;
  v = controlWrapperImpl->TestCalculateChildSizeBase( child, Dali::Dimension::ALL_DIMENSIONS );
  DALI_TEST_EQUALS( v, 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperRelayoutDependentOnChildrenBase(void)
{
  TestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  bool v = false;

  v = controlWrapperImpl->TestRelayoutDependentOnChildrenBase( Dali::Dimension::ALL_DIMENSIONS );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( v, true, TEST_LOCATION );

  controlWrapper.SetResizePolicy( Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS );
  v = controlWrapperImpl->TestRelayoutDependentOnChildrenBase( Dali::Dimension::WIDTH );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( v, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperRegisterVisualToSelf(void)
{
  ToolkitTestApplication application;

  Test::ObjectDestructionTracker objectDestructionTracker;

  {
    Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
    ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

    objectDestructionTracker.Start( controlWrapper );

    Property::Index index = 1;

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base visual;

    Property::Map map;
    map[Visual::Property::TYPE] = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual( map );
    DALI_TEST_CHECK( visual );

    // Register to self
    controlWrapperImpl->RegisterVisual( index, visual );

    DALI_TEST_EQUALS( objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION ); // Control not destroyed yet
    DALI_TEST_EQUALS( controlWrapperImpl->GetVisual( index ), visual, TEST_LOCATION );
  }

  DALI_TEST_EQUALS( objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION ); // Should be destroyed

  END_TEST;
}

int UtcDaliControlWrapperRegisterDisabledVisual(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  controlWrapperImpl->RegisterVisual( TEST_PROPERTY, visual, false );

  DALI_TEST_EQUALS( controlWrapperImpl->GetVisual( TEST_PROPERTY ), visual, TEST_LOCATION );
  DALI_TEST_EQUALS( controlWrapperImpl->IsVisualEnabled( TEST_PROPERTY ), false, TEST_LOCATION );

  Stage::GetCurrent().Add( controlWrapper );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( controlWrapperImpl->IsVisualEnabled( TEST_PROPERTY ), false, TEST_LOCATION );

  DALI_TEST_EQUALS( controlWrapper.OnStage(), true, TEST_LOCATION );

  controlWrapperImpl->EnableVisual( TEST_PROPERTY, true );

  DALI_TEST_EQUALS( controlWrapperImpl->IsVisualEnabled( TEST_PROPERTY ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperRegisterUnregisterVisual(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  Property::Index index = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base visual;

  Property::Map map;
  map[Visual::Property::TYPE] = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual( map );
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  controlWrapperImpl->RegisterVisual( index, visual );

  DALI_TEST_EQUALS( controlWrapperImpl->GetVisual( index ), visual, TEST_LOCATION );

  // Unregister visual
  controlWrapperImpl->UnregisterVisual( index );

  DALI_TEST_CHECK( !controlWrapperImpl->GetVisual( index ) );

  END_TEST;
}

int UtcDaliControlWrapperTransitionDataMap1N(void)
{
  TestApplication application;

  Property::Map map;
  map["target"] = "Actor1";
  map["property"] = "randomProperty";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  //DummyControl actor = DummyControl::New();
  controlWrapper.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  controlWrapper.SetName("Actor1");
  controlWrapper.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(controlWrapper);

  Animation anim = controlWrapperImpl->CreateTransition( transition );
  DALI_TEST_CHECK( ! anim );

  Property::Map returnedMap = transition.GetAnimatorAt(0);

  Property::Value* value = returnedMap.Find("property");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( "randomProperty", value->Get<std::string>(), TEST_LOCATION );

  value = returnedMap.Find("initialValue");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( Color::MAGENTA, value->Get<Vector4>(), TEST_LOCATION );

  value = returnedMap.Find("targetValue");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( Color::RED, value->Get<Vector4>(), TEST_LOCATION );

  value = returnedMap.Find("animator");
  DALI_TEST_CHECK( value != NULL);
  Property::Map returnedAnimatorMap = value->Get<Property::Map>();

  value = returnedAnimatorMap.Find("alphaFunction");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( "EASE_OUT", value->Get<std::string>(), TEST_LOCATION );

  value = returnedAnimatorMap.Find("timePeriod");
  DALI_TEST_CHECK( value != NULL);
  Property::Map returnedTimePeriodMap = value->Get<Property::Map>();

  value = returnedTimePeriodMap.Find("delay");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( 0.5f, value->Get<float>(), TEST_LOCATION );

  value = returnedTimePeriodMap.Find("duration");
  DALI_TEST_CHECK( value != NULL);
  DALI_TEST_EQUALS( 1.0f, value->Get<float>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlWrapperApplyThemeStyle(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl( Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT );
  ControlWrapper controlWrapper = ControlWrapper::New( *controlWrapperImpl );

  controlWrapperImpl->ApplyThemeStyle();

  DALI_TEST_CHECK( true );
  END_TEST;
}
