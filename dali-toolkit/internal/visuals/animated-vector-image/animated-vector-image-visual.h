#ifndef DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ImageVisualShaderFactory;
class AnimatedVectorImageVisual;
using AnimatedVectorImageVisualPtr = IntrusivePtr<AnimatedVectorImageVisual>;

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
class AnimatedVectorImageVisual : public Visual::Base, public ConnectionTracker, public VectorAnimationManager::LifecycleObserver
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
  static AnimatedVectorImageVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties);

  /**
   * @brief Create the AnimatedVectorImageVisual using the image URL.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to an animated vector image to use
   * @param[in] size The width and height of the rasterized buffer. The visual size will be used if these are 0.
   * @return A smart-pointer to the newly allocated visual.
   */
  static AnimatedVectorImageVisualPtr New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size = ImageDimensions());

public: // from Visual
  /**
   * @copydoc Visual::Base::GetNaturalSize
   */
  void GetNaturalSize(Vector2& naturalSize) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::EnablePreMultipliedAlpha
   */
  void EnablePreMultipliedAlpha(bool preMultiplied) override;

protected: // From VectorAnimationManager::LifecycleObserver:
  /**
   * @copydoc VectorAnimationManager::LifecycleObserver::VectorAnimationManagerDestroyed()
   */
  void VectorAnimationManagerDestroyed() override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The ImageVisualShaderFactory object
   * @param[in] imageUrl The URL to an animated vector image to use
   * @param[in] size The width and height of the rasterized buffer. The visual size will be used if these are 0.
   */
  AnimatedVectorImageVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, ImageDimensions size);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~AnimatedVectorImageVisual() override;

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

  /**
   * @copydoc Visual::Base::GenerateShader
   */
  Shader GenerateShader() const override;

  /**
   * @copydoc Visual::Base::OnDoAction
   */
  void OnDoAction(const Property::Index actionId, const Property::Value& attributes) override;

  /**
   * @copydoc Visual::Base::OnDoActionExtension
   */
  void OnDoActionExtension(const Property::Index actionId, Dali::Any attributes) override;

private:
  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty(Property::Index index, const Property::Value& value);

  /**
   * @brief Called when the resource is ready.
   * @param[in] status The resource status
   */
  void OnResourceReady(VectorAnimationTask::ResourceStatus status);

  /**
   * @brief Event callback from rasterize thread. This is called after the animation is finished.
   *
   * @param[in] playStateId The play state id
   */
  void OnAnimationFinished(uint32_t playStateId);

  /**
   * @brief Send animation data to the rasterize thread.
   */
  void SendAnimationData();

  /**
   * @brief Set the vector image size.
   */
  void SetVectorImageSize();

  /**
   * @brief Stop the animation.
   */
  void StopAnimation();

  /**
   * @brief Trigger rasterization of the vector content.
   */
  void TriggerVectorRasterization();

  /**
   * @brief Callback when the world scale factor changes.
   */
  void OnScaleNotification(PropertyNotification& source);

  /**
   * @brief Callback when the size changes.
   */
  void OnSizeNotification(PropertyNotification& source);

  /**
   * @brief Callback when the visibility of the actor is changed.
   */
  void OnControlVisibilityChanged(Actor actor, bool visible, DevelActor::VisibilityChange::Type type);

  /**
   * @brief Callback when the visibility of the window is changed.
   */
  void OnWindowVisibilityChanged(Window window, bool visible);

  /**
   * @brief Callback when the event is processed.
   */
  void OnProcessEvents();

  // Undefined
  AnimatedVectorImageVisual(const AnimatedVectorImageVisual& visual) = delete;

  // Undefined
  AnimatedVectorImageVisual& operator=(const AnimatedVectorImageVisual& visual) = delete;

private:
  VisualUrl                          mImageUrl;
  VectorAnimationTask::AnimationData mAnimationData;
  VectorAnimationTaskPtr             mVectorAnimationTask;
  ImageVisualShaderFactory&          mImageVisualShaderFactory;
  PropertyNotification               mScaleNotification;
  PropertyNotification               mSizeNotification;
  Vector2                            mVisualSize;
  Vector2                            mVisualScale;
  Dali::ImageDimensions              mDesiredSize{};
  WeakHandle<Actor>                  mPlacementActor;
  DevelImageVisual::PlayState::Type  mPlayState;
  CallbackBase*                      mEventCallback; // Not owned

  uint32_t mLastSentPlayStateId;

  bool mLoadFailed : 1;
  bool mRendererAdded : 1;
  bool mCoreShutdown : 1;
  bool mRedrawInScalingDown : 1;
  bool mEnableFrameCache : 1;
  bool mUseNativeImage : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_ANIMATED_VECTOR_IMAGE_VISUAL_H
