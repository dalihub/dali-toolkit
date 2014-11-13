#ifndef __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_EFFECT_H__

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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/shader-effects/shader-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/transition-effects/cube-transition-effect.h>

namespace Dali
{

namespace Toolkit
{

class CubeTransitionEffect;

namespace Internal
{

/**
 * Create a image with size of viewAreaSize
 * with the effect image as its center part and (0,0,0,1) at other parts
 */
class FullAreaImageCreator : public ShaderEffect
{

public:

  /**
   * Create an uninitialized FullAreaImageCreator
   * this can be initialized with FullAreaImageCreator::New()
   */
  FullAreaImageCreator(){}

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FullAreaImageCreator(){}

  /**
   * Create an initialized FullAreaImageCreator.
   * @return A handle to a newly allocated Dali resource.
   */
  static FullAreaImageCreator New()
  {
    std::string vertexShader(
      "uniform mediump vec4 uRegion; \n"
       "void main() \n"
      "{\n"
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
      "  vTexCoord.s = (aTexCoord.s - uRegion.s) / uRegion.p;"
      "  vTexCoord.t = ( 1.0 - aTexCoord.t - uRegion.t) / uRegion.q;"
      "}\n"
    );

    std::string fragmentShader(
      "uniform mediump vec4 uRegion; \n"
      "void main() \n"
      "{\n"
      "  if( vTexCoord.s > 0.0 && vTexCoord.s < 1.0 && vTexCoord.t > 0.0 && vTexCoord.t < 1.0) \n"
      "  { \n"
      "    gl_FragColor = texture2D( sEffect, vTexCoord ) * uColor ; \n"
      "  } \n"
      "  else \n"
      "  { \n"
      "    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 ); \n"
      "  } \n"
      "}\n"
    );

    ShaderEffect shaderEffectCustom = ShaderEffect::New(vertexShader, fragmentShader);
    FullAreaImageCreator handle( shaderEffectCustom );

    return handle;
  }

  /**
   * Set up the position and size of the effect texture
   * @param[in] viewArea the size of full-area image to create
   * @param[in] size the size of effect texture
   */
  void SetRegionSize( const Vector2& viewArea, const Vector2& size )
  {
    Vector2 sizeRatio( std::min(1.f, size.x / viewArea.x), std::min(1.f, size.y / viewArea.y) );
    Vector4 region( (1.f-sizeRatio.x)*0.5f,
                    (1.f-sizeRatio.y)*0.5f,
                    sizeRatio.x,
                    sizeRatio.y  );
    SetUniform( "uRegion", region );
  }

private:

  FullAreaImageCreator( ShaderEffect handle )
  : ShaderEffect( handle )
  {}

};



/**
 * CubeTransitionEffect implementation class
 */
class CubeTransitionEffect : public Dali::BaseObject, public ConnectionTracker
{

public:

  /**
   * Destructor
   */
  ~CubeTransitionEffect();

  /**
   * @copydoc Toolkit::CubeTransitionEffect::SetTransitionDuration
   */
  void SetTransitionDuration( float duration );

  /**
   * @copydoc Toolkit::CubeTransitionEffect::GetTransitionDuration
   */
  float GetTransitionDuration() const;

  /**
   * @copydoc Toolkit::CubeTransitionEffect::SetCubeDisplacement
   */
  void SetCubeDisplacement( float displacement );

  /**
   * @copydoc Toolkit::CubeTransitionEffect::GetCubeDisplacement
   */
  float GetCubeDisplacement() const;

  /**
   * @copydoc Toolkit::CubeTransitionEffect::GetRoot
   */
  Actor GetRoot();

  /**
   * @copydoc Toolkit::CubeTransitionEffect::IsTransiting
   */
  bool IsTransiting();

  /**
   * @copydoc Toolkit::CubeTransitionEffect::SetFirstImage
   */
  void SetCurrentImage(ImageActor imageActor);

  /**
   * @copydoc Toolkit::CubeTransitionEffect::SetTargetImage
   */
  void SetTargetImage(ImageActor imageActor);

  /**
   * @copydoc Toolkit::CubeTransitionEffect::StartTransition(bool)
   */
  void StartTransition( bool toNextImage = true );

  /**
   * @copydoc Toolkit::CubeTransitionEffect::StartTransition(Vector2, Vector2)
   */
  void StartTransition( Vector2 panPosition, Vector2 panDisplacement );

  /**
   * @copydoc Toolkit::CubeTransitionEffect::PauseTransition()
   */
  void PauseTransition();

  /**
   * @copydoc Toolkit::CubeTransitionEffect::ResumeTransition()
   */
  void ResumeTransition();

  /**
   * @copydoc Toolkit::CubeTransitionEffect::StopTransition()
   */
  void StopTransition();

public: //Signal

  /**
   * @copydoc Toolkit::CubeTransitionEffect::TransitionCompletedSignal()
   */
  Toolkit::CubeTransitionEffect::TransitionCompletedSignalV2& TransitionCompletedSignal();

protected:

  /**
   * Construct a new CubeTransitionEffect object
   * Called in the constructor of subclasses
   * @param[in] numRows How many rows of cubes
   * @param[in] numColumns How many columns of cubes
   * @param[in] viewAreaSize The size of view area for this transition effect
   */
  CubeTransitionEffect( unsigned int numRows, unsigned int numColumns, Size viewAreaSize );

  /**
   * Initialization steps: creating a layer, two groups of tiles,
   * and one group of actors (cubes) serving as parents of every two tiles (one from each image).
   */
  void Initialize();

private:

  /**
   * Create an image actor to serve as a face of the cube
   * @param[in] image The image to display.
   * @param[in] color The color to set to the actor
   * @return The tile actor created
   */
  ImageActor CreateTile( Image image, const Vector4& color );

  /**
   * Set Image content to tiles
   * As only when the image ready, can we get correct image attributes
   * so inside this function, the process needs to be passed to callBack of image resource loading succeed.
   * @param[in] imageActor The imageActor whose image content will be set to the tiles
   */
  void SetImage(ImageActor imageActor);

  /**
   * Callback function of image resource loading succeed
   * Set image and pixelArea to tiles
   * @param[in] image The image content of the imageActor for transition
   */
  void OnImageLoaded(Image image);

  /**
   * Callback function of transition animation finished
   * Hide transition layer, show current imageActor, and set isAnimating flag to false
   * @param[in] source The cube transition animation
   */
  void OnTransitionFinished(Animation& source);

  /**
   * This method is called after the CubeTransitionEffect has been initialized.  Derived classes should do
   * any second phase initialization by overriding this method.
   */
  virtual void OnInitialize() { }

  /**
   * This method is called after the a new transition is activated.
   * Derived classes should do any specialized transition process by overriding this method.
   * @param[in] panPosition The press down position of panGesture
   * @param[in] panDisplacement The displacement vector of panGesture
   */
  virtual void OnStartTransition( Vector2 panPosition, Vector2 panDisplacement ) {}

  /**
   * This method is called when the transition is forced stop in the middle of animation.
   * Derived classed should set the rotation status of the cubes to the same as the final state when the animation is finished completely.
   * So that the next transition would be started correctly.
   */
  virtual void OnStopTransition() {}


protected:

  unsigned int               mNumRows;
  unsigned int               mNumColumns;
  Size                       mViewAreaSize;
  ActorContainer             mBoxes;
  std::vector< ImageActor >  mTiles[2];
  int                        mRotateIndex;
  Size                       mTileSize;
  Actor                      mRoot;

  ImageActor                 mCurrentImage;
  unsigned int               mContainerIndex;           //have the value 0 or 1, refer to mTiles[0] or mTiles[1]

  bool                       mChangeTurningDirection;
  bool                       mIsToNextImage;            //if true, cubes rotate counter-clockwise; else clockwise
  bool                       mIsImageLoading;

  float                      mAnimationDuration;
  Animation                  mAnimation;
  bool                       mIsAnimating;
  bool                       mIsPaused;

  float                      mCubeDisplacement;

  bool                       mFirstTransition;

  RenderTask                 mOffScreenTask;
  FrameBufferImage           mOffScreenBuffer[2];
  ImageActor                 mEmptyImage;
  FullAreaImageCreator       mFullImageCreator;
  unsigned int               mBufferIndex;

  static const Vector4       FULL_BRIGHTNESS;
  static const Vector4       HALF_BRIGHTNESS;

private:

  Toolkit::CubeTransitionEffect::TransitionCompletedSignalV2 mTransitionCompletedSignalV2;

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CubeTransitionEffect& GetImpl(Dali::Toolkit::CubeTransitionEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::CubeTransitionEffect&>(handle);
}

inline const Internal::CubeTransitionEffect& GetImpl(const Dali::Toolkit::CubeTransitionEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::CubeTransitionEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_CUBE_TRANSITION_EFFECT_H__ */
