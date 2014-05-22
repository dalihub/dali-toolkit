#ifndef __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__
#define __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__

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
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
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
 * CubeTransitionEffect is a base class of custom transition effect on Image actors
 * The two images are partitioned into tiles and serves as two perpendicular faces of cubes
 * By rotating these cubes to transit from one image to another
 *
 * Usage example:
 *
 * @code
 *
 * //create a new CubeTransitionEffect
 * //use the New funtion of subclass ( CubeTransitionWaveEffect or CubeTransitionCrossEffect )
 * CubeTransitionEffect cubeEffect = CubeTransitionWaveEffect::New(numRows, numColumns, viewAreaSize);
 *
 * //set the duration of transition animation
 * cubeEffect.SetTransitionDuration( animationDuration );
 *
 * //set the displacement of bouncing movement during cube's rotation
 * cubeEffect.SetCubeDisplacement( cubeDisplacement  );
 *
 * // Add to stage
 * stage.Add( cubeEffect.GetRoot() );
 *
 * // Set the current image,
 * // only need to set at beginning or when the current image was transited to with no effect or other effect
 * cubeEffect.SetCurrentImage( firstImageActor );
 *
 * // Set target image, paired with startTransition. These two steps would be repeated as needed
 * cubeEffect.SetTargetimage( secondImageActor );
 * // Activate the effect
 * //   no param / param ture: default horizontally left panGesture
 * //   or param false:  default horizontally right panGesture
 * //   or params position & displacement: specified the panGesture
 * cubeEffect.StartTransition( );
 *
 * @endcode
 */
class CubeTransitionEffect : public BaseHandle
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
   * Getet the displacement of bouncing animation during cube's rotation
   * @return displacement The displacement of bouncing animation
   */
  float GetCubeDisplacement() const;

  /**
   * Return the transition effect root actor, should then be added to stage
   * @return The transition effect root actor
   */
  Actor GetRoot();

  /**
   * Return the transition status
   * @return True if the transition is under processing; false if finished
   */
  bool IsTransiting();

  /**
   * Set the current image to transite from
   * if using this same effect continuely, only need to set once
   * @param[in] imageActor The current imageActor
   */
  void SetCurrentImage(ImageActor imageActor);

  /**
   * Set the target image to transit to
   * @param[in] imageActor The new imageActor showing on stage
   */
  void SetTargetImage(ImageActor imageActor);

  /**
   * Activate the transition animation with horizontally left/right panGesture
   * @pre target image is set
   * @param[in] toNextImage Horizontally left panGesture if ture, horizontally right if false
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
   * Also set the rotation and position of cubes, colors of tile to the same as the final state when the animation if finished completely
   * It does nothing if the animation is not running.
   */
  void StopTransition();

public: //Signal

  //signal name
  static const char* const SIGNAL_TRANSITION_COMPLETED;

  //Transition animation completed signal
  typedef SignalV2< void ( CubeTransitionEffect, ImageActor ) > TransitionCompletedSignalV2;

  /**
   * Signal emitted when the transition has completed animation
   * A callback of the following type may be connected
   * @code
   *   void YourCallbackName( CubeTransitionEffect cubeEffect, ImageActor currentImage );
   * @endcode
   * @return The Signal to connect to.
   */
  TransitionCompletedSignalV2& TransitionCompletedSignal();

public: // Not intended for developer use

  CubeTransitionEffect( Internal::CubeTransitionEffect* impl );

}; //class CubeTransitionEffect

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_CUBE_TRANSITION_EFFECT_H__ */
