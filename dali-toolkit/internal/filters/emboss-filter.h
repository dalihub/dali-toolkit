#ifndef __DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H__
#define __DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include "image-filter.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * An embossing image filter, implements Dali::Toolkit::Internal::ImageFilter
 */
class EmbossFilter : public ImageFilter
{
public:
  /**
   * Construct an empty filter
   */
  EmbossFilter();

  /**
   * Destructor
   */
  virtual ~EmbossFilter();

public: // From ImageFilter
  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Enable
  virtual void Enable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Disable
  virtual void Disable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Refresh
  virtual void Refresh();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::SetSize
  virtual void SetSize( const Vector2& size );

private:
  /**
   * Setup position and parameters for camera
   */
  void SetupCamera();

  /**
   * Setup render tasks for blur
   */
  void CreateRenderTasks();

private:
  EmbossFilter( const EmbossFilter& );
  EmbossFilter& operator=( const EmbossFilter& );

private: // Attributes

  RenderTask       mRenderTaskForEmboss1;
  RenderTask       mRenderTaskForEmboss2;
  RenderTask       mRenderTaskForOutput;
  FrameBufferImage mImageForEmboss1;
  FrameBufferImage mImageForEmboss2;
  CameraActor      mCameraActor;
  ImageActor       mActorForInput1;
  ImageActor       mActorForInput2;
  ImageActor       mActorForEmboss1;
  ImageActor       mActorForEmboss2;
  Actor            mActorForComposite;
}; // class EmbossFilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H__

