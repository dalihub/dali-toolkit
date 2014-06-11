//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/internal/controls/magnifier/magnifier-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

const char* DEFAULT_FRAME_IMAGE_PATH = DALI_IMAGE_DIR "magnifier-image-frame.png";

const float IMAGE_BORDER_INDENT = 14.0f;            ///< Indent of border in pixels.

/**
 * ImageBorderSizeConstraint
 */
struct ImageBorderSizeConstraint
{
  ImageBorderSizeConstraint()
  : mSizeOffset(Vector3(IMAGE_BORDER_INDENT - 1, IMAGE_BORDER_INDENT - 1, 0.0f) * 2.0f)
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& referenceSizeProperty)
  {
    const Vector3& referenceSize = referenceSizeProperty.GetVector3();

    return referenceSize + mSizeOffset;
  }

  Vector3 mSizeOffset;                        ///< The amount to offset the size from referenceSize
};

struct CameraActorPositionConstraint
{
  CameraActorPositionConstraint(const Vector2& stageSize, float defaultCameraDistance = 0.0f)
  : mStageSize(stageSize),
    mDefaultCameraDistance(defaultCameraDistance)
  {
  }

  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourcePositionProperty)
  {
    const Vector3& sourcePosition = sourcePositionProperty.GetVector3();

    return Vector3( sourcePosition.x + mStageSize.x * 0.5f,
                    sourcePosition.y + mStageSize.y * 0.5f,
                    sourcePosition.z + mDefaultCameraDistance);
  }

  Vector2 mStageSize;
  float mDefaultCameraDistance;

};

struct RenderTaskViewportPositionConstraint
{
  RenderTaskViewportPositionConstraint(const Vector2& stageSize)
  : mStageSize(stageSize)
  {
  }

  Vector2 operator()(const Vector2& current,
                     const PropertyInput& positionProperty,
                     const PropertyInput& magnifierSizeProperty,
                     const PropertyInput& magnifierScaleProperty)
  {
    Vector2 position(positionProperty.GetVector3()); // World position?

    //position -= mStageSize * 0.5f;

    // should be updated when:
    // Magnifier's world position/size/scale/parentorigin/anchorpoint changes.
    // or Magnifier camera's world position changes.
    Vector3 size = magnifierSizeProperty.GetVector3() * magnifierScaleProperty.GetVector3();

    // Reposition, and resize viewport to reflect the world bounds of this Magnifier actor.
    position.x += (mStageSize.width - size.width) * 0.5f;
    position.y += (mStageSize.height - size.height) * 0.5f;

    return position;
  }

  Vector2 mStageSize;
};

struct RenderTaskViewportSizeConstraint
{
  RenderTaskViewportSizeConstraint()
  {
  }

  Vector2 operator()(const Vector2& current,
                     const PropertyInput& magnifierSizeProperty,
                     const PropertyInput& magnifierScaleProperty)
  {
    return Vector2(magnifierSizeProperty.GetVector3() * magnifierScaleProperty.GetVector3());
  }
};

/**
 * Returns relative border (0.0f...1.0f x 0.0f...1.0f)
 * from an absolute pixel specified border.
 * @param[in] absolute A border using absolute pixel coordinates
 * @param[in] width The width of the texture
 * @param[in] height The height of the texture.
 * @return A border relative to the size of the Image texture dimensions.
 */
Vector4 GetRelativeBorder(Vector4 absolute, float width, float height)
{
  return Vector4( absolute.x / width,
                  absolute.y / height,
                  absolute.z / width,
                  absolute.w / height);
}

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Magnifier
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Toolkit::Magnifier Magnifier::New()
{
  // Create the implementation
  MagnifierPtr magnifier(new Magnifier());

  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::Magnifier handle(*magnifier);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  magnifier->Initialize();

  return handle;
}

Magnifier::Magnifier()
: ControlImpl(true),
  mPropertySourcePosition(Property::INVALID_INDEX),
  mActorSize(Vector3::ZERO),
  mMagnificationFactor(1.0f)
{
}

void Magnifier::SetSourceActor(Actor actor)
{
  mTask.SetSourceActor( actor );
}

void Magnifier::SetSourcePosition(const Vector3& position)
{
  Self().SetProperty(mPropertySourcePosition, position);
}

void Magnifier::Initialize()
{
  Actor self = Self();
  mPropertySourcePosition = self.RegisterProperty( Toolkit::Magnifier::SOURCE_POSITION_PROPERTY_NAME, Vector3::ZERO );
  Vector2 stageSize(Stage::GetCurrent().GetSize());

  Layer dummyLayer = Layer::New();
  Stage().GetCurrent().Add(dummyLayer);
  dummyLayer.SetParentOrigin(ParentOrigin::CENTER);

  // NOTE:
  // sourceActor is a dummy delegate actor that takes the source property position,
  // and generates a WORLD_POSITION, which is 1 frame behind the source property.
  // This way the constraints for determining the camera position (source) and those
  // for determining viewport position use the same 1 frame old values.
  // A simple i) CameraPos = f(B), ii) B = f(A) set of constraints wont suffice as
  // although CameraPos will use B, which is A's previous value. The constraint will
  // not realise that B is still dirty as far as constraint (i) is concerned.
  // Perhaps this is a bug in the way the constraint system factors into what is dirty
  // and what is not.
  mSourceActor = Actor::New();
  dummyLayer.Add(mSourceActor);
  mSourceActor.SetParentOrigin(ParentOrigin::CENTER);
  Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( self, mPropertySourcePosition ),
                                                    EqualToConstraint() );
  mSourceActor.ApplyConstraint(constraint);

  // create the render task this will render content on top of everything
  // based on camera source position.
  InitializeRenderTask();

  // set up some constraints to:
  // i) reposition (dest) frame actor based on magnifier actor's world position (this is 1 frame delayed)
  // ii) reposition and resize (dest) the render task's viewport based on magnifier actor's world position (1 frame delayed) & size.
  // iii) reposition (source) camera actor based on magnifier source actor's world position (this is 1 frame delayed)

  // Apply constraint to camera's position
  // Position our camera at the same distance from its target as the default camera is.
  // The camera position doesn't affect how we render, just what we render (due to near and far clip planes)
  // NOTE: We can't interrogate the default camera's position as it is not known initially (takes 1 frame
  // for value to update).
  // But we can determine the initial position using the same formula:
  // distance = stage.height * 0.5 / tan(FOV * 0.5)

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask renderTask = taskList.GetTask(0u);
  float fov = renderTask.GetCameraActor().GetFieldOfView();
  mDefaultCameraDistance = (stageSize.height * 0.5f) / tanf(fov * 0.5f);

  // Use a 1 frame delayed source position to determine the camera actor's position.
  // This is necessary as the viewport is determined by the Magnifier's Actor's World position (which is computed
  // at the end of the update cycle i.e. after constraints have been applied.)
  //Property::Index propertySourcePositionDelayed = mCameraActor.RegisterProperty("delayed-source-position", Vector3::ZERO);

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                    Source( mSourceActor, Actor::WORLD_POSITION ),
                                                    CameraActorPositionConstraint(stageSize, mDefaultCameraDistance) );
  mCameraActor.ApplyConstraint(constraint);

  // Apply constraint to render-task viewport position
  constraint = Constraint::New<Vector2>( RenderTask::VIEWPORT_POSITION,
                                         Source( self, Actor::WORLD_POSITION ),//mPropertySourcePosition ),
                                         Source( self, Actor::SIZE ),
                                         Source( self, Actor::WORLD_SCALE),
                                         RenderTaskViewportPositionConstraint(stageSize) );
  mTask.ApplyConstraint(constraint);

  // Apply constraint to render-task viewport position
  constraint = Constraint::New<Vector2>( RenderTask::VIEWPORT_SIZE,
                                         Source( self, Actor::SIZE ),
                                         Source( self, Actor::WORLD_SCALE),
                                         RenderTaskViewportSizeConstraint() );
  mTask.ApplyConstraint(constraint);
}

Magnifier::~Magnifier()
{

}

void Magnifier::InitializeRenderTask()
{
  Stage stage = Stage::GetCurrent();

  RenderTaskList taskList = stage.GetRenderTaskList();

  mTask = taskList.CreateTask();
  mTask.SetInputEnabled(false);
  mTask.SetClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
  mTask.SetClearEnabled(true);

  mCameraActor = CameraActor::New();
  mCameraActor.SetType(Camera::FREE_LOOK);
  mCameraActor.SetRotation(Quaternion(M_PI, Vector3::YAXIS)); // Look at stage

  stage.Add(mCameraActor);
  mTask.SetCameraActor( mCameraActor );

  SetFrameVisibility(true);
}

bool Magnifier::GetFrameVisibility() const
{
  return mFrameLayer;
}

void Magnifier::SetFrameVisibility(bool visible)
{
  if(visible && !mFrameLayer)
  {
    Actor self(Self());

    Layer mFrameLayer = Layer::New();
    mFrameLayer.SetParentOrigin( ParentOrigin::CENTER );
    Stage::GetCurrent().Add(mFrameLayer);

    Image image = Image::New( DEFAULT_FRAME_IMAGE_PATH );
    ImageActor frame = ImageActor::New( image );
    frame.SetDrawMode(DrawMode::OVERLAY);
    frame.SetStyle( ImageActor::STYLE_NINE_PATCH );

    frame.SetNinePatchBorder( Vector4::ONE * IMAGE_BORDER_INDENT );
    mFrameLayer.Add(frame);

    // Apply position constraint to the frame
    Constraint constraint = Constraint::New<Vector3>( Actor::POSITION,
                                                      Source( self, Actor::WORLD_POSITION ),
                                                      EqualToConstraint() );
    frame.ApplyConstraint(constraint);

    // Apply scale constraint to the frame
    constraint = Constraint::New<Vector3>( Actor::SCALE,
                                           Source( self, Actor::SCALE ),
                                           EqualToConstraint() );
    frame.ApplyConstraint(constraint);

    Source(self, Actor::SCALE),

    // Apply size constraint to the the frame
    constraint = Constraint::New<Vector3>(Actor::SIZE,
                                          Source(self, Actor::SIZE),
                                          ImageBorderSizeConstraint());
    frame.ApplyConstraint(constraint);
  }
  else if(!visible && mFrameLayer)
  {
    Stage::GetCurrent().Remove(mFrameLayer);
    mFrameLayer.Reset();
  }
}

void Magnifier::OnControlSizeSet(const Vector3& targetSize)
{
  // TODO: Once Camera/CameraActor properties function as proper animatable properties
  // this code can disappear.
  // whenever the size of the magnifier changes, the field of view needs to change
  // to compensate for the new size of the viewport. this cannot be done within
  // a constraint yet as Camera/CameraActor properties are not animatable/constrainable.
  mActorSize = targetSize;
  Update();
}

float Magnifier::GetMagnificationFactor() const
{
  return mMagnificationFactor;
}

void Magnifier::SetMagnificationFactor(float value)
{
  mMagnificationFactor = value;
  Update();
}

void Magnifier::Update()
{
  // TODO: Make Camera settings (fieldofview/aspectratio) as animatable constraints.

  // should be updated when:
  // Magnifier's world size/scale changes.
  Actor self(Self());
  Vector3 worldSize = mActorSize * self.GetCurrentWorldScale();

  // Adjust field of view to scale content

  // size.height / 2
  // |------/
  // |d    /
  // |i   /
  // |s  /
  // |t /
  // |./
  // |/ <--- fov/2 radians.
  //
  const float fov = atanf( 0.5f * worldSize.height / mDefaultCameraDistance / mMagnificationFactor) * 2.0f;
  mCameraActor.SetFieldOfView( fov );

  // Adjust aspect ratio to compensate for rectangular viewports.
  mCameraActor.SetAspectRatio( worldSize.width / worldSize.height );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
