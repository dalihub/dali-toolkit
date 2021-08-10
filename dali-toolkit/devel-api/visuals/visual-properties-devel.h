#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelVisual
{
/**
 * @brief All the visual types.
 */
enum Type
{
  BORDER         = Dali::Toolkit::Visual::BORDER,
  COLOR          = Dali::Toolkit::Visual::COLOR,
  GRADIENT       = Dali::Toolkit::Visual::GRADIENT,
  IMAGE          = Dali::Toolkit::Visual::IMAGE,
  MESH           = Dali::Toolkit::Visual::MESH,
  PRIMITIVE      = Dali::Toolkit::Visual::PRIMITIVE,
  WIREFRAME      = Dali::Toolkit::Visual::WIREFRAME,
  TEXT           = Dali::Toolkit::Visual::TEXT,
  N_PATCH        = Dali::Toolkit::Visual::N_PATCH,
  SVG            = Dali::Toolkit::Visual::SVG,
  ANIMATED_IMAGE = Dali::Toolkit::Visual::ANIMATED_IMAGE,

  ANIMATED_GRADIENT     = ANIMATED_IMAGE + 1, ///< Renders an animated gradient.
  ANIMATED_VECTOR_IMAGE = ANIMATED_IMAGE + 2, ///< Renders an animated vector image.
  ARC                   = ANIMATED_IMAGE + 3, ///< Renders an arc.
};

/**
 * @brief Visual Properties
 */
namespace Property
{
enum Type
{
  TYPE                = Dali::Toolkit::Visual::Property::TYPE,
  SHADER              = Dali::Toolkit::Visual::Property::SHADER,
  TRANSFORM           = Dali::Toolkit::Visual::Property::TRANSFORM,
  PREMULTIPLIED_ALPHA = Dali::Toolkit::Visual::Property::PREMULTIPLIED_ALPHA,
  MIX_COLOR           = Dali::Toolkit::Visual::Property::MIX_COLOR,
  OPACITY             = Dali::Toolkit::Visual::Property::OPACITY,

  /**
   * @brief The fitting mode of the visual.
   * @details Name "fittingMode", type FittingMode (Property::INTEGER) or Property::STRING.
   * @see DevelVisual::FittingMode
   * @note The default is defined by the type of visual (if it's suitable to be stretched or not).
   */
  VISUAL_FITTING_MODE = OPACITY + 1,

  /**
   * @brief The radius for the rounded corners of the visual.
   * @details Name "cornerRadius", type Property::FLOAT or Prooperty::VECTOR4, animatable
   * @note By default, it is Vector::ZERO.
   * @note Only Property::Vector4 can be animated.
   * @note Each radius will clamp internally to the half of smaller of the visual width and visual height.
   * @note Their may exist some alias when you use it as ClippingMode::CLIP_CHILDREN.
   * @note Radius value are used in clockwise order from top-left-corner to bottom-left-corner.
   *       When radius is Vector4(x, y, z, w)
   *       x    y
   *        +--+
   *        |  |
   *        +--+
   *       w    z
   */
  CORNER_RADIUS = OPACITY + 2,

  /**
   * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute (in world units).
   * @details Name "cornerRadiusPolicy", type Property::INTEGER.
   * @see Policy::Type
   * @note By default, it is ABSOLUTE to the visual's size.
   *       If it it RELATIVE, the corner radius value is relative to the smaller of the visual width and visual height.
   */
  CORNER_RADIUS_POLICY = OPACITY + 3,

  /**
   * @brief The width for the borderline of the visual.
   * @details Name "borderlineWidth", type Property::FLOAT, animatable
   * @note Optional. Default value is 0.0f.
   */
  BORDERLINE_WIDTH = OPACITY + 4,

  /**
   * @brief The color for the borderline of the visual.
   * @details Name "borderlineColor", type Property::VECTOR4, animatable
   * @note Default value is Color::BLACK.
   * @note This color is affected by opacity.
   */
  BORDERLINE_COLOR = OPACITY + 5,

  /**
   * @brief The offset from the visual borderline (recommend [-1.0f to 1.0f]).
   * @details Name "borderlineOffset", type Property::FLOAT, animatable
   * @note Default value is 0.0f.
   * @note This value will clamp internally to [-1.0f to 1.0f].
   */
  BORDERLINE_OFFSET = OPACITY + 6,
};

} // namespace Property

/**
 * @brief The values of this enum determine how the visual should be fit to the view
 */
enum FittingMode
{
  FIT_KEEP_ASPECT_RATIO,      ///< The visual should be scaled to fit, preserving aspect ratio
  FILL,                       ///< The visual should be stretched to fill, not preserving aspect ratio
  OVER_FIT_KEEP_ASPECT_RATIO, ///< The visual should be scaled to fit, preserving aspect ratio. The visual will be filled without empty area, and outside is cropped away.
  CENTER,                     ///< The visual should keep original size of image. It is not scaled and not strecthed.
  FIT_HEIGHT,                 ///< The visual should be scaled to fit, preserving aspect ratio. Height is scaled proportionately to maintain aspect ratio. It will be deprecated.
  FIT_WIDTH                   ///< The visual should be scaled to fit, preserving aspect ratio. Width is scaled proportionately to maintain aspect ratio. It will be deprecated.
};

/**
 * @brief Devel Visual Transform for the offset or size.
 */
namespace Transform
{
/**
 * @brief Visual Transform Property.
 */
namespace Property
{
/**
 * @brief Enumeration for the type of Transform Property.
 */
enum Type
{
  SIZE_POLICY = Dali::Toolkit::Visual::Transform::Property::SIZE_POLICY,

  /**
   * @brief Extra size value that will be added to the computed visual size.
   * @details Name "extraSize", type Vector2.
   *
   * @note It is an absolute value.
   *       The property can be used when a user want to set a visual size as a combined value of `relative` and `absolute`.
   *       For example, when a user want to set a visual size to (ControlSize * 2 + 10),
   *       The transform map will be,
   *       transformMap.Add( Transform::Property::SIZE, Vector2( 2.0f, 2.0f ) )
   *                   .Add( Transform::Property::SIZE_POLICY, Vector2( Transform::Policy::Relative, Transform::Policy::Relative ) )
   *                   .Add( Transform::Property::EXTRA_SIZE, Vector2( 10.0f, 10.0f ) );
   */
  EXTRA_SIZE = SIZE_POLICY + 1,
};

} // namespace Property

} // namespace Transform

} // namespace DevelVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_VISUAL_PROPERTIES_DEVEL_H
