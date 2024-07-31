#ifndef DALI_TOOLKIT_SVG_UPLOAD_OBSERVER_H
#define DALI_TOOLKIT_SVG_UPLOAD_OBSERVER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/public-api/rendering/texture-set.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief Base class used to observe the load / rasterize status of a svg.
 */
class SvgLoaderObserver
{
public:
  typedef Signal<void(SvgLoaderObserver*)> DestructionSignalType; ///< Signal prototype for the Destruction Signal.

public:
  /**
   * @brief Constructor.
   */
  SvgLoaderObserver() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~SvgLoaderObserver();

  /**
   * @brief Returns the destruction signal for load.
   * This is emitted when the observer is destroyed.
   * This is used by the observer notifier to mark this observer as destroyed (IE. It no longer needs notifying).
   */
  DestructionSignalType& LoadDestructionSignal()
  {
    return mLoadDestructionSignal;
  }

  /**
   * @brief Returns the destruction signal for rasterize.
   * This is emitted when the observer is destroyed.
   * This is used by the observer notifier to mark this observer as destroyed (IE. It no longer needs notifying).
   */
  DestructionSignalType& RasterizeDestructionSignal()
  {
    return mRasterizeDestructionSignal;
  }

public:
  /**
   * The action to be taken once the async load has finished.
   * This should be overridden by the deriving class.
   *
   * @param[in] loadId Id of load request.
   * @param[in] vectorImageRenderer Renderer class for svg image. It could be empty handle if rasterize failed.
   */
  virtual void LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer) = 0;

  /**
   * The action to be taken once the async rasterize has finished.
   * This should be overridden by the deriving class.
   *
   * @param[in] rasterizeId Id of rasterize request.
   * @param[in] textureSet Rasterize texture set. It could be empty handle if rasterize failed.
   * @param[in] atlasRect The atlas rect of the rasterized image.
   */
  virtual void RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet, Vector4 atlasRect) = 0;

private:
  DestructionSignalType mLoadDestructionSignal;      ///< The destruction signal emitted when the observer is destroyed.
  DestructionSignalType mRasterizeDestructionSignal; ///< The destruction signal emitted when the observer is destroyed.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SVG_UPLOAD_OBSERVER_H
