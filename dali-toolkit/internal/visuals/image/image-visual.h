#ifndef DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/resource-image.h>

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
class ImageVisual: public Visual::Base, public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   */
  ImageVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~ImageVisual();

public:  // from Visual

  /**
   * @copydoc Visual::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Visual::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:
  /**
   * @copydoc Visual::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Visual::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor );

public:

  /**
   * Get the standard image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] atlasing Whether texture atlasing is applied.
   * @param[in] defaultTextureWrapping Whether the default texture wrap mode is applied.
   */
  static Shader GetImageShader( VisualFactoryCache& factoryCache, bool atlasing, bool defaultTextureWrapping );

  /**
   * @brief Sets the image of this visual to the resource at imageUrl
   * The visual will load the Image asynchronously when the associated actor is put on stage, and destroy the image when it is off stage
   *
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] imageUrl The URL of the image resource to use
   * @param[in] size The width and height to fit the loaded image to.
   * @param[in] fittingMode The FittingMode of the resource to load
   * @param[in] samplingMode The SamplingMode of the resource to load
   */
  void SetImage( Actor& actor,
                 const std::string& imageUrl,
                 ImageDimensions size=ImageDimensions(),
                 FittingMode::Type fittingMode = FittingMode::DEFAULT,
                 Dali::SamplingMode::Type samplingMode = SamplingMode::BOX_THEN_LINEAR );

  /**
   * @brief Sets the image to be rendered by this visual
   *
   * @param[in] actor The Actor the renderer is applied to if, empty if the renderer has not been applied to any Actor
   * @param[in] image The image to use
   */
  void SetImage( Actor& actor, const Image& image );

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
   *
   * @return Returns the created Dali::Renderer
   */
  Renderer CreateRenderer() const;

  /**
   * @brief Creates the Dali::Renderer for NativeImage with custom sampler type and prefix, initializing it
   *
   * @return Returns the created Dali::Renderer
   */
  Renderer CreateNativeImageRenderer() const;

  /**
   * @brief Query whether resources requires to be loaded synchronously.
   * @return Returns true if synchronoud resource loading is required, false otherwise.
   */
  bool IsSynchronousResourceLoading() const;

  /**
   * @brief Do the synchronous resource loading
   */
  void DoSynchronousResourceLoading();

  /**
   * Load the image.
   * @param[in] url The URL of the image resource to use.
   * @param[in] synchronousLoading If true, the resource is loaded synchronously, otherwise asynchronously.
   */
  Image LoadImage( const std::string& url, bool synchronousLoading );

  /**
   * Load the image and create a texture set to hold the texture, with automatic atlasing applied.
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param[in] url The URL of the image resource to use.
   * @param[in] synchronousLoading If true, the resource is loaded synchronously, otherwise asynchronously.
   */
  TextureSet CreateTextureSet( Vector4& textureRect, const std::string& url, bool synchronousLoading );

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

  /**
   * Set shader code for nativeimage if it exists
   */
  void SetNativeFragmentShaderCode( Dali::NativeImage& nativeImage );

private:
  Image mImage;
  PixelData mPixels;
  Vector4 mPixelArea;

  std::string mImageUrl;
  Dali::ImageDimensions mDesiredSize;
  Dali::FittingMode::Type mFittingMode;
  Dali::SamplingMode::Type mSamplingMode;
  Dali::WrapMode::Type mWrapModeU;
  Dali::WrapMode::Type mWrapModeV;

  std::string mNativeFragmentShaderCode;
  bool mNativeImageFlag;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_IMAGE_VISUAL_H */
