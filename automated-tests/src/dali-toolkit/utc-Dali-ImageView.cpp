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
 *
 */

#include <unistd.h>
#include <sstream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.

#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-vector-image-renderer.h>
#include "dummy-control.h"

#include <test-encoded-image-buffer.h>
#include <test-native-image.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali/devel-api/scripting/scripting.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_image_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_image_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME  = "gallery_image_01.jpg";
const char* TEST_IMAGE_FILE_NAME2 = "gallery_image_02.jpg";

// resolution: 1024*1024
const char* TEST_IMAGE_1 = TEST_RESOURCE_DIR "/TB-gloss.png";
const char* TEST_IMAGE_2 = TEST_RESOURCE_DIR "/tb-norm.png";

const char* TEST_BROKEN_IMAGE_DEFAULT = TEST_RESOURCE_DIR "/broken.png";
const char* TEST_BROKEN_IMAGE_S       = TEST_RESOURCE_DIR "/broken_s.9.png";
const char* TEST_BROKEN_IMAGE_M       = TEST_RESOURCE_DIR "/broken_m.9.png";
const char* TEST_BROKEN_IMAGE_L       = TEST_RESOURCE_DIR "/broken_l.9.png";
const char* TEST_BROKEN_IMAGE_01      = TEST_RESOURCE_DIR "/button-up.9.png";
const char* TEST_BROKEN_IMAGE_02      = TEST_RESOURCE_DIR "/heartsframe.9.png";

const char* TEST_INVALID_NPATCH_FILE_NAME_01 = "invalid1.9.png";

// resolution: 34*34, pixel format: RGBA8888
static const char* gImage_34_RGBA = TEST_RESOURCE_DIR "/icon-edit.png";
// resolution: 600*600, pixel format: RGB888
static const char* gImage_600_RGB = TEST_RESOURCE_DIR "/test-image-600.jpg";

// resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
const char* TEST_GIF_FILE_NAME = TEST_RESOURCE_DIR "/anim.gif";

const char* TEST_SVG_FILE_NAME                   = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_ANIMATED_VECTOR_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/insta_camera.json";
const char* TEST_WEBP_FILE_NAME                  = TEST_RESOURCE_DIR "/dali-logo.webp";

void TestUrl(ImageView imageView, const std::string url)
{
  Property::Value value = imageView.GetProperty(imageView.GetPropertyIndex("image"));

  std::string urlActual;
  DALI_TEST_CHECK(value.Get(urlActual));
  DALI_TEST_EQUALS(urlActual, url, TEST_LOCATION);
}

} // namespace

int UtcDaliImageViewNewP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  DALI_TEST_CHECK(imageView);

  END_TEST;
}

int UtcDaliImageViewNewUrlP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New(TEST_IMAGE_FILE_NAME);
  DALI_TEST_CHECK(imageView);

  TestUrl(imageView, TEST_IMAGE_FILE_NAME);

  END_TEST;
}

int UtcDaliImageViewConstructorP(void)
{
  ToolkitTestApplication application;

  ImageView imageView;

  DALI_TEST_CHECK(!imageView);

  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  ToolkitTestApplication application;

  // Initialize an object, ref count == 1
  ImageView imageView = ImageView::New();

  ImageView copy(imageView);
  DALI_TEST_CHECK(copy);

  END_TEST;
}

int UtcDaliImageViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_EQUALS(1, imageView.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  imageView.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == imageView.GetProperty<bool>(Actor::Property::SENSITIVE));

  ImageView moved = std::move(imageView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!imageView);

  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  ImageView copy(imageView);
  DALI_TEST_CHECK(copy);
  DALI_TEST_EQUALS(imageView, copy, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_EQUALS(1, imageView.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  imageView.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == imageView.GetProperty<bool>(Actor::Property::SENSITIVE));

  ImageView moved;
  moved = std::move(imageView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!imageView);

  END_TEST;
}

int UtcDaliImageViewDownCastP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  BaseHandle object(imageView);

  ImageView imageView2 = ImageView::DownCast(object);
  DALI_TEST_CHECK(imageView2);

  ImageView imageView3 = DownCast<ImageView>(object);
  DALI_TEST_CHECK(imageView3);

  END_TEST;
}

int UtcDaliImageViewDownCastN(void)
{
  ToolkitTestApplication application;

  BaseHandle unInitializedObject;

  ImageView imageView1 = ImageView::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!imageView1);

  ImageView imageView2 = DownCast<ImageView>(unInitializedObject);
  DALI_TEST_CHECK(!imageView2);

  END_TEST;
}

int UtcDaliImageViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("ImageView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  ImageView imageView = ImageView::DownCast(handle);
  DALI_TEST_CHECK(imageView);

  END_TEST;
}

int UtcDaliImageViewSetGetProperty01(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Property::Index idx = imageView.GetPropertyIndex("image");
  DALI_TEST_EQUALS(idx, (Property::Index)ImageView::Property::IMAGE, TEST_LOCATION);

  imageView.SetProperty(idx, TEST_IMAGE_FILE_NAME);
  TestUrl(imageView, TEST_IMAGE_FILE_NAME);

  END_TEST;
}

int UtcDaliImageViewPreMultipliedAlphaPng(void)
{
  ToolkitTestApplication application;

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Property::Map imageMap;
  imageMap[ImageVisual::Property::URL]            = gImage_34_RGBA;
  imageMap[ImageVisual::Property::RELEASE_POLICY] = ImageVisual::ReleasePolicy::NEVER; // To keep the texture cache

  ImageView imageView1 = ImageView::New();
  imageView1.SetProperty(ImageView::Property::IMAGE, imageMap);

  application.GetScene().Add(imageView1);

  Property::Value value = imageView1.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  bool            enable;
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(enable); // Default value is true

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(enable); // Keep true

  // conventional alpha blending
  Renderer renderer1 = imageView1.GetRendererAt(0);
  value              = renderer1.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(enable);

  int srcFactorRgb    = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  int destFactorRgb   = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  int srcFactorAlpha  = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  int destFactorAlpha = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);
  DALI_TEST_CHECK(srcFactorRgb == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA);
  DALI_TEST_CHECK(srcFactorAlpha == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA);

  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION); // A new texture should be generated.
  textureTrace.Reset();

  // Disable pre-multiplied alpha blending
  imageView1.SetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA, false);

  // Reload the image
  Property::Map attributes;
  DevelControl::DoAction(imageView1, ImageView::Property::IMAGE, DevelImageVisual::Action::RELOAD, attributes);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable);

  // conventional alpha blending
  value = renderer1.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable);

  srcFactorRgb    = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  destFactorRgb   = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  srcFactorAlpha  = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  destFactorAlpha = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);
  DALI_TEST_CHECK(srcFactorRgb == BlendFactor::SRC_ALPHA);
  DALI_TEST_CHECK(destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA);
  DALI_TEST_CHECK(srcFactorAlpha == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA);

  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION); // A new texture should be generated.
  textureTrace.Reset();

  // Make a new ImageView using the same image
  ImageView imageView2 = ImageView::New();
  imageView2.SetProperty(ImageView::Property::IMAGE, imageMap);

  application.GetScene().Add(imageView2);

  application.SendNotification();
  application.Render();

  Renderer renderer2 = imageView2.GetRendererAt(0);
  value              = renderer2.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(enable);

  srcFactorRgb    = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  destFactorRgb   = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  srcFactorAlpha  = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  destFactorAlpha = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);
  DALI_TEST_CHECK(srcFactorRgb == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA);
  DALI_TEST_CHECK(srcFactorAlpha == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA);

  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION); // The cached texture should be used.

  END_TEST;
}

int UtcDaliImageViewPreMultipliedAlphaJpg(void)
{
  ToolkitTestApplication application;

  // Set up trace debug
  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Property::Map imageMap;
  imageMap[ImageVisual::Property::URL]            = gImage_600_RGB;
  imageMap[ImageVisual::Property::RELEASE_POLICY] = ImageVisual::ReleasePolicy::NEVER; // To keep the texture cache

  ImageView imageView1 = ImageView::New();
  imageView1.SetProperty(ImageView::Property::IMAGE, imageMap);

  application.GetScene().Add(imageView1);

  Property::Value value = imageView1.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  bool            enable;
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(enable); // Default value is true

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable); // Should be false after loading

  // conventional alpha blending
  Renderer renderer1 = imageView1.GetRendererAt(0);
  value              = renderer1.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable);

  int srcFactorRgb    = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  int destFactorRgb   = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  int srcFactorAlpha  = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  int destFactorAlpha = renderer1.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);
  DALI_TEST_CHECK(srcFactorRgb == BlendFactor::SRC_ALPHA);
  DALI_TEST_CHECK(destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA);
  DALI_TEST_CHECK(srcFactorAlpha == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA);

  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION); // A new texture should be generated.
  textureTrace.Reset();

  ImageView imageView2 = ImageView::New();
  imageView2.SetProperty(ImageView::Property::IMAGE, imageMap);

  // Disable pre-multiplied alpha blending
  imageView2.SetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA, false);

  application.GetScene().Add(imageView2);

  application.SendNotification();
  application.Render();

  value = imageView2.GetProperty(ImageView::Property::PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable);

  // conventional alpha blending
  Renderer renderer2 = imageView2.GetRendererAt(0);
  value              = renderer2.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(enable));
  DALI_TEST_CHECK(!enable);

  srcFactorRgb    = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  destFactorRgb   = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  srcFactorAlpha  = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  destFactorAlpha = renderer2.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);
  DALI_TEST_CHECK(srcFactorRgb == BlendFactor::SRC_ALPHA);
  DALI_TEST_CHECK(destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA);
  DALI_TEST_CHECK(srcFactorAlpha == BlendFactor::ONE);
  DALI_TEST_CHECK(destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA);

  DALI_TEST_EQUALS(textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION); // The cached texture should be used.

  END_TEST;
}

int UtcDaliImageViewPixelArea(void)
{
  // Test pixel area property
  ToolkitTestApplication application;

  static std::vector<UniformData> customUniforms =
    {
      UniformData("pixelArea", Property::Type::VECTOR4),
    };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  // Gif image, use AnimatedImageVisual internally
  // Atlasing is applied to pack multiple frames, use custom wrap mode
  ImageView     gifView = ImageView::New();
  const Vector4 pixelAreaVisual(0.f, 0.f, 2.f, 2.f);
  gifView.SetProperty(ImageView::Property::IMAGE,
                      Property::Map().Add(ImageVisual::Property::URL, TEST_GIF_FILE_NAME).Add(ImageVisual::Property::PIXEL_AREA, pixelAreaVisual));

  // Add to stage
  Integration::Scene stage = application.GetScene();
  stage.Add(gifView);

  // loading started
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(gifView.GetRendererCount() == 1u);

  const Vector4 fullTextureRect(0.f, 0.f, 1.f, 1.f);
  // test that the pixel area value defined in the visual property map is registered on renderer
  Renderer        renderer       = gifView.GetRendererAt(0);
  Property::Value pixelAreaValue = renderer.GetProperty(renderer.GetPropertyIndex("pixelArea"));
  DALI_TEST_EQUALS(pixelAreaValue.Get<Vector4>(), pixelAreaVisual, TEST_LOCATION);

  // test that the shader has the default pixel area value registered.
  Shader shader  = renderer.GetShader();
  pixelAreaValue = shader.GetProperty(shader.GetPropertyIndex("pixelArea"));
  DALI_TEST_EQUALS(pixelAreaValue.Get<Vector4>(), fullTextureRect, TEST_LOCATION);

  // test that the uniform uses the pixelArea property on the renderer.
  TestGlAbstraction& gl = application.GetGlAbstraction();
  Vector4            pixelAreaUniform;
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("pixelArea", pixelAreaUniform));
  DALI_TEST_EQUALS(pixelAreaVisual, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  // set the pixelArea property on the control
  const Vector4 pixelAreaControl(-1.f, -1.f, 3.f, 3.f);
  gifView.SetProperty(ImageView::Property::PIXEL_AREA, pixelAreaControl);
  application.SendNotification();
  application.Render(16);

  // check the pixelArea property on the control
  pixelAreaValue = gifView.GetProperty(gifView.GetPropertyIndex("pixelArea"));
  DALI_TEST_EQUALS(pixelAreaValue.Get<Vector4>(), pixelAreaControl, TEST_LOCATION);
  // test that the uniform uses the pixelArea property on the control.
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("pixelArea", pixelAreaUniform));
  DALI_TEST_EQUALS(pixelAreaControl, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithoutAltasing(void)
{
  ToolkitTestApplication     application;
  TestGlAbstraction&         gl          = application.GetGlAbstraction();
  const std::vector<GLuint>& textures    = gl.GetBoundTextures();
  size_t                     numTextures = textures.size();

  // Async loading, no atlasing for big size image
  ImageView imageView = ImageView::New(gImage_600_RGB);

  // By default, Aysnc loading is used
  application.GetScene().Add(imageView);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const std::vector<GLuint>& textures2 = gl.GetBoundTextures();
  DALI_TEST_GREATER(textures2.size(), numTextures, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithAtlasing(void)
{
  ToolkitTestApplication application;

  //Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map imageMap;

  imageMap[ImageVisual::Property::URL]            = gImage_34_RGBA;
  imageMap[ImageVisual::Property::DESIRED_HEIGHT] = 34;
  imageMap[ImageVisual::Property::DESIRED_WIDTH]  = 34;
  imageMap[ImageVisual::Property::ATLASING]       = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Toolkit::Control::Property::PADDING, Extents(10u, 10u, 10u, 10u));

  // By default, Aysnc loading is used
  // loading is not started if the actor is offScene

  application.GetScene().Add(imageView);

  imageView.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
  DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithAtlasing02(void)
{
  ToolkitTestApplication application;

  //Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map asyncLoadingMap;
  asyncLoadingMap["url"]                = gImage_34_RGBA;
  asyncLoadingMap["desiredHeight"]      = 34;
  asyncLoadingMap["desiredWidth"]       = 34;
  asyncLoadingMap["synchronousLoading"] = false;
  asyncLoadingMap["atlasing"]           = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(ImageView::Property::IMAGE, asyncLoadingMap);

  application.GetScene().Add(imageView);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
  DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSyncLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading and size using index key property map");

  Property::Map syncLoadingMap;
  syncLoadingMap[ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
  syncLoadingMap[ImageVisual::Property::ATLASING]            = true;

  // Sync loading, no atlasing for big size image
  {
    ImageView imageView = ImageView::New();

    // Sync loading is used
    syncLoadingMap[ImageVisual::Property::URL] = gImage_600_RGB;
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);
  }

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New();

    // Sync loading is used
    syncLoadingMap[ImageVisual::Property::URL]            = gImage_34_RGBA;
    syncLoadingMap[ImageVisual::Property::DESIRED_HEIGHT] = 34;
    syncLoadingMap[ImageVisual::Property::DESIRED_WIDTH]  = 34;
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);

    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params),
                     true,
                     TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewSyncLoading02(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading and size using string key property map");

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New();

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap["url"]                = gImage_34_RGBA;
    syncLoadingMap["desiredHeight"]      = 34;
    syncLoadingMap["desiredWidth"]       = 34;
    syncLoadingMap["synchronousLoading"] = true;
    syncLoadingMap["atlasing"]           = true;
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);

    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params),
                     true,
                     TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewAsyncLoadingEncodedBuffer(void)
{
  ToolkitTestApplication     application;
  TestGlAbstraction&         gl          = application.GetGlAbstraction();
  const std::vector<GLuint>& textures    = gl.GetBoundTextures();
  size_t                     numTextures = textures.size();

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);

  // Async loading, no atlasing for big size image
  ImageView imageView = ImageView::New(url.GetUrl());

  // By default, Aysnc loading is used
  application.GetScene().Add(imageView);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const std::vector<GLuint>& textures2 = gl.GetBoundTextures();
  DALI_TEST_GREATER(textures2.size(), numTextures, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingEncodedBufferWithAtlasing(void)
{
  ToolkitTestApplication application;

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);
  ImageUrl           url2   = Toolkit::Image::GenerateUrl(buffer);

  // Generate url is not equal to url2
  // NOTE : This behavior may changed when ImageUrl compare operator changed.
  DALI_TEST_CHECK(url != url2);
  // Generate url's string is equal to url2's string
  DALI_TEST_CHECK(url.GetUrl() == url2.GetUrl());

  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  url2                       = Toolkit::Image::GenerateUrl(buffer2);

  // Check whethere two url are not equal
  DALI_TEST_CHECK(url.GetUrl() != url2.GetUrl());

  // Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map imageMap;

  imageMap[ImageVisual::Property::URL]            = url.GetUrl();
  imageMap[ImageVisual::Property::DESIRED_HEIGHT] = 600;
  imageMap[ImageVisual::Property::DESIRED_WIDTH]  = 600;
  imageMap[ImageVisual::Property::ATLASING]       = true;

  // No atlasing with big image
  ImageView imageView_bigdesired = ImageView::New();
  imageView_bigdesired.SetProperty(ImageView::Property::IMAGE, imageMap);
  imageView_bigdesired.SetProperty(Toolkit::Control::Property::PADDING, Extents(10u, 10u, 10u, 10u));

  imageMap[ImageVisual::Property::DESIRED_HEIGHT] = 0;
  imageMap[ImageVisual::Property::DESIRED_WIDTH]  = 0;

  // No atlasing with zero desired size
  ImageView imageView_nodesired = ImageView::New();
  imageView_nodesired.SetProperty(ImageView::Property::IMAGE, imageMap);
  imageView_nodesired.SetProperty(Toolkit::Control::Property::PADDING, Extents(10u, 10u, 10u, 10u));

  imageMap[ImageVisual::Property::DESIRED_HEIGHT] = 34;
  imageMap[ImageVisual::Property::DESIRED_WIDTH]  = 34;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Toolkit::Control::Property::PADDING, Extents(10u, 10u, 10u, 10u));

  // By default, Aysnc loading is used
  // loading is not started if the actor is offScene
  application.GetScene().Add(imageView);
  application.GetScene().Add(imageView_bigdesired);
  application.GetScene().Add(imageView_nodesired);
  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.Render(16);
  application.SendNotification();

  // Change url to url2
  imageMap[ImageVisual::Property::URL] = url2.GetUrl();
  imageView.SetProperty(ImageView::Property::IMAGE, imageMap);

  imageView.SetProperty(Dali::Actor::Property::LAYOUT_DIRECTION, Dali::LayoutDirection::RIGHT_TO_LEFT);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
  DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSyncLoadingEncodedBuffer(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading from EncodedImageBuffer");

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_34_RGBA);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New();

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap["url"]                = url.GetUrl();
    syncLoadingMap["alphaMaskUrl"]       = gImage_34_RGBA;
    syncLoadingMap["desiredHeight"]      = 34;
    syncLoadingMap["desiredWidth"]       = 34;
    syncLoadingMap["synchronousLoading"] = true;
    syncLoadingMap["atlasing"]           = true;
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);

    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params),
                     true,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewAddedTexture(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing image view with texture provided manager url");

  ImageView imageView = ImageView::New();

  // empty texture is ok, though pointless from app point of view
  TextureSet  empty;
  std::string url = TextureManager::AddTexture(empty);
  DALI_TEST_CHECK(url.size() > 0u);

  Property::Map propertyMap;
  propertyMap[ImageVisual::Property::URL] = url;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageViewSizeWithBackground(void)
{
  ToolkitTestApplication application;

  int       width     = 100;
  int       height    = 200;
  ImageView imageView = ImageView::New();

  imageView.SetProperty(Control::Property::BACKGROUND,
                        {
                          {Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                          {Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg"},
                          {ImageVisual::Property::DESIRED_WIDTH, width},
                          {ImageVisual::Property::DESIRED_HEIGHT, height},
                        });

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetCurrentProperty<Vector3>(Actor::Property::SIZE).width, (float)width, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetCurrentProperty<Vector3>(Actor::Property::SIZE).height, (float)height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSizeWithBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground  = 100;
  int heightBackground = 200;
  int width            = 600;
  int height           = 600;

  ImageView imageView = ImageView::New();

  imageView.SetProperty(Control::Property::BACKGROUND,
                        {
                          {Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                          {Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg"},
                          {ImageVisual::Property::DESIRED_WIDTH, widthBackground},
                          {ImageVisual::Property::DESIRED_HEIGHT, heightBackground},
                        });

  imageView.SetImage(gImage_600_RGB); // 1 to 1 ratio, 600x600 pixels

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetCurrentProperty<Vector3>(Actor::Property::SIZE).width, (float)width, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetCurrentProperty<Vector3>(Actor::Property::SIZE).height, (float)height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackground(void)
{
  ToolkitTestApplication application;

  int widthBackground  = 100;
  int heightBackground = 200;

  ImageView imageView = ImageView::New();

  imageView.SetProperty(Control::Property::BACKGROUND,
                        {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                         {Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg"},
                         {ImageVisual::Property::DESIRED_WIDTH, widthBackground},
                         {ImageVisual::Property::DESIRED_HEIGHT, heightBackground}});

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();

  Control control = Control::DownCast(imageView);
  DALI_TEST_CHECK(control);
  DALI_TEST_EQUALS(imageView.GetHeightForWidth(123.f), control.GetHeightForWidth(123.f), TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetWidthForHeight(321.f), control.GetWidthForHeight(321.f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground  = 100;
  int heightBackground = 200;
  int width            = 300;
  int height           = 300;

  ImageView imageView = ImageView::New();

  imageView.SetProperty(Control::Property::BACKGROUND,
                        {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                         {Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg"},
                         {ImageVisual::Property::DESIRED_WIDTH, widthBackground},
                         {ImageVisual::Property::DESIRED_HEIGHT, heightBackground}}); // 1 to 2 ratio

  imageView.SetImage(gImage_600_RGB); // 1 to 1 ratio

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetHeightForWidth(width), (float)height, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetWidthForHeight(height), (float)width, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSetImageUrl(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  imageView.SetImage(TEST_IMAGE_FILE_NAME);
  TestUrl(imageView, TEST_IMAGE_FILE_NAME);

  imageView.SetImage(TEST_IMAGE_FILE_NAME2);
  TestUrl(imageView, TEST_IMAGE_FILE_NAME2);

  END_TEST;
}

bool    gResourceReadySignalFired = false;
Vector3 gNaturalSize;

void ResourceReadySignal(Control control)
{
  gResourceReadySignalFired = true;
}

void OnResourceReadySignalSVG(Control control)
{
  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(control);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Fitting mode should not be applied at this point
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::SIZE)->Get<Vector2>(), Vector2::ZERO, TEST_LOCATION);
}

int UtcDaliImageViewCheckResourceReady(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New(TEST_GIF_FILE_NAME);

  imageView.SetProperty(Control::Property::BACKGROUND,
                        {{Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE},
                         {Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg"},
                         {ImageVisual::Property::DESIRED_WIDTH, 100},
                         {ImageVisual::Property::DESIRED_HEIGHT, 200}});

  DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.GetScene().Add(imageView);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSetImageTypeChangesP(void)
{
  ToolkitTestApplication application;

  ImageView                   imageView   = ImageView::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);

  application.GetScene().Add(imageView);

  std::string           url;
  Property::Map         map;
  Toolkit::Visual::Base visual;

  Property::Value value = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual                = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  value.Get(map);
  DALI_TEST_CHECK(map.Empty()); // Value should be empty
  DALI_TEST_CHECK(!visual);     // Visual should be invalid

  // Set a URL
  imageView.SetImage("TEST_URL");

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(value.Get(url));  // Value should NOT be empty
  DALI_TEST_CHECK(!value.Get(map)); // Value should be empty
  DALI_TEST_CHECK(visual);          // Visual should be valid

  // Set an empty URL
  imageView.SetImage("");

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  value.Get(map);
  DALI_TEST_CHECK(map.Empty()); // Value should be empty
  DALI_TEST_CHECK(!visual);     // Visual should be invalid

  // Set a URL in property map
  Property::Map propertyMap;
  propertyMap[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  DALI_TEST_CHECK(value.Get(map));  // Value should NOT be empty
  DALI_TEST_CHECK(visual);          // Visual should be valid

  // Set a URL in property map again
  propertyMap[ImageVisual::Property::URL] = gImage_34_RGBA;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  DALI_TEST_CHECK(value.Get(map));  // Value should NOT be empty
  DALI_TEST_CHECK(visual);          // Visual should be valid

  // Set an empty URL in property map
  propertyMap[ImageVisual::Property::URL] = std::string();
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  DALI_TEST_CHECK(value.Get(map));  // Value should NOT be empty
  DALI_TEST_CHECK(!visual);         // Visual should be invalid

  // Set a URL in property map again
  propertyMap[ImageVisual::Property::URL] = gImage_34_RGBA;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  DALI_TEST_CHECK(value.Get(map));  // Value should NOT be empty
  DALI_TEST_CHECK(visual);          // Visual should be valid

  // Set an empty property map
  propertyMap.Clear();
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.SendNotification();
  application.Render(16);

  value  = imageView.GetProperty(imageView.GetPropertyIndex("image"));
  visual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);

  DALI_TEST_CHECK(!value.Get(url)); // Value should be empty
  DALI_TEST_CHECK(value.Get(map));  // Value should NOT be empty
  DALI_TEST_CHECK(map.Empty());     // But PropertyMap should be empty
  DALI_TEST_CHECK(!visual);         // Visual should be invalid

  END_TEST;
}

int UtcDaliImageViewResourceUrlP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_CHECK(imageView.GetProperty(ImageView::Property::IMAGE).Get<std::string>().empty());

  imageView.SetProperty(ImageView::Property::IMAGE, "TestString");
  DALI_TEST_EQUALS(imageView.GetProperty(ImageView::Property::IMAGE).Get<std::string>(), "TestString", TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewReplaceImage(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New(TEST_IMAGE_1);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  gResourceReadySignalFired = false;

  imageView.SetImage(TEST_IMAGE_2);

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewReloadAlphaMaskImage(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  ImageView     dummy     = ImageView::New();
  ImageView     imageView = ImageView::New();
  Property::Map propertyMap;

  // To keep alpha mask cached, scene on some dummy image.
  // Note : If we don't cache alpha mask image, the reference count of mask image become zero.
  // In this case, we might need to wait mask image loading, which is not neccesary & can be changed behavior.
  propertyMap[ImageVisual::Property::URL]            = gImage_600_RGB;
  propertyMap[ImageVisual::Property::ALPHA_MASK_URL] = TEST_BROKEN_IMAGE_DEFAULT;
  dummy.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.GetScene().Add(dummy);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  propertyMap.Clear();
  propertyMap[ImageVisual::Property::URL]            = gImage_34_RGBA;
  propertyMap[ImageVisual::Property::ALPHA_MASK_URL] = TEST_BROKEN_IMAGE_DEFAULT;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render(16);

  // Load image and use cached mask. Now we try to apply masking.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadySignalFired, false, TEST_LOCATION);

  // Cancel apply masking.
  imageView.Unparent();

  application.SendNotification();
  application.Render(16);

  // Reload same image again.
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render(16);

  // Finish apply masking.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

void OnRelayoutOverride(Size size)
{
  gNaturalSize = size; // Size Relayout is using
}

int UtcDaliImageViewReplaceImageAndGetNaturalSize(void)
{
  ToolkitTestApplication application;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New(TEST_IMAGE_1);
  imageView.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyControl.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS);

  dummyControl.Add(imageView);
  dummyImpl.SetRelayoutCallback(&OnRelayoutOverride);
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(gNaturalSize.width, 1024.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gNaturalSize.height, 1024.0f, TEST_LOCATION);

  gNaturalSize = Vector3::ZERO;

  imageView.SetImage(gImage_600_RGB);

  // Waiting for resourceReady so SendNotifcation not called here.

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gNaturalSize.width, 600.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gNaturalSize.height, 600.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithImmediateLoad(void)
{
  tet_infoline("Test Setting Image with IMMEDIATE load and receving ResourceReadySignal before staged.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageMap;

  imageMap[ImageVisual::Property::URL]         = gImage_34_RGBA;
  imageMap[ImageVisual::Property::LOAD_POLICY] = ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Creating ImageView without URL so image does not start loading");
  ImageView imageView = ImageView::New();
  tet_infoline("Connect to image loaded signal before setting image");
  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);
  tet_infoline("Setting Image with IMMEDIATE load, signal already connected so will be triggered.");
  imageView.SetProperty(ImageView::Property::IMAGE, imageMap);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithReusedImage(void)
{
  tet_infoline("Test Setting Image that was already loaded by another ImageView and still getting ResourceReadySignal.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageMap;

  imageMap[ImageVisual::Property::URL]         = gImage_34_RGBA;
  imageMap[ImageVisual::Property::LOAD_POLICY] = ImageVisual::LoadPolicy::IMMEDIATE;

  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);
  imageView.SetProperty(ImageView::Property::IMAGE, imageMap);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
  gResourceReadySignalFired = false;

  ImageView imageViewWithExistingImage = ImageView::New();
  imageViewWithExistingImage.ResourceReadySignal().Connect(&ResourceReadySignal);
  imageViewWithExistingImage.SetProperty(ImageView::Property::IMAGE, imageMap);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithReusedImage02(void)
{
  tet_infoline("Test Setting Image that was already loaded by another ImageView and still getting ResourceReadySignal when staged.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageImmediateLoadingMap;
  imageImmediateLoadingMap[ImageVisual::Property::URL]         = gImage_34_RGBA;
  imageImmediateLoadingMap[ImageVisual::Property::LOAD_POLICY] = ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Immediate load an image");
  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);
  imageView.SetProperty(ImageView::Property::IMAGE, imageImmediateLoadingMap);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  tet_infoline("Check image loaded");
  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
  gResourceReadySignalFired = false;

  tet_infoline("Create another ImageView with the same URL");
  ImageView imageViewWithExistingImage = ImageView::New(gImage_34_RGBA);
  tet_infoline("Connect to ResourceReady signal for second ImageView, it should still fire as resource is ready");
  imageViewWithExistingImage.ResourceReadySignal().Connect(&ResourceReadySignal);

  application.GetScene().Add(imageViewWithExistingImage);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPaddingProperty(void)
{
  ToolkitTestApplication application;

  ImageView     imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[Toolkit::Visual::Property::TYPE]       = Toolkit::Visual::IMAGE;
  imagePropertyMap[Toolkit::ImageVisual::Property::URL]   = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
  imagePropertyMap[ImageVisual::Property::DESIRED_WIDTH]  = 128;
  imagePropertyMap[ImageVisual::Property::DESIRED_HEIGHT] = 128;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView.SetProperty(Control::Property::PADDING, Extents(15, 10, 5, 10));
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetProperty<Extents>(Control::Property::PADDING), Extents(15, 10, 5, 10), TEST_LOCATION);

  ImageView childImage = ImageView::New();
  childImage.SetBackgroundColor(Color::BLACK);
  childImage.SetProperty(Actor::Property::SIZE, Vector2(10.f, 10.f));
  imageView.Add(childImage);

  application.SendNotification();
  application.Render();

  // Child ImageView should be positioned dependinig on Parent ImageView's Padding value
  DALI_TEST_EQUALS(childImage.GetProperty<Vector3>(Dali::Actor::Property::POSITION), Vector3(15, 5, 0), TEST_LOCATION);

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(15, 5), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPaddingProperty02(void)
{
  ToolkitTestApplication application;

  ImageView     imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imagePropertyMap[Toolkit::ImageVisual::Property::URL]        = TEST_RESOURCE_DIR "/Kid1.svg";
  imagePropertyMap[ImageVisual::Property::DESIRED_WIDTH]       = 128;
  imagePropertyMap[ImageVisual::Property::DESIRED_HEIGHT]      = 128;
  imagePropertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView.SetProperty(Control::Property::PADDING, Extents(15, 10, 5, 10));
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetProperty<Extents>(Control::Property::PADDING), Extents(15, 10, 5, 10), TEST_LOCATION);

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(15, 5), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPaddingProperty03(void)
{
  tet_infoline("Test Setting Image Padding then removing it.");

  ToolkitTestApplication application;

  ImageView     imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imagePropertyMap[Toolkit::ImageVisual::Property::URL]        = TEST_RESOURCE_DIR "/Kid1.svg";
  imagePropertyMap[ImageVisual::Property::DESIRED_WIDTH]       = 128;
  imagePropertyMap[ImageVisual::Property::DESIRED_HEIGHT]      = 128;
  imagePropertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView.SetProperty(Control::Property::PADDING, Extents(15, 10, 5, 10));
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetProperty<Extents>(Control::Property::PADDING), Extents(15, 10, 5, 10), TEST_LOCATION);

  tet_infoline("Remove Padding and test Visual is position correctly");

  imageView.SetProperty(Control::Property::PADDING, Extents(0, 0, 0, 0));

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPaddingProperty04(void)
{
  tet_infoline("Test Setting Image Padding then removing it. Visual Fitting Mode as Fill");

  ToolkitTestApplication application;

  ImageView     imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imagePropertyMap[Toolkit::ImageVisual::Property::URL]        = TEST_RESOURCE_DIR "/Kid1.svg";
  imagePropertyMap[ImageVisual::Property::DESIRED_WIDTH]       = 128;
  imagePropertyMap[ImageVisual::Property::DESIRED_HEIGHT]      = 128;
  imagePropertyMap[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::FILL;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView.SetProperty(Control::Property::PADDING, Extents(15, 10, 5, 10));
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.GetProperty<Extents>(Control::Property::PADDING), Extents(15, 10, 5, 10), TEST_LOCATION);

  tet_infoline("Remove Padding and test Visual is position correctly");

  imageView.SetProperty(Control::Property::PADDING, Extents(0, 0, 0, 0));

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewTransformTest01(void)
{
  tet_infoline("Test Setting a offset transform on the ImageView");

  ToolkitTestApplication application;

  ImageView     imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE)
    .Add(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/Kid1.svg")
    .Add(ImageVisual::Property::DESIRED_WIDTH, 120)
    .Add(ImageVisual::Property::DESIRED_HEIGHT, 120)
    .Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FILL)
    .Add(Visual::Property::TRANSFORM,
         Property::Map().Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY,
                             Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE))
           .Add(Toolkit::Visual::Transform::Property::OFFSET, Vector2(8, 8)));

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imagePropertyMap);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(imageView);

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
  Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
  Property::Map               resultMap;
  imageVisual.CreatePropertyMap(resultMap);

  Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transformValue);
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK(retMap);

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET)->Get<Vector2>(), Vector2(8, 8), TEST_LOCATION);
  DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::OFFSET_POLICY)->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewUsingAtlasAndGetNaturalSize(void)
{
  ToolkitTestApplication application;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE]          = Toolkit::Visual::IMAGE;
  imageMap[Toolkit::ImageVisual::Property::URL]      = gImage_34_RGBA;
  imageMap[Toolkit::ImageVisual::Property::ATLASING] = true;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Vector3 naturalSize = imageView.GetNaturalSize();

  DALI_TEST_EQUALS(naturalSize.width, 34.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(naturalSize.height, 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFillMode(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual without padding and set the fill-mode to fill");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB);
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FittingMode::FILL);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2::ONE, TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Toolkit::Visual::Transform::Policy::RELATIVE);

  END_TEST;
}

int UtcDaliImageViewFittingModeFitKeepAspectRatio(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using FitKeepAspectRatio ( image: [600,600], view: [600,700] )");
  tet_infoline("  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  // If there's
  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 50), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesFill(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using Fill ( image: [600,600], view: [600,700] )");
  tet_infoline("  There should be no need to change the transform, only size is changed to fit view");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FILL);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2::ONE, TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Toolkit::Visual::Transform::Policy::RELATIVE);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesOverfitKeepAspectRatio(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using OverFitKeepAspectRatio ( image: [600,600], view: [600,500] )");
  tet_infoline("  offset or size is the same as view, but adjust internally using pixelArea ");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 500));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  // If there's
  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 500), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesCenter01(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using Center ( image: [600,600], view: [700,700] )");
  tet_infoline("  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::CENTER);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(700, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(50, 50), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesCenter02(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using Center ( image: [600,600], view: [500,500] )");
  tet_infoline("  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::CENTER);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(700, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(50, 50), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesFitHeight01(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using FitHeight ( image: [600,600], view: [600,700] )");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_HEIGHT);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 700), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesFitHeight02(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using FitHeight ( image: [600,600], view: [700,600] )");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 249x169 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_HEIGHT);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(700, 600));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(50, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesFitWidth01(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using FitWidth ( image: [600,600], view: [600,700] )");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 600x600 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_WIDTH);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 50), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesFitWidth02(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using FitWidth ( image: [600,600], view:[700,600] )");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, gImage_600_RGB); // 249x169 image
  imageMap.Add(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_WIDTH);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(700, 600));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(700, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesChangeFittingMode01(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliImageViewFittingModesChangeFittingMode, image: [600,600], view:[800,700]");

  ImageView imageView = ImageView::New();

  // 1. Render using FittingMode::SHRINK_TO_FIT
  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(700, 700), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(50, 0), TEST_LOCATION);

  // 2. Render again using DevelVisaul::CENTER
  Property::Map imageMap2;
  imageMap2[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap2[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap2[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::CENTER;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap2);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);

  visual.CreatePropertyMap(returnedMap);

  value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(100, 50), TEST_LOCATION);

  // 3. Render again using before fittingMode
  Property::Map imageMap3;
  imageMap3[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap3[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap3[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap3);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  visual.CreatePropertyMap(returnedMap);

  value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(700, 700), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(50, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesChangeFittingMode02(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliImageViewFittingModesChangeFittingMode, image: [600,600], view:[800,700]");

  ImageView imageView = ImageView::New();

  // 1. Render using FittingMode::OVER_FIT_KEEP_ASPECT_RATIO
  Property::Map imageMap;
  imageMap[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(800, 700), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION);

  // 2. Render again using DevelVisaul::CENTER
  Property::Map imageMap2;
  imageMap2[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap2[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap2[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::CENTER;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap2);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);

  visual.CreatePropertyMap(returnedMap);

  value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(600, 600), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(100, 50), TEST_LOCATION);

  // 3. Render again using before fittingMode
  Property::Map imageMap3;
  imageMap3[Toolkit::Visual::Property::TYPE]            = Toolkit::Visual::IMAGE;
  imageMap3[Toolkit::ImageVisual::Property::URL]        = gImage_600_RGB;
  imageMap3[DevelVisual::Property::VISUAL_FITTING_MODE] = Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap3);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(800, 700));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  visual.CreatePropertyMap(returnedMap);

  value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(800, 700), TEST_LOCATION); // Change the internal size according to the image view size

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE), TEST_LOCATION);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFittingModesWithAnimatedVectorImageVisual(void)
{
  ToolkitTestApplication application;

  tet_infoline("Create an ImageVisual using SCALE_TO_FILL and animated vector image ( image: [600,600], view:[600,600] )");

  ImageView     imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add(Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE);
  imageMap.Add(Toolkit::ImageVisual::Property::URL, TEST_ANIMATED_VECTOR_IMAGE_FILE_NAME); // 249x169 image

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, imageMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(600, 600));

  application.GetScene().Add(imageView);

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual(Toolkit::Internal::GetImplementation(imageView), Toolkit::ImageView::Property::IMAGE);
  Property::Map         returnedMap;
  visual.CreatePropertyMap(returnedMap);

  Property::Value* value = returnedMap.Find(Toolkit::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(value);
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK(map);

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2::ONE, TEST_LOCATION); // Relative size so will take up 100%

  value = map->Find(Toolkit::Visual::Transform::Property::SIZE_POLICY);
  DALI_TEST_CHECK(value);
  DALI_TEST_CHECK(value->Get<int>() == Toolkit::Visual::Transform::Policy::RELATIVE);

  value = map->Find(Toolkit::Visual::Transform::Property::OFFSET);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<Vector2>(), Vector2(0, 0), TEST_LOCATION); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewCustomShader(void)
{
  ToolkitTestApplication application;

  // Set a custom shader with an image url
  {
    Property::Map     properties;
    Property::Map     shader;
    const std::string vertexShader                    = "Foobar";
    const std::string fragmentShader                  = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

    properties[Visual::Property::TYPE]     = Visual::IMAGE;
    properties[Visual::Property::SHADER]   = shader;
    properties[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty(ImageView::Property::IMAGE, properties);

    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    Renderer        renderer = imageView.GetRendererAt(0);
    Shader          shader2  = renderer.GetShader();
    Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
    Property::Map*  map      = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

    Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);
  }

  // Set a custom shader after setting an image url
  {
    Property::Map     properties;
    Property::Map     shader;
    const std::string vertexShader                    = "Foobar";
    const std::string fragmentShader                  = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    ImageView imageView = ImageView::New(TEST_IMAGE_FILE_NAME);
    imageView.SetProperty(ImageView::Property::IMAGE, properties);

    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    Renderer        renderer = imageView.GetRendererAt(0);
    Shader          shader2  = renderer.GetShader();
    Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
    Property::Map*  map      = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

    Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);
  }

  // Set a custom shader before setting an image url
  {
    Property::Map     properties;
    Property::Map     shader;
    const std::string vertexShader                    = "Foobar";
    const std::string fragmentShader                  = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    ImageView imageView = ImageView::New();
    imageView.SetProperty(ImageView::Property::IMAGE, properties);
    imageView.SetProperty(ImageView::Property::IMAGE, TEST_IMAGE_FILE_NAME);

    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    Renderer        renderer = imageView.GetRendererAt(0);
    Shader          shader2  = renderer.GetShader();
    Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
    Property::Map*  map      = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

    Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);
  }

  // Set a custom shader after setting a property map
  {
    Property::Map     properties;
    Property::Map     shader;
    const std::string vertexShader                    = "Foobar";
    const std::string fragmentShader                  = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    Property::Map properties1;
    properties1[Visual::Property::TYPE]     = Visual::IMAGE;
    properties1[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty(ImageView::Property::IMAGE, properties1);
    imageView.SetProperty(ImageView::Property::IMAGE, properties);

    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    Renderer        renderer = imageView.GetRendererAt(0);
    Shader          shader2  = renderer.GetShader();
    Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
    Property::Map*  map      = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

    Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);
  }

  // Set a custom shader before setting a property map
  {
    Property::Map     properties;
    Property::Map     shader;
    const std::string vertexShader                    = "Foobar";
    const std::string fragmentShader                  = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER]   = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    Property::Map properties1;
    properties1[Visual::Property::TYPE]     = Visual::IMAGE;
    properties1[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty(ImageView::Property::IMAGE, properties);
    imageView.SetProperty(ImageView::Property::IMAGE, properties1);

    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    Renderer        renderer = imageView.GetRendererAt(0);
    Shader          shader2  = renderer.GetShader();
    Property::Value value    = shader2.GetProperty(Shader::Property::PROGRAM);
    Property::Map*  map      = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* fragment = map->Find("fragment"); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS(fragmentShader, fragment->Get<std::string>(), TEST_LOCATION);

    Property::Value* vertex = map->Find("vertex"); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS(vertexShader, vertex->Get<std::string>(), TEST_LOCATION);
  }

  END_TEST;
}

namespace
{
static int gFailCounter = 0;
const int  MAX_RETRIES(3);

void ReloadImage(ImageView imageView)
{
  Property::Map imageImmediateLoadingMap;
  imageImmediateLoadingMap[ImageVisual::Property::URL]         = "Non-existant-image.jpg";
  imageImmediateLoadingMap[ImageVisual::Property::LOAD_POLICY] = ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Immediate load an image");
  imageView.SetProperty(ImageView::Property::IMAGE, imageImmediateLoadingMap);
}

void ResourceFailedReload(Control control)
{
  gFailCounter++;
}
} // namespace

int UtcDaliImageViewReloadFailedOnResourceReadySignal(void)
{
  tet_infoline("Test reloading failed image from within signal handler.");

  ToolkitTestApplication application;

  gFailCounter = 0;

  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect(&ResourceFailedReload);
  DALI_TEST_EQUALS(gFailCounter, 0, TEST_LOCATION);
  ReloadImage(imageView);

  // loading started, this waits for the loader thread to complete
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gFailCounter, 1, TEST_LOCATION);

  ReloadImage(imageView);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gFailCounter, 2, TEST_LOCATION);

  ReloadImage(imageView);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gFailCounter, 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewLoadRemoteSVG(void)
{
  tet_infoline("Test load from a remote server.");

  ToolkitTestApplication application;

  {
    Toolkit::ImageView imageView;
    imageView = Toolkit::ImageView::New();
    imageView.SetImage("https://dev.w3.org/SVG/tools/svgweb/samples/svg-files/check.svg");
    imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    imageView.SetProperty(Actor::Property::SIZE, Vector2(300, 300));
    imageView.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 150.0f, 0.0f));

    application.GetScene().Add(imageView);

    DALI_TEST_CHECK(imageView);

    DALI_TEST_EQUALS(imageView.GetRendererCount(), 0u, TEST_LOCATION);

    application.SendNotification();

    // Wait for loading & rasterization
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
  }

  // Without size set
  {
    Toolkit::ImageView imageView;
    imageView = Toolkit::ImageView::New();
    imageView.SetImage("https://dev.w3.org/SVG/tools/svgweb/samples/svg-files/check.svg");
    imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    imageView.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 150.0f, 0.0f));

    application.GetScene().Add(imageView);

    DALI_TEST_CHECK(imageView);

    DALI_TEST_EQUALS(imageView.GetRendererCount(), 0u, TEST_LOCATION);

    application.SendNotification();

    // Wait for loading & rasterization
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewSyncSVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image sync loading");

  {
    ImageView imageView = ImageView::New();

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);

    application.GetScene().Add(imageView);
    DALI_TEST_CHECK(imageView);

    application.SendNotification();
    Vector3 naturalSize = imageView.GetNaturalSize();

    DALI_TEST_EQUALS(naturalSize.width, 100.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(naturalSize.height, 100.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewSyncSVGLoading02(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image sync loading");

  {
    ImageView imageView = ImageView::New();

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    syncLoadingMap.Insert(DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO);
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);
    imageView.ResourceReadySignal().Connect(&OnResourceReadySignalSVG);

    application.GetScene().Add(imageView);
    DALI_TEST_CHECK(imageView);

    application.SendNotification();
    application.Render();

    // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
    Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(imageView);
    Toolkit::Visual::Base       imageVisual = DevelControl::GetVisual(controlImpl, ImageView::Property::IMAGE);
    Property::Map               resultMap;
    imageVisual.CreatePropertyMap(resultMap);

    Property::Value* transformValue = resultMap.Find(Visual::Property::TRANSFORM);
    DALI_TEST_CHECK(transformValue);
    Property::Map* retMap = transformValue->GetMap();
    DALI_TEST_CHECK(retMap);

    // Image Visual should be positioned depending on ImageView's padding
    DALI_TEST_EQUALS(retMap->Find(Visual::Transform::Property::SIZE)->Get<Vector2>(), Vector2(100, 100), TEST_LOCATION);

    Vector3 naturalSize = imageView.GetNaturalSize();
    DALI_TEST_EQUALS(naturalSize.width, 100.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(naturalSize.height, 100.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewAsyncSVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image async loading");

  {
    ImageView imageView = ImageView::New();

    // Async loading is used - default value of SYNCHRONOUS_LOADING is false.
    Property::Map propertyMap;
    propertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    propertyMap.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");
    imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

    application.GetScene().Add(imageView);
    DALI_TEST_CHECK(imageView);

    application.SendNotification();

    // Wait for loading
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    Vector3 naturalSize = imageView.GetNaturalSize();
    DALI_TEST_EQUALS(naturalSize.width, 100.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(naturalSize.height, 100.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewSVGLoadingSyncSetInvalidValue(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image async loading");

  // Sync loading
  {
    ImageView imageView = ImageView::New();

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/svg1.svg");

    // Check to set invalid value
    // The SYNCHRONOUS_LOADING property must be set to the bool value.
    // Check if error log is outputted when setting other value like string.
    // Even if the wrong value is set, the image will be shown normally, and the synchronous value should be the default value(false).
    syncLoadingMap.Insert(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, std::to_string(5));
    imageView.SetProperty(ImageView::Property::IMAGE, syncLoadingMap);

    application.GetScene().Add(imageView);
    DALI_TEST_CHECK(imageView);

    application.SendNotification();

    // Wait for loading
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    Vector3 naturalSize = imageView.GetNaturalSize();
    DALI_TEST_EQUALS(naturalSize.width, 100.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(naturalSize.height, 100.0f, TEST_LOCATION);

    Property::Value value = imageView.GetProperty(ImageView::Property::IMAGE);
    Property::Map*  map   = value.GetMap();
    DALI_TEST_CHECK(map);

    Property::Value* sync = map->Find(Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING);
    DALI_TEST_CHECK(sync);
    DALI_TEST_EQUALS(false, sync->Get<bool>(), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewSvgLoadingFailureLocalFile(void)
{
  // Local svg file - invalid file path
  {
    ToolkitTestApplication application;

    TestGlAbstraction& gl           = application.GetGlAbstraction();
    TraceCallStack&    textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New(TEST_RESOURCE_DIR "/foo.svg");
    imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

    DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

    application.GetScene().Add(imageView);

    application.SendNotification();

    // loading started, this waits for the loader thread - load
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

    // Should be shown a broken image
    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);
  }

  // Local svg file - invalid file path without size set
  {
    ToolkitTestApplication application;

    TestGlAbstraction& gl           = application.GetGlAbstraction();
    TraceCallStack&    textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    gResourceReadySignalFired = false;
    textureTrace.Reset();

    ImageView imageView = ImageView::New(TEST_RESOURCE_DIR "/foo.svg");
    imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

    DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

    application.GetScene().Add(imageView);

    application.SendNotification();

    // loading started, this waits for the loader thread - load & rasterize
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

    // Should be shown a broken image
    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);
  }

  // Local svg file - invalid file
  {
    ToolkitTestApplication application;

    TestGlAbstraction& gl           = application.GetGlAbstraction();
    TraceCallStack&    textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    gResourceReadySignalFired = false;
    textureTrace.Reset();

    ImageView imageView = ImageView::New(TEST_RESOURCE_DIR "/invalid.svg");
    imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

    DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

    application.GetScene().Add(imageView);

    application.SendNotification();

    // loading started, this waits for the loader thread - load & rasterize
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

    // Should be shown a broken image
    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewSvgLoadingFailureRemoteFile01(void)
{
  // Remote svg file
  {
    ToolkitTestApplication application;

    TestGlAbstraction& gl           = application.GetGlAbstraction();
    TraceCallStack&    textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New("https://127.0.0.1/foobar.svg");
    imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
    imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

    DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

    application.GetScene().Add(imageView);

    application.SendNotification();

    // loading started, this waits for the loader thread - load & rasterize
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

    // Should be shown a broken image
    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewSvgLoadingFailureRemoteFile02(void)
{
  // Remote svg file without size set
  {
    ToolkitTestApplication application;

    TestGlAbstraction& gl           = application.GetGlAbstraction();
    TraceCallStack&    textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New("https://127.0.0.1/foobar.svg");
    imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

    DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

    application.GetScene().Add(imageView);

    application.SendNotification();

    // loading started, this waits for the loader thread - load & rasterize
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

    // Should be shown a broken image
    DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewSvgRasterizationFailure(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  ImageView imageView = ImageView::New(TEST_RESOURCE_DIR "/invalid1.svg");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);

  application.GetScene().Add(imageView);

  application.SendNotification();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(ImageView::Property::IMAGE), Visual::ResourceStatus::FAILED, TEST_LOCATION);

  // Should be shown a broken image
  DALI_TEST_EQUALS(imageView.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSvgChageSize(void)
{
  ToolkitTestApplication application;

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  ImageView imageView = ImageView::New(TEST_SVG_FILE_NAME);
  application.GetScene().Add(imageView);

  application.SendNotification();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::VectorImageRenderer::GetLoadCount(), 1, TEST_LOCATION);

  // Change actor size, then rasterization should be done again
  imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.SendNotification();

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  // We should not load the file again.
  DALI_TEST_EQUALS(Test::VectorImageRenderer::GetLoadCount(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSvgAtlasing(void)
{
  ToolkitTestApplication application;

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map propertyMap;
  propertyMap["url"]      = TEST_SVG_FILE_NAME;
  propertyMap["atlasing"] = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  application.GetScene().Add(imageView);

  application.SendNotification();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  // use atlas
  TraceCallStack::NamedParams params1;
  params1["width"] << 100;
  params1["height"] << 100;
  DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexSubImage2D", params1), true, TEST_LOCATION);

  imageView.SetProperty(Actor::Property::SIZE, Vector2(600.f, 600.f));

  application.SendNotification();

  // Wait for rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  callStack.Reset();

  application.SendNotification();
  application.Render(16);

  // not use atlas
  TraceCallStack::NamedParams params2;
  params2["width"] << 600;
  params2["height"] << 600;
  DALI_TEST_EQUALS(callStack.FindMethodAndParams("TexImage2D", params2), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewTVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing TVG image loading");

  {
    ImageView imageView = ImageView::New(TEST_RESOURCE_DIR "/test.tvg");
    application.GetScene().Add(imageView);
    DALI_TEST_CHECK(imageView);

    application.SendNotification();

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render(16);

    Vector3 naturalSize = imageView.GetNaturalSize();

    DALI_TEST_EQUALS(naturalSize.width, 100.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(naturalSize.height, 100.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewSvgDesiredSize01(void)
{
  ToolkitTestApplication application;

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  int       desiredWidth = 100, desiredHeight = 150;
  ImageView imageView = ImageView::New(TEST_SVG_FILE_NAME, ImageDimensions(desiredWidth, desiredHeight));

  application.GetScene().Add(imageView);

  application.SendNotification();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << desiredWidth << ", " << desiredHeight;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliImageViewSvgDesiredSize02(void)
{
  ToolkitTestApplication application;

  TestGlAbstraction& gl           = application.GetGlAbstraction();
  TraceCallStack&    textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  int       desiredWidth = 150, desiredHeight = 100;
  ImageView imageView                   = ImageView::New();
  imageView[ImageView::Property::IMAGE] = Property::Map().Add("url", TEST_SVG_FILE_NAME).Add("desiredWidth", desiredWidth).Add("desiredHeight", desiredHeight);
  application.GetScene().Add(imageView);

  application.SendNotification();

  // Wait for loading & rasterization
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  {
    std::stringstream out;
    out << GL_TEXTURE_2D << ", " << 0u << ", " << desiredWidth << ", " << desiredHeight;
    DALI_TEST_CHECK(textureTrace.FindMethodAndParams("TexImage2D", out.str().c_str()));
  }

  END_TEST;
}

int UtcDaliImageViewImageLoadFailure01(void)
{
  ToolkitTestApplication application;

  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL, TEST_BROKEN_IMAGE_S);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL, TEST_BROKEN_IMAGE_M);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE, TEST_BROKEN_IMAGE_L);

  std::string brokenUrl;
  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_S, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_M, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_L, brokenUrl, TEST_LOCATION);

  ImageView imageView = ImageView::New("invalidUrl.png");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));

  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewImageLoadFailure02(void)
{
  ToolkitTestApplication application;

  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL, TEST_BROKEN_IMAGE_DEFAULT);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL, TEST_BROKEN_IMAGE_M);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE, TEST_BROKEN_IMAGE_L);

  std::string brokenUrl;
  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_DEFAULT, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_M, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_L, brokenUrl, TEST_LOCATION);

  ImageView imageView = ImageView::New("invalidUrl.png");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(30.f, 30.f));
  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewImageLoadFailure03(void)
{
  ToolkitTestApplication application;

  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL, TEST_BROKEN_IMAGE_01);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL, TEST_BROKEN_IMAGE_02);

  std::string brokenUrl;
  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_01, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_02, brokenUrl, TEST_LOCATION);

  ImageView imageView = ImageView::New("invalidUrl.png");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewImageLoadFailure04(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New("invalidUrl.png");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  application.GetScene().Add(imageView);
  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL, TEST_BROKEN_IMAGE_S);
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL, "invalidBroken.png");
  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE, TEST_BROKEN_IMAGE_L);

  ImageView imageView2 = ImageView::New("invalidUrl.png");
  imageView2.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  application.GetScene().Add(imageView2);

  std::string brokenUrl;
  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::SMALL);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_S, brokenUrl, TEST_LOCATION);

  brokenUrl = DevelStyleManager::GetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::LARGE);
  DALI_TEST_EQUALS(TEST_BROKEN_IMAGE_L, brokenUrl, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DevelStyleManager::SetBrokenImageUrl(styleManager, DevelStyleManager::BrokenImageType::NORMAL, "invalidBroken.9.png");

  ImageView imageView3 = ImageView::New("invalidUrl.png");
  imageView3.SetProperty(Actor::Property::SIZE, Vector2(100.f, 100.f));
  application.GetScene().Add(imageView3);

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  END_TEST;
}

namespace
{
static int gResourceReadySignalCounter = 0;

void OnResourceReadySignal01(Control control)
{
  gResourceReadySignalCounter++;

  if(control.GetVisualResourceStatus(ImageView::Property::IMAGE) == Visual::ResourceStatus::READY)
  {
    if(gResourceReadySignalCounter == 1)
    {
      // Set image twice
      // It makes the first new visual be deleted immediately
      ImageView::DownCast(control).SetImage(gImage_34_RGBA);
      ImageView::DownCast(control).SetImage(gImage_34_RGBA);
    }
  }
  else if(control.GetVisualResourceStatus(ImageView::Property::IMAGE) == Visual::ResourceStatus::FAILED)
  {
    // Make the resource ready immediately
    control[ImageView::Property::IMAGE] = gImage_600_RGB;
  }
}

void OnResourceReadySignal02(Control control)
{
  if(++gResourceReadySignalCounter == 1)
  {
    // It makes the first new visual be deleted immediately
    // The first image will not be loaded.
    control[ImageView::Property::IMAGE] = Property::Map().Add(ImageVisual::Property::URL, gImage_600_RGB).Add(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER);
    control[ImageView::Property::IMAGE] = TEST_IMAGE_1;
  }
}

ImageView gImageView1;
ImageView gImageView2;
ImageView gImageView3;
ImageView gImageView4;

void OnResourceReadySignal03(Control control)
{
  if(gResourceReadySignalCounter == 0)
  {
    // Queue loading
    // 1. Use cached image, then LoadComplete will be called right after OnResourceReadySignal03.
    gImageView2[ImageView::Property::IMAGE] = gImage_34_RGBA;

    // 2. Load a new image
    gImageView3[ImageView::Property::IMAGE] = TEST_IMAGE_1;

    // 3. Use the new image again
    gImageView1[ImageView::Property::IMAGE] = TEST_IMAGE_1;
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal03);
  }
  else if(gResourceReadySignalCounter == 1)
  {
    // This is called from TextureManager::ProcessQueuedTextures().
    gImageView1.Unparent();
    gImageView1.Reset();
  }
  gResourceReadySignalCounter++;
}

void OnSimpleResourceReadySignal(Control control)
{
  // simply increate counter
  gResourceReadySignalCounter++;
}

int gResourceReadySignal04ComesOrder = 0;

void OnResourceReadySignal04(Control control)
{
  gResourceReadySignalCounter++;
  tet_printf("rc %d\n", gResourceReadySignalCounter);
  if(gResourceReadySignalCounter == 1)
  {
    auto scene = gImageView1.GetParent();

    // Request load something
    // We hope this request result is return later than gImageView2.
    gImageView3 = ImageView::New(TEST_IMAGE_1);
    gImageView3.ResourceReadySignal().Connect(&OnResourceReadySignal04);
    scene.Add(gImageView3);
    gImageView4 = ImageView::New(TEST_IMAGE_2);
    gImageView4.ResourceReadySignal().Connect(&OnResourceReadySignal04);
    scene.Add(gImageView4);

    if(control == gImageView1)
    {
      gResourceReadySignal04ComesOrder = 1;
    }
    else
    {
      gResourceReadySignal04ComesOrder = 2;
    }
  }
  if(gResourceReadySignalCounter == 2)
  {
    if(gResourceReadySignal04ComesOrder == 1 && control == gImageView2)
    {
      // Scene off first one.
      gImageView1.Unparent();

      // Scene off second one.
      gImageView2.Unparent();
    }
    else if(gResourceReadySignal04ComesOrder == 2 && control == gImageView1)
    {
      // Scene off first one.
      gImageView2.Unparent();

      // Scene off second one.
      gImageView1.Unparent();
    }
    else
    {
      // We can't check that this utc fail case. just pass always when we come here.
      gResourceReadySignal04ComesOrder = -1;
    }

    // If we don't seperate index of FreeList area
    // and if we don't queue remove during obversing,
    // cache index become something invalid data.
    // In this case, some strange observer can be called.
    // For example, gImageView4.LoadComplete will be called.
  }
}

void OnResourceReadySignal05(Control control)
{
  gResourceReadySignalCounter++;

  // Request load with same image
  // NOTE : The url must not be same as gImageView1
  const int viewCount = 4;
  for(int i = 0; i < viewCount; ++i)
  {
    gImageView1.Add(ImageView::New("invalid2.jpg"));
  }
}

int gResourceReadySignal06ComesOrder = 0;

void OnResourceReadySignal06(Control control)
{
  gResourceReadySignalCounter++;
  if(gResourceReadySignalCounter == 1)
  {
    auto scene = gImageView1.GetParent();

    // Request load something
    // We hope this request result is return later than gImageView2.

    Property::Map map1;
    map1[Toolkit::ImageVisual::Property::URL]            = TEST_IMAGE_1;
    map1[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = TEST_BROKEN_IMAGE_DEFAULT;

    gImageView3 = ImageView::New();
    gImageView3.SetProperty(Toolkit::ImageView::Property::IMAGE, map1);
    gImageView3.ResourceReadySignal().Connect(&OnResourceReadySignal06);

    Property::Map map2;
    map2[Toolkit::ImageVisual::Property::URL]            = TEST_IMAGE_2;
    map2[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = TEST_BROKEN_IMAGE_S;
    gImageView4                                          = ImageView::New();
    gImageView4.SetProperty(Toolkit::ImageView::Property::IMAGE, map2);
    gImageView4.ResourceReadySignal().Connect(&OnResourceReadySignal06);

    if(control == gImageView1)
    {
      gResourceReadySignal06ComesOrder = 1;
    }
    else
    {
      gResourceReadySignal06ComesOrder = 2;
    }
  }
  if(gResourceReadySignalCounter == 2)
  {
    if(gResourceReadySignal06ComesOrder == 1 && control == gImageView2)
    {
      // Scene off first one.
      gImageView1.Unparent();

      // Scene off second one.
      gImageView2.Unparent();
    }
    else if(gResourceReadySignal06ComesOrder == 2 && control == gImageView1)
    {
      // Scene off first one.
      gImageView2.Unparent();

      // Scene off second one.
      gImageView1.Unparent();
    }
    else
    {
      // We can't check that this utc fail case. just pass always when we come here.
      gResourceReadySignal06ComesOrder = -1;
    }

    // If we don't seperate index of FreeList area
    // and if we don't queue remove during obversing,
    // cache index become something invalid data.
    // In this case, some strange observer can be called.
    // For example, gImageView4.LoadComplete will be called.
  }
}

void OnResourceReadySignal07(Control control)
{
  gResourceReadySignalCounter++;
  // Load masked image
  tet_printf("rc %d %d\n", gResourceReadySignalCounter, static_cast<bool>(gImageView2));

  if(!gImageView2)
  {
    auto scene = gImageView1.GetParent();

    Property::Map map1;
    map1[Toolkit::ImageVisual::Property::URL]            = TEST_IMAGE_1;
    map1[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = TEST_BROKEN_IMAGE_DEFAULT;

    gImageView2 = ImageView::New();
    gImageView2.SetProperty(Toolkit::ImageView::Property::IMAGE, map1);
    gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal07);

    scene.Add(gImageView2);
  }
}

void OnResourceReadySignal08(Control control)
{
  gResourceReadySignalCounter++;

  if(gImageView1)
  {
    gImageView1.Unparent();
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Unparent();
    gImageView2.Reset();
  }
}

std::size_t gResourceReadySignal09Emitted = false;

void OnResourceReadySignal09(Control control)
{
  gResourceReadySignalCounter++;

  if(gImageView1 && !gResourceReadySignal09Emitted)
  {
    gResourceReadySignal09Emitted = true;
    gImageView1.ResourceReadySignal().Disconnect(&OnResourceReadySignal09);

    // Try to load cached invalid nine patch image. It will request load now.
    gImageView1.SetImage(TEST_INVALID_NPATCH_FILE_NAME_01);
    gImageView2.SetImage(TEST_INVALID_NPATCH_FILE_NAME_01);

    // Destroy all visuals immediatly.
    gImageView1.Unparent();
    gImageView1.Reset();
    gImageView2.Unparent();
    gImageView2.Reset();
  }
}
constexpr int gResourceReadySignal10MaxCounter = 5;

void OnResourceReadySignal10(Control control)
{
  gResourceReadySignalCounter++;

  tet_printf("OnResourceReadySignal10 comes!\n");
  if(gResourceReadySignalCounter < gResourceReadySignal10MaxCounter)
  {
    tet_printf("OnResourceReadySignal10 Set image\n");
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, gImage_34_RGBA);
    tet_printf("OnResourceReadySignal10 Set image done\n");
  }
}

void OnResourceReadySignal11(Control control)
{
  gResourceReadySignalCounter++;

  if(!gImageView2)
  {
    auto scene = gImageView1.GetParent();

    // Try to load animated image visual here which is already cached, and then ignore forcely.

    Property::Map map1;
    map1[Toolkit::ImageVisual::Property::URL] = TEST_GIF_FILE_NAME;

    gImageView2 = ImageView::New();
    gImageView2.SetProperty(Toolkit::ImageView::Property::IMAGE, map1);

    gImageView3 = ImageView::New();
    gImageView3.SetProperty(Toolkit::ImageView::Property::IMAGE, map1);

    scene.Add(gImageView2);
    gImageView2.Unparent();

    scene.Add(gImageView3);
    gImageView3.Unparent();
    gImageView3.Reset(); // Destroy visual
  }
}

} // namespace

int UtcDaliImageViewSetImageOnResourceReadySignal01(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  ImageView imageView = ImageView::New(gImage_34_RGBA);
  imageView.ResourceReadySignal().Connect(&OnResourceReadySignal01);

  application.GetScene().Add(imageView);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);

  // Create a new ImageView to cache the image
  ImageView imageView1 = ImageView::New(gImage_600_RGB);
  application.GetScene().Add(imageView1);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Reset count
  gResourceReadySignalCounter = 0;

  imageView[ImageView::Property::IMAGE] = "invalid.jpg";

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Run idle callback
  application.RunIdles();

  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal02(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  ImageView imageView = ImageView::New(gImage_34_RGBA);
  imageView.ResourceReadySignal().Connect(&OnResourceReadySignal02);

  application.GetScene().Add(imageView);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Wait for loading an image
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal03(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  gImageView1 = ImageView::New(gImage_34_RGBA);
  application.GetScene().Add(gImageView1);

  // Wait for loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  gImageView2 = ImageView::New(gImage_600_RGB);
  gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal03);
  application.GetScene().Add(gImageView2);

  gImageView3 = ImageView::New();
  application.GetScene().Add(gImageView3);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageViewOnResourceReadySignalWithBrokenAlphaMask01(void)
{
  tet_infoline("Test signal handler when image / mask image is broken.");

  ToolkitTestApplication application;

  auto TestResourceReadyUrl = [&application](int eventTriggerCount, bool isSynchronous, const std::string& url, const std::string& mask, const char* location) {
    gResourceReadySignalCounter = 0;

    Property::Map map;
    map[Toolkit::ImageVisual::Property::URL] = url;
    if(!mask.empty())
    {
      map[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = mask;
    }
    map[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = isSynchronous;

    ImageView imageView                            = ImageView::New();
    imageView[Toolkit::ImageView::Property::IMAGE] = map;
    imageView[Actor::Property::SIZE]               = Vector2(100.0f, 200.0f);
    imageView.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);

    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render();

    if(!isSynchronous)
    {
      // Wait for loading
      DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(eventTriggerCount), true, location);
    }
    tet_printf("test %s [sync:%d] signal fired\n", url.c_str(), isSynchronous ? 1 : 0);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, location);

    imageView.Unparent();
  };

  for(int synchronous = 0; synchronous <= 1; synchronous++)
  {
    tet_printf("Test normal case (sync:%d)\n", synchronous);
    TestResourceReadyUrl(1, synchronous, gImage_600_RGB, "", TEST_LOCATION);
    TestResourceReadyUrl(3, synchronous, gImage_600_RGB, gImage_34_RGBA, TEST_LOCATION); // 3 event trigger required : 2 image load + 1 apply mask

    tet_printf("Test broken image case (sync:%d)\n", synchronous);
    TestResourceReadyUrl(1, synchronous, "invalid.jpg", "", TEST_LOCATION);
    TestResourceReadyUrl(2, synchronous, "invalid.jpg", gImage_34_RGBA, TEST_LOCATION);

    tet_printf("Test broken mask image case (sync:%d)\n", synchronous);
    TestResourceReadyUrl(2, synchronous, gImage_600_RGB, "invalid.png", TEST_LOCATION);

    tet_printf("Test broken both image, mask image case (sync:%d)\n", synchronous);
    TestResourceReadyUrl(2, synchronous, "invalid.jpg", "invalid.png", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliImageViewOnResourceReadySignalWithBrokenAlphaMask02(void)
{
  tet_infoline("Test signal handler when image try to use cached-and-broken mask image.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  auto TestBrokenMaskResourceReadyUrl = [&application](const std::string& url, const char* location) {
    Property::Map map;
    map[Toolkit::ImageVisual::Property::URL] = url;
    // Use invalid mask url
    map[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = "invalid.png";

    ImageView imageView                            = ImageView::New();
    imageView[Toolkit::ImageView::Property::IMAGE] = map;
    imageView[Actor::Property::SIZE]               = Vector2(100.0f, 200.0f);
    imageView.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);

    application.GetScene().Add(imageView);

    // Don't unparent imageView, for keep the cache.
  };

  // Use more than 4 images (The number of LocalImageLoadThread)
  const std::vector<std::string> testUrlList = {gImage_34_RGBA, gImage_600_RGB, "invalid.jpg" /* invalid url */, TEST_IMAGE_1, TEST_IMAGE_2, TEST_BROKEN_IMAGE_DEFAULT};

  int expectResourceReadySignalCounter = 0;

  for(auto& url : testUrlList)
  {
    TestBrokenMaskResourceReadyUrl(url, TEST_LOCATION);
    expectResourceReadySignalCounter++;
  }

  // Remain 1 signal due to we use #URL + 1 mask image.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(expectResourceReadySignalCounter + 1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(gResourceReadySignalCounter, expectResourceReadySignalCounter, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewCheckVariousCaseSendOnResourceReadySignal(void)
{
  tet_infoline("Test signal handler various case.");

  auto TestResourceReadyUrl = [](int eventTriggerCount, bool isSynchronous, bool loadSuccess, const std::string& url, const std::string& mask, const char* location) {
    ToolkitTestApplication application;

    gResourceReadySignalCounter = 0;

    Property::Map map;
    map[Toolkit::ImageVisual::Property::URL] = url;
    if(!mask.empty())
    {
      map[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = mask;
    }
    map[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = isSynchronous;

    ImageView imageView                            = ImageView::New();
    imageView[Toolkit::ImageView::Property::IMAGE] = map;
    imageView[Actor::Property::SIZE]               = Vector2(100.0f, 200.0f);
    imageView.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);

    application.GetScene().Add(imageView);
    application.SendNotification();
    application.Render();

    // Wait for loading
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(eventTriggerCount), true, location);

    tet_printf("test %s [sync:%d] signal fired\n", url.c_str(), isSynchronous ? 1 : 0);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, location);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(Toolkit::ImageView::Property::IMAGE), loadSuccess ? Toolkit::Visual::ResourceStatus::READY : Toolkit::Visual::ResourceStatus::FAILED, location);

    imageView.Unparent();
  };

  auto TestAuxiliaryResourceReadyUrl = [](bool isSynchronous, bool loadSuccess, const std::string& url, const std::string& auxiliaryUrl, const char* location) {
    ToolkitTestApplication application;

    gResourceReadySignalCounter = 0;

    Property::Map map;
    map[Toolkit::ImageVisual::Property::URL]                        = url;
    map[Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE]       = auxiliaryUrl;
    map[Toolkit::DevelImageVisual::Property::AUXILIARY_IMAGE_ALPHA] = 0.5f;
    map[Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING]        = isSynchronous;

    ImageView imageView = ImageView::New();
    imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
    imageView.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);
    application.GetScene().Add(imageView);

    application.SendNotification();
    application.Render();

    if(!isSynchronous)
    {
      // Wait for loading
      DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, location);
    }

    tet_printf("test %s [sync:%d] signal fired\n", url.c_str(), isSynchronous ? 1 : 0);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, location);
    DALI_TEST_EQUALS(imageView.GetVisualResourceStatus(Toolkit::ImageView::Property::IMAGE), loadSuccess ? Toolkit::Visual::ResourceStatus::READY : Toolkit::Visual::ResourceStatus::FAILED, TEST_LOCATION);

    imageView.Unparent();
  };

  // Case 1 : asynchronous loading
  tet_printf("Test invalid single simple image Asynchronous\n");

  // Test normal case
  TestResourceReadyUrl(1, 0, 1, gImage_600_RGB, "", TEST_LOCATION);
  TestResourceReadyUrl(2, 0, 1, TEST_SVG_FILE_NAME, "", TEST_LOCATION); // load & rasterize
  TestResourceReadyUrl(1, 0, 1, TEST_BROKEN_IMAGE_L, "", TEST_LOCATION);

  TestResourceReadyUrl(2, 0, 1, TEST_GIF_FILE_NAME, "", TEST_LOCATION);                   // 2 image loading - batch size
  TestResourceReadyUrl(2, 0, 1, TEST_ANIMATED_VECTOR_IMAGE_FILE_NAME, "", TEST_LOCATION); // load & rasterize

  TestResourceReadyUrl(3, 0, 1, gImage_600_RGB, gImage_34_RGBA, TEST_LOCATION); // 2 image loading + 1 applymask

  TestAuxiliaryResourceReadyUrl(0, 1, TEST_BROKEN_IMAGE_L, gImage_34_RGBA, TEST_LOCATION);

  // Test broken case
  TestResourceReadyUrl(1, 0, 0, "invalid.jpg", "", TEST_LOCATION);
  TestResourceReadyUrl(1, 0, 0, "invalid.svg", "", TEST_LOCATION);
  TestResourceReadyUrl(1, 0, 0, "invalid.9.png", "", TEST_LOCATION);
  TestResourceReadyUrl(1, 0, 0, "invalid.gif", "", TEST_LOCATION);  // 1 image loading
  TestResourceReadyUrl(1, 0, 0, "invalid.json", "", TEST_LOCATION); // 0 rasterize

  TestResourceReadyUrl(2, 0, 0, "invalid.jpg", "invalid.png", TEST_LOCATION);  // 2 image loading
  TestResourceReadyUrl(2, 0, 1, gImage_600_RGB, "invalid.png", TEST_LOCATION); // 2 image loading
  TestResourceReadyUrl(2, 0, 0, "invalid.jpg", gImage_34_RGBA, TEST_LOCATION); // 2 image loading

  TestAuxiliaryResourceReadyUrl(0, 0, "invalid.9.png", "invalid.png", TEST_LOCATION);
  TestAuxiliaryResourceReadyUrl(0, 1, TEST_BROKEN_IMAGE_L, "invalid.png", TEST_LOCATION);
  TestAuxiliaryResourceReadyUrl(0, 0, "invalid.9.png", gImage_34_RGBA, TEST_LOCATION);

  // Case 2 : Synchronous loading
  tet_printf("Test invalid single simple image Synchronous\n");

  // Test normal case
  TestResourceReadyUrl(0, 1, 1, gImage_600_RGB, "", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 1, TEST_SVG_FILE_NAME, "", TEST_LOCATION); // synchronous rasterize
  TestResourceReadyUrl(0, 1, 1, TEST_BROKEN_IMAGE_L, "", TEST_LOCATION);

  TestResourceReadyUrl(1, 1, 1, TEST_GIF_FILE_NAME, "", TEST_LOCATION); // first frame image loading sync + second frame image loading async

  TestResourceReadyUrl(0, 1, 1, gImage_600_RGB, gImage_34_RGBA, TEST_LOCATION);

  TestAuxiliaryResourceReadyUrl(1, 1, TEST_BROKEN_IMAGE_L, gImage_34_RGBA, TEST_LOCATION);

  // Test broken case
  TestResourceReadyUrl(0, 1, 0, "invalid.jpg", "", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 0, "invalid.svg", "", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 0, "invalid.9.png", "", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 0, "invalid.gif", "", TEST_LOCATION);

  TestResourceReadyUrl(0, 1, 0, "invalid.jpg", "invalid.png", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 1, gImage_600_RGB, "invalid.png", TEST_LOCATION);
  TestResourceReadyUrl(0, 1, 0, "invalid.jpg", gImage_34_RGBA, TEST_LOCATION);

  TestAuxiliaryResourceReadyUrl(1, 0, "invalid.9.png", "invalid.png", TEST_LOCATION);
  TestAuxiliaryResourceReadyUrl(1, 1, TEST_BROKEN_IMAGE_L, "invalid.png", TEST_LOCATION);
  TestAuxiliaryResourceReadyUrl(1, 0, "invalid.9.png", gImage_34_RGBA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal04(void)
{
  tet_infoline("Test texturemanager's remove queue works well within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter      = 0;
  gResourceReadySignal04ComesOrder = 0;

  gImageView1 = ImageView::New("invalid.jpg"); // request invalid image, to make loading failed fast.
  gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal04);
  application.GetScene().Add(gImageView1);

  gImageView2 = ImageView::New("invalid.png"); // request invalid image, to make loading failed fast.
  gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal04);
  application.GetScene().Add(gImageView2);

  application.SendNotification();
  application.Render();

  tet_infoline("Try to load 2 invalid image");

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  tet_infoline("load done");

  // We can test this UTC only if gImageView1 and gImageView2 loaded done.
  if(gResourceReadySignal04ComesOrder == -1)
  {
    tet_infoline("Bad news.. gImageView3 or gImageView4 loaded faster than others. just skip this UTC");
  }
  else
  {
    // gImageView3 and gImageView4 load must not be successed yet.
    DALI_TEST_EQUALS(gImageView3.GetRendererCount(), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(gImageView4.GetRendererCount(), 0u, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    tet_infoline("Try to load 2 valid image");

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 4, TEST_LOCATION);

    tet_infoline("load done");

    // gImageView3 and gImageView4 load must be successed now.
    DALI_TEST_EQUALS(gImageView3.GetRendererAt(0).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(gImageView4.GetRendererAt(0).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
  }

  END_TEST;
}
int UtcDaliImageViewSetImageOnResourceReadySignal05(void)
{
  tet_infoline("Test multiple views with same image during ResourceReady load the image only 1 times");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  gImageView1 = ImageView::New("invalid.jpg"); // request invalid image, to make loading failed fast.
  gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal05);
  application.GetScene().Add(gImageView1);

  application.SendNotification();
  application.Render();

  tet_infoline("Try to load 1 invalid.jpg image");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

  tet_infoline("Try to load 1 invalid2.jpg image");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  tet_infoline("Now we don't have any image to be loaded. Check event thread trigger failed.");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);

  gImageView1.Unparent();
  gImageView1.Reset();

  END_TEST;
}
int UtcDaliImageViewSetImageOnResourceReadySignal06(void)
{
  tet_infoline("Test texturemanager's remove image & mask queue works well within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter      = 0;
  gResourceReadySignal06ComesOrder = 0;

  Property::Map map;
  map[Toolkit::ImageVisual::Property::URL]            = "invalid.jpg";
  map[Toolkit::ImageVisual::Property::ALPHA_MASK_URL] = "invalid.png";

  gImageView1 = ImageView::New(); // request invalid image, to make loading failed fast.
  gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal06);
  application.GetScene().Add(gImageView1);

  gImageView2 = ImageView::New(); // request invalid image, to make loading failed fast.
  gImageView2.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal06);
  application.GetScene().Add(gImageView2);

  application.SendNotification();
  application.Render();

  tet_infoline("Try to load 2 invalid image");

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  tet_infoline("load done");

  // We can test this UTC only if gImageView1 and gImageView2 loaded done.
  if(gResourceReadySignal06ComesOrder == -1)
  {
    tet_infoline("Bad news.. gImageView3 or gImageView4 loaded faster than others. just skip this UTC");
  }
  else
  {
    // gImageView3 and gImageView4 load must not be successed yet.
    DALI_TEST_EQUALS(gImageView3.GetRendererCount(), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(gImageView4.GetRendererCount(), 0u, TEST_LOCATION);

    application.GetScene().Add(gImageView3);
    application.GetScene().Add(gImageView4);
    application.SendNotification();
    application.Render();

    tet_infoline("Try to load 2 valid image");

    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

    tet_infoline("load done");
  }
  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal07(void)
{
  tet_infoline("Test texturemanager's remove image & mask queue works well within signal handler 02.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  Property::Map map;
  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }

  gImageView1 = ImageView::New();
  gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal07);
  application.GetScene().Add(gImageView1);

  application.SendNotification();
  application.Render();

  // Load gImageView1

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

  tet_infoline("load image1 done");

  // Load gImageView2 and mask

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

  // gImageView2 mask apply done

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

  tet_infoline("load image2 done");
  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal08(void)
{
  tet_infoline("Test remove npatch images during resource ready");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  Property::Map map;
  map[Toolkit::ImageVisual::Property::URL] = TEST_BROKEN_IMAGE_M;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }

  // Case 1 : Remove all images during resource ready.
  try
  {
    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal08);
    application.GetScene().Add(gImageView1);

    application.SendNotification();
    application.Render();

    // Load gImageView1
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }

  // Clear cache.
  application.SendNotification();
  application.Render();

  gResourceReadySignalCounter = 0;

  // Case 2 : Remove all images when we use cached resource.
  try
  {
    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView1.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);
    application.GetScene().Add(gImageView1);

    application.SendNotification();
    application.Render();

    // Load gImageView1
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    gImageView2 = ImageView::New();
    gImageView2.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal08);
    application.GetScene().Add(gImageView2);
    DALI_TEST_EQUALS(gResourceReadySignalCounter, 2, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }
  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal09(void)
{
  tet_infoline("Test load invalid npatch images during invalid resource ready");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  Property::Map map;
  map[Toolkit::ImageVisual::Property::URL] = TEST_INVALID_NPATCH_FILE_NAME_01;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }
  if(gImageView3)
  {
    gImageView3.Reset();
  }

  // Dummy view with npatch image
  ImageView dummyView = ImageView::New(TEST_BROKEN_IMAGE_M);
  application.GetScene().Add(dummyView);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Case 1 : Reload images during resource ready.
  try
  {
    gResourceReadySignal09Emitted = false;

    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal09);
    application.GetScene().Add(gImageView1);

    gImageView2 = ImageView::New();
    application.GetScene().Add(gImageView2);

    // Load TEST_INVALID_NPATCH_FILE_NAME_01
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    // Load TEST_INVALID_NPATCH_FILE_NAME_01 one more times.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }

  // Clear cache.
  application.SendNotification();
  application.Render();

  gResourceReadySignalCounter = 0;

  // Case 2 : Remove all images when we use cached resource.
  try
  {
    gResourceReadySignal09Emitted = false;

    gImageView3 = ImageView::New();
    gImageView3.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView3.ResourceReadySignal().Connect(&OnSimpleResourceReadySignal);
    application.GetScene().Add(gImageView3);

    gImageView2 = ImageView::New();
    application.GetScene().Add(gImageView2);

    // Load gImageView2
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal09);
    application.GetScene().Add(gImageView1);

    // Load gImageView1
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    // Load TEST_INVALID_NPATCH_FILE_NAME_01 one more times.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }
  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }
  if(gImageView3)
  {
    gImageView3.Reset();
  }

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal10(void)
{
  tet_infoline("Test ResourceReady signal comes more than 2 times.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }

  // Dummy view to cache image.
  ImageView dummyView = ImageView::New(gImage_34_RGBA);
  application.GetScene().Add(dummyView);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  try
  {
    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, gImage_34_RGBA);
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal10);
    application.GetScene().Add(gImageView1); // It will call resourceReady signal 1 time.

    tet_printf("ResourceReady called %d times\n", gResourceReadySignalCounter);

    DALI_TEST_GREATER(gResourceReadySignal10MaxCounter, gResourceReadySignalCounter, TEST_LOCATION); // Check whether resource ready call too much.

    for(int i = 0; i < gResourceReadySignal10MaxCounter; ++i)
    {
      tet_printf("RunIdles\n");
      // Executes the idle callbacks.
      application.RunIdles();
      application.SendNotification();
      application.Render();
      tet_printf("RunIdles done\n");
    }
    tet_printf("ResourceReady called %d times\n", gResourceReadySignalCounter);

    DALI_TEST_EQUALS(gResourceReadySignalCounter, gResourceReadySignal10MaxCounter, TEST_LOCATION);

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }

  // Clear cache.
  application.SendNotification();
  application.Render();

  gResourceReadySignalCounter = 0;

  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal11(void)
{
  tet_infoline("Test ResourceReady Add AnimatedImageVisual and then Remove immediately.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }
  if(gImageView3)
  {
    gImageView3.Reset();
  }

  try
  {
    gImageView1 = ImageView::New();
    gImageView1.SetProperty(Toolkit::ImageView::Property::IMAGE, TEST_GIF_FILE_NAME);
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal11);
    application.GetScene().Add(gImageView1); // It will call resourceReady signal 1 time.

    tet_printf("ResourceReady called %d times\n", gResourceReadySignalCounter);

    DALI_TEST_EQUALS(gResourceReadySignalCounter, 0, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    // Load gImageView1
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    tet_printf("ResourceReady called %d times\n", gResourceReadySignalCounter);

    DALI_TEST_EQUALS(gResourceReadySignalCounter, 1, TEST_LOCATION);

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    // Exception should not happened
    DALI_TEST_CHECK(false);
  }

  // Clear cache.
  application.SendNotification();
  application.Render();

  gResourceReadySignalCounter = 0;

  // Clear image view for clear test

  if(gImageView1)
  {
    gImageView1.Reset();
  }
  if(gImageView2)
  {
    gImageView2.Reset();
  }
  if(gImageView3)
  {
    gImageView3.Reset();
  }

  END_TEST;
}

int UtcDaliImageViewUseSameUrlWithAnimatedImageVisual(void)
{
  tet_infoline("Test multiple views with same image in animated image visual");
  ToolkitTestApplication application;

  gImageView1 = ImageView::New(TEST_WEBP_FILE_NAME);
  application.GetScene().Add(gImageView1);

  tet_infoline("Remove imageView and Create new imageView with same url");
  application.GetScene().Remove(gImageView1);
  gImageView2 = ImageView::New(TEST_WEBP_FILE_NAME);
  application.GetScene().Add(gImageView2);

  application.SendNotification();
  application.Render();

  tet_infoline("Check the ImageView load image successfully");
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageViewNpatchImageCacheTest01(void)
{
  tet_infoline("Test npatch image cached");

  ToolkitTestApplication application;
  ImageView              imageView[2];

  auto& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  auto& textureCallStack = gl.GetTextureTrace();
  textureCallStack.Enable(true);
  textureCallStack.EnableLogging(true);

  auto TestNPatch = [&](int index, const std::string& nPatchImageUrl, const char* location) {
    if(imageView[index])
    {
      imageView[index].Unparent();
    }

    // Ensure remove npatch cache if required.
    application.SendNotification();
    application.Render();

    imageView[index] = ImageView::New(nPatchImageUrl);
    imageView[index].SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
    application.GetScene().Add(imageView[index]);
  };

  TestNPatch(0, TEST_BROKEN_IMAGE_M, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("trace : \n%s\n", textureCallStack.GetTraceString().c_str());

  // Check we gen only 1 textures
  DALI_TEST_EQUALS(textureCallStack.CountMethod("GenTextures"), 1, TEST_LOCATION);
  textureCallStack.Reset();

  // Let we use cached textures
  for(int i = 0; i < 10; i++)
  {
    TestNPatch(1, TEST_BROKEN_IMAGE_M, TEST_LOCATION);
    TestNPatch(0, TEST_BROKEN_IMAGE_M, TEST_LOCATION);
  }

  application.SendNotification();
  application.Render();
  // Check we use cached npatch data so we don't generate new texture textures
  DALI_TEST_EQUALS(textureCallStack.CountMethod("GenTextures"), 0, TEST_LOCATION);

  // Clear all cached
  imageView[0].Unparent();
  imageView[0].Reset();
  imageView[1].Unparent();
  imageView[1].Reset();

  application.SendNotification();
  application.Render();

  textureCallStack.Reset();
  // Let we use deference textures
  TestNPatch(1, TEST_BROKEN_IMAGE_S, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  // Try to load multiple times.
  for(int i = 0; i < 3; i++)
  {
    TestNPatch(0, TEST_BROKEN_IMAGE_M, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    TestNPatch(1, TEST_BROKEN_IMAGE_S, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  }
  application.SendNotification();
  application.Render();

  imageView[0].Unparent();
  imageView[0].Reset();
  imageView[1].Unparent();
  imageView[1].Reset();

  application.SendNotification();
  application.Render();

  // Check memory leak
  DALI_TEST_EQUALS(textureCallStack.CountMethod("GenTextures"), textureCallStack.CountMethod("DeleteTextures"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewNpatchImageCacheTest02(void)
{
  tet_infoline("Test npatch image cached with border");

  ToolkitTestApplication application;
  ImageView              imageView[2];

  auto& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  auto& textureCallStack = gl.GetTextureTrace();
  textureCallStack.Enable(true);
  textureCallStack.EnableLogging(true);

  auto TestBorderImage = [&](int index, const std::string& normalImageUrl, const Rect<int> border, const char* location) {
    Property::Map map;
    map[Toolkit::Visual::Property::TYPE]        = Toolkit::Visual::N_PATCH;
    map[Toolkit::ImageVisual::Property::URL]    = normalImageUrl;
    map[Toolkit::ImageVisual::Property::BORDER] = border;

    imageView[index] = ImageView::New();
    imageView[index].SetProperty(Toolkit::ImageView::Property::IMAGE, map);
    imageView[index].SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
    application.GetScene().Add(imageView[index]);
  };

  TestBorderImage(0, gImage_34_RGBA, Rect<int>(0, 0, 0, 0), TEST_LOCATION);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  tet_printf("trace : \n%s\n", textureCallStack.GetTraceString().c_str());

  // Check we gen only 1 textures
  DALI_TEST_EQUALS(textureCallStack.CountMethod("GenTextures"), 1, TEST_LOCATION);
  textureCallStack.Reset();

  // Let we use cached textures
  for(int i = 0; i < 10; i++)
  {
    TestBorderImage(0, gImage_34_RGBA, Rect<int>(i, i + 1, i + 2, i + 3), TEST_LOCATION);
    TestBorderImage(1, gImage_34_RGBA, Rect<int>(i + 1, i, i + 3, i + 2), TEST_LOCATION);
  }

  application.SendNotification();
  application.Render();

  // Check we use cached npatch data so we don't generate new texture textures
  DALI_TEST_EQUALS(textureCallStack.CountMethod("GenTextures"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewPlaceholderImage01(void)
{
  tet_infoline("Test imageView use placeholder image");

  ToolkitTestApplication application;
  Property::Map          map;
  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(imageView);

  Property::Value value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  std::string     url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url.empty());
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);

  application.SendNotification();
  application.Render();

  value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url == gImage_34_RGBA);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  // Replace Image test
  map[Toolkit::ImageVisual::Property::URL]    = TEST_IMAGE_1;
  map[ImageView::Property::PLACEHOLDER_IMAGE] = gImage_34_RGBA;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_2;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  // Replace Image test2
  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_2;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render(900);

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageViewPlaceholderImage02(void)
{
  tet_infoline("Test imageView use placeholder image without resource ready");

  ToolkitTestApplication application;

  Property::Map map;
  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  DALI_TEST_EQUALS(imageView.IsResourceReady(), false, TEST_LOCATION);
  imageView.ResourceReadySignal().Connect(&ResourceReadySignal);
  application.GetScene().Add(imageView);

  Property::Value value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  std::string     url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url.empty());

  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url == gImage_34_RGBA);

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalFired, true, TEST_LOCATION);

  gResourceReadySignalFired                = false;
  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(imageView.IsResourceReady(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gResourceReadySignalFired, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewTransitionEffect01(void)
{
  tet_infoline("Test imageView use transition effect");

  ToolkitTestApplication application;
  Property::Map          map;
  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  map[Toolkit::Visual::Property::OPACITY]  = 0.9f;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(imageView);

  Property::Value value = imageView.GetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT);
  bool            transition;
  DALI_TEST_CHECK(value.Get(transition));
  DALI_TEST_CHECK(transition == false);
  imageView.SetProperty(Toolkit::ImageView::Property::ENABLE_TRANSITION_EFFECT, true);

  application.SendNotification();
  application.Render();

  value = imageView.GetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT);
  DALI_TEST_CHECK(value.Get(transition));
  DALI_TEST_CHECK(transition == true);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  // Test transition effect with placeholder
  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, "");
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  // Test transition effect without placeholder
  map[Toolkit::ImageVisual::Property::URL] = TEST_IMAGE_1;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = "";
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  map[Toolkit::ImageVisual::Property::URL] = gImage_600_RGB;
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  imageView.SetImage(TEST_IMAGE_1);
  application.SendNotification();
  application.Render();

  imageView.SetImage(gImage_600_RGB);
  application.SendNotification();
  application.Render(9000);

  imageView.SetImage("");
  application.SendNotification();
  application.Render();

  imageView.SetImage(TEST_IMAGE_1);
  application.SendNotification();
  application.Render();

  // Clear all cached
  imageView.Unparent();
  imageView.Reset();

  END_TEST;
}

int UtcDaliImageViewTransitionEffect02(void)
{
  tet_infoline("Test imageView use transition effect with replace image");

  ToolkitTestApplication application;

  Property::Map map;

  ImageView imageView = ImageView::New();
  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(imageView);

  Property::Value value;
  value = imageView.GetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT);
  bool transition;
  DALI_TEST_CHECK(value.Get(transition));
  DALI_TEST_CHECK(transition == false);
  imageView.SetProperty(Toolkit::ImageView::Property::ENABLE_TRANSITION_EFFECT, true);

  value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  std::string url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url.empty());
  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, "");
  application.SendNotification();
  application.Render();

  imageView.SetProperty(Toolkit::ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  application.SendNotification();
  application.Render();

  value = imageView.GetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT);
  DALI_TEST_CHECK(value.Get(transition));
  DALI_TEST_CHECK(transition == true);

  value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url == gImage_34_RGBA);

  imageView.SetProperty(Toolkit::ImageView::Property::IMAGE, map);
  application.SendNotification();
  application.Render();

  // Clear all cached
  imageView.Unparent();
  imageView.Reset();

  END_TEST;
}

int UtcDaliImageViewTransitionEffect03(void)
{
  tet_infoline("Test imageView use transition effect with placeholder");

  ToolkitTestApplication application;
  Property::Map          map;

  ImageView imageView = ImageView::New();
  imageView.SetImage("");
  imageView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  imageView.SetProperty(ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  imageView.SetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT, true);
  application.GetScene().Add(imageView);

  //DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  tet_infoline("(1)");
  imageView.SetImage(gImage_600_RGB);
  imageView.SetProperty(ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render(16);

  Property::Value value;
  value = imageView.GetProperty(ImageView::Property::ENABLE_TRANSITION_EFFECT);
  bool transition;
  DALI_TEST_CHECK(value.Get(transition));
  DALI_TEST_CHECK(transition == true);

  value = imageView.GetProperty(ImageView::Property::PLACEHOLDER_IMAGE);
  std::string url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_CHECK(url == gImage_34_RGBA);

  imageView.SetImage("");
  application.SendNotification();
  application.Render(16);

  imageView.SetImage(TEST_IMAGE_1);
  imageView.SetProperty(ImageView::Property::PLACEHOLDER_IMAGE, gImage_34_RGBA);
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  application.SendNotification();
  application.Render(16);

  // Clear all cached
  imageView.Unparent();
  imageView.Reset();

  END_TEST;
}
