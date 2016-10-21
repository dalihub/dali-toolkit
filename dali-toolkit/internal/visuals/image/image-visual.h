#ifndef DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/devel-api/image-loader/atlas-upload-observer.h>

namespace Dali
{

class NativeImage;

namespace Toolkit
{

namespace Internal
{

class ImageVisual;
typedef IntrusivePtr< ImageVisual > ImageVisualPtr;

/**
 * The visual which renders an image to the control's quad
 *
 * The following properties are optional
 *
 * | %Property Name     | Type             |
 * |--------------------|------------------|
 * | url                | STRING           |
 * | fittingMode        | STRING           |
 * | samplingMode       | STRING           |
 * | desiredWidth       | INT              |
 * | desiredHeight      | INT              |
 * | synchronousLoading | BOOLEAN          |
 *
 * where imageFittingMode should be one of the following fitting modes:
 *   "SHRINK_TO_FIT"
 *   "SCALE_TO_FIT"
 *   "FIT_WIDTH"
 *   "FIT_HEIGHT"
 *   "DEFAULT"
 *
 * where imageSamplingMode should be one of the following sampling modes:
 *   "BOX"
 *   "NEAREST"
 *   "LINEAR"
 *   "BOX_THEN_NEAREST"
 *   "BOX_THEN_LINEAR"
 *   "NO_FILTER"
 *   "DONT_CARE"
 *   "DEFAULT"
 *
 */
class ImageVisual: public Visual::Base, public ConnectionTracker, public AtlasUploadObserver
{
public:

  /**
   * @brief Create a new image visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @return A smart-pointer to the newly allocated visual.
   */
  static ImageVisualPtr New( VisualFactoryCache& factoryCache );

  /**
   * @brief Create a new image visual with a URL.
   *
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  static ImageVisualPtr New( VisualFactoryCache& factoryCache,
                             const std::string& imageUrl,
                             ImageDimensions size = ImageDimensions(),
                             FittingMode::Type fittingMode = FittingMode::DEFAULT,
                             Dali::SamplingMode::Type samplingMode = SamplingMode::BOX_THEN_LINEAR );

  /**
   * @brief Create a new image visual with an Image type.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] image The image to use
   */
  static ImageVisualPtr New( VisualFactoryCache& factoryCache, const Image& image );

public:  // from Visual

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

  /**
   * @copydoc Visual::Base::DoSetProperty
   */
  virtual void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @copydoc Visual::Base::DoGetProperty
   */
  virtual Dali::Property::Value DoGetProperty( Dali::Property::Index index );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   */
  ImageVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief Constructor with a URL.
   *
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  ImageVisual( VisualFactoryCache& factoryCache,
               const std::string& imageUrl,
               ImageDimensions size,
               FittingMode::Type fittingMode,
               Dali::SamplingMode::Type samplingMode );

  /**
   * @brief Constructor with an Image type.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] image The image to use
   */
  ImageVisual( VisualFactoryCache& factoryCache, const Image& image );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ImageVisual();

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  virtual void DoSetProperties( const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::Base::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor );

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  virtual void OnSetTransform();

public:

  /**
   * Get the standard image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] atlasing Whether texture atlasing is applied.
   * @param[in] defaultTextureWrapping Whether the default texture wrap mode is applied.
   */
  static Shader GetImageShader( VisualFactoryCache& factoryCache, bool atlasing, bool defaultTextureWrapping );

  /**
   * @copydoc AtlasUploadObserver::UploadCompleted
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  virtual void UploadCompleted();

private:

  /**
   * @brief Applies the image to the texture set used for this renderer
   *
   * @param[in] image The Image to apply to the texture set used for this renderer
   */
  void ApplyImageToSampler( const Image& image );

  /**
   * @brief Initializes the Dali::Renderer from an image url string
   *
   * @param[in] imageUrl The image url string to intialize this ImageVisual from
   */
  void InitializeRenderer( const std::string& imageUrl );

  /**
   * @brief Initializes the Dali::Renderer from an image handle
   *
   * @param[in] image The image handle to intialize this ImageVisual from
   */
  void InitializeRenderer( const Image& image );

  /**
   * @brief Creates the Dali::Renderer (potentially from the renderer cache), initializing it
   * @param[in] textures to use
   */
  void CreateRenderer( TextureSet& textures );

  /**
   * @brief Creates the Dali::Renderer for NativeImage with custom sampler type and prefix, initializing it
   * @param NativeImageRenderer
   */
  void CreateNativeImageRenderer( NativeImage& nativeImage );

  /**
   * @brief Query whether resources requires to be loaded synchronously.
   * @return Returns true if synchronous resource loading is required, false otherwise.
   */
  bool IsSynchronousResourceLoading() const;

  /**
   * @brief Load the resource synchronously
   */
  void LoadResourceSynchronously();

  /**
   * Creates the texture set and adds the texture to it
   * @param[out] textureRect The texture area of the texture in the atlas.
   * @param[in] url The URL of the image resource to use.
   * @param[in] synchronousLoading If true, the resource is loaded synchronously, otherwise asynchronously.
   * @param[in] attemptAtlasing If true will attempt atlasing, otherwise create unique texture
   * @return the texture set to use
   */
  TextureSet CreateTextureSet( Vector4& textureRect, const std::string& url, bool synchronousLoading, bool attemptAtlasing );

  /**
   * Callback function of image resource loading succeed
   * @param[in] image The Image content that we attempted to load from mImageUrl
   */
  void OnImageLoaded( ResourceImage image );

  /**
   * Set the value to the uTextureRect uniform
   * @param[in] textureRect The texture rectangular area.
   */
  void SetTextureRectUniform( const Vector4& textureRect  );

  /**
   * Clean the renderer from cache, and remove the image from atlas if it is not used anymore
   */
  void CleanCache(const std::string& url);

private:

  Image mImage;
  PixelData mPixels;
  Vector4 mPixelArea;
  WeakHandle<Actor> mPlacementActor;
  std::string mImageUrl;

  Dali::ImageDimensions mDesiredSize;
  Dali::FittingMode::Type mFittingMode:3;
  Dali::SamplingMode::Type mSamplingMode:4;
  Dali::WrapMode::Type mWrapModeU:3;
  Dali::WrapMode::Type mWrapModeV:3;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H */
