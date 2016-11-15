#ifndef DALI_TOOLKIT_ATLAS_UPLOAD_OBSERVER_H
#define DALI_TOOLKIT_ATLAS_UPLOAD_OBSERVER_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/signals/callback.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class ImageAtlas;
}

/**
 * @brief Base class used to observe the upload status of the ImageAtlas when requesting an image atlasing.
 *
 * Derived class should implement the UploadCompleted method which would get executed once the texture is ready.
 */
class DALI_IMPORT_API AtlasUploadObserver
{
public:

  /**
   * @brief Constructor.
   */
  AtlasUploadObserver();

  /**
   * @brief Virtual destructor.
   */
  virtual ~AtlasUploadObserver();

  /**
   * The action to be taken once the upload is completed.
   */
  virtual void UploadCompleted() = 0;

public: // not intended for developer, called by ImageAtlas internally to get notified when this observer dies

  /**
   * @brief Register an ImageAtlas which be notified when the observer is destructing.
   * @param[in] imageAtlas The ImageAtlas object to get notification about the destruction of the observer.
   */
  void Register( Internal::ImageAtlas& imageAtlas );

  /**
   * @brief Unregister an ImageAtlas which be notified when the observer is destructing.
   * @param[in] imageAtlas The ImageAtlas object to get notification about the destruction of the observer.
   */
  void Unregister( Internal::ImageAtlas& imageAtlas );

private:

  Vector<Internal::ImageAtlas*> mAtlasList; ///< The list of the registered ImageAtlas object

};

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_ATLAS_UPLOAD_OBSERVER_H */
