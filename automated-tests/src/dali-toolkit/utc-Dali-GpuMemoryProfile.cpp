/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-manager.h>
#include <dali-toolkit/public-api/image-loader/image-url-utils.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>
#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-texture-upload-manager.h>

using namespace Dali;

namespace
{
void SetGpuMemoryProfileEnvironment(const char* value)
{
#if defined(_MSC_VER)
  _putenv_s("DALI_GPU_MEMORY_PROFILE_ENABLED", value ? value : "");
#else
  if(value)
  {
    setenv("DALI_GPU_MEMORY_PROFILE_ENABLED", value, 1);
  }
  else
  {
    unsetenv("DALI_GPU_MEMORY_PROFILE_ENABLED");
  }
#endif
}
} // namespace

void gpu_memory_profile_startup(void)
{
  test_return_value = TET_UNDEF;
  SetGpuMemoryProfileEnvironment("1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_GPU_MEMORY_PROFILE_ENABLED", "1");
}

void gpu_memory_profile_cleanup(void)
{
  SetGpuMemoryProfileEnvironment(nullptr);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_GPU_MEMORY_PROFILE_ENABLED", nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliToolkitGpuMemoryProfileVisuals(void)
{
  ToolkitTestApplication application;

  Toolkit::ImageView svg = Toolkit::ImageView::New();
  Property::Map      svgImage;
  svgImage.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  svgImage.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");
  svgImage.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  svg.SetProperty(Toolkit::ImageView::Property::IMAGE, svgImage);
  application.GetScene().Add(svg);
  DALI_TEST_CHECK(DevelActor::GetNaturalSize(svg).width > 0.0f);

  Toolkit::ImageView jpeg = Toolkit::ImageView::New();
  Property::Map      jpegImage;
  jpegImage.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  jpegImage.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg");
  jpegImage.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  jpeg.SetProperty(Toolkit::ImageView::Property::IMAGE, jpegImage);
  application.GetScene().Add(jpeg);
  DALI_TEST_CHECK(DevelActor::GetNaturalSize(jpeg).width > 0.0f);

  Toolkit::TextLabel label = Toolkit::TextLabel::New("GPU memory profile text");
  application.GetScene().Add(label);

  Property::Map textProperties;
  textProperties.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT);
  textProperties.Insert(Toolkit::TextVisual::Property::TEXT, "GPU memory profile TextVisual");
  Toolkit::Visual::Base textVisual  = Toolkit::VisualFactory::Get().CreateVisual(textProperties);
  Toolkit::Control      textControl = Toolkit::Control::New();
  textControl.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 60.0f));
  Toolkit::DevelControl::RegisterVisual(Toolkit::GetImplementation(textControl), 1001, textVisual);
  application.GetScene().Add(textControl);

  Property::Map gradientProperties;
  gradientProperties.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::GRADIENT);
  gradientProperties.Insert(Toolkit::GradientVisual::Property::START_POSITION, Vector2(-0.5f, -0.5f));
  gradientProperties.Insert(Toolkit::GradientVisual::Property::END_POSITION, Vector2(0.5f, 0.5f));
  gradientProperties.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, Property::Array().Add(Color::RED).Add(Color::GREEN));
  Toolkit::Visual::Base gradientVisual  = Toolkit::VisualFactory::Get().CreateVisual(gradientProperties);
  Toolkit::Control      gradientControl = Toolkit::Control::New();
  gradientControl.SetProperty(Actor::Property::SIZE, Vector2(160.0f, 80.0f));
  Toolkit::DevelControl::RegisterVisual(Toolkit::GetImplementation(gradientControl), 1002, gradientVisual);
  application.GetScene().Add(gradientControl);

  Toolkit::ImageView patch = Toolkit::ImageView::New();
  patch.SetImage(TEST_RESOURCE_DIR "/heartsframe.9.png");
  patch.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(patch);

  Toolkit::AtlasManager atlas = Toolkit::AtlasManager::New();
  atlas.CreateAtlas({64u, 64u, 8u, 8u}, Pixel::RGBA8888);
  application.SendNotification();
  application.Render();

  // Advance the fake three-second profiler timer for roughly twelve seconds.
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileEffects(void)
{
  ToolkitTestApplication application;

  Toolkit::Control backgroundBlur = Toolkit::Control::New();
  backgroundBlur.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  application.GetScene().Add(backgroundBlur);
  backgroundBlur.SetRenderEffect(Toolkit::BackgroundBlurEffect::New());

  Toolkit::Control gaussianBlur = Toolkit::Control::New();
  gaussianBlur.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  application.GetScene().Add(gaussianBlur);
  gaussianBlur.SetRenderEffect(Toolkit::GaussianBlurEffect::New(8u));

  Toolkit::Control tinyBackgroundBlur = Toolkit::Control::New();
  tinyBackgroundBlur.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  application.GetScene().Add(tinyBackgroundBlur);
  tinyBackgroundBlur.SetRenderEffect(Toolkit::BackgroundBlurEffect::New(1u));

  Toolkit::Control tinyGaussianBlur = Toolkit::Control::New();
  tinyGaussianBlur.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  application.GetScene().Add(tinyGaussianBlur);
  tinyGaussianBlur.SetRenderEffect(Toolkit::GaussianBlurEffect::New(1u));

  Toolkit::Control maskSource = Toolkit::Control::New();
  maskSource.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  application.GetScene().Add(maskSource);
  Toolkit::Control masked = Toolkit::Control::New();
  masked.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  application.GetScene().Add(masked);
  masked.SetRenderEffect(Toolkit::MaskEffect::New(maskSource));

  Toolkit::Control offscreen = Toolkit::Control::New();
  offscreen.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  offscreen.SetBackgroundColor(Color::RED);
  application.GetScene().Add(offscreen);
  offscreen.SetProperty(Toolkit::DevelControl::Property::OFFSCREEN_RENDERING, Toolkit::DevelControl::OffScreenRenderingType::REFRESH_ALWAYS);

  application.SendNotification();
  application.Render();
  application.GetPlatform().TriggerTimer();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileImagePaths(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", "1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1");
  ToolkitTestApplication application;
  Test::TextureUploadManager::InitalizeGraphicsController(application.GetGraphicsController());

  auto*     pixels = static_cast<unsigned char*>(std::calloc(64u, 1u));
  PixelData data   = PixelData::New(pixels, 64u, 4u, 4u, Pixel::RGBA8888, PixelData::FREE);
  DALI_TEST_CHECK(Toolkit::ImageUrlUtils::GenerateUrl(data));

  Toolkit::ImageView animated = Toolkit::ImageView::New();
  Property::Map      animatedImage;
  animatedImage.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
  animatedImage.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/anim.gif");
  animatedImage.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, TEST_RESOURCE_DIR "/mask.png");
  animatedImage.Insert(Toolkit::DevelImageVisual::Property::MASKING_TYPE, Toolkit::DevelImageVisual::MaskingType::MASKING_ON_RENDERING);
  animatedImage.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  animated.SetProperty(Toolkit::ImageView::Property::IMAGE, animatedImage);
  application.GetScene().Add(animated);

  Toolkit::ImageView fastTrack = Toolkit::ImageView::New();
  Property::Map      fastTrackImage;
  fastTrackImage.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  fastTrackImage.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1-yuv420.jpg");
  fastTrackImage.Insert(Toolkit::DevelImageVisual::Property::FAST_TRACK_UPLOADING, true);
  fastTrack.SetProperty(Toolkit::ImageView::Property::IMAGE, fastTrackImage);
  application.GetScene().Add(fastTrack);

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2, 5, true));
  Dali::Devel::TextureUploadManager::Get().ResourceUpload();
  application.SendNotification();
  application.Render();
  application.GetPlatform().TriggerTimer();
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", nullptr);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", nullptr);
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileMaskedAnimated(void)
{
  ToolkitTestApplication application;
  Toolkit::ImageView     imageView = Toolkit::ImageView::New();
  Property::Map          image;
  image.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
  image.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/anim.gif");
  image.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, TEST_RESOURCE_DIR "/mask.png");
  image.Insert(Toolkit::DevelImageVisual::Property::MASKING_TYPE, Toolkit::DevelImageVisual::MaskingType::MASKING_ON_LOADING);
  image.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, image);
  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileSynchronousAnimatedMask(void)
{
  ToolkitTestApplication application;

  for(auto maskingType : {Toolkit::DevelImageVisual::MaskingType::MASKING_ON_LOADING,
                          Toolkit::DevelImageVisual::MaskingType::MASKING_ON_RENDERING})
  {
    Toolkit::ImageView imageView = Toolkit::ImageView::New();
    Property::Map      image;
    image.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
    image.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/anim.gif");
    image.Insert(Toolkit::ImageVisual::Property::ALPHA_MASK_URL, TEST_RESOURCE_DIR "/mask.png");
    image.Insert(Toolkit::DevelImageVisual::Property::MASKING_TYPE, maskingType);
    image.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, image);
    imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(imageView.GetRendererCount() > 0u);
    const auto textureCount = imageView.GetRendererAt(0u).GetTextures().GetTextureCount();
    DALI_TEST_EQUALS(textureCount, maskingType == Toolkit::DevelImageVisual::MaskingType::MASKING_ON_RENDERING ? 2u : 1u, TEST_LOCATION);
  }

  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileYuvWebpAndJpeg(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", "1");
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", "1");
  ToolkitTestApplication application;

  const char* urls[] = {TEST_RESOURCE_DIR "/application-icon-45.webp", TEST_RESOURCE_DIR "/gallery-small-1-yuv420.jpg"};
  for(unsigned int index = 0u; index < 2u; ++index)
  {
    Property::Map image;
    image.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    image.Insert(Toolkit::ImageVisual::Property::URL, urls[index]);
    image.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    Toolkit::Visual::Base visual  = Toolkit::VisualFactory::Get().CreateVisual(image, Toolkit::VisualFactory::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
    Toolkit::Control      control = Toolkit::Control::New();
    control.SetProperty(Actor::Property::SIZE, Vector2(128.0f, 128.0f));
    Toolkit::DevelControl::RegisterVisual(Toolkit::GetImplementation(control), 1003u + index, visual);
    application.GetScene().Add(control);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(control.GetRendererCount() > 0u);
    DALI_TEST_EQUALS(control.GetRendererAt(0u).GetTextures().GetTextureCount(), index == 0u ? 4u : 3u, TEST_LOCATION);
  }

  Toolkit::ImageView animated = Toolkit::ImageView::New();
  Property::Map      animatedImage;
  animatedImage.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
  animatedImage.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/application-icon-45.webp");
  animatedImage.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  animated.SetProperty(Toolkit::ImageView::Property::IMAGE, animatedImage);
  animated.SetProperty(Actor::Property::SIZE, Vector2(128.0f, 128.0f));
  application.GetScene().Add(animated);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(animated.GetRendererCount() > 0u);
  DALI_TEST_EQUALS(animated.GetRendererAt(0u).GetTextures().GetTextureCount(), 4u, TEST_LOCATION);

  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_LOAD_IMAGE_YUV_PLANES", nullptr);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_ENABLE_DECODE_JPEG_TO_YUV_420", nullptr);
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileNPatchUpload(void)
{
  ToolkitTestApplication application;
  Property::Map          image;
  image.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::N_PATCH);
  image.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/heartsframe.9.png");
  Toolkit::Visual::Base visual  = Toolkit::VisualFactory::Get().CreateVisual(image);
  Toolkit::Control      control = Toolkit::Control::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  Toolkit::DevelControl::RegisterVisual(Toolkit::GetImplementation(control), 1005, visual);
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(control.GetRendererCount() > 0u);
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileAsyncSvg(void)
{
  ToolkitTestApplication application;
  Toolkit::ImageView     svg = Toolkit::ImageView::New();
  Property::Map          image;
  image.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  image.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");
  svg.SetProperty(Toolkit::ImageView::Property::IMAGE, image);
  application.GetScene().Add(svg);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2, 5, true));
  application.SendNotification();
  application.Render();
  for(unsigned int tick = 0u; tick < 4u; ++tick)
  {
    application.GetPlatform().TriggerTimer();
  }
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliToolkitGpuMemoryProfileAutoScroll(void)
{
  ToolkitTestApplication application;

  for(bool async : {false, true})
  {
    Toolkit::TextLabel label = Toolkit::TextLabel::New("A long GPU memory profile text label that should scroll across the control");
    label.SetProperty(Actor::Property::SIZE, Vector2(80.0f, 30.0f));
    label.SetProperty(Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL, true);
    if(async)
    {
      label.SetProperty(Toolkit::DevelTextLabel::Property::RENDER_MODE, Toolkit::DevelTextLabel::Render::ASYNC_AUTO);
    }
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
    if(async)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
      application.SendNotification();
      application.Render();
    }
  }
  application.GetPlatform().TriggerTimer();
  DALI_TEST_CHECK(true);
  END_TEST;
}
