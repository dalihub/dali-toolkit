#ifndef DALI_TOOLKIT_DEVEL_ANIMATED_GRADIENT_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_DEVEL_ANIMATED_GRADIENT_VISUAL_PROPERTIES_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{
/**
 *
 */

/**
 * @brief AnimatedGradientVisual is to render a smooth transition of colors to the control's quad with animation.
 */
namespace DevelAnimatedGradientVisual
{
/**
 * @brief AnimatedGradientVisual Property
 */
namespace Property
{
/**
 * @brief AnimatedGradientVisual Property
 */
enum
{
  /**
   * @brief The form of gradient.
   * @details Name "gradientType", type GradientType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is GradientType::LINEAR.
   * @see GradientType::Type
   */
  GRADIENT_TYPE = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The coordinate system inside of control's quad.
   * @details Name "unitType", type UnitType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is UnitType::OBJECT_BOUNDING_BOX.
   * @see UnitType::Type
   */
  UNIT_TYPE,

  /**
   * @brief The policy of color when gradient coordinate is not between 0 and 1.
   * @details Name "spreadType", type SpreadType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is SpreadType::REFLECT.
   * @see SpreadType::Type
   */
  SPREAD_TYPE,

  /**
   * @brief The position of a gradient coordinate is 0. If GRADIENT_TYPE is RADIAL, than it will be center of circle.
   * @details Name "startPosition", type Property::Vector2 or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property
   * @note If not supplied, default is Vector2( -0.5f, 0.0f ).
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property
   */
  START_POSITION,

  /**
   * @brief The color of a gradient coordinate is 0.
   * @details Name "startColor", type Property::Vector4 or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is Vector4( 143.0f, 170.0f, 220.0f, 255.0f ) / 255.0f.
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   */
  START_COLOR,

  /**
   * @brief The position of a gradient coordinate is 1.
   * @details Name "endPosition", type Property::Vector2 or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is Vector2( 0.5f, 0.0f ).
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   */
  END_POSITION,

  /**
   * @brief The color of a gradient coordinate is 1.
   * @details Name "endColor", type Property::Vector4 or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is Vector4( 255.0f, 163.0f, 163.0f, 255.0f ) / 255.0f.
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property
   */
  END_COLOR,

  /**
   * @brief The center of rotate START_POSITION and END_POSITION.
   * @details Name "rotateCenter", type Property::Vector2 or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is Vector2( 0.0f, 0.0f ).
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property
   */
  ROTATE_CENTER,

  /**
   * @brief The amount of rotate START_POSITION and END_POSITION in radian.
   * @details Name "rotateAmount", type Property::FLOAT or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is 0.0f.
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property
   */
  ROTATE_AMOUNT,

  /**
   * @brief The offset of gradient coordinate. The point will have a color where (gradient coordinate + offset).
   * @details Name "offset", type Property::FLOAT or Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   * @note If not supplied, default is Animation from 0.0f to 2.0f, with duration 3.0f seconds.
   * @note This can be animated when you use type as Property::Map which index is AnimatedGradientVisual::AnimationParameter::Property.
   */
  OFFSET
};

} // namespace Property

/**
 * @brief The type of gradient form. It decide the method of calculate gradient coordinate.
 * - If GradientType::Type is LINEAR, gradient coordinate is dot product with the line which contain START_POSITION and END_POSITION.
 * - If GradientType::Type is RADIAL, gradient coordinate is euclidean distance from START_POSITION.
 */
namespace GradientType
{
/**
 * @brief The type of gradient form. It decide the method of calculate gradient coordinate.
 * - If GradientType::Type is LINEAR, gradient coordinate is dot product with the line which contain START_POSITION and END_POSITION.
 * - If GradientType::Type is RADIAL, gradient coordinate is euclidean distance from START_POSITION.
 */
enum Type
{
  LINEAR, ///< Draw gradient linear form.
  RADIAL  ///< Draw gradient radial form.
};

} // namespace GradientType

/**
 * @brief The type of coordinate system for certain attributes of the points in a gradients.
 * This applies to the START_POSITION, END_POSITION, and ROTATE_CENTER.
 */
namespace UnitType
{
/**
 * @brief The type of coordinate system for certain attributes of the points in a gradients.
 * This applies to the START_POSITION, END_POSITION, and ROTATE_CENTER.
 */
enum Type
{
  OBJECT_BOUNDING_BOX, ///< Use positions coordinate in bottom-left(-0.5,-0.5) ~ top-right(0.5,0.5).
  USER_SPACE,          ///< Use positions coordinate in bottom-left(-ActorSize.xy * 0.5) ~ top-right(ActorSize.xy * 0.5).
};

} // namespace UnitType

/**
 * @brief The policies that define what happens if the gradient coordinate is not between 0 and 1.
 */
namespace SpreadType
{
/**
 * @brief The policies that define what happens if the gradient coordinate is not between 0 and 1.
 */
enum Type
{
  REFLECT, ///<  Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc.
  REPEAT,  ///< Repeat the gradient pattern start-to-end, start-to-end, start-to-end etc.
  CLAMP,   ///< Use the terminal colors of gradient.
};

} // namespace SpreadType

/**
 * @brief AnimatedGradientVisual::AnimationParameter is information of each properties animation.
 * Value animate from start to target during duration seconds.
 * Each start/target value type for AnimatedGradientVisual Property is :
 * - START_POSITION, END_POSITION, ROTATE_CENTER is Property::Vector2
 * - START_COLOR, END_COLOR is Property::Vector4
 * - ROTATE_AMOUNT, OFFSET is Property::FLOAT
 * You can set the animation information to AnimatedGradientVisual Property by using Property::MAP.
 */
namespace AnimationParameter
{
/**
 * @brief AnimatedGradientVisual::AnimationParameter Property
 */
enum Property
{

  /**
   * @brief The start value of this animation.
   * @details Name "startValue", type depends on AnimatedGradientVisual Property.
   * @note If not supplied, default is Property::Value( 0.0f ).
   */
  START = DevelAnimatedGradientVisual::Property::OFFSET + 1,

  /**
   * @brief The target value of this animation.
   * @details Name "targetValue", type depends on AnimatedGradientVisual Property.
   * @note If not supplied, default is Property::Value( 0.0f ).
   */
  TARGET,

  /**
   * @brief The direction of this animation.
   * @details Name "directionType", type DirectionType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is DirectionType::FORWARD.
   * @see DirectionType::Type
   */
  DIRECTION,

  /**
   * @brief The duration of this animation in seconds.
   * @details Name "duration", type Property::FLOAT.
   * @note If not supplied, default is 3.0f.
   */
  DURATION,

  /**
   * @brief The delay of this animation in seconds.
   * If delay is positive, wait the animation 'delay' seconds.
   * If delay is negative, skip the animation '-delay' seconds.
   * @details Name "delay", type Property::FLOAT.
   * @note If not supplied, default is 0.0f.
   */
  DELAY,

  /**
   * @brief The repeat count of this animation.
   * If repeat is negative, animate unlimited loop.
   * If repeat is zero, animation will no run.
   * If repeat is positive, animate 'repeat' times.
   * @details Name "repeat", type Property::INTEGER.
   * @note If not supplied, default is 0.
   */
  REPEAT,

  /**
   * @brief The delay before each loop of this animation in seconds.
   * @details Name "repeatDelay", type Property::FLOAT.
   * @note If not supplied, default is 0.0f.
   */
  REPEAT_DELAY,

  /**
   * @brief The motion of this animation.
   * @details Name "motionType", type MotionType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is MotionType::LOOP.
   * @see MotionType::Type
   */
  MOTION_TYPE,

  /**
   * @brief The easing option of this animation.
   * @details Name "easingType", type EasingType::Type (Property::INTEGER) or Property::STRING.
   * @note If not supplied, default is EasingType::LINEAR.
   * @see EasingType::Type
   */
  EASING_TYPE
};

/**
 * @brief The type of animation direction
 */
namespace DirectionType
{
/**
 * @brief The type of animation direction
 */
enum Type
{
  FORWARD,  ///< Animate value from START to TARGET
  BACKWARD, ///< Animate value frome TARGET to START
};

} // namespace DirectionType

/**
 * @brief The type of animation motion
 */
namespace MotionType
{
/**
 * @brief The type of animation motion
 */
enum Type
{
  LOOP,   ///< Animate loopingmode restart
  MIRROR, ///< Animate loopingmode auto_reverse
};

} // namespace MotionType

/**
 * @brief The type of animation easing
 */
namespace EasingType
{
/**
 * @brief The type of animation easing
 */
enum Type
{
  LINEAR, ///< Easing animation linear
  IN,     ///< Ease-in animation (slow start -> fast finish)
  OUT,    ///< Ease-out animation (fast start -> slow finish)
  IN_OUT, ///< Ease-in and Ease-out animation (slow start -> slow finish)
};

} // namespace EasingType

} // namespace AnimationParameter

} // namespace DevelAnimatedGradientVisual

/**
 *
 */

} // namespace Toolkit

} // namespace Dali

#endif
