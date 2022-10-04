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
 */

#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <thread>

#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>
#include <toolkit-vector-animation-renderer.h>
#include "dummy-control.h"

#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_animated_vector_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_animated_vector_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_VECTOR_IMAGE_FILE_NAME            = TEST_RESOURCE_DIR "/insta_camera.json";
const char* TEST_VECTOR_IMAGE_FILE_NAME_FRAME_DROP = "framedrop.json";
const char* TEST_VECTOR_IMAGE_INVALID_FILE_NAME    = "invalid.json";

bool gAnimationFinishedSignalFired = false;

void VisualEventSignal(Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
{
  if(visualIndex == DummyControl::Property::TEST_VISUAL && signalId == DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED)
  {
    gAnimationFinishedSignalFired = true;
  }
}

} // namespace

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetAnimatedVectorImageVisual01: Request animated vector image visual with a json url");

  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions());
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  // Test SetOffScene().
  actor.Unparent();
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetAnimatedVectorImageVisual02: Request animated vector image visual with a Property::Map");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  actor.Unparent();
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetAnimatedVectorImageVisual03: Request animated vector image visual with a Property::Map");

  int             startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack(startFrame);
  playRange.PushBack(endFrame);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(DevelImageVisual::Property::PLAY_RANGE, playRange)
    .Add(DevelVisual::Property::CORNER_RADIUS, 50.0f)
    .Add(DevelVisual::Property::BORDERLINE_WIDTH, 20.0f)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  actor.Unparent();
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetAnimatedVectorImageVisual04: Request animated vector image visual with a Property::Map");

  int             startFrame = 1, endFrame = 3;
  int             desiredWidth = 100, desiredHeight = 150;
  float           cornerRadius     = 22.0f;
  float           borderlineWidth  = 2.0f;
  Vector4         borderlineColor  = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  float           borderlineOffset = 0.1f;
  Property::Array playRange;
  playRange.PushBack(startFrame);
  playRange.PushBack(endFrame);

  Property::Map propertyMap;
  propertyMap.Add("visualType", DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add("url", TEST_VECTOR_IMAGE_FILE_NAME)
    .Add("loopCount", 3)
    .Add("playRange", playRange)
    .Add("stopBehavior", DevelImageVisual::StopBehavior::FIRST_FRAME)
    .Add("loopingMode", DevelImageVisual::LoopingMode::AUTO_REVERSE)
    .Add("redrawInScalingDown", false)
    .Add("cornerRadius", cornerRadius)
    .Add("borderlineWidth", borderlineWidth)
    .Add("borderlineColor", borderlineColor)
    .Add("borderlineOffset", borderlineOffset)
    .Add("desiredWidth", desiredWidth)
    .Add("desiredHeight", desiredHeight);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);

  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  Property::Map resultMap;
  visual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_VECTOR_IMAGE_FILE_NAME);

  value = resultMap.Find(DevelImageVisual::Property::LOOP_COUNT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 3);

  value = resultMap.Find(DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY);
  DALI_TEST_CHECK(value);

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(result->GetElementAt(0).Get<int>() == startFrame);
  DALI_TEST_CHECK(result->GetElementAt(1).Get<int>() == endFrame);

  value = resultMap.Find(DevelImageVisual::Property::STOP_BEHAVIOR, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::StopBehavior::FIRST_FRAME);

  value = resultMap.Find(DevelImageVisual::Property::LOOPING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::LoopingMode::AUTO_REVERSE);

  value = resultMap.Find(DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == false);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(cornerRadius, cornerRadius, cornerRadius, cornerRadius), TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, "cornerRadiusPolicy");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::Transform::Policy::ABSOLUTE);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineWidth, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), borderlineColor, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineOffset, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::DESIRED_WIDTH, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), desiredWidth, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::DESIRED_HEIGHT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), desiredHeight, TEST_LOCATION);

  actor.Unparent();
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualGetPropertyMap01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualGetPropertyMap01");

  int             startFrame = 1, endFrame = 3;
  int             desiredWidth = 100, desiredHeight = 150;
  Vector4         cornerRadius(50.0f, 22.0f, 0.0f, 3.0f);
  float           borderlineWidth  = 2.3f;
  Vector4         borderlineColor  = Vector4(0.3f, 0.3f, 1.0f, 1.0f);
  float           borderlineOffset = 0.3f;
  Property::Array playRange;
  playRange.PushBack(startFrame);
  playRange.PushBack(endFrame);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(DevelImageVisual::Property::PLAY_RANGE, playRange)
    .Add(DevelVisual::Property::CORNER_RADIUS, cornerRadius)
    .Add(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::RELATIVE)
    .Add(DevelVisual::Property::BORDERLINE_WIDTH, borderlineWidth)
    .Add(DevelVisual::Property::BORDERLINE_COLOR, borderlineColor)
    .Add(DevelVisual::Property::BORDERLINE_OFFSET, borderlineOffset)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false)
    .Add(ImageVisual::Property::DESIRED_WIDTH, desiredWidth)
    .Add(ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);

  // request AnimatedVectorImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(propertyMap);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Property::Map resultMap;
  resultMap = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_VECTOR_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_VECTOR_IMAGE_FILE_NAME);

  value = resultMap.Find(DevelImageVisual::Property::LOOP_COUNT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 3);

  value = resultMap.Find(DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY);
  DALI_TEST_CHECK(value);

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(result->GetElementAt(0).Get<int>() == startFrame);
  DALI_TEST_CHECK(result->GetElementAt(1).Get<int>() == endFrame);

  value = resultMap.Find(DevelImageVisual::Property::STOP_BEHAVIOR, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::StopBehavior::CURRENT_FRAME);

  value = resultMap.Find(DevelImageVisual::Property::LOOPING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::LoopingMode::RESTART);

  value = resultMap.Find(DevelImageVisual::Property::CONTENT_INFO, Property::MAP);
  DALI_TEST_CHECK(value);

  value = resultMap.Find(DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == true); // Check default value

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), cornerRadius, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS_POLICY, "cornerRadiusPolicy");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::Transform::Policy::RELATIVE);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, "borderlineWidth");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineWidth, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), borderlineColor, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), borderlineOffset, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::DESIRED_WIDTH, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), desiredWidth, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::DESIRED_HEIGHT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), desiredHeight, TEST_LOCATION);

  // request AnimatedVectorImageVisual with an URL
  Visual::Base visual2 = factory.CreateVisual(TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions());

  resultMap.Clear();
  visual2.CreatePropertyMap(resultMap);

  // check the property values from the returned map from a visual
  value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelVisual::ANIMATED_VECTOR_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_VECTOR_IMAGE_FILE_NAME);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPlayback(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliAnimatedVectorImageVisualPlayback");

  {
    // request AnimatedVectorImageVisual with a property map
    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
        .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
        .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false));

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    Property::Map attributes;
    tet_infoline("Test Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    Property::Map    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    Property::Value* value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::PLAYING);

    tet_infoline("Test Pause action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PAUSE, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::PAUSED);

    tet_infoline("Test Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::PLAYING);

    tet_infoline("Test Stop action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::STOPPED);

    tet_infoline("Test Stop action again");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::STOPPED);

    tet_infoline("Test Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::PLAYING);

    tet_infoline("Off stage");
    dummyControl.Unparent();

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::STOPPED);

    tet_infoline("On stage again");
    application.GetScene().Add(dummyControl);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::STOPPED);

    tet_infoline("Test Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

    application.SendNotification();
    application.Render(16);

    map   = dummyControl.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
    value = map.Find(DevelImageVisual::Property::PLAY_STATE);
    DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::PLAYING);

    // Change Size
    Vector3 newSize(100.0f, 100.0f, 0.0f);
    dummyControl.SetProperty(Actor::Property::SIZE, newSize);

    application.SendNotification();
    application.Render(16);

    // Size should be changed
    Vector3 naturalSize = dummyControl.GetNaturalSize();
    DALI_TEST_CHECK(naturalSize == newSize);

    dummyControl.Unparent();
  }

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualCustomShader Test custom shader");

  VisualFactory     factory = VisualFactory::Get();
  Property::Map     properties;
  Property::Map     shader;
  const std::string vertexShader                    = "Foobar";
  const std::string fragmentShader                  = "Foobar sampler2D Foobar";
  shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

  properties[Visual::Property::TYPE]     = Visual::IMAGE;
  properties[Visual::Property::SHADER]   = shader;
  properties[ImageVisual::Property::URL] = TEST_VECTOR_IMAGE_FILE_NAME;

  Visual::Base visual = factory.CreateVisual(properties);

  // trigger creation through setting on stage
  DummyControl        dummy     = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummy.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  dummy.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(dummy);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Renderer        renderer = dummy.GetRendererAt(0);
  Shader          shader2  = renderer.GetShader();
  Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map      = value.GetMap();
  DALI_TEST_CHECK(map);

  std::string      resultFragmentShader, resultVertexShader;
  Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
  fragment->Get(resultFragmentShader);
  DALI_TEST_CHECK(resultFragmentShader.find(fragmentShader) != std::string::npos);

  Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
  vertex->Get(resultVertexShader);
  DALI_TEST_CHECK(resultVertexShader.find(vertexShader) != std::string::npos);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualNaturalSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualNaturalSize");

  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
      .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
      .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false));
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  Vector2 naturalSize;

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - load
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  visual.GetNaturalSize(naturalSize);

  DALI_TEST_EQUALS(naturalSize, Vector2(100.0f, 100.0f), TEST_LOCATION); // 100x100 is the content default size.

  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.SendNotification();
  application.Render();

  visual.GetNaturalSize(naturalSize);

  DALI_TEST_EQUALS(naturalSize, controlSize, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualLoopCount(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualLoopCount");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPlayRange(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualPlayRange");

  int             startFrame = 1, endFrame = 3;
  Property::Array array;
  array.PushBack(endFrame);
  array.PushBack(startFrame);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::PLAY_RANGE, array)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  Property::Map    map              = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value            = map.Find(DevelImageVisual::Property::PLAY_RANGE);
  int              totalFrameNumber = map.Find(DevelImageVisual::Property::TOTAL_FRAME_NUMBER)->Get<int>();

  int              resultStartFrame, resultEndFrame;
  Property::Array* result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(startFrame, resultStartFrame, TEST_LOCATION);
  DALI_TEST_EQUALS(endFrame, resultEndFrame, TEST_LOCATION);

  // Set invalid play range
  array.Clear();
  array.PushBack(1);
  array.PushBack(100);

  attributes.Clear();
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(resultStartFrame, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(resultEndFrame, totalFrameNumber - 1, TEST_LOCATION); // Should be clamped

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PAUSE, Property::Map());

  application.SendNotification();
  application.Render();

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 3, TEST_LOCATION);

  array.Clear();
  array.PushBack(0);
  array.PushBack(2);

  attributes.Clear();
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(0, resultStartFrame, TEST_LOCATION);
  DALI_TEST_EQUALS(2, resultEndFrame, TEST_LOCATION);

  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 2, TEST_LOCATION); // CURRENT_FRAME_NUMBER should be changed also.

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPlayRangeMarker(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualPlayRangeMarker");

  Property::Array array;
  array.PushBack(VECTOR_ANIMATION_MARKER_NAME_1);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::PLAY_RANGE, array)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // renderer is added to actor
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  int              resultStartFrame, resultEndFrame;
  Property::Array* result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_START_FRAME_1, resultStartFrame, TEST_LOCATION);
  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_END_FRAME_1, resultEndFrame, TEST_LOCATION);

  // Set 2 markers
  array.Clear();
  array.PushBack(VECTOR_ANIMATION_MARKER_NAME_1);
  array.PushBack(VECTOR_ANIMATION_MARKER_NAME_2);

  attributes.Clear();
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_START_FRAME_1, resultStartFrame, TEST_LOCATION);
  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_END_FRAME_2, resultEndFrame, TEST_LOCATION);

  // Set invalid play range
  array.Clear();
  array.PushBack(1);
  array.PushBack(VECTOR_ANIMATION_MARKER_NAME_1);

  attributes.Clear();
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  result = value->GetArray();
  result->GetElementAt(0).Get(resultStartFrame);
  result->GetElementAt(1).Get(resultEndFrame);

  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_START_FRAME_1, resultStartFrame, TEST_LOCATION); // Should not be changed
  DALI_TEST_EQUALS(VECTOR_ANIMATION_MARKER_END_FRAME_2, resultEndFrame, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualAnimationFinishedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualAnimationFinishedSignal");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  DevelControl::VisualEventSignal(actor).Connect(&VisualEventSignal);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Wait for animation finish - render, finish
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::PLAY_STATE);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::PlayState::STOPPED);

  DALI_TEST_EQUALS(gAnimationFinishedSignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualJumpTo(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualJumpTo");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  tet_printf("1. Visual is created.\n");

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 2);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 2, TEST_LOCATION);

  tet_printf("2. The current frame number is [%d].\n", value->Get<int>());

  Property::Array array;
  array.PushBack(0);
  array.PushBack(2);

  Property::Map attributes;
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 2, TEST_LOCATION);

  tet_printf("3. The current frame number is [%d].\n", value->Get<int>());

  // Change play range
  attributes.Clear();
  array.Clear();

  array.PushBack(0);
  array.PushBack(4);

  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, array);
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  attributes.Clear();
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Wait for animation finish
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Jump to 3
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 3, TEST_LOCATION);

  tet_printf("4. The current frame number is [%d].\n", value->Get<int>());

  // Jump to the same position
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 3, TEST_LOCATION);

  tet_printf("5. The current frame number is [%d].\n", value->Get<int>());

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualUpdateProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualUpdateProperty");

  int             startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack(startFrame);
  playRange.PushBack(endFrame);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(DevelImageVisual::Property::PLAY_RANGE, playRange)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::LOOP_COUNT);
  DALI_TEST_EQUALS(value->Get<int>(), 3, TEST_LOCATION);

  value = map.Find(DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY);
  DALI_TEST_CHECK(value);

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(result->GetElementAt(0).Get<int>() == startFrame);
  DALI_TEST_CHECK(result->GetElementAt(1).Get<int>() == endFrame);

  playRange.Clear();
  playRange.PushBack(0);
  playRange.PushBack(2);

  Property::Map attributes;
  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, playRange);
  attributes.Add(DevelImageVisual::Property::LOOP_COUNT, 5);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::LOOP_COUNT);
  DALI_TEST_EQUALS(value->Get<int>(), 5, TEST_LOCATION);

  value  = map.Find(DevelImageVisual::Property::PLAY_RANGE);
  result = value->GetArray();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(result->GetElementAt(0).Get<int>() == 0);
  DALI_TEST_CHECK(result->GetElementAt(1).Get<int>() == 2);

  attributes.Clear();

  playRange.Clear();
  playRange.PushBack(startFrame);
  playRange.PushBack(endFrame);

  attributes.Add(DevelImageVisual::Property::PLAY_RANGE, playRange);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::PLAY_RANGE);

  result = value->GetArray();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(result->GetElementAt(0).Get<int>() == startFrame);
  DALI_TEST_CHECK(result->GetElementAt(1).Get<int>() == endFrame);

  // Play and update property
  attributes.Clear();
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  attributes.Add(DevelImageVisual::Property::LOOP_COUNT, 10);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  application.SendNotification();
  application.Render();

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::LOOP_COUNT);
  DALI_TEST_EQUALS(value->Get<int>(), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualStopBehavior(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualStopBehavior");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::FIRST_FRAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 3 - load, render, animation finished
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 0, TEST_LOCATION); // Should be the first frame

  // Change stop behavior
  attributes.Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  attributes.Clear();

  // Play again
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);

  Property::Value* value1           = map.Find(DevelImageVisual::Property::TOTAL_FRAME_NUMBER);
  int              totalFrameNumber = value1->Get<int>();

  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), totalFrameNumber - 1, TEST_LOCATION); // Should be the last frame

  // Change stop behavior
  attributes.Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::CURRENT_FRAME);
  attributes.Add(DevelImageVisual::Property::LOOP_COUNT, -1);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  attributes.Clear();

  // Play again
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Pause
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PAUSE, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map                    = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value                  = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  int currentFrameNumber = value->Get<int>();

  // Stop
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes);

  // To make event trigger
  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), currentFrameNumber, TEST_LOCATION); // Should be same with currentFrameNumber

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualLoopingMode(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualLoopingMode");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(DevelImageVisual::Property::LOOP_COUNT, 3)
    .Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME)
    .Add(DevelImageVisual::Property::LOOPING_MODE, DevelImageVisual::LoopingMode::AUTO_REVERSE)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 3 - load, render, animation finished
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  Property::Map    map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 0, TEST_LOCATION); // Should be the first frame because of auto reverse

  // Change stop behavior
  attributes.Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::CURRENT_FRAME);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // Play again
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), 0, TEST_LOCATION); // Should be the first frame

  // Change looping mode
  attributes.Add(DevelImageVisual::Property::LOOPING_MODE, DevelImageVisual::LoopingMode::RESTART);

  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  // Play again
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Property::Value* value1           = map.Find(DevelImageVisual::Property::TOTAL_FRAME_NUMBER);
  int              totalFrameNumber = value1->Get<int>();

  map   = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  value = map.Find(DevelImageVisual::Property::CURRENT_FRAME_NUMBER);
  DALI_TEST_EQUALS(value->Get<int>(), totalFrameNumber - 1, TEST_LOCATION); // Should be the last frame

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPropertyNotification(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualPropertyNotification");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  auto textureSet = renderer.GetTextures();
  auto texture    = textureSet.GetTexture(0);

  DALI_TEST_EQUALS(controlSize.width, texture.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(controlSize.height, texture.GetHeight(), TEST_LOCATION);

  // Change scale
  Vector3 controlScale(2.0f, 2.0f, 1.0f);
  actor.SetProperty(Actor::Property::SCALE, controlScale);

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  textureSet = renderer.GetTextures();
  texture    = textureSet.GetTexture(0);

  DALI_TEST_EQUALS(controlSize.width * controlScale.width, texture.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(controlSize.height * controlScale.height, texture.GetHeight(), TEST_LOCATION);

  // Size animation
  controlSize         = Vector2(50.0f, 100.0f);
  Animation animation = Animation::New(1.0);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), Vector3(controlSize.x, controlSize.y, 0.0f));
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  textureSet = renderer.GetTextures();
  texture    = textureSet.GetTexture(0);

  DALI_TEST_EQUALS(controlSize.width * controlScale.width, texture.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(controlSize.height * controlScale.height, texture.GetHeight(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualMultipleInstances(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualMultipleInstances");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual1 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual1);

  DummyControl      actor1     = DummyControl::New(true);
  DummyControlImpl& dummyImpl1 = static_cast<DummyControlImpl&>(actor1.GetImplementation());
  dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual1);

  Vector2 controlSize(20.f, 30.f);
  actor1.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor1);

  propertyMap.Clear();
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual2 = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual2);

  DummyControl      actor2     = DummyControl::New(true);
  DummyControlImpl& dummyImpl2 = static_cast<DummyControlImpl&>(actor2.GetImplementation());
  dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);

  actor2.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor2);

  application.SendNotification();
  application.Render();

  // Trigger count is 4 - load & render a frame for each instance
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

  DevelControl::DoAction(actor2, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, Property::Map());

  // To make event trigger
  actor2.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Property::Map attributes;
  attributes.Add(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME);

  DevelControl::DoAction(actor1, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);
  DevelControl::DoAction(actor2, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelVisual::Action::UPDATE_PROPERTY, attributes);

  DevelControl::DoAction(actor1, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, Property::Map());

  // renderer is added to actor
  DALI_TEST_CHECK(actor1.GetRendererCount() == 1u);
  Renderer renderer1 = actor1.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer1);

  // renderer is added to actor
  DALI_TEST_CHECK(actor2.GetRendererCount() == 1u);
  Renderer renderer2 = actor2.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer2);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualControlVisibilityChanged(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualControlVisibilityChanged");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Check rendering behavior
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_CHECK(renderer.GetProperty<int>(DevelRenderer::Property::RENDERING_BEHAVIOR) == DevelRenderer::Rendering::CONTINUOUSLY);

  actor.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render();

  // Check rendering behavior again
  DALI_TEST_CHECK(renderer.GetProperty<int>(DevelRenderer::Property::RENDERING_BEHAVIOR) == DevelRenderer::Rendering::IF_REQUIRED);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualWindowVisibilityChanged(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualWindowVisibilityChanged");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Check rendering behavior
  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_CHECK(renderer.GetProperty<int>(DevelRenderer::Property::RENDERING_BEHAVIOR) == DevelRenderer::Rendering::CONTINUOUSLY);

  Window window = DevelWindow::Get(actor);
  window.Hide();

  application.SendNotification();
  application.Render();

  // Check rendering behavior again
  DALI_TEST_CHECK(renderer.GetProperty<int>(DevelRenderer::Property::RENDERING_BEHAVIOR) == DevelRenderer::Rendering::IF_REQUIRED);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualInvalidFile01(void)
{
  ToolkitTestApplication application;
  tet_infoline("Request loading with invalid file - should draw broken image");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_INVALID_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - load
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check resource status
  Visual::ResourceStatus status = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_EQUALS(status, Visual::ResourceStatus::FAILED, TEST_LOCATION);

  // The broken image should be shown.
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualInvalidFile02(void)
{
  ToolkitTestApplication application;
  tet_infoline("Request loading with invalid file - should draw broken image");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_INVALID_FILE_NAME);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));

  application.SendNotification();
  application.Render();

  // Add to the Scene after loading
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Check resource status
  Visual::ResourceStatus status = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_EQUALS(status, Visual::ResourceStatus::FAILED, TEST_LOCATION);

  // The broken image should be shown.
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualInvalidFile03(void)
{
  ToolkitTestApplication application;
  tet_infoline("Request loading with invalid file without size set - should draw broken image");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_INVALID_FILE_NAME)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - load
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Check resource status
  Visual::ResourceStatus status = actor.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL);
  DALI_TEST_EQUALS(status, Visual::ResourceStatus::FAILED, TEST_LOCATION);

  // The broken image should be shown.
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualFrameDrops(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualFrameDrops");

  Property::Map propertyMap;
  propertyMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
    .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME_FRAME_DROP)
    .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false);

  Visual::Base visual = VisualFactory::Get().CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  Vector2 controlSize(20.f, 30.f);
  actor.SetProperty(Actor::Property::SIZE, controlSize);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load, render the first frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  Property::Map    map              = actor.GetProperty<Property::Map>(DummyControl::Property::TEST_VISUAL);
  Property::Value* value            = map.Find(DevelImageVisual::Property::TOTAL_FRAME_NUMBER);
  int              totalFrameNumber = value->Get<int>();

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  // Make delay to drop frames
  Test::VectorAnimationRenderer::DelayRendering(170); // longer than 16.6 * 10frames

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - calculating frame drops
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Check dropped frame
  uint32_t frames = Test::VectorAnimationRenderer::GetDroppedFrames();
  DALI_TEST_CHECK(frames > 0);
  DALI_TEST_CHECK(frames <= static_cast<uint32_t>(totalFrameNumber));

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualSize");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();

  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions());
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - resource ready
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  textureTrace.Enable(true);

  application.SendNotification();
  application.Render();

  {
    int               width = 100, height = 100; // 100x100 is the content default size.
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - resource ready
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  textureTrace.Reset();

  application.SendNotification();
  application.Render();

  {
    int               width = 200, height = 200;
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << width << ", " << height;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

namespace
{
bool gDynamicPropertyCallbackFired = false;

Property::Value FillColorCallback(int32_t id, VectorAnimationRenderer::VectorProperty property, uint32_t frameNumber)
{
  gDynamicPropertyCallbackFired = true;

  if(frameNumber < 3)
  {
    return Vector3(0, 0, 1);
  }
  else
  {
    return Vector3(1, 0, 0);
  }
}
} // namespace

int UtcDaliAnimatedVectorImageVisualDynamicProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualDynamicProperty");

  VisualFactory factory = VisualFactory::Get();
  Visual::Base  visual  = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE)
      .Add(ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME)
      .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, false));
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.GetScene().Add(actor);

  gDynamicPropertyCallbackFired = false;

  // Set dynamic property
  DevelAnimatedVectorImageVisual::DynamicPropertyInfo info;
  info.id       = 1;
  info.keyPath  = "Test.Path";
  info.property = static_cast<int>(VectorAnimationRenderer::VectorProperty::FILL_COLOR);
  info.callback = MakeCallback(FillColorCallback);

  DevelControl::DoActionExtension(actor, DummyControl::Property::TEST_VISUAL, DevelAnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY, Any(info));

  Property::Map attributes;
  DevelControl::DoAction(actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - load & render a frame
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  // Test whether the property callback is called
  DALI_TEST_EQUALS(gDynamicPropertyCallbackFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualDesiredSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedVectorImageVisualDesiredSize");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  int                desiredWidth = 150, desiredHeight = 200;

  Visual::Base visual = VisualFactory::Get().CreateVisual(TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions(desiredWidth, desiredHeight));
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - resource ready
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  textureTrace.Enable(true);

  application.SendNotification();
  application.Render();

  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << desiredWidth << ", " << desiredHeight;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  // Unparent to make next trigger
  actor.Unparent();

  application.SendNotification();
  application.Render();

  // Set visual size
  actor.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - resource ready
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  textureTrace.Reset();

  application.SendNotification();
  application.Render();

  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << desiredWidth << ", " << desiredHeight;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str())); // The size should not be changed
  }

  END_TEST;
}
