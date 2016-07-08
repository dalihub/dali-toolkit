#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__

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

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/**
 * CubeTransitionEffect implementation class
 */
class CubeTransitionEffect;

} // namespace Internal

/**
 * CubeTransitionEffect is a base class of custom transition effect on Images
 * The two images are partitioned into tiles and serves as two perpendicular faces of cubes
 * By rotating these cubes to transit from one image to another
 *
 * Usage example:
 *
 * @code
 *
 * //create a new CubeTransitionEffect
 * //use the New funtion of subclass ( CubeTransitionWaveEffect or CubeTransitionCrossEffect )
 * CubeTransitionEffect cubeEffect = CubeTransitionWaveEffect::New(numRows, numColumns);
 *
 * //set the duration of transition animation
 * cubeEffect.SetTransitionDuration( animationDuration );
 *
 * //set the displacement of bouncing movement during cube's rotation
 * cubeEffect.SetCubeDisplacement( cubeDisplacement  );
 *
 * // Add to stage
 * stage.Add( cubeEffect );
 *
 * // Set the current image,
 * // only need to set at beginning or when the current image was transitioned to with no effect or other effect
 * cubeEffect.SetCurrentImage( firstImage );
 *
 * // Set target image, paired with startTransition. These two steps would be repeated as needed
 * cubeEffect.SetTargetimage( secondImage );
 * // Activate the effect
 * //   no param / param ture: default horizontally left panGesture
 * //   or param false:  default horizontally right panGesture
 * //   or params position & displacement: specified the panGesture
 * cubeEffect.StartTransition( );
 *
 * @endcode
 *
 * Signals
 * | %Signal Name        | Method                           |
 * |---------------------|----------------------------------|
 * | transitionCompleted | @ref TransitionCompletedSignal() |
 */
class DALI_IMPORT_API CubeTransitionEffect : public Control
{
public:

  /**
   * Create an uninitialized CubeTransitionEffect;
   * this can be initialized by New function of its subclass
   */
  CubeTransitionEffect();

  /**
   * Destructor
   */
  ~CubeTransitionEffect();

  /**
   * @brief Downcast an Object handle to a CubeTransitionEffect handle.
   *
   * If handle points to a CubeTransitionEffect object the downcast produces
   * a valid handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle A handle to an object
   * @return A handle to a CubeTransitionEffect object or an uninitialized handle
   */
  static CubeTransitionEffect DownCast( BaseHandle handle );

  /**
   * Set the duration of transition animation
   * @param[in] duration The duration of transition animation
   */
  void SetTransitionDuration( float duration );

  /**
   * Get the duration of transition animation
   * @return duration The duration of transition animation
   */
  float GetTransitionDuration() const;

  /**
   * Set the displacement of bouncing animation during cube's rotation
   * @param[in] displacement The displacement of bouncing animation
   */
  void SetCubeDisplacement( float displacement );

  /**
   * Get the displacement of bouncing animation during cube's rotation
   * @return displacement The displacement of bouncing animation
   */
  float GetCubeDisplacement() const;

  /**
   * Return the transition status
   * @return True if the transition is under processing; false if finished
   */
  bool IsTransitioning();

  /**
   * Set the current texture to transition from
   * if using this same effect continually, only need to set once
   * @param[in] texture The current texture
   */
  void SetCurrentTexture( Texture texture );

  /**
   * Set the target texture to transit to
   * @param[in] texture The new Texture showing on stage
   */
  void SetTargetTexture( Texture texture );

  /**
   * Activate the transition animation with horizontally left/right panGesture
   * @pre target image is set
   * @param[in] toNextImage Horizontally left panGesture if true, horizontally right if false
   */
  void StartTransition( bool toNextImage = true );

  /**
   * Activate the transition animation with specified panGesture
   * @pre target image is set
   * @param[in] panPosition The press down position of panGesture
   * @param[in] panDisplacement The displacement vector of panGesture
   */
  void StartTransition( Vector2 panPosition, Vector2 panDisplacement );

  /**
   * Pause the transition animation.
   * It does nothing if the animation is not running.
   */
  void PauseTransition();

  /**
   * Re-Activate the transition animation after it is paused by calling PauseTransition().
   * It does nothing in other cases.
   */
  void ResumeTransition();

  /**
   * Inactivate the transition animation if it is running.
   * Also set the rotation and position of cubes, colors of tile to the same as the start state when the animation if finished completely
   * It does nothing if the animation is not running.
   */
  void StopTransition();

public: //Signal

  //Transition animation completed signal
  typedef Signal< void ( CubeTransitionEffect, Texture ) >  TransitionCompletedSignalType;

  /**
   * Signal emitted when the transition has completed animation
   * A callback of the following type may be connected
   * @code
   *   void YourCallbackName( CubeTransitionEffect cubeEffect, Texture currentTexture );
   * @endcode
   * @return The Signal to connect to.
   */
  TransitionCompletedSignalType& TransitionCompletedSignal();

public: // Not intended for developer use

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL CubeTransitionEffect( Internal::CubeTransitionEffect& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL CubeTransitionEffect( Dali::Internal::CustomActor* internal );

}; //class CubeTransitionEffect

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__ */
