#ifndef DALI_TOOLKIT_INTERNAL_TEXTURE_UPLOAD_OBSERVER_H
#define DALI_TOOLKIT_INTERNAL_TEXTURE_UPLOAD_OBSERVER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Base class used to observe the upload status of a texture.
 *
 * Derived class must implement the LoadComplete method which is
 * executed once the texture is ready to draw.
 */
class TextureUploadObserver
{
public:
  typedef Signal<void(TextureUploadObserver*)> DestructionSignalType; ///< Signal prototype for the Destruction Signal.

  enum class ReturnType
  {
    PIXEL_BUFFER = 0,
    TEXTURE,
    ANIMATED_IMAGE_TEXTURE
  };

  struct TextureInformation
  {
    TextureInformation(ReturnType returnType, int32_t textureId, TextureSet textureSet, bool useAtlasing, const Vector4& atlasRect, bool preMultiplied);
    TextureInformation(ReturnType returnType, int32_t textureId, TextureSet textureSet, const std::string& url, bool preMultiplied);
    TextureInformation(ReturnType returnType, Devel::PixelBuffer pixelBuffer, const std::string& url, bool preMultiplied);
    TextureInformation(ReturnType returnType, int32_t textureId, uint32_t frameCount, uint32_t interval);

    TextureInformation();

    ReturnType         returnType;    ///< Returned Texture type.
    int32_t            textureId;     ///< The textureId of the loaded texture in the TextureManager
    TextureSet         textureSet;    ///< The TextureSet containing the Texture
    bool               useAtlasing;   ///< True if atlasing was used (note: this may be different to what was requested)
    const Vector4&     atlasRect;     ///< If using atlasing, this is the rectangle within the atlas to use.
    bool               preMultiplied; ///< True if the image had pre-multiplied alpha applied
    Devel::PixelBuffer pixelBuffer;   ///< The PixelBuffer of the loaded image.
    std::string_view   url;           ///< The url address of the loaded image.
    uint32_t           frameCount{0}; ///< The frameCount of the animated image
    uint32_t           interval{0};   ///< Time interval between currently loaded frame and next frame.
  };

public:
  /**
   * @brief Constructor.
   */
  TextureUploadObserver();

  /**
   * @brief Virtual destructor.
   */
  virtual ~TextureUploadObserver();

  /**
   * The action to be taken once the async load has finished.
   * And in case of texture loading, this method is called after uploading.
   * This should be overridden by the deriving class.
   *
   * @param[in] loadSuccess True if the texture load was successful (i.e. the resource is available). If false, then the resource failed to load. In future, this will automatically upload a "broken" image.
   * @param[in] textureInformation Structure that contains loaded texture information.
   */
  virtual void LoadComplete(bool loadSuccess, TextureInformation textureInformation) = 0;

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
