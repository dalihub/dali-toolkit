#ifndef __DALI_TOOLKIT_BUBBLE_EMMITER_H__
#define __DALI_TOOLKIT_BUBBLE_EMMITER_H__

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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
  /**
   * @brief BubbleEmitter implementation class.
   */
  class BubbleEmitter;
}

/**
 * @brief BubbleEmitter is used to display lots of moving bubbles on the stage.
 *
 * This is done by applying BubbleEffect to multiple specifically created meshActors.
 */
class DALI_IMPORT_API BubbleEmitter : public Control
{
public:

  /**
   * @brief Create an empty BubbleEmitter handle.
   */
  BubbleEmitter();

  /**
   * @brief Virtual destructor.
   */
  ~BubbleEmitter();

  /**
   * @brief Create an initialized BubbleEmitter.
   *
   * @param[in] winSize The size of the bubble moving area, usually the same size as the background image actor.
   * @param[in] shapeImage The alpha channnel of this texture defines the bubble shape.
   * @param[in] maximumNumberOfBubble The maximum number of bubble needed.
   * @param[in] bubbleSizeRange The size range of the bubbles; x component is the low bound, and y component is the up bound.
   * @return The initialized BubbleEmitter object.
   */
  static BubbleEmitter New( const Vector2& winSize,
                            Image shapeImage,
                            unsigned int maximumNumberOfBubble,
                            const Vector2& bubbleSizeRange );


  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   * @param[in] handle The handle to copy
   */
  BubbleEmitter( const BubbleEmitter& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] rhs The object to point at
   * @return A reference to this
   */
  BubbleEmitter& operator=( const BubbleEmitter& rhs );

  /**
   * @brief Downcast an Object handle to SuperBlurView.
   *
   * If handle points to a BubbleEmitter, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a BubbleEmitter or an uninitialized handle
   */
  static BubbleEmitter DownCast( BaseHandle handle );

  /**
   * @brief Return the root actor of all bubbles, should then be added to stage.
   *
   * @return The bubble root actor.
   */
  Actor GetRootActor();

  /**
   * @brief Set Background image.
   *
   * The bubbles pick color from this image with HSV values adjusted.
   * @param[in] bgImage The background image which provide color to bubbles.
   * @param[in] hsvDelta The hsv channel difference used to adjust the background image color.
   *            If set these vector as Vector3::Zero, original colors are used.
   */
  void SetBackground( Image bgImage, const Vector3& hsvDelta );

  /**
   * @brief Set bubble shape.
   *
   * The bubble mesh is a rectangular patch, but its displayed shape is decided by the alpha channel of the shape image.
   * @param[in] shapeImage The image whose alpha channel defines the bubble shape.
   */
  void SetShapeImage( Image shapeImage );

  /**
   * @brief Set the scale factor applied to all the bubbles.
   *
   * @param [in] scale The scale factor applied on bubbles.
   */
  void SetBubbleScale( float scale );

  /**
   * @brief Set the density of the bubble.
   *
   * Ideally every bubble's moving track is controlled by different uniforms in shader.
   * To increase the density, 'density' number of bubbles are sharing one group of uniforms, but with random offsets between these bubbles.
   * The available densities are one to nine only. The default value is five.
   * By set the density bigger than one, instead of emit one bubble each time, a 'density' number of bubbles are emitted.
   * @param[in] density The density of the bubble.
   */
  void SetBubbleDensity( unsigned int density );

  /**
   * @brief Enable different blending mode for rendering.
   *
   * @param[in] enable If false, the default blending function if ImageActor is used.
   */
  void SetBlendMode( bool enable );

  /**
   * @brief Add a bubble movement to the animation.
   *
   * @param[in] animation The animation reference.
   * By passing the animation into BubbleEmitter, the animation's duration and how many bubbles contained within this animation are freely decided in App.
   * @param[in] emitPosition The start position of the bubble movement.
   * @param[in] direction The direction used to constrain the bubble to move in an adjacent direction around it.
   * @param[in] displacement The displacement used to bound the moving distance of the bubble.
   */
  void EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement );

  /**
   * @brief Reset all the parameters controlling the bubbles after animation.
   */
  void Restore();

public: // Not intended for developer use

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL BubbleEmitter(Internal::BubbleEmitter& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL BubbleEmitter(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_BUBBLE_EMMITER_H__ */
