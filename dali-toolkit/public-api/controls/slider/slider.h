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

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserving 1000 property indices
  };

  /**
   * @brief An enumeration of properties belonging to the Slider class.
   */
  struct Property
  {
    enum
    {
      LOWER_BOUND = PROPERTY_START_INDEX, ///< Property, name "lower-bound",            type FLOAT
      UPPER_BOUND,                        ///< Property, name "upper-bound",            type FLOAT
      VALUE,                              ///< Property, name "value",                  type FLOAT
      HIT_REGION,                         ///< Property, name "hit-region",             type VECTOR2
      BACKING_REGION,                     ///< Property, name "backing-region",         type VECTOR2
      HANDLE_REGION,                      ///< Property, name "handle-region",          type VECTOR2
      BACKING_IMAGE_NAME,                 ///< Property, name "backing-image-name",     type STRING
      HANDLE_IMAGE_NAME,                  ///< Property, name "handle-image-name",      type STRING
      PROGRESS_IMAGE_NAME,                ///< Property, name "progress-image-name",    type STRING
      POPUP_IMAGE_NAME,                   ///< Property, name "popup-image-name",       type STRING
      POPUP_ARROW_IMAGE_NAME,             ///< Property, name "popup-arrow-image-name", type STRING
      DISABLE_COLOR,                      ///< Property, name "disable-color",          type VECTOR4
      POPUP_TEXT_COLOR,                   ///< Property, name "popup-text-color",       type VECTOR4
      VALUE_PRECISION,                    ///< Property, name "value-precision",        type INTEGER
      SHOW_POPUP,                         ///< Property, name "show-popup",             type BOOLEAN
      SHOW_VALUE,                         ///< Property, name "show-value",             type BOOLEAN
      ENABLED,                            ///< Property, name "enabled",                type BOOLEAN
      MARKS,                              ///< Property, name "marks",                  type Property::Array<FLOAT>
      SNAP_TO_MARKS,                      ///< Property, name "snap-to-marks",          type BOOLEAN
      MARK_TOLERANCE,                     ///< Property, name "mark-tolerance",         type FLOAT
    };
  };

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
