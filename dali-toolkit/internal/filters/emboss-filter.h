#ifndef DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H
#define DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/renderer.h>

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
  void Enable() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Disable
  void Disable() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Refresh
  void Refresh() override;

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::SetSize
  void SetSize(const Vector2& size) override;

private:
  /**
   * Setup render tasks for blur
   */
  void CreateRenderTasks();

private:
  EmbossFilter(const EmbossFilter&);
  EmbossFilter& operator=(const EmbossFilter&);

private: // Attributes
  RenderTask  mRenderTaskForEmboss1;
  RenderTask  mRenderTaskForEmboss2;
  RenderTask  mRenderTaskForOutput;
  FrameBuffer mFrameBufferForEmboss1;
  FrameBuffer mFrameBufferForEmboss2;
  Actor       mActorForInput1;
  Actor       mActorForInput2;
  Renderer    mRendererForEmboss1;
  Renderer    mRendererForEmboss2;
  Actor       mActorForComposite;
}; // class EmbossFilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_EMBOSS_FILTER_H
