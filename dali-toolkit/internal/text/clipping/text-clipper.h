#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_CLIPPER_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_CLIPPER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class Clipper;
typedef IntrusivePtr<Clipper> ClipperPtr;

/**
 * @brief A helper class for clipping actors using a FrameBufferImage.
 */
class Clipper : public RefObject, public ConnectionTracker
{
public:

  /**
   * @brief Create a clipper.
   *
   * @param[in] size The size of the clipping region.
   */
  static ClipperPtr New( const Vector2& size );

  /**
   * @brief Children added to this actor will be clipped with the specified region.
   *
   * @note This is done by rendering to a FrameBufferImage which must then be displayed; see also GetImageView().
   * @return The root actor.
   */
  Actor GetRootActor() const;

  /**
   * @brief This actor will display the resulting FrameBufferImage.
   *
   * @return The image view.
   */
  Actor GetImageView() const;

  /**
   * @brief Refresh the contents of the FrameBufferImage.
   *
   * @param[in] size The size of the clipping region.
   */
  void Refresh( const Vector2& size );

private: // Implementation

  /**
   * @brief Second-phase init
   *
   * @param[in] size The size of the clipping region.
   */
  void Initialize( const Vector2& size );

  /**
   * The renderer is not created until the clipper actor is set on stage, only by then the blend function could be set.
   *
   * @param[in] actor The actor connected to stage.
   */
  void OnStageConnect( Dali::Actor actor );

  /**
   * Construct a new Clipper.
   */
  Clipper();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Clipper();

private:

  // Undefined copy constructor and assignment operators
  Clipper(const Clipper&);
  Clipper& operator=(const Clipper& rhs);

private: // Data

  Layer mOffscreenRootActor;
  CameraActor mOffscreenCameraActor;
  ImageView mImageView;
  RenderTask mRenderTask;
  Vector2 mCurrentOffscreenSize;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_CLIPPER_H__
