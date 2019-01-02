#ifndef DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/weak-handle.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ImageVisualShaderFactory;
class VectorRasterizeThread;
class AnimatedVectorImageVisual;
using AnimatedVectorImageVisualPtr = IntrusivePtr< AnimatedVectorImageVisual >;

/**
 * The visual which renders an animated vector image using VectorAnimationRenderer.
 * VectorAnimationRenderer renders the animation image and this visuall controls the images.
 *
 * The following property is essential
 *
 * | %Property Name           | Type             |
 * |--------------------------|------------------|
 * | url                      | STRING           |
 *
 */
class AnimatedVectorImageVisual: public Visual::Base
{
public:

  /**
   * @brief Create the AnimatedVectorImageVisual using the image URL.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to an animated vector image to use
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static AnimatedVectorImageVisualPtr New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties );

  /**
   * @brief Create the AnimatedVectorImageVisual using the image URL.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to an animated vector image to use
   * @return A smart-pointer to the newly allocated visual.
   */
  static AnimatedVectorImageVisualPtr New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl );

public:  // from Visual

  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  virtual void DoCreateInstancePropertyMap( Property::Map& map ) const override;

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to an animated vector image to use
   */
  AnimatedVectorImageVisual( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~AnimatedVectorImageVisual();

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  virtual void DoSetProperties( const Property::Map& propertyMap ) override;

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::DoSetOffStage
   */
  virtual void DoSetOffStage( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  virtual void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::OnDoAction
   */
  virtual void OnDoAction( const Property::Index actionId, const Property::Value& attributes ) override;

private:

  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty( Property::Index index, const Property::Value& value );

  /**
   * @brief Event callback from rasterize thread. This is called after the first frame is ready.
   */
  void OnResourceReady();

  /**
   * @brief Event callback from rasterize thread. This is called after the animation is finished.
   */
  void OnAnimationFinished();

  // Undefined
  AnimatedVectorImageVisual( const AnimatedVectorImageVisual& visual ) = delete;

  // Undefined
  AnimatedVectorImageVisual& operator=( const AnimatedVectorImageVisual& visual ) = delete;

private:
  ImageVisualShaderFactory&                    mImageVisualShaderFactory;
  VisualUrl                                    mUrl;
  VectorRasterizeThread                        mVectorRasterizeThread;
  Vector2                                      mVisualSize;
  WeakHandle< Actor >                          mPlacementActor;
  DevelAnimatedVectorImageVisual::Action::Type mActionStatus;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H
