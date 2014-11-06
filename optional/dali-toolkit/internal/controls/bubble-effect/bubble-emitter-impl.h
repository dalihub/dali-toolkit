#ifndef __DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_BUBBLE_EMITTER_IMPL_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/mesh-actor.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/bubble-effect/bubble-emitter.h>
#include <dali-toolkit/public-api/shader-effects/bubble-effect/bubble-effect.h>

#include <map>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

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
   * @copydoc Toolkit::BubbleEmitter::StartExplosion
   */
  void StartExplosion( float duration, float multiple );

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
   * Callback function of the finished signal of off-screen render task.
   * @param[in] source The render task used to create the color adjusted background image.
   */
  void OnRenderFinished(RenderTask& source);

  /**
   * Callback function from Stage to tell us if the context has been regained.
   */
  void OnContextRegained();

  /**
   * Generate the material object which is attached to the meshActor to describe its color, texture, texture mapping mode etc.
   */
  void GenMaterial();

  /**
   * Add a vertex to the mesh data.
   * @param[in] vertices The collection of vertices.
   * @param[in] XYZ The vertex position coordinates.
   * @param[in] UV The vertex texture coordinate.
   */
  void AddVertex(MeshData::VertexContainer& vertices, Vector3 XYZ, Vector2 UV);

  /**
   * Add a triangle to the mesh data.
   * @param[in] faces The collection od FaceIndex items.
   * @param[in] v0 The index of the first point of the triangle.
   * @param[in] v1 The index of the second point of the triangle.
   * @param[in] v3 The index of the first point of the triangle.
   */
  void AddTriangle(MeshData::FaceIndices& faces,size_t v0, size_t v1, size_t v2);

  /**
   * Create a new mesh.
   * @param[in] meshData The MeshData object which encompasses all the data required to describe and render the 3D mesh.
   * @param[in] numberOfBubble The triangle number in the meshData is 2*numOfBubble; two triangles for each bubble
   */
  void ConstructBubbleMesh( MeshData& meshData, unsigned int numOfBubble);

  /**
   * Set the uniform values to the shader effect to emit a bubble
   * @param[in] effect The BubbleEffect to render the current bubble
   * @param[in] curUniform The index of the uniform array in the shader
   * @param[in] emitPosition The start position of the bubble movement.
   * @param[in] displacement The displacement used to bound the moving distance of the bubble.
   */
  void SetBubbleParameter( BubbleEffect& effect, unsigned int curUniform,
                           const Vector2& emitPosition, const Vector2& displacement );

  /**
   * Set the uniform values to the shader effect to emit a bubble
   * @param[in] effect The BubbleEffect to render the current bubble
   * @param[in] curUniform The index of the uniform array in the shader
   * @param[in] emitPosition The start position of the bubble movement.
   * @param[in] direction The direction used to constrain the bubble to move in an adjacent direction around it.
   * @param[in] displacement The displacement used to bound the moving distance of the bubble.
   */
  void SetBubbleParameter( BubbleEffect& effect, unsigned int curUniform,
                           const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement );

  /**
   * Callback function of the explosion animation finished signal to reset the shader parameters
   * @param[in] source The explosion animation.
   */
  void OnExplosionFinished( Animation& source );

  /**
   * Return a random value between the given interval.
   * @param[in] f0 The low bound
   * @param[in] f1 The up bound
   * @return A random value between the given interval
   */
  float RandomRange(float f0, float f1);

private:

  Vector2                     mMovementArea;        ///< The size of the bubble moving area, usually the same size as the background image actor.
  Image                       mShapeImage;          ///< The alpha channnel of this texture defines the bubble shape.
  Actor                       mBubbleRoot;          ///<The bubble root actor. Need to add it to stage to get the bubbles rendered.

  unsigned int                mNumBubblePerShader;  ///< How many bubbles for each BubbleEffect shader.
  unsigned int                mNumShader;           ///< How many BubbleEffect shaders are used.
  unsigned int                mTotalNumOfBubble;    ///< mNumBubblePerShader*mNumShader.
  bool                        mRenderTaskRunning;   ///< If the background render task is currently running

  Vector2                     mBubbleSizeRange;     ///< The bubble size range.

  std::vector<Mesh>           mMesh;                ///< The mesh vector, each mesh is used to create a meshActor which applies a BubbleEffect.
  std::vector<MeshActor>      mMeshActor;           ///< The meshActor vector, its size is mNumShader.
  MeshActor                   mMeshActorForNoise;   ///< An Extra mesh data to emit bubbles which emit bubble in totally random angle.
  Material                    mCustomMaterial;      ///< The material object which is attached to the meshActor to describe its color, texture, texture mapping mode etc.

  std::vector<BubbleEffect>   mEffect;              ///< The bubbleEffect vector, corresponding to the mMeshActoe vector.
  BubbleEffect                mEffectForNoise;      ///< The extra bubbleEffect, corresponding to the mMeshActorForNoise.

  unsigned int                mCurrentUniform;      ///< Keep track of the uniform index for the newly emitted bubble

  Vector3                     mHSVDelta;            ///< The HSV difference used to adjust the background image color.
  Image                       mBackgroundImage;     ///< The original background image
  FrameBufferImage            mEffectImage;         ///< The image stores the adjusted color of the background image.The bubbles pick color from this image.
  CameraActor                 mCameraActor;         ///< The render task views the scene from the perspective of this actor.

  unsigned int                mDensity;             ///< How many bubbles will emit at each time, they are controlled by same uniforms in the shader.

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
