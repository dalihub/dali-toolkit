#ifndef DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H
#define DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H

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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/property-buffer.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class BubbleRenderer;

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
                                     Texture shapeTexture,
                                     unsigned int maximumNumberOfBubble,
                                     const Vector2& bubbleSizeRange );

  /**
   * @copydoc Toolkit::BubbleEmitter::GetRootActor
   */
  Actor GetRootActor();

  /**
   * @copydoc Toolkit::BubbleEmitter::SetBackground
   */
  void SetBackground( Texture bgTexture, const Vector3& hsvDelta );

  /**
   * @copydoc Toolkit::BubbleEmitter::SetShape
   */
  void SetBubbleShape( Texture shapeTexture );

  /**
   * @copydoc Toolkit::BubbleEmiter::SetBubbleScale
   */
  void SetBubbleScale( float scale );

  /**
   * @copydoc Toolkit::BubbleEmitter::SetBubbleDensity
   */
  void SetBubbleDensity( unsigned int density );

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
   * @param[in] shapeTexture The alpha channnel of this texture defines the bubble shape.
   * @param[in] maximumNumberOfBubble The maximum number of bubble needed.
   * @param[in] bubbleSizeRange The size range of the bubbles; x component is the minimal size, and y component is the maximum size.
   */
  BubbleEmitter( const Vector2& movementArea,
                 Texture shapeTexture,
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
   * @param[in] source The render task used to create the color adjusted background texture.
   */
  void OnRenderFinished(RenderTask& source);

  /**
   * Callback function from Stage to tell us if the context has been regained.
   */
  void OnContextRegained();

  /**
   * Set the uniform values to the shader effect to emit a bubble
   * @param[in] bubbleRenderer The BubbleRenderer
   * @param[in] curUniform The index of the uniform array in the shader
   * @param[in] emitPosition The start position of the bubble movement.
   * @param[in] direction The direction used to constrain the bubble to move in an adjacent direction around it.
   * @param[in] displacement The displacement used to bound the moving distance of the bubble.
   */
  void SetBubbleParameter( BubbleRenderer& bubbleRenderer, unsigned int curUniform,
                           const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement );

private:

  Actor                       mBubbleRoot;          ///<The bubble root actor. Need to add it to stage to get the bubbles rendered.
  Texture                     mShapeTexture;        ///< The alpha channnel of this texture defines the bubble shape.
  Texture                     mBackgroundTexture;   ///< The original background texture
  Texture                     mEffectTexture;       ///< Texture which stores the adjusted color of the background image.The bubbles pick color from this image.
  FrameBuffer                 mFrameBuffer;         ///< FrameBuffer used for offscreen rendering
  CameraActor                 mCameraActor;         ///< The render task views the scene from the perspective of this actor.

  Geometry                    mMeshGeometry;         ///< The mesh geometry which contains the vertices and indices data
  TextureSet                  mTextureSet;           ///< The texture set which controls the bubble display
  std::vector<BubbleRenderer> mBubbleRenderers;      ///< The BubbleRenderer vector, its size is mNumShader.

  Vector2                     mMovementArea;        ///< The size of the bubble moving area, usually the same size as the background.
  Vector2                     mBubbleSizeRange;     ///< The size range of the bubbles; x component is the low bound, and y component is the up bound.
  Vector3                     mHSVDelta;            ///< The HSV difference used to adjust the background image color.

  unsigned int                mNumBubblePerRenderer;   ///< How many bubbles for each BubbleRenderer.
  unsigned int                mNumRenderer;            ///< How many BubbleRenderers are used.
  unsigned int                mDensity;             ///< How many bubbles will emit at each time, they are controlled by same uniforms in the shader.
  unsigned int                mTotalNumOfBubble;    ///< mNumBubblePerShader*mNumShader.
  unsigned int                mCurrentBubble;       ///< Keep track of the index for the newly emitted bubble
  unsigned int                mRandomSeed;          ///< Seed to generate random number.

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

#endif // DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H
