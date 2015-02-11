#ifndef __DALI_TOOLKIT_INTERNAL_MAGNIFIER_H__
#define __DALI_TOOLKIT_INTERNAL_MAGNIFIER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/actors/image-actor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/magnifier/magnifier.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Magnifier;

typedef IntrusivePtr<Magnifier>    MagnifierPtr;

/**
 * @copydoc Toolkit::Magnifier
 */
class Magnifier : public Control
{
public:

  /**
   * Create a new Magnifier.
   * @return A public handle to the newly allocated Magnifier.
   */
  static Dali::Toolkit::Magnifier New();

public:

  /**
   * @copydoc Toolkit::ImageView::SetSourceActor
   */
  void SetSourceActor(Actor actor);

  /**
   * @copydoc Toolkit::ImageView::SetSourcePosition
   */
  void SetSourcePosition(const Vector3& position);

  /**
   * @copydoc Toolkit::ImageView::GetFrameVisibility
   */
  bool GetFrameVisibility() const;

  /**
   * @copydoc Toolkit::ImageView::SetFrameVisibility
   */
  void SetFrameVisibility(bool visible);

  /**
   * @copydoc Toolkit::ImageView::GetMagnificationFactor
   */
  float GetMagnificationFactor() const;

  /**
   * @copydoc Toolkit::ImageView::SetMagnificationFactor
   */
  void SetMagnificationFactor(float value);

  /**
   * Update magnification
   */
  void Update();

protected:

  /**
   * Construct a new Magnifier.
   */
  Magnifier();

  /**
   * 2nd-phase initialization.
   */
  void Initialize();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Magnifier();

private:

  /**
   * Initializes the render task required to render contents.
   */
  void InitializeRenderTask();

private:

  virtual void OnControlSizeSet(const Vector3& targetSize);

private:

  // Undefined
  Magnifier(const Magnifier&);

  // Undefined
  Magnifier& operator=(const Magnifier& rhs);

private:

  RenderTask mTask;                             ///< Render Task to render the source actor contents.
  CameraActor mCameraActor;                     ///< CameraActor attached to RenderTask
  ImageActor mFrame;                            ///< The Magnifier Frame
  Property::Index mPropertySourcePosition;      ///< Source Position ("source-position")
  Actor mSourceActor;                           ///< Source Delegate Actor represents the source position to read.
  float mDefaultCameraDistance;                 ///< Default RenderTask's camera distance from target.
  Vector3 mActorSize;                           ///< The Actor size
  float mMagnificationFactor;                   ///< Magnification factor 1.0f is default. same as content.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Magnifier& GetImpl(Toolkit::Magnifier& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<Toolkit::Internal::Magnifier&>(handle);
}

inline const Toolkit::Internal::Magnifier& GetImpl(const Toolkit::Magnifier& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<const Toolkit::Internal::Magnifier&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_MAGNIFIER_H__
