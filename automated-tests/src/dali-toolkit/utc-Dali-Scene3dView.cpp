/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/scene3d-view/scene3d-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_scene_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_scene_view_cleanup(void)
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
const char* TEST_GLTF_FILE_NAME[] = {TEST_RESOURCE_DIR "/AnimatedCube.gltf",
                                     TEST_RESOURCE_DIR "/InterpolationTest.gltf",
                                     TEST_RESOURCE_DIR "/coverageTest.gltf"};
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

int UtcDaliScene3dViewConstructorP(void)
{
  ToolkitTestApplication application;

  Scene3dView scene3dView;

  DALI_TEST_CHECK( !scene3dView );
  END_TEST;
}

int UtcDaliScene3dViewCopyConstructorP(void)
{
  ToolkitTestApplication application;

  // Initialize an object, ref count == 1
  Scene3dView scene3dView = Scene3dView::New( TEST_GLTF_FILE_NAME[0] );

  Scene3dView copy( scene3dView );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliScene3dViewCopyConstructor2P(void)
{
  ToolkitTestApplication application;

  // Initialize an object, ref count == 1
  Toolkit::Scene3dView scene3dView = Toolkit::Scene3dView::New( TEST_GLTF_FILE_NAME[0], TEST_DIFFUSE_TEXTURE, TEST_SPECULAR_TEXTURE, Vector4::ONE );

  Scene3dView copy( scene3dView );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliScene3dViewAssignmentOperatorP(void)
{
  ToolkitTestApplication application;

  Scene3dView scene3dView = Scene3dView::New( TEST_GLTF_FILE_NAME[0] );

  Scene3dView copy( scene3dView );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( scene3dView == copy );
  END_TEST;
}

int UtcDaliScene3dViewNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewNewP");

  // Create the Slider actor
  Scene3dView scene3dView;
  DALI_TEST_CHECK( !scene3dView );

  scene3dView = Scene3dView::New( TEST_GLTF_FILE_NAME[0] );
  DALI_TEST_CHECK( scene3dView );

  END_TEST;
}

int UtcDaliScene3dViewDestructorP(void)
{
  ToolkitTestApplication application;

  Scene3dView* scene3dView = new Scene3dView();
  delete scene3dView;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliScene3dViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewDownCast");

  Toolkit::Scene3dView view = Toolkit::Scene3dView::New( TEST_GLTF_FILE_NAME[0] );
  BaseHandle handle(view);

  Toolkit::Scene3dView scene3dView = Toolkit::Scene3dView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( scene3dView );
  DALI_TEST_CHECK( scene3dView == view );
  END_TEST;
}

int UtcDaliScene3dViewSetLight(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewSetLight");

  Toolkit::Scene3dView view = Toolkit::Scene3dView::New( TEST_GLTF_FILE_NAME[0] );

  bool lightSet = view.SetLight( Scene3dView::LightType::DIRECTIONAL_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
  DALI_TEST_CHECK( lightSet );
  bool lightSet2 = view.SetLight( Scene3dView::LightType::POINT_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
  DALI_TEST_CHECK( lightSet2 );

  END_TEST;
}

int UtcDaliScene3dViewGetCamera(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewGetCamera");

  Toolkit::Scene3dView view = Toolkit::Scene3dView::New( TEST_GLTF_FILE_NAME[0] );

  CameraActor camera = view.GetDefaultCamera();
  DALI_TEST_CHECK( camera );

  CameraActor camera2 = view.GetCamera( -1 );
  DALI_TEST_CHECK( !camera2 );

  CameraActor camera3 = view.GetCamera( 0 );
  DALI_TEST_CHECK( camera3 );

  CameraActor camera4 = view.GetCamera( view.GetCameraCount() - 1 );
  DALI_TEST_CHECK( camera4 );

  END_TEST;
}

int UtcDaliScene3dViewAnimations(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewAnimations");

  for( auto filename : TEST_GLTF_FILE_NAME )
  {
    Toolkit::Scene3dView view = Toolkit::Scene3dView::New( filename );

    bool playAnimation = view.PlayAnimations();
    DALI_TEST_CHECK( playAnimation );
  }

  END_TEST;
}

int UtcDaliScene3dViewAnimations2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScene3dViewAnimations2");

  Toolkit::Scene3dView view = Toolkit::Scene3dView::New( TEST_GLTF_FILE_NAME[0] );

  bool animated = true;
  unsigned int animationCount = view.GetAnimationCount();
  for( unsigned int i = 0; i < animationCount; ++i )
  {
    animated = ( animated && view.PlayAnimation( i ) );
  }
  DALI_TEST_CHECK( animated );

  END_TEST;
}
