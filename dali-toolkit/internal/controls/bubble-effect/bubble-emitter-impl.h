#ifndef __DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/geometry/geometry.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/shader-effects/sampler.h>
#include <dali/public-api/shader-effects/material.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class BubbleActor;
typedef IntrusivePtr<BubbleActor> BubbleActorPtr;

/**
 * BubbleEmitter implementation class.
 */
class BubbleEmitter : public Control
{
public:

  /**
   * Destructor
   */
  ~BubbleEmitter();

  /**
   * @copydoc Toolkit::BubbleEmitter::New
   */
  static Toolkit::BubbleEmitter New( const Vector2& winSize,
                                     Image shapeImage,
                                     unsigned int maximumNumberOfBubble,
                                     const Vector2& bubbleSizeRange );

  /**
   * @copydoc Toolkit::BubbleEmitter::GetRootActor
   */
  Actor GetRootActor();

  /**
   * @copydoc Toolkit::BubbleEmitter::SetBackground
   */
  void SetBackground( Image bgImage, const Vector3& hsvDelta );

  /**
   * @copydoc Toolkit::BubbleEmitter::SetShapeImage
   */
  void SetShapeImage( Image shapeImage );

  /**
   * @copydoc Toolkit::BubbleEmiter::SetBubbleScale
   */
  void SetBubbleScale( float scale );

  /**
   * @copydoc Toolkit::BubbleEmitter::SetBubbleDensity
   */
  void SetBubbleDensity( unsigned int density );

  /**
   * @copydoc Toolkit::BubbleEmitter::SetBlendMode
   */
  void SetBlendMode( bool enable );

  /**
   * @copydoc Toolkit::BubbleEmitter::EmitBubble
   */
  void EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement );

  /**
   * @copydoc Toolkit::BubbleEmitter::Restore
   */
  void Restore();

private:

  /**
   * Construct a new BubbleEmitter object.
   * @param[in] movementArea The size of the bubble moving area
   * @param[in] shapeImage The alpha channnel of this texture defines the bubble shape.
   * @param[in] maximumNumberOfBubble The maximum number of bubble needed.
   * @param[in] bubbleSizeRange The size range of the bubbles; x component is the minimal size, and y component is the maximum size.
   */
  BubbleEmitter( const Vector2& movementArea,
                 Image shapeImage,
                 unsigned int maximumNumberOfBubble,
                 const Vector2& bubbleSizeRange );

  /**
   * This method is called after the CubeTransitionEffect has been initialized.
   * The meshActors and BubbleEffects are created here.
   */
  void OnInitialize();

  /**
   * Create the geometry of a mesh.
   * @param[in] numOfPatch The triangle number in the mesh is 2*numOfPatch; two triangles for each bubble.
   * @return The mesh geometry.
   */
  Geometry CreateGeometry( unsigned int numOfPatch );

  /**
   * Callback function of the finished signal of off-screen render task.
   * @param[in] source The render task used to create the color adjusted background image.
   */
  void OnRenderFinished(RenderTask& source);

  /**
   * Callback function from Stage to tell us if the context has been regained.
   */
  void OnContextRegained();

  /**
   * Set the uniform values to the shader effect to emit a bubble
   * @param[in] bubbleActor The BubbleActor to render the current bubble
   * @param[in] curUniform The index of the uniform array in the shader
   * @param[in] emitPosition The start position of the bubble movement.
   * @param[in] direction The direction used to constrain the bubble to move in an adjacent direction around it.
   * @param[in] displacement The displacement used to bound the moving distance of the bubble.
   */
  void SetBubbleParameter( BubbleActorPtr bubbleActor, unsigned int curUniform,
                           const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement );

private:

  Actor                       mBubbleRoot;          ///<The bubble root actor. Need to add it to stage to get the bubbles rendered.
  Image                       mShapeImage;          ///< The alpha channnel of this texture defines the bubble shape.
  Image                       mBackgroundImage;     ///< The original background image
  FrameBufferImage            mEffectImage;         ///< The image stores the adjusted color of the background image.The bubbles pick color from this image.
  CameraActor                 mCameraActor;         ///< The render task views the scene from the perspective of this actor.

  Sampler                     mSamplerBackground;    ///< The sampler which provides the background image to material
  Sampler                     mSamplerBubbleShape;   ///< The sampler which provides the bubble shape image to material
  Geometry                    mMeshGeometry;         ///< The mesh geometry which contains the vertices and indices data
  Material                    mMaterial;             ///< The material which controls the bubble display
  std::vector<BubbleActorPtr> mBubbleActors;         ///< The meshActor vector, its size is mNumShader.

  Vector2                     mMovementArea;        ///< The size of the bubble moving area, usually the same size as the background image actor.
  Vector2                     mBubbleSizeRange;     ///< The size range of the bubbles; x component is the low bound, and y component is the up bound.
  Vector3                     mHSVDelta;            ///< The HSV difference used to adjust the background image color.

  unsigned int                mNumBubblePerActor;   ///< How many bubbles for each BubbleActor.
  unsigned int                mNumActor;            ///< How many BubbleActors are used.
  unsigned int                mDensity;             ///< How many bubbles will emit at each time, they are controlled by same uniforms in the shader.
  unsigned int                mTotalNumOfBubble;    ///< mNumBubblePerShader*mNumShader.
  unsigned int                mCurrentBubble;       ///< Keep track of the index for the newly emitted bubble

  bool                        mRenderTaskRunning;   ///< If the background render task is currently running

};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::BubbleEmitter& GetImpl(Dali::Toolkit::BubbleEmitter& obj)
{
  DALI_ASSERT_ALWAYS(obj && "BubbleEmitter handle is empty");
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::BubbleEmitter&>(handle);
}

inline const Internal::BubbleEmitter& GetImpl(const Dali::Toolkit::BubbleEmitter& obj)
{
  DALI_ASSERT_ALWAYS(obj && "BubbleEmitter handle is empty");
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::BubbleEmitter&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H__ */
