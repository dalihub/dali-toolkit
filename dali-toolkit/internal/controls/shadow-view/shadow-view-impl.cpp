/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "shadow-view-impl.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/controls/shadow-view/shadow-view-impl.h>
#include <dali-toolkit/internal/filters/blur-two-pass-filter.h>

// TODO:
// pixel format / size - set from JSON
// aspect ratio property needs to be able to be constrained also for cameras. (now do-able)
// default near clip value


/////////////////////////////////////////////////////////
// IMPLEMENTATION NOTES

// As the ShadowView actor changes size, the amount of pixels we need to blur changes. Therefore we need some way of doing this. However:-
// OnSetSize() does not get called when ShadowView object size is modified using a Constraint.
// OnSizeAnimation() only gets called once per AnimateTo/By() and if an Animation has N such calls then only the final one will end up being used. Therefore we can't use
// OnSizeAnimation() to alter render target sizes.
// To get around the above problems, we use fixed sized render targets, from the last SetSize() call (which calls OnSetSize()), then we adjust the internal cameras / actors
// to take account of the changed ShadowView object size, projecting to the unchanged render target sizes. This is done relative to the fixed render target / actor sizes
// by using constraints relative to the ShadowView actor size.

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

using namespace Dali;

BaseHandle Create()
{
  return Toolkit::ShadowView::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ShadowView, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

const float BLUR_STRENGTH_DEFAULT = 1.0f;

const Vector3 DEFAULT_LIGHT_POSITION(300.0f, 250.0f, 600.0f);
const float DEFAULT_FIELD_OF_VIEW_RADIANS = Math::PI / 4.0f; // 45 degrees

const Vector4 DEFAULT_SHADOW_COLOR = Vector4(0.2f, 0.2f, 0.2f, 0.8f);

const char* const SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME = "uLightCameraProjectionMatrix";
const char* const SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME = "uLightCameraViewMatrix";
const char* const SHADER_SHADOW_COLOR_PROPERTY_NAME = "uShadowColor";
const char* const BLUR_STRENGTH_PROPERTY_NAME = "BlurStrengthProperty";
const char* const SHADOW_COLOR_PROPERTY_NAME = "ShadowColorProperty";

const char* const RENDER_SHADOW_VERTEX_SOURCE =

  " attribute mediump vec2 aPosition;\n"
  " uniform mediump mat4 uMvpMatrix;\n"
  " uniform mediump mat4 uModelMatrix;\n"
  " uniform vec3 uSize;\n"
  " varying vec2 vTexCoord;\n"

  " uniform mediump mat4 uLightCameraProjectionMatrix;\n"
  " uniform mediump mat4 uLightCameraViewMatrix;\n"
  "\n"
  "void main()\n"
  "{\n"
    "  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n"
    "  vertexPosition.xyz *= uSize;\n"
    "  gl_Position = uMvpMatrix * vertexPosition;\n"
    "  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;\n"
    "  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);\n"
  "}\n";

const char* const RENDER_SHADOW_FRAGMENT_SOURCE =
  "varying mediump vec2 vTexCoord;\n"
  "uniform lowp vec4 uShadowColor;\n"
  "uniform sampler2D sTexture;\n"

  "void main()\n"
  "{\n"
  "  lowp float alpha;\n"
  "  alpha = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;\n"
  "  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);\n"
  "}\n";

} // namespace

ShadowView::ShadowView( float downsampleWidthScale, float downsampleHeightScale )
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mChildrenRoot(Actor::New()),
  mCachedShadowColor(DEFAULT_SHADOW_COLOR),
  mCachedBackgroundColor(DEFAULT_SHADOW_COLOR.r, DEFAULT_SHADOW_COLOR.g, DEFAULT_SHADOW_COLOR.b, 0.0f),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mShadowColorPropertyIndex(Property::INVALID_INDEX),
  mDownsampleWidthScale(downsampleWidthScale),
  mDownsampleHeightScale(downsampleHeightScale)
{
}

ShadowView::~ShadowView()
{
}

Toolkit::ShadowView ShadowView::New(float downsampleWidthScale, float downsampleHeightScale)
{
  ShadowView* impl = new ShadowView(downsampleWidthScale, downsampleHeightScale);

  Dali::Toolkit::ShadowView handle = Dali::Toolkit::ShadowView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void ShadowView::SetShadowPlaneBackground(Actor shadowPlaneBackground)
{
  mShadowPlaneBg = shadowPlaneBackground;

  mShadowPlane = Toolkit::ImageView::New( mOutputImage );
  mShadowPlane.SetName( "SHADOW_PLANE" );
  mShadowPlane.SetParentOrigin(ParentOrigin::CENTER);
  mShadowPlane.SetAnchorPoint(AnchorPoint::CENTER);

  mShadowPlane.SetProperty( Toolkit::ImageView::Property::IMAGE, mShadowVisualMap );
  SetShaderConstants();

  // Rather than parent the shadow plane drawable and have constraints to move it to the same
  // position, instead parent the shadow plane drawable on the shadow plane passed in.
  mShadowPlaneBg.Add(mShadowPlane);
  mShadowPlane.SetParentOrigin(ParentOrigin::CENTER);
  mShadowPlane.SetZ(1.0f);

  ConstrainCamera();

  mShadowPlane.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  mBlurRootActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
}

void ShadowView::SetPointLight(Actor pointLight)
{
  mPointLight = pointLight;

  ConstrainCamera();
}

void ShadowView::SetPointLightFieldOfView(float fieldOfView)
{
  mCameraActor.SetFieldOfView(fieldOfView);
}

void ShadowView::SetShadowColor(Vector4 color)
{
  mCachedShadowColor = color;
  mCachedBackgroundColor.r = color.r;
  mCachedBackgroundColor.g = color.g;
  mCachedBackgroundColor.b = color.b;

  if( mShadowPlane )
  {
    mShadowPlane.SetProperty( mShadowColorPropertyIndex, mCachedShadowColor );
  }
  if(mRenderSceneTask)
  {
    mRenderSceneTask.SetClearColor( mCachedBackgroundColor );
  }
}

void ShadowView::Activate()
{
  DALI_ASSERT_ALWAYS( Self().OnStage() && "ShadowView should be on stage before calling Activate()\n" );

  // make sure resources are allocated and start the render tasks processing
  CreateRenderTasks();
}

void ShadowView::Deactivate()
{
  DALI_ASSERT_ALWAYS( Self().OnStage() && "ShadowView should be on stage before calling Deactivate()\n" )

  // stop render tasks processing
  // Note: render target resources are automatically freed since we set the Image::Unused flag
  RemoveRenderTasks();
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void ShadowView::OnInitialize()
{
  // root actor to parent all user added actors. Used as source actor for shadow render task.
  mChildrenRoot.SetParentOrigin( ParentOrigin::CENTER );
  mChildrenRoot.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  mCameraActor = CameraActor::New(stageSize);

  mCameraActor.SetParentOrigin( ParentOrigin::CENTER );

  // Target is constrained to point at the shadow plane origin
  mCameraActor.SetNearClippingPlane( 1.0f );
  mCameraActor.SetType( Dali::Camera::FREE_LOOK ); // Camera orientation constrained to point at shadow plane world position
  mCameraActor.SetOrientation(Radian(Degree(180)), Vector3::YAXIS);
  mCameraActor.SetPosition(DEFAULT_LIGHT_POSITION);


  Property::Map customShader;
  customShader[ Toolkit::Visual::Shader::Property::VERTEX_SHADER ] = RENDER_SHADOW_VERTEX_SOURCE;
  customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = RENDER_SHADOW_FRAGMENT_SOURCE;

  customShader[ Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X ] = 20;
  customShader[ Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y ] = 20;

  customShader[ Toolkit::Visual::Shader::Property::HINTS ] = Shader::Hint::OUTPUT_IS_TRANSPARENT;

  mShadowVisualMap[ Toolkit::DevelVisual::Property::SHADER ] = customShader;

  // Create render targets needed for rendering from light's point of view
  mSceneFromLightRenderTarget = FrameBufferImage::New( stageSize.width, stageSize.height, Pixel::RGBA8888 );

  mOutputImage = FrameBufferImage::New( stageSize.width * 0.5f, stageSize.height * 0.5f, Pixel::RGBA8888 );

  //////////////////////////////////////////////////////
  // Connect to actor tree

  Self().Add( mChildrenRoot );
  Stage::GetCurrent().Add( mCameraActor );

  mBlurFilter.SetRefreshOnDemand(false);
  mBlurFilter.SetInputImage(mSceneFromLightRenderTarget);
  mBlurFilter.SetOutputImage(mOutputImage);
  mBlurFilter.SetSize(stageSize * 0.5f);
  mBlurFilter.SetPixelFormat(Pixel::RGBA8888);

  mBlurRootActor = Actor::New();
  mBlurRootActor.SetName( "BLUR_ROOT_ACTOR" );

  // Turn off inheritance to ensure filter renders properly
  mBlurRootActor.SetParentOrigin( ParentOrigin::CENTER );
  mBlurRootActor.SetInheritPosition(false);
  mBlurRootActor.SetInheritOrientation(false);
  mBlurRootActor.SetInheritScale(false);
  mBlurRootActor.SetColorMode(USE_OWN_COLOR);

  Self().Add(mBlurRootActor);

  mBlurFilter.SetRootActor(mBlurRootActor);
  mBlurFilter.SetBackgroundColor(Vector4::ZERO);

  CustomActor self = Self();
  // Register a property that the user can use to control the blur in the internal object
  mBlurStrengthPropertyIndex = self.RegisterProperty(BLUR_STRENGTH_PROPERTY_NAME, BLUR_STRENGTH_DEFAULT);

  Constraint blurStrengthConstraint = Constraint::New<float>( mBlurFilter.GetHandleForAnimateBlurStrength(), mBlurFilter.GetBlurStrengthPropertyIndex(), EqualToConstraint() );
  blurStrengthConstraint.AddSource( Source( self, mBlurStrengthPropertyIndex) );
  blurStrengthConstraint.Apply();
}

void ShadowView::OnChildAdd( Actor& child )
{
  Control::OnChildAdd( child );

  if( child != mChildrenRoot && child != mBlurRootActor)
  {
    mChildrenRoot.Add( child );
  }
}

void ShadowView::OnChildRemove( Actor& child )
{
  mChildrenRoot.Remove( child );

  Control::OnChildRemove( child );
}

void ShadowView::ConstrainCamera()
{
  if( mPointLight && mShadowPlane )
  {
    // Constrain camera to look directly at center of shadow plane. (mPointLight position
    // is under control of application, can't use transform inheritance)

    Constraint cameraOrientationConstraint = Constraint::New<Quaternion> ( mCameraActor, Actor::Property::ORIENTATION, &LookAt );
    cameraOrientationConstraint.AddSource( Source( mShadowPlane, Actor::Property::WORLD_POSITION ) );
    cameraOrientationConstraint.AddSource( Source( mPointLight,  Actor::Property::WORLD_POSITION ) );
    cameraOrientationConstraint.AddSource( Source( mShadowPlane, Actor::Property::WORLD_ORIENTATION ) );
    cameraOrientationConstraint.Apply();

    Constraint pointLightPositionConstraint = Constraint::New<Vector3>( mCameraActor, Actor::Property::POSITION, EqualToConstraint() );
    pointLightPositionConstraint.AddSource( Source( mPointLight, Actor::Property::WORLD_POSITION ) );
    pointLightPositionConstraint.Apply();
  }
}

void ShadowView::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // We want the first task to render the scene from the light
  mRenderSceneTask = taskList.CreateTask();

  mRenderSceneTask.SetCameraActor( mCameraActor );
  mRenderSceneTask.SetSourceActor( mChildrenRoot );
  mRenderSceneTask.SetTargetFrameBuffer( mSceneFromLightRenderTarget );
  mRenderSceneTask.SetInputEnabled( false );
  mRenderSceneTask.SetClearEnabled( true );

  // background color for render task should be the shadow color, but with alpha 0
  // we don't want to blend the edges of the content with a BLACK at alpha 0, but
  // the same shadow color at alpha 0.
  mRenderSceneTask.SetClearColor( mCachedBackgroundColor );

  mBlurFilter.Enable();
}

void ShadowView::RemoveRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderSceneTask);
  mRenderSceneTask.Reset();

  mBlurFilter.Disable();
}

void ShadowView::SetShaderConstants()
{
  Property::Index lightCameraProjectionMatrixPropertyIndex = mShadowPlane.RegisterProperty( SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );
  Constraint projectionMatrixConstraint = Constraint::New<Dali::Matrix>( mShadowPlane, lightCameraProjectionMatrixPropertyIndex, EqualToConstraint() );
  projectionMatrixConstraint.AddSource( Source( mCameraActor, CameraActor::Property::PROJECTION_MATRIX ) );
  projectionMatrixConstraint.Apply();

  Property::Index lightCameraViewMatrixPropertyIndex = mShadowPlane.RegisterProperty( SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );
  Constraint viewMatrixConstraint = Constraint::New<Dali::Matrix>( mShadowPlane, lightCameraViewMatrixPropertyIndex, EqualToConstraint() );
  viewMatrixConstraint.AddSource( Source( mCameraActor, CameraActor::Property::VIEW_MATRIX ) );
  viewMatrixConstraint.Apply();

  mShadowColorPropertyIndex = mShadowPlane.RegisterProperty( SHADER_SHADOW_COLOR_PROPERTY_NAME, mCachedShadowColor );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
