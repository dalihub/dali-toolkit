#ifndef __DALI_TOOLKIT_INTERNAL_SHADOW_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_SHADOW_VIEW_H__

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

// EXTERNAL INCLUDES
#include <sstream>
#include <cmath>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/shadow-view/shadow-view.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/filters/blur-two-pass-filter.h>

namespace Dali
{

namespace Toolkit
{

class ShadowView;

namespace Internal
{

/**
 * ShadowView implementation class
 */
class ShadowView : public Control
{
public:

  /**
   * @copydoc Dali::Toolkit::ShadowView::ShadowView
   */
  ShadowView();

  /**
   * @copydoc Dali::Toolkit::ShadowView::ShadowView
   */
  ShadowView(float downsampleWidthScale, float downsampleHeightScale);

  /**
   * @copydoc Dali::Toolkit::ShadowView::~ShadowView
   */
  virtual ~ShadowView();

  /**
   * @copydoc Dali::Toolkit::ShadowView::New(float downsampleWidthScale, float downsampleHeightScale)
   */
  static Dali::Toolkit::ShadowView New(float downsampleWidthScale, float downsampleHeightScale);

  /**
   * @copydoc Dali::Toolkit::ShadowView::SetShadowPlaneBackground(Actor shadowPlaneBackground)
   */
  void SetShadowPlaneBackground(Actor shadowPlaneBackground);

  /**
   * @copydoc Dali::Toolkit::ShadowView::SetPointLight(Actor pointLight)
   */
  void SetPointLight(Actor pointLight);

  /**
   * @copydoc Dali::Toolkit::ShadowView::SetPointLightFieldOfView(float fieldOfView)
   */
  void SetPointLightFieldOfView(float fieldOfView);

  /**
   * @copydoc Dali::Toolkit::ShadowView::SetShadowColor(Vector4 color)
   */
  void SetShadowColor(Vector4 color);

  /**
   * @copydoc Dali::Toolkit::ShadowView::Activate()
   */
  void Activate();

  /**
   * @copydoc Dali::Toolkit::ShadowView::Deactivate()
   */
  void Deactivate();

  /**
   * @copydoc Dali::Toolkit::ShadowView::GetBlurStrengthPropertyIndex()
   */
  Property::Index GetBlurStrengthPropertyIndex() const {return mBlurStrengthPropertyIndex;}

  /**
   * @copydoc Dali::Toolkit::ShadowView::GetShadowColorPropertyIndex()
   */
  Property::Index GetShadowColorPropertyIndex() const {return mShadowColorPropertyIndex;}

  void SetShaderConstants();

private:

  virtual void OnInitialize();

  /**
   * @copydoc Control::OnChildAdd()
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Control::OnChildRemove()
   */
  virtual void OnChildRemove( Actor& child );

  /**
   * Constrain the camera actor to the position of the point light, pointing
   * at the center of the shadow plane.
   */
  void ConstrainCamera();

  void CreateRenderTasks();
  void RemoveRenderTasks();
  void CreateBlurFilter();

private:
  Toolkit::ImageView mShadowPlane; // Shadow renders into this actor
  Actor mShadowPlaneBg; // mShadowPlane renders directly in front of this actor
  Actor mPointLight;  // Shadow is cast from this point light

  /////////////////////////////////////////////////////////////
  FrameBufferImage mSceneFromLightRenderTarget;  // for rendering normal scene seen from light to texture instead of the screen

  FrameBufferImage mOutputImage;

  Actor mChildrenRoot; // Subtree for all user added child actors that should be rendered normally
  Actor mBlurRootActor; // Root actor for blur filter processing
  RenderTask mRenderSceneTask;

  CameraActor mCameraActor; // Constrained to same position as mPointLight and pointing at mShadowPlane

  Property::Map mShadowVisualMap;
  BlurTwoPassFilter mBlurFilter;

  Vector4 mCachedShadowColor;                               ///< Cached Shadow color.
  Vector4 mCachedBackgroundColor;                           ///< Cached Shadow background color (same as shadow color but with alpha at 0.0)

  /////////////////////////////////////////////////////////////
  // Properties that can be animated
  Property::Index mBlurStrengthPropertyIndex;
  Property::Index mShadowColorPropertyIndex;
  float mDownsampleWidthScale;
  float mDownsampleHeightScale;

private:

  // Undefined copy constructor.
  ShadowView( const ShadowView& );

  // Undefined assignment operator.
  ShadowView& operator=( const ShadowView& );
};

} // namespace Internal


// Helpers for public-api forwarding methods
inline Toolkit::Internal::ShadowView& GetImpl( Toolkit::ShadowView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::ShadowView&>(handle);
}

inline const Toolkit::Internal::ShadowView& GetImpl( const Toolkit::ShadowView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::ShadowView&>(handle);
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SHADOW_VIEW_H__
