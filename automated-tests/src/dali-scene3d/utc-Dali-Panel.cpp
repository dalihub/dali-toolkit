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
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <stdlib.h>
#include <iostream>

#include <dali-toolkit/devel-api/focus-manager/keyboard-focus-manager-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>

#include <dali-scene3d/public-api/controls/panel/panel.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali/devel-api/actors/camera-actor-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

void panel_startup(void)
{
  test_return_value = TET_UNDEF;
}

void panel_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Negative test case for a method
int UtcDaliPanelUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelUninitialized");

  Scene3D::Panel panel;

  try
  {
    // New() must be called to create a Panel or it wont be valid.
    Actor a = Actor::New();
    panel.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!panel);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliPanelNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelNew");

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);
  END_TEST;
}

int UtcDaliPanelCopy(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelCopy");

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);

  Scene3D::Panel panelCopy = Scene3D::Panel(panel);
  DALI_TEST_CHECK(panelCopy);

  END_TEST;
}

int UtcDaliPanelDestruct(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelDestruct");

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);
  panel.Reset();
  DALI_TEST_CHECK(!panel);
  END_TEST;
}

int UtcDaliPanelOnSceneOffScene(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelOnSceneOffScene");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  application.GetWindow().Add(sceneView);

  int previousTaskCount = application.GetWindow().GetRenderTaskList().GetTaskCount();

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);
  sceneView.Add(panel);

  int taskCount = application.GetWindow().GetRenderTaskList().GetTaskCount();
  DALI_TEST_EQUALS(previousTaskCount + 1, taskCount, TEST_LOCATION);

  panel.Unparent();
  panel.Reset();

  DALI_TEST_EQUALS(previousTaskCount, application.GetWindow().GetRenderTaskList().GetTaskCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanelSetPanelResolution01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelSetPanelResolution01");

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);

  DALI_TEST_EQUALS(Vector2::ZERO, panel.GetPanelResolution(), TEST_LOCATION);

  panel.SetPanelResolution(Vector2(300, 500));

  DALI_TEST_EQUALS(Vector2(300, 500), panel.GetPanelResolution(), TEST_LOCATION);

  panel.SetPanelResolution(Vector2(500, 300));

  DALI_TEST_EQUALS(Vector2(500, 300), panel.GetPanelResolution(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanelSetPanelResolution02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelSetPanelResolution02");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  application.GetWindow().Add(sceneView);

  int previousTaskCount = application.GetWindow().GetRenderTaskList().GetTaskCount();

  Scene3D::Panel panel = Scene3D::Panel::New();
  DALI_TEST_CHECK(panel);
  sceneView.Add(panel);

  int taskCount = application.GetWindow().GetRenderTaskList().GetTaskCount();
  DALI_TEST_EQUALS(previousTaskCount + 1, taskCount, TEST_LOCATION);

  panel.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_EQUALS(Vector2(300, 500), panel.GetPanelResolution(), TEST_LOCATION);

  RenderTask task = application.GetWindow().GetRenderTaskList().GetTask(taskCount - 1); // Newly added task.
  DALI_TEST_CHECK(task);

  Dali::FrameBuffer framebuffer = task.GetFrameBuffer();
  DALI_TEST_CHECK(framebuffer);

  Dali::Texture texture = framebuffer.GetColorTexture();
  DALI_TEST_CHECK(texture);

  DALI_TEST_EQUALS(300, texture.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(500, texture.GetHeight(), TEST_LOCATION);

  panel.SetPanelResolution(Vector2(500, 300));
  DALI_TEST_EQUALS(Vector2(500, 300), panel.GetPanelResolution(), TEST_LOCATION);

  framebuffer = task.GetFrameBuffer();
  DALI_TEST_CHECK(framebuffer);

  texture = framebuffer.GetColorTexture();
  DALI_TEST_CHECK(texture);

  DALI_TEST_EQUALS(500, texture.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(300, texture.GetHeight(), TEST_LOCATION);

  END_TEST;
}

namespace
{
/**
 * For the diffuse and specular cube map texture.
 * These textures are based off version of Wave engine sample
 * Take from https://github.com/WaveEngine/Samples
 *
 * Copyright (c) 2025 Wave Coorporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
const char* TEST_DIFFUSE_TEXTURE  = TEST_RESOURCE_DIR "/forest_irradiance.ktx";
const char* TEST_SPECULAR_TEXTURE = TEST_RESOURCE_DIR "/forest_radiance.ktx";

static bool gResourceReadyCalled = false;
void        OnResourceReady(Control control)
{
  gResourceReadyCalled = true;
}

Dali::Scene3D::ModelNode GetContentPlaneNode(Dali::Scene3D::Panel panel)
{
  Dali::Scene3D::ModelNode contentPlaneNode;
  Dali::Actor              panelNode;
  for(uint32_t i = 0; i < panel.GetChildCount(); ++i)
  {
    Dali::Actor        actor     = panel.GetChildAt(i);
    Scene3D::ModelNode modelNode = Scene3D::ModelNode::DownCast(actor);
    if(modelNode)
    {
      panelNode = modelNode;
      break;
    }
  }

  if(panelNode)
  {
    DALI_TEST_EQUALS(3, panelNode.GetChildCount(), TEST_LOCATION);

    Dali::Actor childPanel = panelNode.GetChildAt(0u);
    if(childPanel)
    {
      contentPlaneNode = Scene3D::ModelNode::DownCast(childPanel);
    }
  }

  return contentPlaneNode;
}

Dali::Scene3D::ModelNode GetBackPlaneNode(Dali::Scene3D::Panel panel)
{
  Dali::Scene3D::ModelNode backPlaneNode;
  Dali::Actor              panelNode;
  for(uint32_t i = 0; i < panel.GetChildCount(); ++i)
  {
    Dali::Actor        actor     = panel.GetChildAt(i);
    Scene3D::ModelNode modelNode = Scene3D::ModelNode::DownCast(actor);
    if(modelNode)
    {
      panelNode = modelNode;
      break;
    }
  }

  if(panelNode)
  {
    DALI_TEST_EQUALS(3, panelNode.GetChildCount(), TEST_LOCATION);

    Dali::Actor childPanel = panelNode.GetChildAt(1u);
    if(childPanel)
    {
      backPlaneNode = Scene3D::ModelNode::DownCast(childPanel);
    }
  }

  return backPlaneNode;
}

Dali::Scene3D::ModelNode GetDoubleSidedPlaneNode(Dali::Scene3D::Panel panel)
{
  Dali::Scene3D::ModelNode backPlaneNode;
  Dali::Actor              panelNode;
  for(uint32_t i = 0; i < panel.GetChildCount(); ++i)
  {
    Dali::Actor        actor     = panel.GetChildAt(i);
    Scene3D::ModelNode modelNode = Scene3D::ModelNode::DownCast(actor);
    if(modelNode)
    {
      panelNode = modelNode;
      break;
    }
  }

  if(panelNode)
  {
    DALI_TEST_EQUALS(3, panelNode.GetChildCount(), TEST_LOCATION);

    Dali::Actor childPanel = panelNode.GetChildAt(2u);
    if(childPanel)
    {
      backPlaneNode = Scene3D::ModelNode::DownCast(childPanel);
    }
  }

  return backPlaneNode;
}

Dali::Texture GetDiffuseTexture(Dali::Scene3D::Panel panel)
{
  Dali::Texture texture;

  Dali::Actor childPanel = GetContentPlaneNode(panel);
  if(childPanel)
  {
    Renderer renderer = childPanel.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      uint32_t   textureCnt = textureSet.GetTextureCount();
      DALI_TEST_CHECK(textureCnt >= 2);
      texture = textureSet.GetTexture(textureCnt - 2u);
    }
  }

  DALI_TEST_CHECK(texture);

  return texture;
}

Dali::Texture GetSpecularTexture(Dali::Scene3D::Panel panel)
{
  Dali::Texture texture;

  Dali::Actor childPanel = GetContentPlaneNode(panel);
  if(childPanel)
  {
    Renderer renderer = childPanel.GetRendererAt(0u);
    if(renderer)
    {
      TextureSet textureSet = renderer.GetTextures();
      uint32_t   textureCnt = textureSet.GetTextureCount();
      DALI_TEST_CHECK(textureCnt >= 2);
      texture = textureSet.GetTexture(textureCnt - 1u);
    }
  }

  DALI_TEST_CHECK(texture);

  return texture;
}
} // namespace

int UtcDaliPanelIBLWithSceneView(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelIBLWithSceneView");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.ResourceReadySignal().Connect(OnResourceReady);
  application.GetWindow().Add(sceneView);

  Scene3D::Panel panel1 = Scene3D::Panel::New();
  panel1.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_CHECK(panel1);
  sceneView.Add(panel1);

  Scene3D::Panel panel2 = Scene3D::Panel::New();
  panel2.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_CHECK(panel2);
  sceneView.Add(panel2);

  gResourceReadyCalled = false;
  DALI_TEST_EQUALS(gResourceReadyCalled, false, TEST_LOCATION);
  sceneView.SetImageBasedLightSource(TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gResourceReadyCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(GetDiffuseTexture(panel1), GetDiffuseTexture(panel2), TEST_LOCATION);
  DALI_TEST_EQUALS(GetSpecularTexture(panel1), GetSpecularTexture(panel2), TEST_LOCATION);

  // For coverage
  sceneView.SetImageBasedLightScaleFactor(0.4f);

  // Reset
  sceneView.SetImageBasedLightSource("", "");

  END_TEST;
}

int UtcDaliPanelSetGetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelSetGetProperty");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  application.GetWindow().Add(sceneView);

  Scene3D::Panel panel = Scene3D::Panel::New();
  panel.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_CHECK(panel);
  sceneView.Add(panel);

  Dali::Scene3D::ModelNode contentPlaneNode = GetContentPlaneNode(panel);
  DALI_TEST_CHECK(contentPlaneNode);
  DALI_TEST_CHECK(contentPlaneNode.GetModelPrimitiveCount() > 0);
  DALI_TEST_CHECK(contentPlaneNode.GetModelPrimitive(0u));

  Dali::Scene3D::Material contentPlaneMaterial = contentPlaneNode.GetModelPrimitive(0u).GetMaterial();
  DALI_TEST_CHECK(contentPlaneMaterial);

  Dali::Scene3D::ModelNode backPlaneNode = GetBackPlaneNode(panel);
  DALI_TEST_CHECK(backPlaneNode);
  DALI_TEST_CHECK(backPlaneNode.GetModelPrimitiveCount() > 0);
  DALI_TEST_CHECK(backPlaneNode.GetModelPrimitive(0u));

  Dali::Scene3D::Material backPlaneMaterial = backPlaneNode.GetModelPrimitive(0u).GetMaterial();
  DALI_TEST_CHECK(backPlaneMaterial);

  Dali::Scene3D::ModelNode doubleSidedPlaneNode = GetDoubleSidedPlaneNode(panel);
  DALI_TEST_CHECK(doubleSidedPlaneNode);
  DALI_TEST_CHECK(doubleSidedPlaneNode.GetModelPrimitiveCount() > 0);
  DALI_TEST_CHECK(doubleSidedPlaneNode.GetModelPrimitive(0u));

  Vector4 backPlaneMaterialBaseColorFactor = backPlaneMaterial.GetProperty<Vector4>(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR);
  DALI_TEST_EQUALS(Vector3(backPlaneMaterialBaseColorFactor), panel.GetProperty<Vector3>(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR), TEST_LOCATION);

  panel.SetProperty(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR, Vector3(1.0f, 0.0f, 0.0f));
  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f), panel.GetProperty<Vector3>(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR), TEST_LOCATION);

  backPlaneMaterialBaseColorFactor = backPlaneMaterial.GetProperty<Vector4>(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR);
  DALI_TEST_EQUALS(Vector3(backPlaneMaterialBaseColorFactor), panel.GetProperty<Vector3>(Dali::Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR), TEST_LOCATION);

  bool                                   isTransparent;
  Dali::Scene3D::Material::AlphaModeType contentPlaneAlphaMode;
  bool                                   isUsingBackFacePlane;
  bool                                   isBackPlaneVisible;
  bool                                   isDoubleSided;

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  // Case 1. transparent false, double sided false, useBackFacePlane false;
  // Front Material Alpha Mode : Opaque, Back Plane Visible : false, Front Material Double Sided : false
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, false);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  isUsingBackFacePlane  = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible    = !isTransparent && isUsingBackFacePlane;
  isDoubleSided         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);

  // Case 2. transparent false, double sided false, useBackFacePlane true;
  // Front Material Alpha Mode : Opaque, Back Plane Visible : true, Front Material Double Sided : false
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, true);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);

  // Case 2. transparent false, double sided true, useBackFacePlane false;
  // Front Material Alpha Mode : Opaque, Back Plane Visible : false, Front Material Double Sided : true
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, false);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), true, TEST_LOCATION);

  // Case 2. transparent false, double sided true, useBackFacePlane true;
  // Front Material Alpha Mode : Opaque, Back Plane Visible : true, Front Material Double Sided : false
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, true);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);

  // Case 1. transparent true, double sided false, useBackFacePlane false;
  // Front Material Alpha Mode : Blend, Back Plane Visible : false, Front Material Double Sided : false
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, false);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::BLEND, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);

  // Case 2. transparent true, double sided false, useBackFacePlane true;
  // Front Material Alpha Mode : Blend, Back Plane Visible : false, Front Material Double Sided : false
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, false);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, true);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::BLEND, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);

  // Case 2. transparent true, double sided true, useBackFacePlane false;
  // Front Material Alpha Mode : Blend, Back Plane Visible : false, Front Material Double Sided : true
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, false);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::BLEND, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), true, TEST_LOCATION);

  // Case 2. transparent true, double sided true, useBackFacePlane true;
  // Front Material Alpha Mode : Blend, Back Plane Visible : false, Front Material Double Sided : true
  panel.SetProperty(Dali::Scene3D::Panel::Property::TRANSPARENT, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::DOUBLE_SIDED, true);
  panel.SetProperty(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE, true);

  isTransparent         = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::TRANSPARENT);
  contentPlaneAlphaMode = contentPlaneMaterial.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Dali::Scene3D::Material::Property::ALPHA_MODE);
  DALI_TEST_EQUALS(contentPlaneAlphaMode, isTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE, TEST_LOCATION);
  isUsingBackFacePlane = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::USE_BACK_FACE_PLANE);
  isBackPlaneVisible   = !isTransparent && isUsingBackFacePlane;
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isBackPlaneVisible, TEST_LOCATION);
  isDoubleSided = panel.GetProperty<bool>(Dali::Scene3D::Panel::Property::DOUBLE_SIDED);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), isDoubleSided && !isBackPlaneVisible, TEST_LOCATION);

  DALI_TEST_EQUALS(contentPlaneAlphaMode, Dali::Scene3D::Material::AlphaModeType::BLEND, TEST_LOCATION);
  DALI_TEST_EQUALS(backPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), false, TEST_LOCATION);
  DALI_TEST_EQUALS(doubleSidedPlaneNode.GetProperty<bool>(Dali::Actor::Property::VISIBLE), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanelSetGetContent(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelSetPanelResolution");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  application.GetWindow().Add(sceneView);

  Scene3D::Panel panel = Scene3D::Panel::New();
  panel.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_CHECK(panel);
  sceneView.Add(panel);

  Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
  control.SetProperty(Dali::Actor::Property::SIZE, Vector2(300, 300));
  control.SetBackgroundColor(Color::BLACK);
  panel.SetContent(control);
  DALI_TEST_EQUALS(control, panel.GetContent(), TEST_LOCATION);

  Dali::Toolkit::Control retrievedControl;
  Dali::Layer            panelRootLayer;
  for(uint32_t i = 0; i < panel.GetChildCount(); ++i)
  {
    Dali::Actor child = panel.GetChildAt(i);
    Dali::Layer layer = Dali::Layer::DownCast(child);
    if(layer)
    {
      panelRootLayer = layer;
      break;
    }
  }
  DALI_TEST_CHECK(panelRootLayer);

  for(uint32_t i = 0; i < panelRootLayer.GetChildCount(); ++i)
  {
    Dali::Actor            child        = panelRootLayer.GetChildAt(i);
    Dali::Toolkit::Control childControl = Dali::Toolkit::Control::DownCast(child);
    if(childControl)
    {
      retrievedControl = childControl;
      break;
    }
  }
  DALI_TEST_CHECK(retrievedControl);

  DALI_TEST_EQUALS(control, retrievedControl, TEST_LOCATION);

  Dali::Toolkit::Control secondControl = Dali::Toolkit::Control::New();
  secondControl.SetProperty(Dali::Actor::Property::SIZE, Vector2(300, 300));
  secondControl.SetBackgroundColor(Color::BLACK);
  panel.SetContent(secondControl);
  DALI_TEST_EQUALS(secondControl, panel.GetContent(), TEST_LOCATION);

  retrievedControl.Reset();
  panelRootLayer.Reset();
  for(uint32_t i = 0; i < panel.GetChildCount(); ++i)
  {
    Dali::Actor child = panel.GetChildAt(i);
    Dali::Layer layer = Dali::Layer::DownCast(child);
    if(layer)
    {
      panelRootLayer = layer;
      break;
    }
  }
  DALI_TEST_CHECK(panelRootLayer);

  for(uint32_t i = 0; i < panelRootLayer.GetChildCount(); ++i)
  {
    Dali::Actor            child        = panelRootLayer.GetChildAt(i);
    Dali::Toolkit::Control childControl = Dali::Toolkit::Control::DownCast(child);
    if(childControl)
    {
      retrievedControl = childControl;
      break;
    }
  }
  DALI_TEST_CHECK(retrievedControl);

  DALI_TEST_EQUALS(secondControl, retrievedControl, TEST_LOCATION);
  DALI_TEST_CHECK(retrievedControl != control);

  DALI_TEST_CHECK(secondControl.GetParent());
  panel.ClearPanel();
  DALI_TEST_CHECK(!secondControl.GetParent());

  END_TEST;
}

int UtcDaliPanelSetGetShadow(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliPanelSetGetShadow");

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  application.GetWindow().Add(sceneView);

  Scene3D::Panel panel = Scene3D::Panel::New();
  panel.SetPanelResolution(Vector2(300, 500));
  DALI_TEST_CHECK(panel);
  sceneView.Add(panel);

  panel.CastShadow(true);
  DALI_TEST_EQUALS(panel.IsShadowCasting(), true, TEST_LOCATION);

  panel.CastShadow(false);
  DALI_TEST_EQUALS(panel.IsShadowCasting(), false, TEST_LOCATION);

  panel.ReceiveShadow(true);
  DALI_TEST_EQUALS(panel.IsShadowReceiving(), true, TEST_LOCATION);

  panel.ReceiveShadow(false);
  DALI_TEST_EQUALS(panel.IsShadowReceiving(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanelRenderTaskOrdering(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliPanelRenderTaskOrdering");

  Integration::Scene scene    = application.GetScene();
  RenderTaskList     taskList = scene.GetRenderTaskList();

  uint32_t   defaultTaskCount  = taskList.GetTaskCount();
  RenderTask defaultRenderTask = taskList.GetTask(defaultTaskCount - 1);
  tet_printf("default Task Cnt : %d\n", defaultTaskCount);

  Scene3D::SceneView sceneView = Scene3D::SceneView::New();
  sceneView.UseFramebuffer(true);
  scene.Add(sceneView);

  uint32_t   afterSceneViewTaskCount = taskList.GetTaskCount();
  RenderTask sceneViewRenderTask     = taskList.GetTask(afterSceneViewTaskCount - 1);
  tet_printf("after SceneView Task cnt : %d\n", afterSceneViewTaskCount);
  DALI_TEST_CHECK(afterSceneViewTaskCount == defaultTaskCount + 1);

  Scene3D::Panel panel = Scene3D::Panel::New();
  sceneView.Add(panel);

  uint32_t   afterPanelTaskCount = taskList.GetTaskCount();
  RenderTask panelRenderTask     = taskList.GetTask(afterPanelTaskCount - 1);
  tet_printf("after Panel Task cnt : %d\n", afterPanelTaskCount);
  DALI_TEST_CHECK(afterPanelTaskCount == afterSceneViewTaskCount + 1);

  Control control1 = Control::New();
  control1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  control1.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 1.0f));
  control1.SetRenderEffect(BackgroundBlurEffect::New());

  panel.Add(control1);

  uint32_t   afterBlurEffectTaskCount = taskList.GetTaskCount();
  RenderTask blurSourceRenderTask     = taskList.GetTask(afterBlurEffectTaskCount - 3);
  RenderTask blurHorizontalRenderTask = taskList.GetTask(afterBlurEffectTaskCount - 2);
  RenderTask blurVerticalRenderTask   = taskList.GetTask(afterBlurEffectTaskCount - 1);
  tet_printf("after blurEffect Task cnt : %d\n", afterBlurEffectTaskCount);
  DALI_TEST_CHECK(afterBlurEffectTaskCount == afterPanelTaskCount + 3);

  tet_printf("defaultRenderTask order : %d\n", defaultRenderTask.GetOrderIndex());
  tet_printf("sceneViewRenderTask order : %d\n", sceneViewRenderTask.GetOrderIndex());
  tet_printf("panelRenderTask order : %d\n", panelRenderTask.GetOrderIndex());
  tet_printf("blurSourceRenderTask order : %d\n", blurSourceRenderTask.GetOrderIndex());
  tet_printf("blurHorizontalRenderTask order : %d\n", blurHorizontalRenderTask.GetOrderIndex());
  tet_printf("blurVerticalRenderTask order : %d\n", blurVerticalRenderTask.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN, defaultRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, sceneViewRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(90, panelRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, blurSourceRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, blurHorizontalRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, blurVerticalRenderTask.GetOrderIndex(), TEST_LOCATION);

  application.SendNotification();

  tet_printf("defaultRenderTask order : %d\n", defaultRenderTask.GetOrderIndex());
  tet_printf("sceneViewRenderTask order : %d\n", sceneViewRenderTask.GetOrderIndex());
  tet_printf("panelRenderTask order : %d\n", panelRenderTask.GetOrderIndex());
  tet_printf("blurSourceRenderTask order : %d\n", blurSourceRenderTask.GetOrderIndex());
  tet_printf("blurHorizontalRenderTask order : %d\n", blurHorizontalRenderTask.GetOrderIndex());
  tet_printf("blurVerticalRenderTask order : %d\n", blurVerticalRenderTask.GetOrderIndex());

  DALI_TEST_EQUALS(INT32_MIN, defaultRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 4, sceneViewRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 3, panelRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN, blurSourceRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 1, blurHorizontalRenderTask.GetOrderIndex(), TEST_LOCATION);
  DALI_TEST_EQUALS(INT32_MIN + 2, blurVerticalRenderTask.GetOrderIndex(), TEST_LOCATION);

  END_TEST;
}
