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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/magnifier/magnifier.h>

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
   * Returns whether the frame is visible or not.
   * @return true if frame is visible, false if not.
   */
  bool GetFrameVisibility() const;

  /**
   * Sets whether the frame part of the magnifier should be visible or not.
   * @param[in] visible true to display frame, false to hide frame.
   */
  void SetFrameVisibility(bool visible);

  /**
   * Get the magnification factor of the magnifier
   * The larger the value the larger the contents magnified.
   * A value of 1.0f indications 1x magnification.
   * @return Magnification factor is returned
   */
  float GetMagnificationFactor() const;

  /**
   * Set the magnification factor of the magnifier
   * The larger the value the larger the contents magnified.
   * A value of 1.0f indications 1x magnification.
   * @param[in] value Magnification factor.
   */
  void SetMagnificationFactor(float value);

  /**
   * Update magnification
   */
  void Update();

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

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

  virtual void OnSizeSet(const Vector3& targetSize);

private:

  // Undefined
  Magnifier(const Magnifier&);

  // Undefined
  Magnifier& operator=(const Magnifier& rhs);

private:

  RenderTask mTask;                             ///< Render Task to render the source actor contents.
  CameraActor mCameraActor;                     ///< CameraActor attached to RenderTask
  Actor mFrame;                                 ///< The Magnifier Frame
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
