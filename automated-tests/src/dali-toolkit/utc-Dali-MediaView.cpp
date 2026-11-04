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
 *
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/media-view/media-view.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/media-frame-source-plugin.h>
#include <dali/integration-api/adaptor-framework/media-view-backend.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dlfcn.h>
#include <algorithm>
#include <limits>
#include "dali-toolkit-test-utils/test-media-frame-source.h"

using namespace Dali;
using namespace Dali::Toolkit;
namespace
{
void Release(void* data)
{
  ++*static_cast<int*>(data);
}

/**
 * @brief Loads the host fixture under the production plugin SONAME for this test.
 */
class SourcePluginLibrary
{
public:
  SourcePluginLibrary()
  : mLibrary(dlopen(TEST_MEDIA_FRAME_SOURCE_PLUGIN_PATH, RTLD_NOW | RTLD_LOCAL))
  {
    DALI_ASSERT_ALWAYS(mLibrary && "MediaView source fixture did not load");
  }

  ~SourcePluginLibrary()
  {
    dlclose(mLibrary);
  }

  SourcePluginLibrary(const SourcePluginLibrary&)            = delete;
  SourcePluginLibrary& operator=(const SourcePluginLibrary&) = delete;

private:
  void* mLibrary;
};

Vector2 MapTextureCoordinate(const Vector4& pixelArea, bool swapAxes, Vector2 point)
{
  if(swapAxes)
  {
    std::swap(point.x, point.y);
  }
  return Vector2(pixelArea.x + pixelArea.z * point.x, pixelArea.y + pixelArea.w * point.y);
}
} //namespace
void utc_dali_mediaview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_mediaview_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliMediaViewNew(void)
{
  ToolkitTestApplication app;
  MediaView              view = MediaView::New();
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(MediaView::DownCast(view));
  DALI_TEST_CHECK(!MediaView::DownCast(Actor::New()));
  DALI_TEST_CHECK(view.GetSource().Empty());
  DALI_TEST_CHECK(!MediaView::New(Any()));
  view.ClearSource();
  view.ClearSource();
  END_TEST;
}

int UtcDaliMediaViewCropAndFlip(void)
{
  ToolkitTestApplication app;
  auto                   view = MediaView::New();
  DALI_TEST_CHECK(view.SetSourceCrop(Rect<float>(0.25f, 0.0f, 0.5f, 1.0f)));
  DALI_TEST_CHECK(!view.SetSourceCrop(Rect<float>(0.75f, 0.0f, 0.5f, 1.0f)));
  DALI_TEST_CHECK(!view.SetSourceCrop(Rect<float>(0.0f, 0.0f, 0.0f, 1.0f)));
  DALI_TEST_CHECK(!view.SetSourceCrop(Rect<float>(std::numeric_limits<float>::quiet_NaN(), 0.0f, 1.0f, 1.0f)));
  DALI_TEST_EQUALS(view.GetSourceCrop().x, 0.25f, TEST_LOCATION);
  DALI_TEST_CHECK(!view.IsHorizontalFlipEnabled());
  DALI_TEST_CHECK(!view.IsVerticalFlipEnabled());
  view.SetHorizontalFlipEnabled(true);
  view.SetVerticalFlipEnabled(true);
  DALI_TEST_CHECK(view.IsHorizontalFlipEnabled());
  DALI_TEST_CHECK(view.IsVerticalFlipEnabled());
  END_TEST;
}

int UtcDaliMediaViewTypeRegistration(void)
{
  ToolkitTestApplication app;
  auto                   type = TypeRegistry::Get().GetTypeInfo("MediaView");
  DALI_TEST_CHECK(type);
  DALI_TEST_CHECK(MediaView::DownCast(type.CreateInstance()));
  END_TEST;
}

int UtcDaliMediaViewFrameOwnership(void)
{
  int returns = 0;
  {
    Dali::Integration::MediaFrame a(Any(1), Vector2(1, 1), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns);
    auto                          b = std::move(a);
    DALI_TEST_CHECK(!a);
    DALI_TEST_CHECK(b);
    Dali::Integration::MediaFrame c;
    c = std::move(b);
    DALI_TEST_EQUALS(returns, 0, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(returns, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewEmptyRenderer(void)
{
  ToolkitTestApplication app;
  auto                   view = MediaView::New();
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  view.ClearSource();
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewPresentationCoordinates(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(!MediaView::New(Any(&source)));
  view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(source.consumer);
  DALI_TEST_CHECK(view.SetSourceCrop(Rect<float>(0.25f, 0.125f, 0.5f, 0.25f)));

  using Transform              = Dali::Integration::MediaFrameTransform;
  const Transform transforms[] = {Transform::IDENTITY, Transform::ROTATE_90, Transform::ROTATE_180,
                                  Transform::ROTATE_270, Transform::HORIZONTAL_FLIP, Transform::VERTICAL_FLIP};
  // Expected unrotated source corners for display corners TL, TR, BL, BR.
  const Vector2 corners[][4] = {
    {Vector2(0, 0), Vector2(1, 0), Vector2(0, 1), Vector2(1, 1)},
    {Vector2(0, 1), Vector2(0, 0), Vector2(1, 1), Vector2(1, 0)},
    {Vector2(1, 1), Vector2(0, 1), Vector2(1, 0), Vector2(0, 0)},
    {Vector2(1, 0), Vector2(1, 1), Vector2(0, 0), Vector2(0, 1)},
    {Vector2(1, 0), Vector2(0, 0), Vector2(1, 1), Vector2(0, 1)},
    {Vector2(0, 1), Vector2(1, 1), Vector2(0, 0), Vector2(1, 0)}};
  const Vector2 displayCorners[] = {Vector2(0, 0), Vector2(1, 0), Vector2(0, 1), Vector2(1, 1)};
  for(unsigned int transformIndex = 0; transformIndex < 6; ++transformIndex)
  {
    for(unsigned int flips = 0; flips < 4; ++flips)
    {
      view.SetHorizontalFlipEnabled((flips & 1u) != 0u);
      view.SetVerticalFlipEnabled((flips & 2u) != 0u);
      source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), transforms[transformIndex], Release, &returns));
      app.SendNotification();
      app.Render();
      DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
      const auto    renderer     = view.GetRendererAt(0);
      const Vector2 visualSize   = renderer.GetProperty<Vector2>(Dali::VisualRenderer::Property::TRANSFORM_SIZE);
      const Vector2 expectedSize = (transformIndex == 1u || transformIndex == 3u) ? Vector2(50.0f, 200.0f) : Vector2(200.0f, 50.0f);
      DALI_TEST_EQUALS(visualSize, expectedSize, 0.0001f, TEST_LOCATION);
      const Vector3 expectedNaturalSize = (transformIndex == 1u || transformIndex == 3u) ? Vector3(64.0f, 256.0f, 0.0f) : Vector3(256.0f, 64.0f, 0.0f);
      DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), expectedNaturalSize, 0.0001f, TEST_LOCATION);
      const Vector4       pixelArea = renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("pixelArea"));
      const Property::Map program   = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
      const std::string   vertex(program.Find("vertex")->Get<Dali::String>().CStr());
      const bool          swapAxes = vertex.find("#define IS_REQUIRED_SWAP_UV") != std::string::npos;
      DALI_TEST_EQUALS(swapAxes, transformIndex == 1u || transformIndex == 3u, TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextureCoordinateTransform"), Property::INVALID_INDEX, TEST_LOCATION);
      for(unsigned int corner = 0; corner < 4; ++corner)
      {
        const Vector2 sourceCorner = corners[transformIndex][corner ^ flips];
        const Vector2 expected(0.25f + 0.5f * sourceCorner.x, 0.125f + 0.25f * sourceCorner.y);
        DALI_TEST_EQUALS(MapTextureCoordinate(pixelArea, swapAxes, displayCorners[corner]), expected, 0.0001f, TEST_LOCATION);
      }
    }
  }
  DALI_TEST_EQUALS(returns, 21, TEST_LOCATION);
  view.ClearSource();
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(!source.consumer);
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(returns, 24, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewVisualLifecycle(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  view.SetProperty(DevelControl::Property::CORNER_RADIUS, Vector4(24.0f, 24.0f, 24.0f, 24.0f));
  view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  Renderer      renderer  = view.GetRendererAt(0);
  Property::Map shaderMap = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(shaderMap.Find("vertex"));
  DALI_TEST_CHECK(std::string(shaderMap.Find("vertex")->Get<Dali::String>().CStr()).find("uTextureCoordinateTransform") == std::string::npos);
  DALI_TEST_CHECK(std::string(shaderMap.Find("vertex")->Get<Dali::String>().CStr()).find("IS_REQUIRED_ROUNDED_CORNER") != std::string::npos);
  source.consumer->OnReturnBuffer();
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(returns, 1, TEST_LOCATION);
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(2), Vector2(512, 256), Dali::Integration::MediaFrameTransform::ROTATE_90, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.SetProperty(DevelControl::Property::CORNER_RADIUS, Vector4::ZERO);
  view.Unparent();
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(returns, 2, TEST_LOCATION);
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(3), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  DALI_TEST_EQUALS(returns, 3, TEST_LOCATION);
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(4), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.ClearSource();
  DALI_TEST_EQUALS(returns, 4, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewContentVisualIndex(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();

  Property::Map properties;
  properties.Insert(Visual::Property::TYPE, Visual::Type::COLOR);
  properties.Insert(ColorVisual::Property::MIX_COLOR, Color::BLUE);
  auto                  extraVisual = VisualFactory::Get().CreateVisual(properties);
  auto&                 controlImpl = Toolkit::GetImplementation(view);
  const Property::Index extraIndex  = Control::CONTROL_PROPERTY_END_INDEX + 1;
  DevelControl::RegisterVisual(controlImpl, extraIndex, extraVisual, DepthIndex::CONTENT);

  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(DevelControl::GetVisual(controlImpl, extraIndex) == extraVisual);

  view.ClearSource();
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(DevelControl::GetVisual(controlImpl, extraIndex) == extraVisual);
  DALI_TEST_EQUALS(returns, 1, TEST_LOCATION);

  DALI_TEST_CHECK(view.SetSource(Any(&source)));
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(2), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(DevelControl::GetVisual(controlImpl, extraIndex) == extraVisual);

  source.consumer->OnReturnBuffer();
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(DevelControl::GetVisual(controlImpl, extraIndex) == extraVisual);
  DALI_TEST_EQUALS(returns, 2, TEST_LOCATION);
  view.ClearSource();
  END_TEST;
}

int UtcDaliMediaViewStreamedVisualDecorations(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  const Vector4          radius(24.0f, 16.0f, 8.0f, 4.0f);
  view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  view.SetProperty(DevelControl::Property::CORNER_RADIUS, radius);
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();

  auto&                 controlImpl  = Toolkit::GetImplementation(view);
  const Property::Index contentIndex = view.GetPropertyIndex("mediaViewContent");
  auto                  visual       = DevelControl::GetVisual(controlImpl, contentIndex);
  DALI_TEST_CHECK(visual);
  Property::Map properties;
  visual.CreatePropertyMap(properties);
  DALI_TEST_CHECK(properties.Find(Visual::Property::TYPE));
  DALI_TEST_EQUALS(properties.Find(Visual::Property::TYPE)->Get<int>(), static_cast<int>(DevelVisual::STREAMED_IMAGE), TEST_LOCATION);

  const auto renderer = view.GetRendererAt(0u);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), radius, TEST_LOCATION);
  Property::Map program = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(std::string(program.Find("vertex")->Get<Dali::String>().CStr()).find("#define IS_REQUIRED_ROUNDED_CORNER") != std::string::npos);
  DALI_TEST_CHECK(std::string(program.Find("fragment")->Get<Dali::String>().CStr()).find("#extension GL_OES_EGL_image_external:require") != std::string::npos);
  DALI_TEST_CHECK(!renderer.GetProperty<bool>(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA));

  const Vector4 squareness(0.5f, 0.5f, 0.5f, 0.5f);
  view.SetProperty(DevelControl::Property::CORNER_SQUARENESS, squareness);
  Property::Map decoration;
  decoration.Insert(DevelVisual::Property::BORDERLINE_WIDTH, 3.0f);
  decoration.Insert(DevelVisual::Property::BORDERLINE_COLOR, Color::RED);
  visual.DoAction(DevelVisual::Action::UPDATE_PROPERTY, decoration);
  app.SendNotification();
  app.Render();

  program = renderer.GetShader().GetProperty<Property::Map>(Shader::Property::PROGRAM);
  const std::string fragment(program.Find("fragment")->Get<Dali::String>().CStr());
  DALI_TEST_CHECK(fragment.find("#define IS_REQUIRED_SQUIRCLE_CORNER") != std::string::npos);
  DALI_TEST_CHECK(fragment.find("#define IS_REQUIRED_BORDERLINE") != std::string::npos);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), squareness, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::RED, TEST_LOCATION);

  view.ClearSource();
  DALI_TEST_EQUALS(returns, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewSceneDestruction(void)
{
  SourcePluginLibrary  library;
  TestMediaFrameSource source;
  int                  returns = 0;
  {
    ToolkitTestApplication app;
    auto                   view = MediaView::New(Any(&source));
    view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
    app.GetScene().Add(view);
    app.SendNotification();
    app.Render();
    source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
    app.SendNotification();
    app.Render();
    DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  }
  DALI_TEST_CHECK(!source.consumer);
  DALI_TEST_EQUALS(returns, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewNaturalSize(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  view.SetSourceCrop(Rect<float>(0.25f, 0.125f, 0.5f, 0.25f));
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3(256.0f, 64.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(DevelActor::GetHeightForWidth(view, 200.0f), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelActor::GetWidthForHeight(view, 50.0f), 200.0f, TEST_LOCATION);
  view.SetProperty(Toolkit::Control::Property::PADDING, Property::Value(Extents(10, 30, 20, 40)));
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3(296.0f, 124.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(DevelActor::GetHeightForWidth(view, 240.0f), 110.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelActor::GetWidthForHeight(view, 110.0f), 240.0f, TEST_LOCATION);
  view.SetProperty(Toolkit::Control::Property::PADDING, Property::Value(Extents()));

  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(2), Vector2(512, 256), Dali::Integration::MediaFrameTransform::ROTATE_90, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3(64.0f, 256.0f, 0.0f), TEST_LOCATION);

  // Non-binary crop fractions must not be applied again by the streamed visual.
  DALI_TEST_CHECK(view.SetSourceCrop(Rect<float>(0.1f, 0.2f, 0.3f, 0.4f)));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3(102.4f, 153.6f, 0.0f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelActor::GetHeightForWidth(view, 200.0f), 300.0f, 0.0001f, TEST_LOCATION);
  view.SetHorizontalFlipEnabled(true);
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3(102.4f, 153.6f, 0.0f), 0.0001f, TEST_LOCATION);

  view.ClearSource();
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(DevelActor::GetNaturalSize(view), Vector3::ZERO, TEST_LOCATION);

  DALI_TEST_EQUALS(returns, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMediaViewPresentationUpdate(void)
{
  ToolkitTestApplication app;
  SourcePluginLibrary    library;
  TestMediaFrameSource   source;
  int                    returns = 0;
  auto                   view    = MediaView::New(Any(&source));
  app.GetScene().Add(view);
  app.SendNotification();
  app.Render();
  view.SetSourceCrop(Rect<float>(0.25f, 0.125f, 0.5f, 0.25f));
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(1), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();

  view.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  app.SendNotification();
  app.Render();
  const auto    renderer = view.GetRendererAt(0u);
  const Texture texture  = renderer.GetTextures().GetTexture(0u);
  const Shader  shader   = renderer.GetShader();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(200.0f, 50.0f), 0.0001f, TEST_LOCATION);
  view.SetSourceCrop(Rect<float>(0.0f, 0.0f, 1.0f, 1.0f));
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(view.GetRendererAt(0u) == renderer);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == texture);
  DALI_TEST_CHECK(renderer.GetShader() == shader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(200.0f, 100.0f), 0.0001f, TEST_LOCATION);
  view.SetHorizontalFlipEnabled(true);
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(renderer.GetShader() == shader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("pixelArea")), Vector4(1.0f, 0.0f, -1.0f, 1.0f), 0.0001f, TEST_LOCATION);
  view.SetProperty(Toolkit::Control::Property::PADDING, Property::Value(Extents(10, 30, 20, 40)));
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(160.0f, 80.0f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), Vector2(10.0f, 50.0f), 0.0001f, TEST_LOCATION);
  view.SetProperty(Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);
  app.SendNotification();
  app.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), Vector2(30.0f, 50.0f), 0.0001f, TEST_LOCATION);
  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(2), Vector2(256, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(view.GetRendererAt(0u) == renderer);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == texture);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(140.0f, 140.0f), 0.0001f, TEST_LOCATION);

  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(3), Vector2(512, 256), Dali::Integration::MediaFrameTransform::ROTATE_90, Release, &returns));
  app.SendNotification();
  app.Render();
  const Shader rotatedShader = renderer.GetShader();
  DALI_TEST_CHECK(view.GetRendererAt(0u) == renderer);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == texture);
  DALI_TEST_CHECK(rotatedShader != shader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(70.0f, 140.0f), 0.0001f, TEST_LOCATION);

  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(4), Vector2(512, 256), Dali::Integration::MediaFrameTransform::ROTATE_270, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(renderer.GetShader() == rotatedShader);

  source.consumer->OnFrame(Dali::Integration::MediaFrame(Any(5), Vector2(512, 256), Dali::Integration::MediaFrameTransform::IDENTITY, Release, &returns));
  app.SendNotification();
  app.Render();
  DALI_TEST_CHECK(renderer.GetShader() != rotatedShader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(160.0f, 80.0f), 0.0001f, TEST_LOCATION);
  view.ClearSource();
  DALI_TEST_EQUALS(returns, 5, TEST_LOCATION);
  END_TEST;
}
