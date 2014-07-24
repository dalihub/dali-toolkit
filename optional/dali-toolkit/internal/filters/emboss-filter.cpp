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

// CLASS HEADER
#include "emboss-filter.h"

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* EMBOSS_FRAGMENT_SOURCE1 =
{
 "uniform vec2 uTexScale;\n"
 "\n"
 "void main()\n"
 "{\n"
 "  vec4 color;\n"
 "# ifdef DEBUG_RENDER\n"
 "  if( vTexCoord.s < 0.495 )\n"
 "  {\n"
 "# endif //def DEBUG_RENDER\n"
 "    color  =  2.0 * texture2D( sTexture, vTexCoord + vec2(0.0, -uTexScale.y) );\n"
 "    color += -1.0 * texture2D( sTexture, vTexCoord );\n"
 "    color += -1.0 * texture2D( sTexture, vTexCoord + vec2(0.0, uTexScale.y) );\n"
 "# ifdef DEBUG_RENDER\n"
 "  }\n"
 "  else if( vTexCoord.s > 0.505 )\n"
 "  {\n"
 "    color = texture2D( sTexture, vTexCoord );\n"
 "  }\n"
 "  else\n"
 "  {\n"
 "    color = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
 "  }\n"
 "# endif //def DEBUG_RENDER\n"
 "  gl_FragColor = uColor * color;\n"
 "}\n"
};

const char* EMBOSS_FRAGMENT_SOURCE2 =
{
 "uniform vec2 uTexScale;\n"
 "\n"
 "void main()\n"
 "{\n"
 "  vec4 color;\n"
 "# ifdef DEBUG_RENDER\n"
 "  if( vTexCoord.s < 0.495 )\n"
 "  {\n"
 "# endif //def DEBUG_RENDER\n"
 "    color  = -1.0 * texture2D( sTexture, vTexCoord + vec2(0.0, -uTexScale.y) );\n"
 "    color += -1.0 * texture2D( sTexture, vTexCoord );\n"
 "    color +=  2.0 * texture2D( sTexture, vTexCoord + vec2(0.0, uTexScale.y) );\n"
 "# ifdef DEBUG_RENDER\n"
 "  }\n"
 "  else if( vTexCoord.s > 0.505 )\n"
 "  {\n"
 "    color = texture2D( sTexture, vTexCoord );\n"
 "  }\n"
 "  else\n"
 "  {\n"
 "    color = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
 "  }\n"
 "# endif //def DEBUG_RENDER\n"
 "  gl_FragColor = uColor * color;\n"
 "}\n"
};

const char* const COMPOSITE_FRAGMENT_SOURCE =
    "void main()\n"
    "{\n"
    "  gl_FragColor = uColor;\n"
    "  gl_FragColor.a *= texture2D( sTexture, vTexCoord).a;\n"
    "}\n";

} // namespace

EmbossFilter::EmbossFilter()
: ImageFilter()
{
}

EmbossFilter::~EmbossFilter()
{
}

void EmbossFilter::Enable()
{
  // Create camera
  mCameraActor = CameraActor::New();
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);

  mImageForEmboss1 = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Image::Unused );
  mImageForEmboss2 = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Image::Unused );

  // create actor to render input with applied emboss effect
  mActorForInput1 = ImageActor::New( mInputImage );
  mActorForInput1.SetParentOrigin( ParentOrigin::CENTER );
  mActorForInput1.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mActorForInput1.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );
  mActorForInput1.SetColor( Color::WHITE );

  mActorForInput2 = ImageActor::New( mInputImage );
  mActorForInput2.SetParentOrigin( ParentOrigin::CENTER );
  mActorForInput2.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mActorForInput2.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );
  mActorForInput2.SetColor( Color::WHITE );

  mActorForEmboss1 = ImageActor::New( mImageForEmboss1 );
  mActorForEmboss1.SetParentOrigin( ParentOrigin::CENTER );
  mActorForEmboss1.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mActorForEmboss1.SetColor( Color::BLACK );
  mActorForEmboss1.SetShaderEffect( ShaderEffect::New( "", COMPOSITE_FRAGMENT_SOURCE ) );

  mActorForEmboss2 = ImageActor::New( mImageForEmboss2 );
  mActorForEmboss2.SetParentOrigin( ParentOrigin::CENTER );
  mActorForEmboss2.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mActorForEmboss2.SetColor( Color::WHITE );
  mActorForEmboss2.SetShaderEffect( ShaderEffect::New( "", COMPOSITE_FRAGMENT_SOURCE ) );

  mActorForComposite = Actor::New();
  mActorForComposite.SetParentOrigin( ParentOrigin::CENTER );
  mActorForComposite.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
  mActorForComposite.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

  // create custom shader effect
  std::ostringstream embossFragmentSource1;
  if( mDebugRender )
  {
    embossFragmentSource1 << "#define DEBUG_RENDER\n";
  }
  embossFragmentSource1 << EMBOSS_FRAGMENT_SOURCE1;
  ShaderEffect effect1 = ShaderEffect::New( "", embossFragmentSource1.str() );
  mActorForInput1.SetShaderEffect( effect1 );
  effect1.SetUniform( "uTexScale", Vector2( 1.0f/mTargetSize.width, 1.0f/mTargetSize.height) * 1.5f );

  std::ostringstream embossFragmentSource2;
  if( mDebugRender )
  {
    embossFragmentSource2 << "#define DEBUG_RENDER\n";
  }
  embossFragmentSource2 << EMBOSS_FRAGMENT_SOURCE2;
  ShaderEffect effect2 = ShaderEffect::New( "", embossFragmentSource2.str() );
  mActorForInput2.SetShaderEffect( effect2 );
  effect2.SetUniform( "uTexScale", Vector2( 1.0f/mTargetSize.width, 1.0f/mTargetSize.height) * 1.5f );

  SetupCamera();
  CreateRenderTasks();

  mRootActor.Add( mActorForInput1 );
  mRootActor.Add( mActorForInput2 );
  mRootActor.Add( mActorForComposite );
  mActorForComposite.Add( mActorForEmboss1 );
  mActorForComposite.Add( mActorForEmboss2 );
  mRootActor.Add( mCameraActor );
}

void EmbossFilter::Disable()
{
  if( mRootActor )
  {
    if( mCameraActor )
    {
      mRootActor.Remove( mCameraActor );
      mCameraActor.Reset();
    }

    if( mActorForInput1 )
    {
      mRootActor.Remove( mActorForInput1 );
      mActorForInput1.Reset();
    }

    if( mActorForInput2 )
    {
      mRootActor.Remove( mActorForInput2 );
      mActorForInput2.Reset();
    }

    if( mActorForComposite )
    {
      mRootActor.Remove( mActorForComposite );
      mActorForComposite.Reset();
      mActorForEmboss1.Reset();
      mActorForEmboss2.Reset();
    }

    RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
    if( mRenderTaskForEmboss1 )
    {
      taskList.RemoveTask(mRenderTaskForEmboss1);
    }

    if( mRenderTaskForEmboss2 )
    {
      taskList.RemoveTask(mRenderTaskForEmboss2);
    }

    mRootActor.Reset();
  }
}

void EmbossFilter::Refresh()
{
  if( mRenderTaskForEmboss1 )
  {
    mRenderTaskForEmboss1.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }
  if( mRenderTaskForEmboss2 )
  {
    mRenderTaskForEmboss2.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }
}

void EmbossFilter::SetupCamera()
{
  // Create and place a camera for the embossing render, corresponding to its render target size
  mCameraActor.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
  mCameraActor.SetNearClippingPlane(1.0f);
  mCameraActor.SetAspectRatio(mTargetSize.width / mTargetSize.height);
  mCameraActor.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  mCameraActor.SetRotation(Quaternion(M_PI, Vector3::YAXIS));
  mCameraActor.SetPosition(0.0f, 0.0f, ((mTargetSize.height * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));
}

void EmbossFilter::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  mRenderTaskForEmboss1 = taskList.CreateTask();
  mRenderTaskForEmboss1.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForEmboss1.SetSourceActor( mActorForInput1 );
  mRenderTaskForEmboss1.SetExclusive(true);
  mRenderTaskForEmboss1.SetInputEnabled( false );
  mRenderTaskForEmboss1.SetClearColor( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
  mRenderTaskForEmboss1.SetClearEnabled( true );
  mRenderTaskForEmboss1.SetTargetFrameBuffer( mImageForEmboss1 );
  mRenderTaskForEmboss1.SetCameraActor( mCameraActor );

  mRenderTaskForEmboss2 = taskList.CreateTask();
  mRenderTaskForEmboss2.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForEmboss2.SetSourceActor( mActorForInput2 );
  mRenderTaskForEmboss2.SetExclusive(true);
  mRenderTaskForEmboss2.SetInputEnabled( false );
  mRenderTaskForEmboss2.SetClearColor( Vector4( 1.0f, 1.0f, 1.0f, 0.0f ) );
  mRenderTaskForEmboss2.SetClearEnabled( true );
  mRenderTaskForEmboss2.SetTargetFrameBuffer( mImageForEmboss2 );
  mRenderTaskForEmboss2.SetCameraActor( mCameraActor );

  mRenderTaskForOutput = taskList.CreateTask();
  mRenderTaskForOutput.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForOutput.SetSourceActor( mActorForComposite );
  mRenderTaskForOutput.SetExclusive(true);
  mRenderTaskForOutput.SetInputEnabled( false );
  mRenderTaskForOutput.SetClearColor( Vector4( 0.5f, 0.5f, 0.5f, 0.0f ) );
  mRenderTaskForOutput.SetClearEnabled( true );
  mRenderTaskForOutput.SetTargetFrameBuffer( mOutputImage );
  mRenderTaskForOutput.SetCameraActor( mCameraActor );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
