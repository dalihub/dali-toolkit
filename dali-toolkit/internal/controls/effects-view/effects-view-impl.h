#ifndef __DALI_TOOLKIT_INTERNAL_EFFECTS_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_EFFECTS_VIEW_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/effects-view/effects-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class GaussianBlurView;
class ImageFilter;

/**
 * EffectsView implementation class
 * @copydoc Dali::Toolkit::EffectsView
 */
class EffectsView : public Control
{
public:
  /// @copydoc Dali::Toolkit::EffectsView New()
  static Toolkit::EffectsView New();

  /**
   * Construct a new EffectsView.
   * @copydoc Toolkit::EffectsView New()
   */
  EffectsView();

  /**
   * Constructor.
   * @copydoc Toolkit::EffectsView New(const unsigned int,const float,const Pixel::Format,const float,const float)
   */
  EffectsView(const unsigned int numSamples, const float blurBellCurveWidth, const int spread,
              const Pixel::Format pixelFormat,
              const float downsampleWidthScale, const float downsampleHeightScale,
              FrameBufferImage image);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~EffectsView();

public:

  /// @copydoc Dali::Toolkit::EffectsView::SetType
  void SetType( Toolkit::EffectsView::EffectType type );

  /// @copydoc Dali::Toolkit::EffectsView::GetType
  Toolkit::EffectsView::EffectType GetType() const;

  /// @copydoc Dali::Toolkit::EffectsView::Refresh
  void Refresh();

  /// @copydoc Dali::Toolkit::EffectsView::SetRefreshOnDemand
  void SetRefreshOnDemand( bool onDemand );

  /// @copydoc Dali::Toolkit::EffectsView::SetPixelFormat
  void SetPixelFormat( Pixel::Format pixelFormat );

  /// @copydoc Dali::Toolkit::EffectsView::SetBackgroundColor(const Vector4&)
  void SetBackgroundColor( const Vector4& color );

  /// @copydoc Dali::Toolkit::GaussianBlurView::GetBackgroundColor
  Vector4 GetBackgroundColor() const;

  /**
   * Set the effect size which decides the size of filter kernel.
   * @param[in] effectSize The effect size.
   */
  void SetEffectSize( int effectSize );

  /**
   * Get the effect size.
   * @return The effect size.
   */
  int GetEffectSize();

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
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

private: // From Control

  /**
   * @copydoc Toolkit::Internal::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& targetSize )
   */
  virtual void OnSizeSet( const Vector3& targetSize );

  /**
   * @copydoc Toolkit::Internal::Control::OnStageConnection
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Internal::Control::OnStageDisconnection
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc Toolkit::Internal::Control::OnChildAdd
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Toolkit::Internal::Control::OnChildRemove
   */
  virtual void OnChildRemove( Actor& child );

private:

  /**
   * Enable the effect when the control is set on stage
   */
  void Enable();

  /**
   * Disable the effect when the control is set off stage
   */
  void Disable();

  /**
   * Setup image filters
   */
  void SetupFilters();

  /**
   * Allocate resources
   */
  void AllocateResources();

  /**
   * Setup cameras
   */
  void SetupCameras();

  /**
   * Create render tasks for internal jobs
   */
  void CreateRenderTasks();

  /**
   * Remove render tasks
   */
  void RemoveRenderTasks();

  /**
   * Refresh render tasks
   */
  void RefreshRenderTasks();

  /**
   * Remove ImageFilters
   */
  void RemoveFilters();

private:

  // Undefined
  EffectsView( const EffectsView& );

  // Undefined
  EffectsView& operator = ( const EffectsView& );

private: // attributes/properties

  /////////////////////////////////////////////////////////////
  // for rendering all user added children to offscreen target
  FrameBufferImage mImageForChildren;
  Toolkit::Visual mRendererForChildren;
  RenderTask mRenderTaskForChildren;
  CameraActor mCameraForChildren;
  Actor mChildrenRoot; // for creating a subtree for all user added child actors

  /////////////////////////////////////////////////////////////
  // background fill color
  Vector4 mBackgroundColor;

  /////////////////////////////////////////////////////////////
  // for checking if we need to reallocate render targets
  Vector2 mTargetSize;
  Vector2 mLastSize;
  /////////////////////////////////////////////////////////////
  // post blur image
  FrameBufferImage mImagePostFilter;
  Toolkit::Visual mRendererPostFilter;

  Vector<ImageFilter*> mFilters;

  /////////////////////////////////////////////////////////////
  // downsampling is used for the separated blur passes to get increased blur with the same number of samples and also to make rendering quicker
  int mEffectSize;

  /////////////////////////////////////////////////////////////
  Toolkit::EffectsView::EffectType mEffectType;
  Pixel::Format mPixelFormat;     ///< pixel format used by render targets

  bool mEnabled:1;
  bool mRefreshOnDemand:1;
}; // class EffectsView

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::EffectsView& GetImpl( Toolkit::EffectsView& effectsView )
{
  DALI_ASSERT_ALWAYS( effectsView );

  Dali::RefObject& handle = effectsView.GetImplementation();

  return static_cast<Toolkit::Internal::EffectsView&>( handle );
}

inline const Toolkit::Internal::EffectsView& GetImpl( const Toolkit::EffectsView& effectsView )
{
  DALI_ASSERT_ALWAYS( effectsView );

  const Dali::RefObject& handle = effectsView.GetImplementation();

  return static_cast<const Toolkit::Internal::EffectsView&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_EFFECTS_VIEW_H__

