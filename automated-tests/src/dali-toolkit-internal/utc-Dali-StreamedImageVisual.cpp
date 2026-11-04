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

#include <stdlib.h>
#include <iostream>
#include <string>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/streamed-image/streamed-image-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
using Toolkit::Internal::StreamedImageVisual;

StreamedImageVisual::Frame MakeFrame(Texture texture)
{
  StreamedImageVisual::Frame frame;
  frame.texture     = texture;
  frame.contentSize = Vector2(512.0f, 256.0f);
  return frame;
}

Toolkit::Internal::StreamedImageVisualPtr NewVisual(const StreamedImageVisual::Frame& frame)
{
  Toolkit::VisualFactory            factoryHandle = Toolkit::VisualFactory::Get();
  Toolkit::Internal::VisualFactory& factory       = GetImplementation(factoryHandle);
  return StreamedImageVisual::New(factory.GetFactoryCache(), frame);
}

std::string GetVertexShaderSource(Renderer renderer)
{
  Property::Value value = renderer.GetShader().GetProperty(Shader::Property::PROGRAM);
  Property::Map*  map   = value.GetMap();
  DALI_TEST_CHECK(map);
  Property::Value* vertexValue = map->Find("vertex");
  DALI_TEST_CHECK(vertexValue);
  Dali::String vertexSource;
  DALI_TEST_CHECK(vertexValue->Get(vertexSource));
  return std::string(vertexSource.CStr());
}

Vector2 MapTextureCoordinate(const Vector4& pixelArea, bool swapAxes, Vector2 point)
{
  if(swapAxes)
  {
    std::swap(point.x, point.y);
  }
  return Vector2(pixelArea.x + pixelArea.z * point.x, pixelArea.y + pixelArea.w * point.y);
}

} // namespace

int UtcDaliInternalStreamedImageVisualTransform(void)
{
  ToolkitTestApplication application;
  tet_infoline("Check the pixel area and UV swap for every rotation and flip combination");

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame   = MakeFrame(texture);
  frame.crop      = Rect<float>(0.25f, 0.125f, 0.5f, 0.25f);

  auto visual = NewVisual(frame);

  DummyControl        control     = DummyControl::New(true);
  Impl::DummyControl& controlImpl = static_cast<Impl::DummyControl&>(control.GetImplementation());
  controlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, Toolkit::Visual::Base(visual.Get()));
  control.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  using Rotation             = StreamedImageVisual::Rotation;
  const Rotation rotations[] = {Rotation::DEGREE_0, Rotation::DEGREE_90, Rotation::DEGREE_180, Rotation::DEGREE_270};
  // Expected unrotated source corners for the display corners TL, TR, BL, BR.
  const Vector2 expectedCorners[][4] = {
    {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)},
    {Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f)},
    {Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f)},
    {Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f)},
  };
  const Vector2 displayCorners[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)};

  for(uint32_t rotationIndex = 0u; rotationIndex < 4u; ++rotationIndex)
  {
    for(uint32_t flips = 0u; flips < 4u; ++flips)
    {
      frame.rotation       = rotations[rotationIndex];
      frame.horizontalFlip = (flips & 1u) != 0u;
      frame.verticalFlip   = (flips & 2u) != 0u;
      visual->SetFrame(frame);
      application.SendNotification();
      application.Render();

      DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
      Renderer renderer = control.GetRendererAt(0u);

      const bool expectedSwap = rotationIndex == 1u || rotationIndex == 3u;
      const bool actualSwap   = GetVertexShaderSource(renderer).find("#define IS_REQUIRED_SWAP_UV") != std::string::npos;
      DALI_TEST_EQUALS(actualSwap, expectedSwap, TEST_LOCATION);

      const Vector4 pixelArea = renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("pixelArea"));
      for(uint32_t corner = 0u; corner < 4u; ++corner)
      {
        // The flips reflect the display coordinate before the rotation table applies.
        Vector2 displayPoint = displayCorners[corner];
        if(frame.horizontalFlip)
        {
          displayPoint.x = 1.0f - displayPoint.x;
        }
        if(frame.verticalFlip)
        {
          displayPoint.y = 1.0f - displayPoint.y;
        }
        uint32_t      lookup   = (displayPoint.x > 0.5f ? 1u : 0u) + (displayPoint.y > 0.5f ? 2u : 0u);
        const Vector2 source   = expectedCorners[rotationIndex][lookup];
        const Vector2 expected = Vector2(frame.crop.x + source.x * frame.crop.width, frame.crop.y + source.y * frame.crop.height);
        DALI_TEST_EQUALS(MapTextureCoordinate(pixelArea, actualSwap, displayCorners[corner]), expected, 0.0001f, TEST_LOCATION);
      }
    }
  }

  END_TEST;
}

int UtcDaliInternalStreamedImageVisualNaturalSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("Check the natural size against the crop and the rotation");

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame   = MakeFrame(texture);

  auto                  visual = NewVisual(frame);
  Toolkit::Visual::Base handle(visual.Get());

  Vector2 naturalSize;
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(512.0f, 256.0f), TEST_LOCATION);

  frame.crop = Rect<float>(0.25f, 0.125f, 0.5f, 0.25f);
  visual->SetFrame(frame);
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(256.0f, 64.0f), TEST_LOCATION);

  frame.rotation = StreamedImageVisual::Rotation::DEGREE_90;
  visual->SetFrame(frame);
  handle.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(64.0f, 256.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalStreamedImageVisualFrameUpdate(void)
{
  ToolkitTestApplication application;
  tet_infoline("Check that frame updates replace the texture but keep the shader when possible");

  Texture textureA = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  Texture textureB = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 512u, 256u);
  auto    frame    = MakeFrame(textureA);

  auto visual = NewVisual(frame);
  DALI_TEST_CHECK(visual->GetFrame().texture == textureA);

  DummyControl        control     = DummyControl::New(true);
  Impl::DummyControl& controlImpl = static_cast<Impl::DummyControl&>(control.GetImplementation());
  controlImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, Toolkit::Visual::Base(visual.Get()));
  control.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(control.GetRendererCount(), 1u, TEST_LOCATION);
  Renderer     renderer      = control.GetRendererAt(0u);
  const Shader initialShader = renderer.GetShader();

  // The same texture presented again keeps both the texture and the shader.
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == textureA);
  DALI_TEST_CHECK(renderer.GetShader() == initialShader);

  // Another plain texture replaces the texture but never the shader.
  frame.texture = textureB;
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == textureB);
  DALI_TEST_CHECK(renderer.GetShader() == initialShader);

  // A rotation that swaps the UV axes requires a new shader.
  frame.rotation = StreamedImageVisual::Rotation::DEGREE_270;
  visual->SetFrame(frame);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(renderer.GetShader() != initialShader);
  DALI_TEST_CHECK(GetVertexShaderSource(renderer).find("#define IS_REQUIRED_SWAP_UV") != std::string::npos);

  // Leaving the scene removes the renderer.
  control.Unparent();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(control.GetRendererCount(), 0u, TEST_LOCATION);

  END_TEST;
}
