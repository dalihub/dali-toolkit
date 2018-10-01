/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/scene/scene.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_scene_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_scene_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

/**
 * For the AnimatedCube.gltf and its Assets
 * Donated by Norbert Nopper for glTF testing.
 * Take from https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
const char* TEST_GLTF_FILE_NAME = TEST_RESOURCE_DIR "/AnimatedCube.gltf";
/**
 * For the diffuse and specular cube map texture.
 * These textures are based off version of Wave engine sample
 * Take from https://github.com/WaveEngine/Samples
 *
 * Copyright (c) 2016 Wave Coorporation
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
const char* TEST_DIFFUSE_TEXTURE = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png";
const char* TEST_SPECULAR_TEXTURE = TEST_RESOURCE_DIR "/forest_specular_cubemap.png";
}

int UtcDaliSceneConstructorP(void)
{
  TestApplication application;

  Scene scene;

  DALI_TEST_CHECK( !scene );
  END_TEST;
}

int UtcDaliSceneCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  Scene scene = Scene::New( TEST_GLTF_FILE_NAME );

  Scene copy( scene );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliSceneCopyConstructor2P(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  Toolkit::Scene scene = Toolkit::Scene::New( TEST_GLTF_FILE_NAME, TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE );

  Scene copy( scene );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliSceneAssignmentOperatorP(void)
{
  TestApplication application;

  Scene scene = Scene::New( TEST_GLTF_FILE_NAME );

  Scene copy( scene );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( scene == copy );
  END_TEST;
}

int UtcDaliSceneNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSceneNewP");

  // Create the Slider actor
  Scene scene;
  DALI_TEST_CHECK( !scene );

  scene = Scene::New( TEST_GLTF_FILE_NAME );
  DALI_TEST_CHECK( scene );

  END_TEST;
}

int UtcDaliSceneDestructorP(void)
{
  ToolkitTestApplication application;

  Scene* scene = new Scene();
  delete scene;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliSceneDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSceneDownCast");

  Toolkit::Scene view = Toolkit::Scene::New( TEST_GLTF_FILE_NAME );
  BaseHandle handle(view);

  Toolkit::Scene scene = Toolkit::Scene::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( scene );
  DALI_TEST_CHECK( scene == view );
  END_TEST;
}

int UtcDaliSceneSetLight(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSceneSetLight");

  Toolkit::Scene view = Toolkit::Scene::New( TEST_GLTF_FILE_NAME );

  bool lightSet = view.SetLight( Scene::LightType::DIRECTIONAL_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
  DALI_TEST_CHECK( lightSet );
  bool lightSet2 = view.SetLight( Scene::LightType::POINT_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
  DALI_TEST_CHECK( lightSet2 );

  END_TEST;
}

int UtcDaliSceneGetCamera(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSceneGetCamera");

  Toolkit::Scene view = Toolkit::Scene::New( TEST_GLTF_FILE_NAME );

  CameraActor camera = view.GetDefaultCamera();
  DALI_TEST_CHECK( camera );

  CameraActor camera2 = view.GetCamera( -1 );
  DALI_TEST_CHECK( camera2 );

  CameraActor camera3 = view.GetCamera( 0 );
  DALI_TEST_CHECK( camera3 );

  END_TEST;
}

int UtcDaliSceneAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliSceneAnimation");

  Toolkit::Scene view = Toolkit::Scene::New( TEST_GLTF_FILE_NAME );

  bool playAnimation = view.PlayAnimations();
  DALI_TEST_CHECK( playAnimation );

  END_TEST;
}
