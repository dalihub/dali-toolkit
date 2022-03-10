/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>
#include <dali-toolkit/devel-api/controls/control-wrapper.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali.h>
#include <dali/devel-api/actors/custom-actor-devel.h>
#include <dali/devel-api/object/csharp-type-registry.h>
#include <dali/public-api/dali-core.h>

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

static bool gKeyInputFocusCallBackCalled;

static void TestKeyInputFocusCallback(Control control)
{
  tet_infoline(" TestKeyInputFocusCallback");

  gKeyInputFocusCallBackCalled = true;
}
} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Impl
{
struct TestCustomControl : public Toolkit::Internal::ControlWrapper
{
  /**
   * Constructor
   */
  TestCustomControl()
  : Toolkit::Internal::ControlWrapper(CustomControlBehaviour(Toolkit::Internal::ControlWrapper::DISABLE_STYLE_CHANGE_SIGNALS |
                                                             Toolkit::Internal::ControlWrapper::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)),
    mDaliProperty(Property::INVALID_INDEX),
    mSizeSet(Vector3::ZERO),
    mTargetSize(Vector3::ZERO),
    mNego(false),
    mDepth(0u)
  {
  }

  TestCustomControl(bool nego)
  : Toolkit::Internal::ControlWrapper(CustomControlBehaviour(Toolkit::Internal::ControlWrapper::DISABLE_STYLE_CHANGE_SIGNALS |
                                                             Toolkit::Internal::ControlWrapper::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)),
    mDaliProperty(Property::INVALID_INDEX),
    mSizeSet(Vector3::ZERO),
    mTargetSize(Vector3::ZERO),
    mNego(nego),
    mDepth(0u)
  {
  }

  /**
   * Destructor
   */
  virtual ~TestCustomControl()
  {
  }

  void Initialize(const char* name = NULL)
  {
    mDaliProperty = Self().RegisterProperty("Dali", std::string("no"), Property::READ_WRITE);

    OnInitialize(name);
  }

  using Control::OnInitialize; ///< To tell the compiler that we really do want to overload OnInitialize in this class & were not trying to override it
  virtual void OnInitialize(const char* name)
  {
  }

  // From Toolkit::Internal::ControlWrapper
  virtual void OnSceneConnection(int depth)
  {
    mDepth = depth;
    Control::OnSceneConnection(depth);
  }
  virtual void OnSceneDisconnection()
  {
    Control::OnSceneDisconnection();
  }
  virtual void OnChildAdd(Actor& child)
  {
    Control::OnChildAdd(child);
  }
  virtual void OnChildRemove(Actor& child)
  {
    Control::OnChildRemove(child);
  }
  virtual void OnPropertySet(Property::Index index, const Property::Value& propertyValue)
  {
    Control::OnPropertySet(index, propertyValue);
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
    mSizeSet = targetSize;
    Control::OnSizeSet(targetSize);
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
    mTargetSize = targetSize;
    Control::OnSizeAnimation(animation, targetSize);
  }
  virtual bool OnHoverEvent(const HoverEvent& event)
  {
    return true;
  }
  virtual bool OnWheelEvent(const WheelEvent& event)
  {
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
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
    return Vector3(0.0f, 0.0f, 0.0f);
  }

  virtual float GetHeightForWidth(float width)
  {
    return 0.0f;
  }

  virtual float GetWidthForHeight(float height)
  {
    return 0.0f;
  }

  void TestRegisterVisual(Property::Index index, Toolkit::Visual::Base visual)
  {
    ControlWrapper::RegisterVisual(index, visual);

    VisualIndices::iterator iter = std::find(mRegisteredVisualIndices.begin(), mRegisteredVisualIndices.end(), index);
    if(iter == mRegisteredVisualIndices.end())
    {
      mRegisteredVisualIndices.push_back(index);
    }
  }

  virtual void OnRelayout(const Vector2& size, RelayoutContainer& container)
  {
    gOnRelayout = true;

    for(VisualIndices::iterator iter = mRegisteredVisualIndices.begin(); iter != mRegisteredVisualIndices.end(); ++iter)
    {
      Visual::Base  visual = GetVisual(*iter);
      Property::Map map; // empty map enforces defaults
      visual.SetTransformAndSize(map, size);
    }
  }

  virtual void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
  {
  }

  virtual void OnCalculateRelayoutSize(Dimension::Type dimension)
  {
  }

  virtual float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
  {
    return 0.0f;
  }

  virtual void OnLayoutNegotiated(float size, Dimension::Type dimension)
  {
  }

  virtual bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS)
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

  float TestGetHeightForWidthBase(float width)
  {
    return GetHeightForWidthBase(width);
  }

  float TestGetWidthForHeightBase(float height)
  {
    return GetWidthForHeightBase(height);
  }

  float TestCalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension)
  {
    return CalculateChildSizeBase(child, dimension);
  }

  bool TestRelayoutDependentOnChildrenBase(Dimension::Type dimension)
  {
    return RelayoutDependentOnChildrenBase(dimension);
  }

  Property::Index mDaliProperty;
  Vector3         mSizeSet;
  Vector3         mTargetSize;
  bool            mNego;
  unsigned int    mDepth;

  typedef std::vector<Property::Index> VisualIndices;
  VisualIndices                        mRegisteredVisualIndices;
};

} // namespace Impl

static std::string      customControlTypeName = "MyTestCustomControl";
static TypeRegistration customControl(customControlTypeName, typeid(Dali::Toolkit::Control), NULL);

int UtcDaliControlWrapperConstructor(void)
{
  ToolkitTestApplication application; // Exceptions require ToolkitTestApplication

  Toolkit::Internal::ControlWrapper* controlWrapperImpl = new Toolkit::Internal::ControlWrapper(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper                     controlWrapper;

  DALI_TEST_CHECK(!ControlWrapper::DownCast(controlWrapper));

  controlWrapper = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  DALI_TEST_CHECK(ControlWrapper::DownCast(controlWrapper));

  Dali::TypeInfo typeInfo = DevelCustomActor::GetTypeInfo(controlWrapper);

  DALI_TEST_EQUALS(typeInfo.GetName(), customControlTypeName, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperDestructor(void)
{
  ToolkitTestApplication application;

  ControlWrapper control = ControlWrapper::New(customControlTypeName, *(new Toolkit::Internal::ControlWrapper(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT)));

  ControlWrapper control2(control);

  DALI_TEST_CHECK(control);
  control.Reset();
  DALI_TEST_CHECK(!control);

  DALI_TEST_CHECK(control2);
  control2.Reset();
  DALI_TEST_CHECK(!control2);

  END_TEST;
}

int UtcDaliControlWrapperRelayoutRequest(void)
{
  ToolkitTestApplication application;

  DALI_TEST_EQUALS(gOnRelayout, false, TEST_LOCATION);

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  application.GetScene().Add(controlWrapper);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayout, true, TEST_LOCATION);
  gOnRelayout = false;

  controlWrapperImpl->TestRelayoutRequest();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gOnRelayout, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperImplGetHeightForWidthBase(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  float width = 300.0f;
  float v     = 0.0f;

  application.SendNotification();
  application.Render();

  v = controlWrapperImpl->TestGetHeightForWidthBase(width);

  DALI_TEST_EQUALS(width, v, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperGetWidthForHeightBase(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  float height = 300.0f;
  float v      = 0.0f;

  application.SendNotification();
  application.Render();

  v = controlWrapperImpl->TestGetWidthForHeightBase(height);

  DALI_TEST_EQUALS(height, v, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperCalculateChildSizeBase(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  Actor child = Actor::New();
  child.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  child.SetProperty(Actor::Property::SIZE, Vector2(150, 150));

  application.SendNotification();
  application.Render();

  float v = 9.99f;
  v       = controlWrapperImpl->TestCalculateChildSizeBase(child, Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_EQUALS(v, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperRelayoutDependentOnChildrenBase(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  bool v = false;

  v = controlWrapperImpl->TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(v, true, TEST_LOCATION);

  controlWrapper.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  v = controlWrapperImpl->TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(v, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperRegisterVisualToSelf(void)
{
  ToolkitTestApplication application;

  Test::ObjectDestructionTracker objectDestructionTracker(application.GetCore().GetObjectRegistry());

  {
    Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
    ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

    objectDestructionTracker.Start(controlWrapper);

    Property::Index index = 1;

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base  visual;

    Property::Map map;
    map[Visual::Property::TYPE]           = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual(map);
    DALI_TEST_CHECK(visual);

    // Register to self
    controlWrapperImpl->RegisterVisual(index, visual);

    DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION); // Control not destroyed yet
    DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(index), visual, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION); // Should be destroyed

  END_TEST;
}

int UtcDaliControlWrapperRegisterVisualWithDepthIndexToSelf(void)
{
  ToolkitTestApplication application;

  Test::ObjectDestructionTracker objectDestructionTracker(application.GetCore().GetObjectRegistry());

  {
    Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
    ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

    objectDestructionTracker.Start(controlWrapper);

    Property::Index index = 1;

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base  visual;

    Property::Map map;
    map[Visual::Property::TYPE]           = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual(map);
    DALI_TEST_CHECK(visual);

    // Register to self
    controlWrapperImpl->RegisterVisual(index, visual, 4);

    DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION); // Control not destroyed yet
    DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(index), visual, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.GetDepthIndex(), 4, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION); // Should be destroyed

  END_TEST;
}

int UtcDaliControlWrapperRegisterDisabledVisual(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  controlWrapperImpl->RegisterVisual(TEST_PROPERTY, visual, false);

  DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(TEST_PROPERTY), visual, TEST_LOCATION);
  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), false, TEST_LOCATION);

  application.GetScene().Add(controlWrapper);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), false, TEST_LOCATION);

  DALI_TEST_EQUALS(controlWrapper.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), true, TEST_LOCATION);

  controlWrapperImpl->EnableVisual(TEST_PROPERTY, true);

  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperRegisterDisabledVisualWithDepthIndex(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  Property::Index TEST_PROPERTY = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  controlWrapperImpl->RegisterVisual(TEST_PROPERTY, visual, false, 10);

  DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(TEST_PROPERTY), visual, TEST_LOCATION);
  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), false, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthIndex(), 10, TEST_LOCATION);

  application.GetScene().Add(controlWrapper);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), false, TEST_LOCATION);

  DALI_TEST_EQUALS(controlWrapper.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), true, TEST_LOCATION);

  controlWrapperImpl->EnableVisual(TEST_PROPERTY, true);

  DALI_TEST_EQUALS(controlWrapperImpl->IsVisualEnabled(TEST_PROPERTY), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperRegisterUnregisterVisual(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  Property::Index index = 1;

  Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
  Toolkit::Visual::Base  visual;

  Property::Map map;
  map[Visual::Property::TYPE]           = Visual::COLOR;
  map[ColorVisual::Property::MIX_COLOR] = Color::RED;

  visual = visualFactory.CreateVisual(map);
  DALI_TEST_CHECK(visual);

  // Register index with a color visual
  controlWrapperImpl->RegisterVisual(index, visual);

  DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(index), visual, TEST_LOCATION);

  // Unregister visual
  controlWrapperImpl->UnregisterVisual(index);

  DALI_TEST_CHECK(!controlWrapperImpl->GetVisual(index));

  END_TEST;
}

int UtcDaliControlWrapperTransitionDataMap1N(void)
{
  ToolkitTestApplication application;

  Property::Map map;
  map["target"]       = "Actor1";
  map["property"]     = "randomProperty";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"]  = Color::RED;
  map["animator"]     = Property::Map()
                      .Add("alphaFunction", "EASE_OUT")
                      .Add("timePeriod", Property::Map().Add("delay", 0.5f).Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New(map);

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  //DummyControl actor = DummyControl::New();
  controlWrapper.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  controlWrapper.SetProperty(Dali::Actor::Property::NAME, "Actor1");
  controlWrapper.SetProperty(Actor::Property::COLOR, Color::CYAN);
  application.GetScene().Add(controlWrapper);

  Animation anim = controlWrapperImpl->CreateTransition(transition);
  DALI_TEST_CHECK(!anim);

  Property::Map returnedMap = transition.GetAnimatorAt(0);

  Property::Value* value = returnedMap.Find("property");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS("randomProperty", value->Get<std::string>(), TEST_LOCATION);

  value = returnedMap.Find("initialValue");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS(Color::MAGENTA, value->Get<Vector4>(), TEST_LOCATION);

  value = returnedMap.Find("targetValue");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS(Color::RED, value->Get<Vector4>(), TEST_LOCATION);

  value = returnedMap.Find("animator");
  DALI_TEST_CHECK(value != NULL);
  Property::Map returnedAnimatorMap = value->Get<Property::Map>();

  value = returnedAnimatorMap.Find("alphaFunction");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS("EASE_OUT", value->Get<std::string>(), TEST_LOCATION);

  value = returnedAnimatorMap.Find("timePeriod");
  DALI_TEST_CHECK(value != NULL);
  Property::Map returnedTimePeriodMap = value->Get<Property::Map>();

  value = returnedTimePeriodMap.Find("delay");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS(0.5f, value->Get<float>(), TEST_LOCATION);

  value = returnedTimePeriodMap.Find("duration");
  DALI_TEST_CHECK(value != NULL);
  DALI_TEST_EQUALS(1.0f, value->Get<float>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperApplyThemeStyle(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  controlWrapperImpl->ApplyThemeStyle();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliControlWrapperTestControlProperties(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  application.GetScene().Add(controlWrapper);

  // "background" property
  Property::Map rendererMap;
  rendererMap[Visual::Property::TYPE]           = Visual::COLOR;
  rendererMap[ColorVisual::Property::MIX_COLOR] = Color::RED;
  controlWrapper.SetProperty(Control::Property::BACKGROUND, rendererMap);
  Property::Value propertyValue = controlWrapper.GetProperty(Control::Property::BACKGROUND);
  Property::Map*  resultMap     = propertyValue.GetMap();
  DALI_TEST_CHECK(resultMap->Find(Toolkit::Visual::Property::TYPE));
  DALI_TEST_EQUALS(resultMap->Find(Toolkit::Visual::Property::TYPE)->Get<int>(), (int)Visual::COLOR, TEST_LOCATION);
  DALI_TEST_CHECK(resultMap->Find(ColorVisual::Property::MIX_COLOR));
  DALI_TEST_EQUALS(resultMap->Find(ColorVisual::Property::MIX_COLOR)->Get<Vector4>(), Color::RED, TEST_LOCATION);

  // "keyInputFocus" property
  controlWrapper.SetProperty(Control::Property::KEY_INPUT_FOCUS, true);
  DALI_TEST_EQUALS(true, controlWrapper.GetProperty(Control::Property::KEY_INPUT_FOCUS).Get<bool>(), TEST_LOCATION);

  // "styleName" property
  controlWrapper.SetProperty(Control::Property::STYLE_NAME, "MyCustomStyle");
  DALI_TEST_EQUALS("MyCustomStyle", controlWrapper.GetProperty(Control::Property::STYLE_NAME).Get<std::string>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlWrapperTypeRegistryCreation(void)
{
  ToolkitTestApplication application;

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo("ControlWrapper");
  DALI_TEST_CHECK(typeInfo)

  // Check that we can't create a ControlWrapper instance
  BaseHandle baseHandle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(!baseHandle)

  END_TEST;
}

void SetProperty(BaseObject* object, const char* const name, Property::Value* value)
{
}
Property::Value* GetProperty(BaseObject* object, const char* const name)
{
  return NULL;
}

int UtcDaliControlWrapperAnimateVisual(void)
{
  tet_infoline("Test that the control wrapper's visuals can be animated by name when registered");

  ToolkitTestApplication         application;
  Test::ObjectDestructionTracker objectDestructionTracker(application.GetCore().GetObjectRegistry());

  {
    Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
    ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

    Property::Index index = Control::CONTROL_PROPERTY_END_INDEX + 1;
    std::string     visualName("colorVisual");
    CSharpTypeRegistry::RegisterProperty(customControlTypeName, visualName, index, Property::VECTOR4, SetProperty, GetProperty);

    objectDestructionTracker.Start(controlWrapper);

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();
    Toolkit::Visual::Base  visual;

    Property::Map map;
    map[Visual::Property::TYPE]           = Visual::COLOR;
    map[ColorVisual::Property::MIX_COLOR] = Color::RED;

    visual = visualFactory.CreateVisual(map);
    DALI_TEST_CHECK(visual);

    // Register to self
    controlWrapperImpl->TestRegisterVisual(index, visual);

    application.GetScene().Add(controlWrapper);
    controlWrapper.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
    application.SendNotification();
    application.Render(0); // Trigger animation start

    Property::Map transition;
    transition["target"]      = visualName;
    transition["property"]    = "mixColor";
    transition["targetValue"] = Color::GREEN;
    Property::Map animator;
    animator["alphaFunction"] = "LINEAR";
    animator["duration"]      = 1.0f;
    animator["delay"]         = 0.0f;
    transition["animator"]    = animator;

    TransitionData transitionData = TransitionData::New(transition);
    Animation      anim           = DevelControl::CreateTransition(*controlWrapperImpl, transitionData);
    anim.Play();

    application.SendNotification();
    application.Render(0); // Trigger animation start

    application.Render(1000); // animation end
    application.Render(10);

    Property::Map visualMap;
    visual.CreatePropertyMap(visualMap);
    Property::Value* value = visualMap.Find(ColorVisual::Property::MIX_COLOR, "mixColor");
    DALI_TEST_CHECK(value != NULL);
    if(value)
    {
      Vector4 testColor = value->Get<Vector4>();
      DALI_TEST_EQUALS(testColor, Color::GREEN, 0.001f, TEST_LOCATION);
    }

    DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), false, TEST_LOCATION); // Control not destroyed yet
    DALI_TEST_EQUALS(controlWrapperImpl->GetVisual(index), visual, TEST_LOCATION);

    application.GetScene().Remove(controlWrapper);
  }

  DALI_TEST_EQUALS(objectDestructionTracker.IsDestroyed(), true, TEST_LOCATION); // Should be destroyed

  END_TEST;
}

int UtcDaliControlWrapperEmitKeyFocusSignal(void)
{
  ToolkitTestApplication application;

  Impl::TestCustomControl* controlWrapperImpl = new ::Impl::TestCustomControl(Toolkit::Internal::ControlWrapper::CONTROL_BEHAVIOUR_DEFAULT);
  ControlWrapper           controlWrapper     = ControlWrapper::New(customControlTypeName, *controlWrapperImpl);

  gKeyInputFocusCallBackCalled = false;
  controlWrapper.KeyInputFocusGainedSignal().Connect(&TestKeyInputFocusCallback);

  application.SendNotification();
  application.Render();

  controlWrapperImpl->EmitKeyInputFocusSignal(true);

  DALI_TEST_CHECK(gKeyInputFocusCallBackCalled);

  END_TEST;
}
