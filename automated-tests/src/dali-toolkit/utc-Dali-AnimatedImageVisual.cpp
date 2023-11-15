/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>

#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void dali_animated_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_animated_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME      = TEST_RESOURCE_DIR "/application-icon-%02d.png";
const char* TEST_GIF_FILE_NAME        = TEST_RESOURCE_DIR "/anim.gif";
const char* TEST_MASK_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/mask.png";
const char* TEST_WEBP_FILE_NAME       = TEST_RESOURCE_DIR "/dali-logo.webp";
} // namespace

void CopyUrlsIntoArray(Property::Array& urls, int startIndex = 0)
{
  for(int i = 20 + startIndex; i <= 30; ++i)
  {
    char* url;
    if(asprintf(&url, TEST_IMAGE_FILE_NAME, i) > 0)
    {
      Property::Value value(url);
      urls.Add(value);
      free(url);
    }
  }
}

int UtcDaliAnimatedImageVisualGetPropertyMap01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualGetPropertyMap");

  // request AnimatedImageVisual with a property map
  VisualFactory factory             = VisualFactory::Get();
  Visual::Base  animatedImageVisual = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
      .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
      .Add(ImageVisual::Property::PIXEL_AREA, Vector4())
      .Add(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT)
      .Add(ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT)
      .Add(ImageVisual::Property::FITTING_MODE, FittingMode::FIT_WIDTH)
      .Add(ImageVisual::Property::SAMPLING_MODE, SamplingMode::NEAREST)
      .Add(ImageVisual::Property::DESIRED_WIDTH, 154)
      .Add(ImageVisual::Property::DESIRED_HEIGHT, 79)
      .Add(ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME)
      .Add(ImageVisual::Property::MASK_CONTENT_SCALE, 1.6f)
      .Add(ImageVisual::Property::CROP_TO_MASK, true)
      .Add(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING)
      .Add(DevelVisual::Property::CORNER_RADIUS, 22.2f)
      .Add(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::ABSOLUTE)
      .Add(DevelVisual::Property::BORDERLINE_WIDTH, 33.3f)
      .Add(DevelVisual::Property::BORDERLINE_COLOR, Color::RED)
      .Add(DevelVisual::Property::BORDERLINE_OFFSET, 0.3f));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::ANIMATED_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_GIF_FILE_NAME);

  value = resultMap.Find(Toolkit::ImageVisual::Property::WRAP_MODE_U, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::REPEAT);

  value = resultMap.Find(Toolkit::ImageVisual::Property::WRAP_MODE_V, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::DEFAULT);

  value = resultMap.Find(Toolkit::ImageVisual::Property::FITTING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == FittingMode::FIT_WIDTH);

  value = resultMap.Find(Toolkit::ImageVisual::Property::SAMPLING_MODE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == SamplingMode::NEAREST);

  value = resultMap.Find(Toolkit::ImageVisual::Property::DESIRED_WIDTH, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 154);

  value = resultMap.Find(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 79);

  value = resultMap.Find(DevelVisual::Property::CORNER_RADIUS, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(22.2f, 22.2f, 22.2f, 22.2f), TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::Transform::Policy::ABSOLUTE);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_WIDTH, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 33.3f, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_COLOR, Property::VECTOR4);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Color::RED, TEST_LOCATION);

  value = resultMap.Find(DevelVisual::Property::BORDERLINE_OFFSET, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.3f, TEST_LOCATION);

  // Check mask properties
  value = resultMap.Find(ImageVisual::Property::ALPHA_MASK_URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_MASK_IMAGE_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::MASK_CONTENT_SCALE, Property::FLOAT);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 1.6f, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::CROP_TO_MASK, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);

  value = resultMap.Find(DevelImageVisual::Property::MASKING_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::MaskingType::MASKING_ON_RENDERING);

  // Natural size getted as desired size
  Vector2 naturalSize;
  animatedImageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(154, 79), TEST_LOCATION);

  // request AnimatedImageVisual with an URL
  Visual::Base animatedImageVisual2 = factory.CreateVisual(TEST_GIF_FILE_NAME, ImageDimensions());
  resultMap.Clear();
  animatedImageVisual2.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::ANIMATED_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_GIF_FILE_NAME);

  // Natural size getted as image size
  animatedImageVisual2.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(50, 50), TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualGetPropertyMap02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualGetPropertyMap for multi image with fixed cache");

  // request AnimatedImageVisual with a property map
  VisualFactory   factory = VisualFactory::Get();
  Property::Array urls;
  CopyUrlsIntoArray(urls);

  Visual::Base animatedImageVisual = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
      .Add("url", urls)
      .Add("batchSize", 4)
      .Add("cacheSize", 20)
      .Add("loopCount", 10)
      .Add("frameDelay", 200)
      .Add("pixelArea", Vector4())
      .Add("wrapModeU", WrapMode::REPEAT)
      .Add("wrapModeV", WrapMode::DEFAULT)
      .Add("fittingMode", FittingMode::FIT_WIDTH)
      .Add("samplingMode", SamplingMode::NEAREST)
      .Add("desiredWidth", 154)
      .Add("desiredHeight", 79)
      .Add("alphaMaskUrl", TEST_MASK_IMAGE_FILE_NAME)
      .Add("maskContentScale", 1.6f)
      .Add("cropToMask", true)
      .Add(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING)
      .Add("cornerRadius", Vector4(50.0f, 25.0f, 12.5f, 33.0f))
      .Add("cornerRadiusPolicy", Visual::Transform::Policy::RELATIVE)
      .Add("borderlineWidth", 20.0f)
      .Add("borderlineColor", Vector4())
      .Add("borderlineOffset", -1.0f));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::ANIMATED_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, "url");
  DALI_TEST_CHECK(value);
  Property::Array* resultUrls = value->GetArray();
  DALI_TEST_CHECK(resultUrls);
  DALI_TEST_EQUALS(resultUrls->Count(), urls.Count(), TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::BATCH_SIZE, "batchSize");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 4, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::CACHE_SIZE, "cacheSize");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 20, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelImageVisual::Property::LOOP_COUNT, "loopCount");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 10, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::FRAME_DELAY, "frameDelay");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 200, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, "totalFrameNumber");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 11, TEST_LOCATION);

  value = resultMap.Find(Toolkit::ImageVisual::Property::WRAP_MODE_U, "wrapModeU");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::REPEAT);

  value = resultMap.Find(Toolkit::ImageVisual::Property::WRAP_MODE_V, "wrapModeV");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == WrapMode::DEFAULT);

  value = resultMap.Find(Toolkit::ImageVisual::Property::FITTING_MODE, "fittingMode");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == FittingMode::FIT_WIDTH);

  value = resultMap.Find(Toolkit::ImageVisual::Property::SAMPLING_MODE, "samplingMode");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == SamplingMode::NEAREST);

  value = resultMap.Find(Toolkit::ImageVisual::Property::DESIRED_WIDTH, "desiredWidth");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 154);

  value = resultMap.Find(Toolkit::ImageVisual::Property::DESIRED_HEIGHT, "desiredHeight");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 79);

  value = resultMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS, "cornerRadius");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(50.0f, 25.0f, 12.5f, 33.0f), TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, "cornerRadiusPolicy");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::Transform::Policy::RELATIVE);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, "borderlineWidth");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 20.0f, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, "borderlineColor");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4::ZERO, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, "borderlineOffset");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), -1.0f, TEST_LOCATION);

  // Check mask properties
  value = resultMap.Find(ImageVisual::Property::ALPHA_MASK_URL, "alphaMaskUrl");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_MASK_IMAGE_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::MASK_CONTENT_SCALE, "maskContentScale");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 1.6f, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::CROP_TO_MASK, "cropToMask");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);

  value = resultMap.Find(DevelImageVisual::Property::MASKING_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == DevelImageVisual::MaskingType::MASKING_ON_RENDERING);

  END_TEST;
}

int UtcDaliAnimatedImageVisualGetPropertyMap03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualGetPropertyMap for multi image rolling cache");

  // request AnimatedImageVisual with a property map
  VisualFactory   factory = VisualFactory::Get();
  Property::Array urls;
  CopyUrlsIntoArray(urls);

  Visual::Base animatedImageVisual = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
      .Add("url", urls)
      .Add("batchSize", 4)
      .Add("cacheSize", 8)
      .Add("loopCount", 10)
      .Add("frameDelay", 200)
      .Add("pixelArea", Vector4())
      .Add("wrapModeU", WrapMode::REPEAT)
      .Add("wrapModeV", WrapMode::DEFAULT)
      .Add("alphaMaskUrl", TEST_MASK_IMAGE_FILE_NAME)
      .Add("maskContentScale", 1.6f)
      .Add("cropToMask", true)
      .Add(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING)
      .Add("cornerRadius", 50.5f));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap(resultMap);
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::ANIMATED_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, "url");
  DALI_TEST_CHECK(value);
  Property::Array* resultUrls = value->GetArray();
  DALI_TEST_CHECK(resultUrls);
  DALI_TEST_EQUALS(resultUrls->Count(), urls.Count(), TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::BATCH_SIZE, "batchSize");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 4, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::CACHE_SIZE, "cacheSize");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 8, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelImageVisual::Property::LOOP_COUNT, "loopCount");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 10, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::FRAME_DELAY, "frameDelay");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 200, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, "totalFrameNumber");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 11, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS, "cornerRadius");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(50.5f, 50.5f, 50.5f, 50.5f), TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, "cornerRadiusPolicy");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::Transform::Policy::ABSOLUTE);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, "borderlineWidth");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.0f, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, "borderlineColor");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Color::BLACK, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, "borderlineOffset");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.0f, TEST_LOCATION);

  // Check mask properties
  value = resultMap.Find(ImageVisual::Property::ALPHA_MASK_URL, "alphaMaskUrl");
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_MASK_IMAGE_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::MASK_CONTENT_SCALE, "maskContentScale");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 1.6f, TEST_LOCATION);

  value = resultMap.Find(ImageVisual::Property::CROP_TO_MASK, "cropToMask");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<bool>(), true, TEST_LOCATION);

  value = resultMap.Find(DevelImageVisual::Property::MASKING_TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == DevelImageVisual::MaskingType::MASKING_ON_RENDERING);

  // Natural size getted as masked image size
  Vector2 naturalSize;
  animatedImageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(100, 100), TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualGetPropertyMap04(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualGetPropertyMap");

  // request AnimatedImageVisual with a property map
  VisualFactory factory             = VisualFactory::Get();
  Visual::Base  animatedImageVisual = factory.CreateVisual(
    Property::Map()
      .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
      .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
      .Add(ImageVisual::Property::BATCH_SIZE, 1)
      .Add(ImageVisual::Property::CACHE_SIZE, 1)
      .Add(ImageVisual::Property::SYNCHRONOUS_LOADING, true)
      .Add(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DETACHED)
      .Add(ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::ATTACHED)
      .Add(DevelVisual::Property::BORDERLINE_WIDTH, 0.4f));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap(resultMap);

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find(Toolkit::Visual::Property::TYPE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Visual::ANIMATED_IMAGE);

  value = resultMap.Find(ImageVisual::Property::URL, Property::STRING);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<std::string>() == TEST_GIF_FILE_NAME);

  value = resultMap.Find(ImageVisual::Property::BATCH_SIZE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 2);

  value = resultMap.Find(ImageVisual::Property::CACHE_SIZE, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == 2);

  value = resultMap.Find(ImageVisual::Property::SYNCHRONOUS_LOADING, Property::BOOLEAN);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<bool>() == true);

  value = resultMap.Find(ImageVisual::Property::RELEASE_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == ImageVisual::ReleasePolicy::DETACHED);

  value = resultMap.Find(ImageVisual::Property::LOAD_POLICY, Property::INTEGER);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == ImageVisual::LoadPolicy::ATTACHED);

  value = resultMap.Find(Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, "totalFrameNumber");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<int>(), 4, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, "borderlineWidth");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.4f, TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, "borderlineColor");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector4>(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  value = resultMap.Find(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, "borderlineOffset");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<float>(), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualImageLoadingFail01(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, "dummy.gif");
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, 0.23f);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::ABSOLUTE);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 1, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 6);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 1, TEST_LOCATION);

    dummyControl.Unparent();
  }

  END_TEST;
}

int UtcDaliAnimatedImageVisualSynchronousLoading(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, 0.23f);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::ABSOLUTE);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    // The first frame is loaded synchronously and load next batch.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 3);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualSynchronousLoadingWithAlphaMask01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualSynchronousLoadingWithAlphaMask01 for CPU Alpha Masking");
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    propertyMap.Insert(ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, 0.23f);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::ABSOLUTE);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    Property::Map testMap;
    visual.CreatePropertyMap(testMap);
    DALI_TEST_EQUALS(*testMap.Find(ImageVisual::Property::ALPHA_MASK_URL), Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    // The first frame is loaded synchronously and load next batch with masking
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualSynchronousLoadingWithAlphaMask02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualSynchronousLoadingWithAlphaMask02 for GPU Alpha Masking");
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    propertyMap.Insert(ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME);
    propertyMap.Insert(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS, 0.23f);
    propertyMap.Insert(DevelVisual::Property::CORNER_RADIUS_POLICY, Visual::Transform::Policy::ABSOLUTE);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    Property::Map testMap;
    visual.CreatePropertyMap(testMap);
    DALI_TEST_EQUALS(*testMap.Find(ImageVisual::Property::ALPHA_MASK_URL), Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    // The first frame is loaded synchronously and load next batch with masking
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualJumpToAction(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 12);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(20);

    tet_infoline("Ready the visual after the visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, Property::Map());

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 6);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(6), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualStopBehavior(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::FIRST_FRAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 8);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(20);

    tet_infoline("Ready the visual after the visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, Property::Map());

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 1);

    // Expect the second batch has been requested
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 4, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualStopBehavior02(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    tet_infoline("Ready the visual after the visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 2, TEST_LOCATION);

    Test::EmitGlobalTimerSignal();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, Property::Map());

    tet_infoline("Ready the visual after the visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualAnimatedImage01(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  tet_infoline("Set cache size same as GIF frame, and try to load same image at another ImageView");
  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    // Batch 2 frames. Now frame 0, 1 cached.
    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 2, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    Test::EmitGlobalTimerSignal();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    // 0 frame removed. and after, batch 2 frames. Now frame 1, 2, 3 cached.
    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);

    Visual::Base        visual2       = factory.CreateVisual(propertyMap);
    DummyControl        dummyControl2 = DummyControl::New(true);
    Impl::DummyControl& dummyImpl2    = static_cast<Impl::DummyControl&>(dummyControl2.GetImplementation());
    dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual2);
    application.GetScene().Add(dummyControl2);

    tet_infoline("Add new view with same url");

    application.SendNotification();
    application.Render();

    // Note that we only re-load 0 frame.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    tet_infoline("Test that we don't try to re-load new image cause it cached");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);

    // Batch 2 frames. Now visual frame 1, 2, 3 cached and visual2 frame 0, 1 cached.
    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 5, TEST_LOCATION);

    textureTrace.Reset();

    tet_infoline("Load some many frames");

    const int repeatCount = 10;
    for(int repeat = 0; repeat < repeatCount; ++repeat)
    {
      Test::EmitGlobalTimerSignal();
      application.SendNotification();
      application.Render(2000);
    }

    DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION); // A new texture should NOT be generated.
    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 5, TEST_LOCATION);

    textureTrace.Reset();

    dummyControl.Unparent();
    dummyControl2.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(20);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask01 for CPU Alpha Masking");
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    application.SendNotification();
    application.Render();

    // load two frame(batch size), load mask image, and request two masking
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(5), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 2, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(20);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask02 for GPU Alpha Masking");
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME);
    propertyMap.Insert(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    Property::Map attributes;
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, attributes);

    application.SendNotification();
    application.Render();

    // load two frame(batch size), load mask image, and request two masking
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(20);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualAnimatedImageWithAlphaMask03 for GPU Alpha Masking with broken mask texture");
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME);
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert(ImageVisual::Property::ALPHA_MASK_URL, "");
    propertyMap.Insert(DevelImageVisual::Property::MASKING_TYPE, DevelImageVisual::MaskingType::MASKING_ON_RENDERING);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);

    application.SendNotification();
    application.Render();

    // load two frame(batch size), load mask image, and request two masking
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 3, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(20);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage01(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 8);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    tet_infoline("Ready the visual after the visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 4, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

    tet_infoline("Test that after 1 tick, and file loads completed, that we have 7 textures");
    Test::EmitGlobalTimerSignal();

    // Expect the second batch has been requested
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 7, TEST_LOCATION);

    tet_infoline("Test that after 2 ticks that we have 6 textures");

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 6, TEST_LOCATION);

    tet_infoline("And that at least 2 textures were requested");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 8, TEST_LOCATION);

    tet_infoline("Test that after 3rd tick that we have 7 textures and 1 request");
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 7, TEST_LOCATION);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 8, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage02(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  tet_infoline("Test that the animated visual has different batch and cache size.");

  {
    Property::Array urls;
    CopyUrlsIntoArray(urls);

    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 0);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 0);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap); // TexMgr::Request load tId:0

    // Check the batch size and cache size need to have minimum 2.
    Property::Map resultMap;
    visual.CreatePropertyMap(resultMap);
    Property::Value* value = resultMap.Find(ImageVisual::Property::BATCH_SIZE, "batchSize");
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<int>(), 2, TEST_LOCATION);
    value = resultMap.Find(ImageVisual::Property::CACHE_SIZE, "cacheSize");
    DALI_TEST_CHECK(value);
    DALI_TEST_EQUALS(value->Get<int>(), 2, TEST_LOCATION);
    visual.Reset();

    // Batch size is 2 and cache size is 3
    propertyMap.Clear();
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 3);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    visual = factory.CreateVisual(propertyMap); // TexMgr::Request load tId:0

    // Expect that each image is loaded each tick
    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl1   = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    visual.Reset();

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    tet_infoline("Ready the visual after the visual is on window");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 1 and 2
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_LOCATION);

    tet_infoline("Test that each tick, a new image is requested");
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:0
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 3
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    tet_infoline("Test that each tick, a new image is requested");
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:1
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 4
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    dummyImpl1.UnregisterVisual(DummyControl::Property::TEST_VISUAL);
    dummyControl.Unparent();

    // Ensure to remove cached texture. (Since we support lazy cache removal)
    application.SendNotification();
    application.Render(16);
    application.SendNotification();
    application.Render(16);

    // Batch size is 9 and cache size is 4
    propertyMap.Clear();
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 3);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 7);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    visual = factory.CreateVisual(propertyMap); // TexMgr::Request load tId:0

    // Expect that each image is loaded each tick
    dummyControl                   = DummyControl::New(true);
    Impl::DummyControl& dummyImpl2 = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);
    visual.Reset();

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    tet_infoline("Ready the visual after the visual is on window");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 1, 2, and 3
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    tet_infoline("Test that each tick, a new image is requested");
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:0
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 4, 5, and 6
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 5, TEST_LOCATION);

    tet_infoline("Test that each tick, a new image is requested");
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:1
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 7, 1, and 2
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 7, TEST_LOCATION);

    tet_infoline("Test that each tick, a new image is requested");
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:2
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16); //glGenTextures 3
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 7, TEST_LOCATION);

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from window deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage03(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  {
    Property::Array urls1, urls2;
    CopyUrlsIntoArray(urls1);
    CopyUrlsIntoArray(urls2);

    Property::Map animatedImageMap1;
    animatedImageMap1.Insert(Visual::Property::TYPE, Visual::IMAGE);
    animatedImageMap1.Insert(ImageVisual::Property::URL, Property::Value(urls1));
    animatedImageMap1.Insert(ImageVisual::Property::BATCH_SIZE, 3);
    animatedImageMap1.Insert(ImageVisual::Property::CACHE_SIZE, 3);
    animatedImageMap1.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    Property::Map animatedImageMap2;
    animatedImageMap2.Insert(Visual::Property::TYPE, Visual::IMAGE);
    animatedImageMap2.Insert(ImageVisual::Property::URL, Property::Value(urls2));
    animatedImageMap2.Insert(ImageVisual::Property::BATCH_SIZE, 2);
    animatedImageMap2.Insert(ImageVisual::Property::CACHE_SIZE, 2);
    animatedImageMap2.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory              = VisualFactory::Get();
    Visual::Base  animatedImageVisual1 = factory.CreateVisual(animatedImageMap1);

    tet_infoline("Create two image views with the same URLs, offset by 1 frame.");

    DummyControl        dummyControl1 = DummyControl::New(true);
    Impl::DummyControl& dummyImpl1    = static_cast<Impl::DummyControl&>(dummyControl1.GetImplementation());
    dummyImpl1.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual1);
    dummyControl1.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl1);

    application.SendNotification();
    application.Render(16);

    tet_infoline("Ready the requested image after the first visual is on stage");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    Visual::Base        animatedImageVisual2 = factory.CreateVisual(animatedImageMap2);
    DummyControl        dummyControl2        = DummyControl::New(true);
    Impl::DummyControl& dummyImpl2           = static_cast<Impl::DummyControl&>(dummyControl2.GetImplementation());
    dummyImpl2.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual2);
    dummyControl2.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl2);
    application.SendNotification();
    application.Render(16);

    tet_infoline("The texture cache should be holding the requested images; check that the renderer has a texture");
    TextureSet ts = dummyControl2.GetRendererAt(0).GetTextures();
    Texture    t1 = ts.GetTexture(0);
    DALI_TEST_EQUALS(ts.GetTextureCount(), 1, TEST_LOCATION);

    tet_infoline("Test that on the first tick, 1 new image is requested");
    Test::EmitGlobalTimerSignal(); // Both visuals should tick

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 3, TEST_LOCATION);

    ts         = dummyControl2.GetRendererAt(0).GetTextures();
    Texture t2 = ts.GetTexture(0);
    DALI_TEST_CHECK(t1 != t2);

    dummyControl1.Unparent();
    dummyControl2.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage04(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl           = application.GetGlAbstraction();
  TraceCallStack&        textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline("Test that if the cache size is the same as the number of urls, that once the cache is full, no new images are loaded");

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 6);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 11);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    tet_infoline("Expect that a batch of 7 textures has been requested.");

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    tet_infoline("Wait for the first batch to complete");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(6), true, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gl.GetLastGenTextureId(), 6, TEST_LOCATION);
    tet_infoline("Test that after 1 tick, and 5 file loads completed, that we have 11 textures");
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    // Expect the second batch has been requested
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(5), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 11, TEST_LOCATION);

    tet_infoline("Test that after 2 ticks that we have 11 textures and no requests");

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 5), false, TEST_LOCATION);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 11, TEST_LOCATION);

    tet_infoline("Test that after 3rd tick that we have 11 textures and no requests");
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 5), false, TEST_LOCATION);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 11, TEST_LOCATION);

    dummyControl.Unparent();
  }

  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage05(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl = application.GetGlAbstraction();

  tet_infoline("Test that if the cache size is the same as the number of urls, that removing a partially loaded visual removes all textures");

  Property::Array urls;
  CopyUrlsIntoArray(urls);

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
    propertyMap.Insert(ImageVisual::Property::URL, Property::Value(urls));
    propertyMap.Insert(ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert(ImageVisual::Property::CACHE_SIZE, 11);
    propertyMap.Insert(ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base  visual  = factory.CreateVisual(propertyMap);

    tet_infoline("Expect that a batch of 4 textures has been requested.");

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    tet_infoline("Wait for the first batch to complete");
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);

    tet_infoline("Test that a timer has been started");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    tet_infoline("Test that a timer has been started");
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    dummyControl.Unparent();
  }

  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  tet_infoline("Test that pending batch of image loads are cancelled instead of uploaded");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(4), true, TEST_LOCATION);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 0, TEST_LOCATION);

  END_TEST;
}

void TestLoopCount(ToolkitTestApplication& application, DummyControl& dummyControl, uint16_t frameCount, uint16_t loopCount, const char* location)
{
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();

  textureTrace.Enable(true);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_INNER_LOCATION(location));

  application.SendNotification();
  application.Render();

  tet_infoline("Test that a timer has been created");
  DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_INNER_LOCATION(location));

  for(uint16_t i = 0; i < loopCount; i++)
  {
    for(uint16_t j = 0; j < frameCount; j++)
    {
      if(i == 0 && j == 0)
      {
        continue; // Because first frame is already showed and we call 2nd frame at the first time of timer animation.
      }
      tet_printf("Test that after %u ticks, and we have %u frame \n", j + 1u, j + 1u);
      Test::EmitGlobalTimerSignal();
      application.SendNotification();
      application.Render(16);

      DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_INNER_LOCATION(location));

      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), 2, TEST_INNER_LOCATION(location));
      DALI_TEST_EQUALS(Test::AreTimersRunning(), true, TEST_INNER_LOCATION(location));
    }
    tet_printf("Test Loop %u \n\n", i + 1u);
  }

  tet_printf("Test that after %u loops, and we have no frame. Timer should stop \n", loopCount);
  Test::EmitGlobalTimerSignal();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(Test::AreTimersRunning(), false, TEST_INNER_LOCATION(location));

  dummyControl.Unparent();
}

int UtcDaliAnimatedImageVisualLoopCount(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliAnimatedImageVisualLoopCount");

  {
    // request AnimatedImageVisual with a property map
    // Test with no (0) loop count
    VisualFactory factory             = VisualFactory::Get();
    Visual::Base  animatedImageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
        .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
        .Add(ImageVisual::Property::PIXEL_AREA, Vector4())
        .Add(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT)
        .Add(ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT)
        .Add(DevelImageVisual::Property::LOOP_COUNT, 0));

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual);
    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    TestLoopCount(application, dummyControl, 4, 0, TEST_LOCATION);

    dummyImpl.UnregisterVisual(DummyControl::Property::TEST_VISUAL);
    animatedImageVisual.Reset();

    application.SendNotification();
    application.Render(16);

    // Test with no (1) loop count. Request AnimatedImageVisual with a property map
    animatedImageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
        .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
        .Add(ImageVisual::Property::PIXEL_AREA, Vector4())
        .Add(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT)
        .Add(ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT)
        .Add(DevelImageVisual::Property::LOOP_COUNT, 1));

    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual);

    TestLoopCount(application, dummyControl, 4, 1, TEST_LOCATION);

    dummyImpl.UnregisterVisual(DummyControl::Property::TEST_VISUAL);
    animatedImageVisual.Reset();

    application.SendNotification();
    application.Render(16);

    // Test with no (100) loop count. Request AnimatedImageVisual with a property map
    animatedImageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
        .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
        .Add(ImageVisual::Property::PIXEL_AREA, Vector4())
        .Add(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT)
        .Add(ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT)
        .Add(DevelImageVisual::Property::LOOP_COUNT, 100));

    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual);

    TestLoopCount(application, dummyControl, 4, 100, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimatedImageVisualPlayback(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction&     gl           = application.GetGlAbstraction();
  TraceCallStack&        textureTrace = gl.GetTextureTrace();

  tet_infoline("UtcDaliAnimatedImageVisualPlayback");

  {
    // request AnimatedImageVisual with a property map
    // Test with forever (-1) loop count
    VisualFactory factory             = VisualFactory::Get();
    Visual::Base  animatedImageVisual = factory.CreateVisual(
      Property::Map()
        .Add(Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE)
        .Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME)
        .Add(ImageVisual::Property::PIXEL_AREA, Vector4())
        .Add(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT)
        .Add(ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT)
        .Add(DevelImageVisual::Property::LOOP_COUNT, -1));

    DummyControl        dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, animatedImageVisual);
    dummyControl.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    textureTrace.Enable(true);
    application.GetScene().Add(dummyControl);
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    tet_infoline("Test that a timer has been created");
    DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::AreTimersRunning(), true, TEST_LOCATION);

    Property::Map attributes;
    tet_infoline("Test Pause action. Timer should stop after Pause action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PAUSE, attributes);
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::AreTimersRunning(), false, TEST_LOCATION);

    tet_infoline("Test Play action. Timer should Restart after Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PLAY, attributes);
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::AreTimersRunning(), true, TEST_LOCATION);

    tet_infoline("Test Stop action. Timer should stop after Stop action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, attributes);
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(Test::AreTimersRunning(), false, TEST_LOCATION);

    tet_infoline("Test Play action. Timer should Restart after Play action");
    DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PLAY, attributes);
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::AreTimersRunning(), true, TEST_LOCATION);

    dummyControl.Unparent();
  }

  END_TEST;
}

int UtcDaliAnimatedImageVisualWrapMode(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualWrapMode");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK(factory);

  // Test wrap mode in animated image visual.
  const int     width  = 950;
  const int     height = 1080;
  const Vector4 pixelArea(0.0f, 0.0f, 950 / 40, 1.0f);

  Property::Map propertyMap;
  propertyMap.Insert(Toolkit::Visual::Property::TYPE, Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_WEBP_FILE_NAME);
  propertyMap.Insert(ImageVisual::Property::PIXEL_AREA, pixelArea);
  propertyMap.Insert(ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK(visual);

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Enable(true);
  texParameterTrace.EnableLogging(true);

  DummyControl      actor     = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(Control::CONTROL_PROPERTY_END_INDEX + 1, visual);
  actor.SetProperty(Actor::Property::SIZE, Vector2(width, height));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetRendererCount() == 1u);

  DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

  // WITHOUT atlasing, the wrapping is handled by setting gl texture parameters
  std::stringstream out;
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_S << ", " << GL_REPEAT;
  DALI_TEST_CHECK(texParameterTrace.FindMethodAndParams("TexParameteri", out.str()));

  // test the uniforms which used to handle the wrap mode
  Renderer renderer = actor.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer);

  Property::Value pixelAreaValue = renderer.GetProperty(renderer.GetPropertyIndex("pixelArea"));
  DALI_TEST_EQUALS(pixelAreaValue.Get<Vector4>(), pixelArea, TEST_LOCATION);

  actor.Unparent();
  DALI_TEST_CHECK(actor.GetRendererCount() == 0u);

  END_TEST;
}

int UtcDaliAnimatedImageVisualDesiredSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliAnimatedImageVisualDesiredSize");

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();

  // Set desiredWidth < 37 and desiredHeight < 50, which is smaller than original image's size.
  int desiredWidth  = 15;
  int desiredHeight = 20;

  Visual::Base visual = VisualFactory::Get().CreateVisual(TEST_GIF_FILE_NAME, ImageDimensions(desiredWidth, desiredHeight));
  DALI_TEST_CHECK(visual);

  DummyControl      actor     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visual);

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Trigger count is 2 - first frame and second frame.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  textureTrace.Enable(true);
  textureTrace.EnableLogging(true);

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

  // Trigger count is 2 - first frame and second frame.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

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
