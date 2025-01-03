/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>

void utc_dali_toolkit_internal_render_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_internal_render_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace Dali::Toolkit
{
namespace Internal
{
class TestRenderEffectImpl;
} // namespace Internal

class TestRenderEffect : public Dali::Toolkit::RenderEffect
{
public:
  static TestRenderEffect New();
  TestRenderEffect()                               = default;
  TestRenderEffect(const TestRenderEffect& handle) = default;
  ~TestRenderEffect()                              = default;

public: // Not intended for use by Application developers
  explicit TestRenderEffect(Internal::TestRenderEffectImpl* testRenderEffectImpl);
};

namespace Internal
{
class TestRenderEffectImpl;
using TestRenderEffectImplPtr = IntrusivePtr<TestRenderEffectImpl>;
class TestRenderEffectImpl : public RenderEffectImpl
{
public:
  static TestRenderEffectImplPtr New()
  {
    TestRenderEffectImplPtr handle = new TestRenderEffectImpl();
    handle->Initialize();
    return handle;
  }

  RenderEffectImplPtr Clone() const override
  {
    return New();
  }

  OffScreenRenderable::Type GetOffScreenRenderableType() override
  {
    return OffScreenRenderable::Type::NONE;
  }

  void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) override
  {
  }

protected:
  TestRenderEffectImpl()
  : mOnActivated(false)
  {
  }
  virtual ~TestRenderEffectImpl()
  {
  }

  void OnInitialize() override
  {
  }

  void OnActivate() override
  {
    mOnActivated = true;
  }

  void OnDeactivate() override
  {
    mOnActivated = false;
  }

public:
  Dali::Toolkit::Control GetOwnerControl() const
  {
    return RenderEffectImpl::GetOwnerControl();
  }

public:
  bool mOnActivated;
};

inline Dali::Toolkit::Internal::TestRenderEffectImpl& GetImplementation(Dali::Toolkit::TestRenderEffect& handle)
{
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Toolkit::Internal::TestRenderEffectImpl&>(object);
}
} // namespace Internal

TestRenderEffect TestRenderEffect::New()
{
  Internal::TestRenderEffectImplPtr internal = Internal::TestRenderEffectImpl::New();
  return TestRenderEffect(internal.Get());
}

TestRenderEffect::TestRenderEffect(Internal::TestRenderEffectImpl* testRenderEffectImpl)
: RenderEffect(testRenderEffectImpl)
{
}
} // namespace Dali::Toolkit

using namespace Dali::Toolkit;

int UtcDaliInternalRenderEffectNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliInternalRenderEffectNewP");

  Toolkit::TestRenderEffect testEffect = Toolkit::TestRenderEffect::New();
  DALI_TEST_CHECK(testEffect);

  // Check that effect is not activate yet.
  Toolkit::Internal::TestRenderEffectImpl& impl = Toolkit::Internal::GetImplementation(testEffect);
  DALI_TEST_EQUALS(impl.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalRenderEffectGetOwnerControl01(void)
{
  tet_infoline("UtcDaliInternalRenderEffecGetOwnerControl01");

  Toolkit::TestRenderEffect testEffect = Toolkit::TestRenderEffect::New();
  DALI_TEST_CHECK(testEffect);

  // Check that effect prototype is not activated.
  Toolkit::Internal::TestRenderEffectImpl& prototype = Toolkit::Internal::GetImplementation(testEffect);
  DALI_TEST_EQUALS(prototype.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  {
    ToolkitTestApplication application;

    Integration::Scene scene = application.GetScene();

    Control control = Control::New();
    control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    scene.Add(control);

    tet_printf("Test effect set, and activate due to control scene on.\n");
    control.SetRenderEffect(testEffect);

    Toolkit::Internal::TestRenderEffectImplPtr impl =
      dynamic_cast<Toolkit::Internal::TestRenderEffectImpl*>(control.GetRenderEffect().GetObjectPtr());

    { // Check that effect prototype is not activated,
      DALI_TEST_EQUALS(impl == &prototype, false, TEST_LOCATION);

      DALI_TEST_EQUALS(prototype.IsActivated(), false, TEST_LOCATION);
      DALI_TEST_EQUALS(prototype.mOnActivated, false, TEST_LOCATION);
      DALI_TEST_EQUALS(prototype.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

      // but the effect is set and activated.
      DALI_TEST_EQUALS(impl->IsActivated(), true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->mOnActivated, true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->GetOwnerControl(), control, TEST_LOCATION);
    }

    control.Unparent();

    {
      DALI_TEST_EQUALS(impl->IsActivated(), false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->mOnActivated, false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->GetOwnerControl(), control, TEST_LOCATION);
    }

    scene.Add(control);

    {
      DALI_TEST_EQUALS(impl->IsActivated(), true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->mOnActivated, true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->GetOwnerControl(), control, TEST_LOCATION);
    }

    // Control released.
    control.Unparent();
    control.Reset();

    tet_printf("Test effect owner control is empty after control destructed.\n");
    DALI_TEST_CHECK(testEffect);

    {
      DALI_TEST_EQUALS(impl->IsActivated(), false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->mOnActivated, false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);
    }

    control = Control::New();
    control.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    control.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
    scene.Add(control);

    tet_printf("Test effect set again.\n");
    control.SetRenderEffect(testEffect);

    Toolkit::Internal::TestRenderEffectImplPtr impl2 =
      dynamic_cast<Toolkit::Internal::TestRenderEffectImpl*>(control.GetRenderEffect().GetObjectPtr());
    {
      DALI_TEST_EQUALS(impl == impl2, false, TEST_LOCATION);

      DALI_TEST_EQUALS(impl->IsActivated(), false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->mOnActivated, false, TEST_LOCATION);
      DALI_TEST_EQUALS(impl->GetOwnerControl(), Toolkit::Control(), TEST_LOCATION); // may be false?

      DALI_TEST_EQUALS(impl2->IsActivated(), true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl2->mOnActivated, true, TEST_LOCATION);
      DALI_TEST_EQUALS(impl2->GetOwnerControl(), control, TEST_LOCATION);
    }
    // Terminate application.
  }

  END_TEST;
}

int UtcDaliInternalRenderEffectGetOwnerControl02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliInternalRenderEffecGetOwnerControl02");

  Integration::Scene scene = application.GetScene();

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  scene.Add(control1);

  Control control2 = Control::New();
  control2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control2.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  scene.Add(control2);

  Toolkit::TestRenderEffect testEffect = Toolkit::TestRenderEffect::New();
  DALI_TEST_CHECK(testEffect);

  // Check that effect is not activate yet.
  Toolkit::Internal::TestRenderEffectImpl& prototype = Toolkit::Internal::GetImplementation(testEffect);
  DALI_TEST_EQUALS(prototype.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  tet_printf("Test effect set, and activate due to control scene on.\n");
  control1.SetRenderEffect(testEffect);

  Toolkit::Internal::TestRenderEffectImpl& impl1 =
    *(dynamic_cast<Toolkit::Internal::TestRenderEffectImpl*>(control1.GetRenderEffect().GetObjectPtr()));

  DALI_TEST_EQUALS(&prototype == &impl1, false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(prototype.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  DALI_TEST_EQUALS(impl1.IsActivated(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.mOnActivated, true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.GetOwnerControl(), control1, TEST_LOCATION);

  tet_printf("Test effect set to another control\n");
  control2.SetRenderEffect(testEffect);

  Toolkit::Internal::TestRenderEffectImpl& impl2 =
    *(dynamic_cast<Toolkit::Internal::TestRenderEffectImpl*>(control2.GetRenderEffect().GetObjectPtr()));
  DALI_TEST_EQUALS(&impl1 == &impl2, false, TEST_LOCATION);

  DALI_TEST_EQUALS(impl2.IsActivated(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.mOnActivated, true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.GetOwnerControl(), control2, TEST_LOCATION);

  tet_printf("Test control1 call ClearRenderEffect don't have any effort to control2's effect\n");
  control1.ClearRenderEffect();

  DALI_TEST_EQUALS(impl1.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  DALI_TEST_EQUALS(impl2.IsActivated(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.mOnActivated, true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.GetOwnerControl(), control2, TEST_LOCATION);

  tet_printf("Test control2 call ClearRenderEffect\n");
  control2.ClearRenderEffect();

  DALI_TEST_EQUALS(impl2.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl2.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  tet_printf("Reset control1 effect\n");
  control1.SetRenderEffect(testEffect);

  Toolkit::Internal::TestRenderEffectImpl& impl3 =
    *(dynamic_cast<Toolkit::Internal::TestRenderEffectImpl*>(control1.GetRenderEffect().GetObjectPtr()));

  DALI_TEST_EQUALS(&impl1 == &impl3, false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.IsActivated(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.mOnActivated, false, TEST_LOCATION);
  DALI_TEST_EQUALS(impl1.GetOwnerControl(), Toolkit::Control(), TEST_LOCATION);

  DALI_TEST_EQUALS(impl3.IsActivated(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl3.mOnActivated, true, TEST_LOCATION);
  DALI_TEST_EQUALS(impl3.GetOwnerControl(), control1, TEST_LOCATION);

  tet_printf("Test control1 call SetRenderEffect with empty handle\n");
  control1.SetRenderEffect(Toolkit::RenderEffect());
  DALI_TEST_CHECK(!control1.GetRenderEffect());

  END_TEST;
}
