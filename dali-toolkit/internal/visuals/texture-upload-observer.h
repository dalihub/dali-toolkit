#ifndef DALI_TOOLKIT_INTERNAL_TEXTURE_UPLOAD_OBSERVER_H
#define DALI_TOOLKIT_INTERNAL_TEXTURE_UPLOAD_OBSERVER_H

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
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class TextureSet;

namespace Toolkit
{
/**
 * @brief Base class used to observe the upload status of a texture.
 *
 * Derived class must implement the UploadComplete method which is
 * executed once the texture is ready to draw.
 */
class TextureUploadObserver
{
public:
  typedef Signal<void(TextureUploadObserver*)> DestructionSignalType; ///< Signal prototype for the Destruction Signal.

  /**
   * @brief Constructor.
   */
  TextureUploadObserver();

  /**
   * @brief Virtual destructor.
   */
  virtual ~TextureUploadObserver();

  /**
   * The action to be taken once the async load has finished and the upload to GPU is completed.
   * This should be overridden by the deriving class.
   *
   * @param[in] loadSuccess True if the texture load was successful (i.e. the resource is available). If false, then the resource failed to load. In future, this will automatically upload a "broken" image.
   * @param[in] textureId   The textureId of the loaded texture in the TextureManager
   * @param[in] textureSet  The TextureSet containing the Texture
   * @param[in] useAtlasing True if atlasing was used (note: this may be different to what was requested)
   * @param[in] atlasRect   If using atlasing, this is the rectangle within the atlas to use.
   * @param[in] preMultiplied True if the image had pre-multiplied alpha applied
   */
  virtual void UploadComplete(bool loadSuccess, int32_t textureId, TextureSet textureSet, bool useAtlasing, const Vector4& atlasRect, bool preMultiplied) = 0;

  /**
   * The action to be taken once the async load has finished.
   * This should be overridden by the deriving class.
   *
   * @param[in] loadSuccess   True if the image load was successful (i.e. the resource is available). If false, then the resource failed to load.
   * @param[in] pixelBuffer   The PixelBuffer of the loaded image.
   * @param[in] url           The url address of the loaded image.
   * @param[in] preMultiplied True if the image had pre-multiplied alpha applied
   */
  virtual void LoadComplete(bool loadSuccess, Devel::PixelBuffer pixelBuffer, const Internal::VisualUrl& url, bool preMultiplied) = 0;

  /**
   * @brief Returns the destruction signal.
   * This is emitted when the observer is destroyed.
   * This is used by the observer notifier to mark this observer as destroyed (IE. It no longer needs notifying).
   */
  DestructionSignalType& DestructionSignal();

private:
  DestructionSignalType mDestructionSignal; ///< The destruction signal emitted when the observer is destroyed.
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXTURE_UPLOAD_OBSERVER_H
