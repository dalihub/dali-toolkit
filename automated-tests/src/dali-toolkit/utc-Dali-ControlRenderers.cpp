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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-renderers.h>

using namespace Dali;
using namespace Toolkit;

void dali_controlrenderers_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_controlrenderers_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliControlRenderersCreateRendererWithShaders(void)
{
  ToolkitTestApplication application;

  Renderer renderer = DevelControl::CreateRenderer("vertexShader", "fragmentShader");

  DALI_TEST_CHECK(renderer);

  END_TEST;
}

int UtcDaliControlRenderersCreateRendererWithHints(void)
{
  ToolkitTestApplication application;

  Uint16Pair gridSize(20, 20);
  Renderer renderer = DevelControl::CreateRenderer("vertexShader", "fragmentShader", Shader::Hint::OUTPUT_IS_TRANSPARENT, "TestShader", gridSize);

  DALI_TEST_CHECK(renderer);

  END_TEST;
}

int UtcDaliControlRenderersSetRendererTexture(void)
{
  ToolkitTestApplication application;

  unsigned int width  = 64;
  unsigned int height = 64;
  Texture     texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);

  Renderer renderer = DevelControl::CreateRenderer("vertexShader", "fragmentShader");
  DevelControl::SetRendererTexture(renderer, texture);

  DALI_TEST_CHECK(renderer);

  END_TEST;
}

int UtcDaliControlRenderersSetRendererTextureWithFrameBuffer(void)
{
  ToolkitTestApplication application;

  unsigned int width  = 64;
  unsigned int height = 64;

  FrameBuffer frameBuffer = FrameBuffer::New(width, height, FrameBuffer::Attachment::NONE);
  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  frameBuffer.AttachColorTexture(texture);

  Renderer renderer = DevelControl::CreateRenderer("vertexShader", "fragmentShader");
  DevelControl::SetRendererTexture(renderer, frameBuffer);

  DALI_TEST_CHECK(renderer);

  END_TEST;
}
