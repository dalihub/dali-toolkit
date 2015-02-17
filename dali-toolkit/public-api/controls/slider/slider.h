#ifndef __DALI_TOOLKIT_SLIDER_H__
#define __DALI_TOOLKIT_SLIDER_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Slider;
}

/**
 * @brief Slider is a control to enable sliding an indicator between two values
 *
 * Signals
 * | %Signal Name      | Method                      |
 * |-------------------|-----------------------------|
 * | value-changed     | @ref ValueChangedSignal()   |
 * | mark              | @ref MarkSignal()           |
 */
class DALI_IMPORT_API Slider : public Control
{
public:

  // Properties
  static const Property::Index LOWER_BOUND_PROPERTY;            ///< Property, name "lower-bound",       type FLOAT
  static const Property::Index UPPER_BOUND_PROPERTY;            ///< Property, name "upper-bound",       type FLOAT
  static const Property::Index VALUE_PROPERTY;                  ///< Property, name "value",             type FLOAT

  static const Property::Index HIT_REGION_PROPERTY;             ///< Property, name "hit-region",        type VECTOR2
  static const Property::Index BACKING_REGION_PROPERTY;         ///< Property, name "backing-region",    type VECTOR2
  static const Property::Index HANDLE_REGION_PROPERTY;          ///< Property, name "handle-region",     type VECTOR2

  static const Property::Index BACKING_IMAGE_NAME_PROPERTY;     ///< Property, name "backing-image-name",  type STRING
  static const Property::Index HANDLE_IMAGE_NAME_PROPERTY;      ///< Property, name "handle-image-name",   type STRING
  static const Property::Index PROGRESS_IMAGE_NAME_PROPERTY;    ///< Property, name "progress-image-name", type STRING
  static const Property::Index POPUP_IMAGE_NAME_PROPERTY;       ///< Property, name "popup-image-name",    type STRING
  static const Property::Index POPUP_ARROW_IMAGE_NAME_PROPERTY; ///< Property, name "popup-arrow-image-name", type STRING

  static const Property::Index DISABLE_COLOR_PROPERTY;          ///< Property, name "disable-color",    type VECTOR4
  static const Property::Index POPUP_TEXT_COLOR_PROPERTY;       ///< Property, name "popup-text-color", type VECTOR4

  static const Property::Index VALUE_PRECISION_PROPERTY;        ///< Property, name "value-precision",  type INT

  static const Property::Index SHOW_POPUP_PROPERTY;             ///< Property, name "show-popup",       type BOOLEAN
  static const Property::Index SHOW_VALUE_PROPERTY;             ///< Property, name "show-value",       type BOOLEAN

  static const Property::Index ENABLED_PROPERTY;                ///< Property, name "enabled",          type BOOLEAN

  static const Property::Index MARKS_PROPERTY;                  ///< Property, name "marks",            type Property::Array<float>
  static const Property::Index SNAP_TO_MARKS_PROPERTY;          ///< Property, name "snap-to-marks",    type BOOLEAN
  static const Property::Index MARK_TOLERANCE_PROPERTY;         ///< Property, name "mark-tolerance",   type FLOAT

public:

  /**
   * Create the Slider control
   * @return A handle to the Slider control.
   */
  static Slider New();

  /**
   * Creates an empty Slider handle
   */
  Slider();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   */
  Slider( const Slider& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  Slider& operator=( const Slider& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Slider();

  /**
   * Downcast an Object handle to Slider. If handle points to a Slider the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Slider or an uninitialized handle
   */
  static Slider DownCast( BaseHandle handle );

public:

  // Signals

  // Value changed
  typedef Signal< bool ( Slider, float ) > ValueChangedSignalType;
  typedef Signal< bool ( Slider, int ) > MarkSignalType;

  /**
   * Signal emitted when the slider value changes
   */
  ValueChangedSignalType& ValueChangedSignal();

  /**
   * Signal emitted when the sliding is finished
   */
  ValueChangedSignalType& SlidingFinishedSignal();

  /**
   * Signal emitted when the slider handle reaches a mark
   */
  MarkSignalType& MarkSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Slider(Internal::Slider& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Slider( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SLIDER_H__
